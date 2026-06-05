#pragma once
#ifndef ROUTER_H
#define ROUTER_H

#include "HttpTypes.h"
#include <memory>

class Router
{
public:
	void addRoute(Method method, const std::string& path, const std::function<void(ParsedRequestObject&, Response&)> &handler);

	Router();

private:
	std::unique_ptr<Route> root_;
};




#endif 
