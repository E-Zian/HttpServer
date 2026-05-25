#pragma once
#ifndef HTTPSERVER_HTTPTYPES_H
#define HTTPSERVER_HTTPTYPES_H
#include <string>

struct ParsedRequest {
    std::string_view Header;
    std::string_view Body;
};

#endif //HTTPSERVER_HTTPTYPES_H