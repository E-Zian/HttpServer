#pragma once
#ifndef HTTPSERVER_IDISPATCHER_H
#define HTTPSERVER_IDISPATCHER_H

#include "model/HttpTypes.h"
#include <string>

class IDispatcher {
public:
    virtual Response dispatch(const std::string& route,ParsedRequestObject& request) const = 0;
    virtual ~IDispatcher() = default;
};

#endif //HTTPSERVER_IDISPATCHER_H