#pragma once
#ifndef ROUTER_H
#define ROUTER_H

#include "Interface/IDispatcher.h"
#include "Interface/IRouteRegistrar.h"
#include "model/HttpTypes.h"
#include <memory>


class Router : public IDispatcher,public IRouteRegistrar
{
public:
	void addRoute(Method method, const std::string& path, const std::function<Response(ParsedRequestObject&)> &handler) const override;

	Response dispatch(ParsedRequestObject& request) const override;

	Router();

private:
	std::unique_ptr<Route> root_;
};




#endif 
