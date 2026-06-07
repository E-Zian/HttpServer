#pragma once
#ifndef HTTPSERVER_HTTPTYPES_H
#define HTTPSERVER_HTTPTYPES_H
#include <string>
#include <unordered_map>
#include <functional>
#include <memory>

struct RequestObject {
    std::string_view header;
    std::string_view body;
};

struct ParsedRequestObject {
	std::string_view method;
	std::string_view route;
	std::string_view httpVersion;
	std::unordered_map<std::string, std::string> header{};
    std::string_view body;

};

enum class HttpStatus {
    OK = 200,
    BAD_REQUEST = 400,
    NOT_FOUND = 404,
    SERVER_ERROR = 500
};

struct Response {
    HttpStatus status;
    std::unordered_map<std::string, std::string> header{};
    std::string body;
};

enum class Method {
    GET,
    POST,
    PUT,
    DELETE,
    PATCH,
    HEAD,
    OPTIONS
};

struct Route {
    std::string segmentName;
    std::unordered_map<Method, std::function<void(ParsedRequestObject&, Response&)>> handler{};
    std::unordered_map<std::string, std::unique_ptr<Route>> children{};
    std::unique_ptr<Route> parameterChild{};
};

#endif //HTTPSERVER_HTTPTYPES_H