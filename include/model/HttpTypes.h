#pragma once
#ifndef HTTPSERVER_HTTPTYPES_H
#define HTTPSERVER_HTTPTYPES_H

#include <string>
#include <unordered_map>
#include <functional>
#include <memory>

enum class Method {
    GET,
    POST,
    PUT,
    DEL,
    PATCH,
    HEAD,
    OPTIONS
};

struct RequestObject {
    std::string_view header;
    std::string_view body;
};

struct ParsedRequestObject {
	Method method;
	std::string route;
	std::string httpVersion;
	std::unordered_map<std::string, std::string> header{};
    std::string_view body;
    std::unordered_map<std::string, std::string> parameterValues{};

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


struct Route {
    std::string segmentName;
    std::unordered_map<Method, std::function<Response(ParsedRequestObject&)>> handler{};
    std::unordered_map<std::string, std::unique_ptr<Route>> children{};
    std::unique_ptr<Route> parameterChild{};
};

#endif //HTTPSERVER_HTTPTYPES_H