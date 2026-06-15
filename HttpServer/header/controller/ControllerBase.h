#pragma once
#ifndef CONTROLLERBASE_H
#define CONTROLLERBASE_H
#include "Router.h"

class ControllerBase {
public:
	explicit ControllerBase(const Router& router);

protected:
	const Router& router_;
};

#endif 
