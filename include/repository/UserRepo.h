#pragma once
#ifndef USERREPO_H
#define USERREPO_H

#include "../Interface/IUserRepo.h"
#include "database/database.h"
#include "model/UserModel.h"
#include <vector>
#include <mutex>

class UserRepo : public IUserRepo
{
	using User = UserModel::User;

public:
	UserRepo(DataBase& db) :db_{ db.getDb() }, sharedMutex_{ db.getMutex()} {};

	[[nodiscard]] std::optional<User> createUser(const UserModel::DTO::CreateUserRequest &createUserRequest) const override;

	[[nodiscard]] std::optional<std::vector<User>> getAllUser() const override;

	[[nodiscard]] std::optional<User> getUserById(int id) const override;

	[[nodiscard]] std::optional<User> updateUser(const User &updateUserRequest) const override;

	[[nodiscard]] bool deleteUserById(int id) const override;

private:
	SQLite::Database& db_;
	std::mutex& sharedMutex_;

};


#endif