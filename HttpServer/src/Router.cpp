#include "Router.h"

void Router::addRoute(Method method, std::string_view path, std::function<void(ParsedRequestObject&, Response&)> handler) {
	routes_.emplace_back(Route(method, path, handler));
}
