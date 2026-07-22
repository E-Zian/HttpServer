#pragma once
#ifndef CONTROLLERBASE_H
#define CONTROLLERBASE_H
#include "Router.h"
#include "../Interface/IRouteRegistrar.h"

class ControllerBase {
public:
	explicit ControllerBase(IRouteRegistrar& router);

protected:
	IRouteRegistrar& router_;
};

#endif 
