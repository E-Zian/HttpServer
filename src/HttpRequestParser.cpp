#include "HttpRequestParser.h"
#include "Helper.h"
#include <optional>
#include <ranges>
#include <string>

namespace {
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
        const std::string headerTitle{headerLine.substr(0, delimiterPosition)};
        return std::make_pair(Helper::toLower(headerTitle), Helper::trim(headerLine.substr(delimiterPosition + 1)));
    }

    std::optional<std::vector<std::string_view> > parseRequestLine(std::string_view requestLine) {
        std::vector<std::string_view> requestLineComponents{};

        size_t start{};
        while (start < requestLine.length()) {
            if (const size_t end{requestLine.find(' ', start)}; end == std::string::npos) {
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

    const std::unordered_map<std::string_view, Method> methodMap = {
        {"GET", Method::GET},
        {"POST", Method::POST},
        {"PUT", Method::PUT},
        {"DELETE", Method::DEL},
        {"PATCH", Method::PATCH},
        {"HEAD", Method::HEAD},
        {"OPTIONS", Method::OPTIONS},
    };

    struct ParsedRouteResult {
        std::unordered_map<std::string, std::string> queryStrings{};
        std::string route;
    };

    ParsedRouteResult parseRoute(std::string_view route) {
        ParsedRouteResult parsedRouteResult{};

        if (const size_t queryStringDelimiterPos{route.find('?')}; queryStringDelimiterPos != std::string_view::npos) {
            std::string_view queryStringSection{route.substr(queryStringDelimiterPos + 1)};

            if (const size_t queryStringEndPos{queryStringSection.find('#')}; queryStringEndPos != std::string_view::npos) {
                queryStringSection = queryStringSection.substr(0, queryStringEndPos);
            }

            for (std::vector<std::string> queryStringComponents{Helper::split(std::string(queryStringSection), '&')};
                 auto &component: queryStringComponents) {

                //change here need find a way to check if its a flag or equal
                const size_t delimiterPos{component.find('=')};
                if (delimiterPos == std::string::npos) {
                    // fuck la need valueless for flags
                    throw std::invalid_argument("Malformed query string");
                }
                std::string key{component.substr(0, delimiterPos)};
                std::string value{component.substr(delimiterPos + 1)};

                parsedRouteResult.queryStrings[std::move(key)] = std::move(value);
            }

            parsedRouteResult.route = std::string(route.substr(0, queryStringDelimiterPos));
        } else {
            parsedRouteResult.route = std::string{route};
        }
        return parsedRouteResult;
    }
}

ParseResultObject HttpRequestParser::parseHeader(const std::string_view rawHeader) {
    ParseResultObject parseResult{};
    size_t positionIterator{};

    const std::string_view requestLine{getHeaderLine(rawHeader, positionIterator)};

    const std::optional<std::vector<std::string_view> > requestLineComponents{parseRequestLine(requestLine)};

    if (!requestLineComponents.has_value()) {
        parseResult.httpStatus = HttpStatus::BAD_REQUEST;
        parseResult.errorMessage = "Malformed request line received on " + std::string(requestLine);
        parseResult.result = false;

        return parseResult;
    }

    const auto methodIt{methodMap.find(requestLineComponents.value()[0])};

    if (methodIt == methodMap.end()) {
        parseResult.httpStatus = HttpStatus::BAD_REQUEST;
        parseResult.errorMessage = "Unsupported Method " + std::string(requestLineComponents.value()[0]);
        parseResult.result = false;

        return parseResult;
    }

    parseResult.parseRequestObject.method = methodIt->second;
    parseResult.parseRequestObject.route = std::string(requestLineComponents.value()[1]);
    parseResult.parseRequestObject.httpVersion = std::string(requestLineComponents.value()[2]);


    while (positionIterator != rawHeader.length()) {
        std::string headerLine{getHeaderLine(rawHeader, positionIterator)};

        std::optional<std::pair<std::string, std::string> > headerPair{parseHeaderLine(headerLine)};
        if (!headerPair.has_value()) {
            parseResult.httpStatus = HttpStatus::BAD_REQUEST;
            parseResult.errorMessage = "Malformed header line received on " + std::string(headerLine);
            parseResult.result = false;

            return parseResult;
        }

        parseResult.parseRequestObject.header[headerPair.value().first] = headerPair.value().second;
    }
    parseResult.result = true;
    return parseResult;
}
