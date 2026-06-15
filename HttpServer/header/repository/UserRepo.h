#pragma
#ifndef USERREPO_H
#define USERREPO_H

#include "database/Database.h"
#include "model/UserModel.h"
#include <vector>

class UserRepo
{
	using User = UserModel::User;

public:
	explicit UserRepo(SQLite::Database& db) :db_{ db } {};

	[[nodiscard]] std::optional<User> createUser(const UserModel::DTO::CreateUserRequest &createUserRequest) const;

	[[nodiscard]] std::optional<std::vector<User>> getAllUser() const;

	[[nodiscard]] std::optional<User> getUserById(int id) const;

	[[nodiscard]] std::optional<User> updateUser(const User &updateUserRequest) const;

	[[nodiscard]] bool deleteUserById(int id) const;

private:
	SQLite::Database& db_;


};


#endif