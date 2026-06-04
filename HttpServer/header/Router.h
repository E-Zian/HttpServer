#pragma once
#ifndef ROUTER_H
#define ROUTER_H

#include "HttpTypes.h"
#include <vector>

class Router
{
public:
	void addRoute(Method method, std::string_view path, std::function<void(ParsedRequestObject&, Response&)> handler);

private:
	std::vector<Route> routes_;

};




#endif 
