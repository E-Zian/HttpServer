#pragma once
#ifndef ROUTER_H
#define ROUTER_H

#include "HttpTypes.h"
#include <vector>

class Router
{
public:
	void addRoute(Method method, const std::string& path, const std::function<void(ParsedRequestObject&, Response&)> &handler);

private:
	std::vector<Route> routes_;

};




#endif 
