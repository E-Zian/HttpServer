#include "Connection.h"
#include "Helper.h"
#include "ResponseFactory.h"
#include <asio/experimental/awaitable_operators.hpp>
#include <iostream>
#include <asio.hpp>
#include <fmt/core.h>

namespace {
    std::optional<size_t> parseRequestForHeader(std::string_view buffer) {
        const std::string delimiter{"\r\n\r\n"};
        const size_t delimiterPosition{buffer.find(delimiter)};

        if (delimiterPosition == std::string::npos) {
            return std::nullopt;
        }

        return delimiterPosition;
    };

    std::string_view getHeaderLine(std::string_view header, size_t &startingPosition) {
        const auto delimiter{"\r\n"};
        const size_t delimiterPosition{header.find(delimiter, startingPosition)};

        const size_t oldStartingPosition{startingPosition};

        if (delimiterPosition == std::string::npos) {
            startingPosition = header.length();
            return header.substr(oldStartingPosition);
        }

        startingPosition = delimiterPosition + 2;
        return header.substr(oldStartingPosition, delimiterPosition - oldStartingPosition);
    }

    std::optional<std::pair<std::string, std::string> > parseHeaderLine(const std::string &headerLine) {
        const auto delimiter{":"};
        const size_t delimiterPosition{headerLine.find(delimiter)};

        if (delimiterPosition == std::string::npos) {
            return std::nullopt;
        }
        return std::make_pair(headerLine.substr(0, delimiterPosition),
                              Helper::trim(headerLine.substr(delimiterPosition + 1)));
    }

    std::optional<std::vector<std::string_view> > parseRequestLine(std::string_view requestLine) {
        std::vector<std::string_view> requestLineComponents{};
        size_t start{};
        while (start < requestLine.length()) {
            size_t end{requestLine.find(' ', start)};

            if (end == std::string::npos) {
                requestLineComponents.push_back(requestLine.substr(start, requestLine.length() - start));
                break;
            } else {
                requestLineComponents.push_back(requestLine.substr(start, end - start));
                start = end + 1;
            }
        }
        if (requestLineComponents.size() < 3) {
            return std::nullopt;
        }
        return requestLineComponents;
    }

    std::string statusToStatusLine(const HttpStatus status) {
        const std::string httpVersion{"HTTP/1.1 "};

        switch (status) {
            case HttpStatus::OK:
                return httpVersion + "200 OK";

            case HttpStatus::BAD_REQUEST:
                return httpVersion + "400 Bad Request";

            case HttpStatus::NOT_FOUND:
                return httpVersion + "404 Not Found";

            default:
                return httpVersion + "500 Internal Server Error";
        }
    }

    std::string headerToString(const std::unordered_map<std::string, std::string> &header) {
        std::string headerLines{};
        headerLines.reserve(header.size() * 50);

        for (auto &headerPair: header) {
            headerLines += headerPair.first + ": " + headerPair.second + "\r\n";
        }
        return headerLines;
    }

    const std::unordered_map<std::string_view, Method> methodMap = {
        {"GET", Method::GET},
        {"POST", Method::POST},
        {"PUT", Method::PUT},
        {"DELETE", Method::DEL},
        {"PATCH", Method::PATCH},
        {"HEAD", Method::HEAD},
        {"OPTIONS", Method::OPTIONS},
    };
}

Connection::Connection(tcp::socket &&connectionSocket, const int connectionId, const IDispatcher &dispatcher)
    : socket_{std::move(connectionSocket)},
      connectionId_{connectionId},
      dispatcher_(dispatcher) {
}

Connection::pointer Connection::create(tcp::socket &&connectionSocket, const int connectionId,
                                       const IDispatcher &dispatcher) {
    return pointer(new Connection(std::move(connectionSocket), connectionId, dispatcher));
}

Connection::~Connection() {
    Helper::displayMessage("Connection ID ({}) Disconnected", connectionId_);
}

asio::awaitable<void> Connection::startRead() {
    try {
        auto self{shared_from_this()};
        std::array<char, 128> receivingBuffer{};

        std::optional<size_t> delimiterPosition{};
        while (!delimiterPosition.has_value()) {
            const size_t len{
                co_await socket_.async_read_some(asio::buffer(receivingBuffer),
                                                 asio::use_awaitable)
            };


            requestReceived_.insert(requestReceived_.end(), receivingBuffer.begin(), receivingBuffer.begin() + len);

            delimiterPosition = parseRequestForHeader(
                std::string_view(requestReceived_.data(), requestReceived_.size()));
        }

        // Display Header

        // Used to remove warning of std::optional delimiterPosition
        if (!delimiterPosition) {
            co_return;
        }
        Helper::displayMessage("Headers Received from Connection Id ({})\n{}", connectionId_,
                               std::string_view(requestReceived_.data(), delimiterPosition.value()));

        request_.header = std::string_view(requestReceived_.data(), delimiterPosition.value());

        size_t startingPosition{};

        const auto requestLine{getHeaderLine(request_.header, startingPosition)};

        auto parsedRequestLineComponents{parseRequestLine(requestLine)};

        if (!parsedRequestLineComponents.has_value()) {
            Helper::displayError("Error Connection ID ({}) Request Failed: {}", connectionId_,
                                 "Malformed Request Line Received");

            co_await writeResponse(
                ResponseFactory::badRequest("Request Failed: Malformed request line received on {}", requestLine));

            co_return;
        };

        auto methodIt{methodMap.find(parsedRequestLineComponents.value()[0])};
        if (methodIt == methodMap.end()) {
            
            Helper::displayError("Error Connection ID ({}) Request Failed: {}", connectionId_,
                "Unsupported Method");

            co_await writeResponse(
                ResponseFactory::badRequest("Request Failed: Unsupported Method {}", parsedRequestLineComponents.value()[0]));
            co_return;
        }

        parsedRequest_.method = methodIt->second;
        parsedRequest_.route = parsedRequestLineComponents.value()[1];
        parsedRequest_.httpVersion = parsedRequestLineComponents.value()[2];


        while (startingPosition != request_.header.length()) {
            std::string headerLine{getHeaderLine(request_.header, startingPosition)};

            std::optional<std::pair<std::string, std::string> > headerPair{parseHeaderLine(headerLine)};
            if (!headerPair.has_value()) {
                Helper::displayError("Error Connection ID ({}) Request Failed: {}{}", connectionId_,
                                     "Malformed header line : ", headerLine);

                co_await writeResponse(
                    ResponseFactory::badRequest("Request Failed: Malformed header line received on {}", headerLine));

                break;
            }
            parsedRequest_.header[headerPair.value().first] = headerPair.value().second;
        }


        // Has Body
        if (parsedRequest_.header.contains("Content-Length")) {
            const int contentLength{std::stoi(parsedRequest_.header["Content-Length"])};
            std::vector<char> receivingBodyBuffer(contentLength);

            // 4 represents the double carriage return and new line
            while (requestReceived_.size() - request_.header.size() - 4 < contentLength) {
                size_t len{
                    co_await socket_.async_read_some(asio::buffer(receivingBodyBuffer),
                                                     asio::use_awaitable)
                };


                requestReceived_.insert(requestReceived_.end(), receivingBodyBuffer.begin(),
                                        receivingBodyBuffer.begin() + static_cast<long long>(len));
            }
            Helper::displayMessage("requestReceived_.size()={}, header.size()={}, delimiterPos={}, contentLength={}",
                                   requestReceived_.size(), request_.header.size(), delimiterPosition.value(),
                                   contentLength);


            request_.body = std::string_view(&requestReceived_[delimiterPosition.value() + static_cast<size_t>(4)],
                                             contentLength);

            Helper::displayMessage("Body Received from Connection Id ({})\n{}", connectionId_, request_.body);

            parsedRequest_.body = request_.body;
        }

        Response response{dispatcher_.dispatch(std::string(parsedRequest_.route), parsedRequest_)};
        co_await writeResponse(response);
    } catch (std::exception &e) {
        Helper::displayError("{}", e.what());
    }
}

asio::awaitable<void> Connection::writeResponse(const Response &response) {
    auto self{shared_from_this()};

    const std::string statusLine{statusToStatusLine(response.status)};
    const std::string headerLines{headerToString(response.header)};

    std::string parsedResponse{statusLine + "\r\n" + headerLines + "\r\n" + response.body};
    Helper::displayMessage("{}", parsedResponse);
    co_await asio::async_write(socket_, asio::buffer(parsedResponse), asio::use_awaitable);
}
