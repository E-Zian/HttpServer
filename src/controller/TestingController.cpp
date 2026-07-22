#include "controller/TestingController.h"
#include "ResponseFactory.h"

namespace {
	Response testing(ParsedRequestObject&request) {
		return ResponseFactory::dummy();
	}

	Response customTesting(ParsedRequestObject& request) {
		return ResponseFactory::customText("This is the /testing route");
	}

	Response jsonTesting(ParsedRequestObject& request) {
		return ResponseFactory::dummyJson();
	}
}

TestingController::TestingController(Router& router) : ControllerBase(router) {
	router_.addRoute(Method::GET, "/", testing);
	router_.addRoute(Method::GET, "/testing", customTesting);
	router_.addRoute(Method::GET, "/json", jsonTesting);
}

