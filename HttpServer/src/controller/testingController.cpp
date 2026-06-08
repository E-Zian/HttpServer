#include "controller/TestingController.h"
#include "ResponseFactory.h"

namespace {
	Response testing(ParsedRequestObject&request) {
		return ResponseFactory::dummy();
	}
}

TestingController::TestingController(const Router& router) : ControllerBase(router) {
	router_.addRoute(Method::GET, "/", testing);
}

