#include "controller/TestingController.h"
namespace {
	Response testing(ParsedRequestObject&request) {


		return Response();
	}
}

TestingController::TestingController(const Router& router) : ControllerBase(router) {
	router_.addRoute(Method::GET, "/", testing);
}

