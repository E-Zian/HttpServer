#pragma
#ifndef UserModel_H
#define UserModel_H

#include <string>
namespace UserModel {
	struct User {
		int id;
		std::string name;
		std::string email;
	};
};

#endif