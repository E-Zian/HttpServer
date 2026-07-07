#include "Router.h"
#include "Helper.h"
#include "ResponseFactory.h"
#include <vector>

namespace {
	const std::unordered_map<Method, std::string_view> methodToString = {
	{ Method::GET,		"GET"},
	{ Method::POST,		"POST"},
	{Method::PUT,		"PUT"},
	{ Method::DEL,		"DELETE"},
	{Method::PATCH,		"PATCH"},
	{Method::HEAD,		"HEAD"},
	{Method::OPTIONS,	"OPTIONS"}
	};
}

Router::Router() : root_(std::make_unique<Route>()) {};

void Router::addRoute(const Method method, const std::string& path, const std::function<Response(ParsedRequestObject&)>& handler) const {

	const std::vector<std::string> pathSegments{ Helper::split(path, '/') };

	Route* currentRoute = root_.get();

	for (const auto& pathSegment : pathSegments) {
		if (pathSegment.size() != 0 && pathSegment[0] == ':') {
			if (currentRoute->parameterChild && currentRoute->parameterChild->segmentName != pathSegment) {
				throw std::invalid_argument("Param conflict at: " + pathSegment + " in added route : " + path);
			}
			if (!currentRoute->parameterChild) {
				currentRoute->parameterChild = std::make_unique<Route>(pathSegment);
			}
			currentRoute = currentRoute->parameterChild.get();
		}
		else {
			const auto& child{ currentRoute->children.emplace(pathSegment, std::make_unique<Route>(pathSegment)).first };
			currentRoute = child->second.get();
		}
	}

	currentRoute->handler[method] = handler;

}

Response Router::dispatch(const std::string& route, ParsedRequestObject& request) const {

	const std::vector<std::string> pathSegments{ Helper::split(route,'/') };
	Route* currentRoute = root_.get();

	for (const auto& pathSegment : pathSegments) {
		if (currentRoute->children.contains(pathSegment)) {
			currentRoute = currentRoute->children[pathSegment].get();
		}
		else if (currentRoute->parameterChild) {
			currentRoute = currentRoute->parameterChild.get();
			request.parameterValues[currentRoute->segmentName] = pathSegment;
		}
		else {
			return ResponseFactory::badRequest("Could not find route {} ", route);
		}
	}
	if (!currentRoute->handler.contains(request.method)) {
		return ResponseFactory::badRequest("Method {} does not exist for route {}", methodToString.at(request.method), route);
	}
	return currentRoute->handler[request.method](request);

}
