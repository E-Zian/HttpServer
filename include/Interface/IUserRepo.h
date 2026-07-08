#pragma once
#ifndef IUSERREPO_H
#define IUSERREPO_H

#include "model/UserModel.h"
#include <optional>
#include <vector>

class IUserRepo {
public:

	virtual std::optional<UserModel::User> createUser(const UserModel::DTO::CreateUserRequest& createUserRequest) const = 0;

	virtual std::optional<std::vector<UserModel::User>> getAllUser() const = 0;

	virtual std::optional<UserModel::User> getUserById(int id) const = 0;

	virtual std::optional<UserModel::User> updateUser(const UserModel::User& updateUserRequest) const = 0;

	virtual bool deleteUserById(int id) const = 0;

	~IUserRepo() = default;
};

#endif