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

	std::optional<User> createUser(const User &newUser) const;

	std::optional<std::vector<User>> getAllUser() const;

	std::optional<User> getUserById(int id) const;

	std::optional<User> updateUserById(int id, const User &newUser) const;

	bool deleteUserById(int id) const;

private:
	SQLite::Database& db_;


};


#endif