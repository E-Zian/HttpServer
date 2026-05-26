#pragma once
#ifndef HTTPSERVER_HTTPTYPES_H
#define HTTPSERVER_HTTPTYPES_H
#include <string>

struct RequestObject {
    std::string_view Header;
    std::string_view Body;
};

enum class HttpStatus {
    OK = 200,
    BAD_REQUEST = 400,
    NOT_FOUND = 404,
    SERVER_ERROR = 500
};

#endif //HTTPSERVER_HTTPTYPES_H