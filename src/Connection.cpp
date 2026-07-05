#include "Connection.h"
#include "Helper.h"
#include "ResponseFactory.h"
#include <asio/experimental/awaitable_operators.hpp>
#include <iostream>
#include <asio.hpp>
#include <fmt/core.h>
#include <algorithm>

namespace {
    std::optional<size_t> parseRequestForDelimiter(std::string_view buffer) {
        const std::string delimiter{"\r\n\r\n"};
        const size_t delimiterPosition{buffer.find(delimiter)};

        if (delimiterPosition == std::string::npos) {
            return std::nullopt;
        }

        return delimiterPosition;
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

            case HttpStatus::REQUEST_TIMEOUT:
                return httpVersion + "408 Request Timeout";

            case HttpStatus::CONTENT_TOO_LARGE:
                return httpVersion + "413 Content Too Large";

            case HttpStatus::REQUEST_HEADERS_TOO_LARGE:
                return httpVersion + "431 Request Headers Too Large";

            default:
                return httpVersion + "500 Internal Server Error";
        }
    }

    std::string headerToString(const std::unordered_map<std::string, std::string> &header) {
        std::string headerLines{};
        headerLines.reserve(header.size() * 50);

        for (const auto &[first, second]: header) {
            headerLines += first + ": " + second + "\r\n";
        }
        return headerLines;
    }
}

Connection::Connection(tcp::socket &&connectionSocket, const size_t connectionId, const IDispatcher &dispatcher)
    : socket_{std::move(connectionSocket)},
      connectionId_{connectionId},
      totalRequests_{},
      dispatcher_(dispatcher),
      clientIp_{socket_.remote_endpoint().address().to_string()} {
}

Connection::pointer Connection::create(tcp::socket &&connectionSocket, const size_t connectionId,
                                       const IDispatcher &dispatcher) {
    return pointer(new Connection(std::move(connectionSocket), connectionId, dispatcher));
}

Connection::~Connection() {
    Helper::displayMessage("Connection ID ({}) Disconnected", connectionId_);
}

asio::awaitable<void> Connection::handleRequest() {
    using namespace asio::experimental::awaitable_operators;
    auto self{shared_from_this()};

    asio::steady_timer timeOutTimer{co_await asio::this_coro::executor};

    try {
        bool keepAlive{};
        do {
            timeOutTimer.expires_after(std::chrono::seconds(10));
            auto result = co_await(
                processRequest()
                || timeOutTimer.async_wait(asio::use_awaitable)
            );

            if (result.index() == 1) {
                Helper::displayError("Connection ({}) timed out", connectionId_);

                co_await writeResponse(ResponseFactory::requestTimeout("Request timed out"));
                co_return;
            }
            keepAlive = std::get<0>(result);
        } while (keepAlive);
    } catch (std::exception &e) {
        Helper::displayError("error occurred in handle request : {}", e.what());
    }
}

asio::awaitable<bool> Connection::processRequest() {
    try {
        auto self{shared_from_this()};

        constexpr size_t maxHeaderSize{8192};
        constexpr size_t maxBodySize{1024 * 1024};

        bool keepAlive{};

        std::string requestReceived{};

        size_t currentHeaderSize{};

        std::array<char, 128> receivingBuffer{};

        std::optional<size_t> delimiterPosition{};
        while (!delimiterPosition.has_value()) {
            const size_t len{
                co_await socket_.async_read_some(asio::buffer(receivingBuffer),
                                                 asio::use_awaitable)
            };

            currentHeaderSize += len;

            if (currentHeaderSize > maxHeaderSize) {
                Helper::displayMessage(
                    "Header Length Received from Connection Id ({}) was too large, value received : {}", connectionId_,
                    currentHeaderSize);

                co_await writeResponse(ResponseFactory::headerTooLarge("Header sent is too large"));
                co_return false;
            }

            requestReceived.insert(requestReceived.end(), receivingBuffer.begin(),
                                   receivingBuffer.begin() + static_cast<long long>(len));

            delimiterPosition = parseRequestForDelimiter(
                std::string_view(requestReceived.data(), requestReceived.size()));
        }

        // Display Header
        // Used to remove warning of std::optional delimiterPosition
        if (!delimiterPosition) {
            co_return false;
        }
        Helper::displayMessage("Headers Received from Connection Id ({})\n{}", connectionId_,
                               std::string_view(requestReceived.data(), delimiterPosition.value()));

        const auto rawHeader = std::string(requestReceived.data(), delimiterPosition.value());

        ParseResultObject parseResult{HttpRequestParser::parseHeader(rawHeader)};

        if (!parseResult.result) {
            Helper::displayError("Connection Id {} Parse Error : {}", connectionId_, parseResult.errorMessage);
            co_await writeResponse(
                ResponseFactory::failedResponse(parseResult.httpStatus, "Parse Error : {}", parseResult.errorMessage));

            co_return false;
        }


        // Has Body
        if (parseResult.parseRequestObject.header.contains("content-length")) {
            size_t contentLength{};
            const std::string &contentLentString{parseResult.parseRequestObject.header["content-length"]};
            const auto [ptr, ec]{
                std::from_chars(contentLentString.data(), contentLentString.data() + contentLentString.size(),
                                contentLength)
            };

            if (ec != std::errc{} || ptr != contentLentString.data() + contentLentString.size()) {
                co_await writeResponse(ResponseFactory::badRequest("Invalid Content-Length: {}", contentLentString));
                co_return false;
            }


            if (contentLength > maxBodySize) {
                Helper::displayMessage(
                    "Content Length Received from Connection Id ({}) was too large, value received : {}", connectionId_,
                    contentLength);

                co_await writeResponse(ResponseFactory::contentTooLarge("Content Length too large"));

                co_return false;
            }

            std::array<char, 128> receivingBodyBuffer{};
            constexpr size_t HEADER_DELIMITER_SIZE = 4;
            while (requestReceived.size() < contentLength + rawHeader.size() + HEADER_DELIMITER_SIZE) {
                size_t len{
                    co_await socket_.async_read_some(asio::buffer(receivingBodyBuffer),
                                                     asio::use_awaitable)
                };


                requestReceived.insert(requestReceived.end(), receivingBodyBuffer.begin(),
                                       receivingBodyBuffer.begin() + static_cast<long long>(len));
            }
            Helper::displayMessage("requestReceived.size()={}, header.size()={}, delimiterPos={}, contentLength={}",
                                   requestReceived.size(), rawHeader.size(), delimiterPosition.value(),
                                   contentLength);


            parseResult.parseRequestObject.body = std::string(
                &requestReceived[delimiterPosition.value() + static_cast<size_t>(HEADER_DELIMITER_SIZE)],
                contentLength);

            Helper::displayMessage("Body Received from Connection Id ({})\n{}", connectionId_,
                                   parseResult.parseRequestObject.body);
        }

        Response response{dispatcher_.dispatch(parseResult.parseRequestObject.route, parseResult.parseRequestObject)};

        ++totalRequests_;

        if (totalRequests_ >= Constants::MAX_REQUEST_PER_CONNECTION) {
            response.header["connection"] = "close";
        }


        if (response.header.contains("connection")) {
            std::string &responseConnectionHeaderValue{response.header["connection"]};
            Helper::toLower(response.header["connection"]);
            keepAlive = responseConnectionHeaderValue != "close";
        } else {
            response.header["connection"] = "keep-alive";
            keepAlive = true;
        }

        if (keepAlive) {
            Helper::displayMessage("Connection id {} kept alive", connectionId_);
        }

        co_await writeResponse(response);


        co_return keepAlive;
    } catch (std::exception &e) {
        Helper::displayError("{}", e.what());
        co_return false;
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
