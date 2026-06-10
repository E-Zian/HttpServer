#pragma once
#ifndef USERCONTROLLER_H
#define USERCONTROLLER_H

#include "Router.h"
#include "ControllerBase.h"

class UserController :public ControllerBase {
public:
	explicit UserController(const Router& router);
private:
};

#endif 