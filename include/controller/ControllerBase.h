#pragma once
#ifndef CONTROLLERBASE_H
#define CONTROLLERBASE_H
#include "Router.h"
#include "../Interface/IRouteRegistrar.h"

class ControllerBase {
public:
	explicit ControllerBase(const IRouteRegistrar& router);

protected:
	const IRouteRegistrar& router_;
};

#endif 
