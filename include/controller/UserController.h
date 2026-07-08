#pragma once
#ifndef USERCONTROLLER_H
#define USERCONTROLLER_H

#include "Router.h"
#include "ControllerBase.h"
#include "repository/UserRepo.h"

class UserController :public ControllerBase {
public:
	UserController(const Router& router,const IUserRepo& repo);
private:
	const IUserRepo& repo_;

	[[nodiscard]] Response createUser(const ParsedRequestObject& request) const;

	[[nodiscard]] Response getAllUsers(const ParsedRequestObject& request) const;

	[[nodiscard]] Response getUser(const ParsedRequestObject& request) const;

	[[nodiscard]] Response updateUser(const ParsedRequestObject& request) const;

	[[nodiscard]] Response deleteUser(const ParsedRequestObject& request) const;
};

#endif 