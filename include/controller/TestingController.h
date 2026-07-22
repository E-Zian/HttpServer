#pragma once
#ifndef HTTPSERVER_TESTINGCONTROLLER_H
#define HTTPSERVER_TESTINGCONTROLLER_H

#include "Router.h"
#include "ControllerBase.h"

class TestingController :public ControllerBase{
	public:
	explicit TestingController(Router& router);
	private:
};

#endif //HTTPSERVER_TESTINGCONTROLLER_H