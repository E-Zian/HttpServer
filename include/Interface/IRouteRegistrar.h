#pragma once
#ifndef IROUTEREGISTRAR_H
#define IROUTEREGISTRAR_H

#include "model/HttpTypes.h"

class IRouteRegistrar {
public:
    virtual void addRoute(Method method, const std::string& path, const std::function<Response(ParsedRequestObject&)>& handler) = 0;
    virtual ~IRouteRegistrar() = default;
};

#endif