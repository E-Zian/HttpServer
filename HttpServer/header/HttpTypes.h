#pragma once
#ifndef HTTPSERVER_HTTPTYPES_H
#define HTTPSERVER_HTTPTYPES_H
#include <string>
#include <unordered_map>

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

#endif //HTTPSERVER_HTTPTYPES_H