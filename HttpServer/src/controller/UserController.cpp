#include "controller/UserController.h"

UserController::UserController(const Router& router,const UserRepo& repo) :ControllerBase(router),repo_{repo} {

}