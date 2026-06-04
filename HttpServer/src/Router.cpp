#include "Router.h"
#include "Helper.h"

void Router::addRoute(const Method method, const std::string& path, const std::function<void(ParsedRequestObject&, Response&)> &handler) {
	
	routes_.emplace_back(Route(method, Helper::split(path,'/'), handler));
}
