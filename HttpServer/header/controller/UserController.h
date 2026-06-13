#pragma once
#ifndef USERCONTROLLER_H
#define USERCONTROLLER_H

#include "Router.h"
#include "ControllerBase.h"
#include "repository/UserRepo.h"

class UserController :public ControllerBase {
public:
	UserController(const Router& router,const UserRepo& repo);
private:
	const UserRepo& repo_;
};

#endif 