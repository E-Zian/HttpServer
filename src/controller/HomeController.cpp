//
// Created by User on 6/20/2026.
//

#include "controller/HomeController.h"

#include "ResponseFactory.h"

namespace  {
}

HomeController::HomeController(Router &router):ControllerBase(router) {
    router_.addRoute(Method::GET,"/favicon.ico",[this](ParsedRequestObject& request) {
        return ResponseFactory::dummy();
    });
}
