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
	UserRepo(const DataBase& db) :db_{ db } {};

	std::optional<User> createUser(User newUser);

	std::vector<User> getAllUser();

	std::optional<User> getUserById(int id);

	std::optional<User> updateUserById(int id,User newUser);

	bool deleteUserById(int id);

private:
	const DataBase& db_;
	
};


#endif