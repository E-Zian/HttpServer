#include "Router.h"
#include "Helper.h"

void Router::addRoute(Method method, std::string& path, std::function<void(ParsedRequestObject&, Response&)> handler) {
	
	routes_.emplace_back(Route(method, Helper::split(path,'/'), handler));
}
