#include "controller/TestingController.h"
#include "ResponseFactory.h"

namespace {
	Response testing(ParsedRequestObject&request) {
		return ResponseFactory::dummy();
	}

	Response customTesting(ParsedRequestObject& request) {
		return ResponseFactory::customText("This is the /testing route");
	}
}

TestingController::TestingController(const Router& router) : ControllerBase(router) {
	router_.addRoute(Method::GET, "/", testing);
	router_.addRoute(Method::GET, "/testing", customTesting);
}

