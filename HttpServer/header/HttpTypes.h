#pragma once
#ifndef HTTPSERVER_HTTPTYPES_H
#define HTTPSERVER_HTTPTYPES_H
#include <string>
#include <unordered_map>
#include <functional>

struct RequestObject {
    std::string_view Header;
    std::string_view Body;
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
    HttpStatus statusLine;
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
    Method method;
    std::vector<std::string> pathSegments;
    std::function<void(ParsedRequestObject&, Response&)> handler;
};

#endif //HTTPSERVER_HTTPTYPES_H