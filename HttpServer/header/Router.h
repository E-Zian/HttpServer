#pragma once
#ifndef ROUTER_H
#define ROUTER_H

#include "HttpTypes.h"
#include <memory>

#include "Interface/IDispatcher.h"

class Router : public IDispatcher
{
public:
	void addRoute(Method method, const std::string& path, const std::function<Response(ParsedRequestObject&)> &handler) const;

	Response dispatch(const std::string& route,ParsedRequestObject& request) const override;

	Router();

private:
	std::unique_ptr<Route> root_;
};




#endif 
