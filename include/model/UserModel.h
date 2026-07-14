#pragma once
#ifndef UserModel_H
#define UserModel_H

#include <string>
#include <nlohmann/json.hpp>
namespace UserModel {

	namespace DTO {
		struct CreateUserRequest {
			std::string name;
			std::string email;
		};
		inline void to_json( nlohmann::json& j, const CreateUserRequest& req) {
			j = nlohmann::json{{"name", req.name}, {"email", req.email}};
		}
		inline void from_json( const nlohmann::json& j, CreateUserRequest& req) {
			j.at("name").get_to(req.name);
			j.at("email").get_to(req.email);
		}

		struct UpdateUserRequest {
			std::string name;
			std::string email;
		};
		inline void to_json( nlohmann::json& j, const UpdateUserRequest& req) {
			j = nlohmann::json{{"name", req.name}, {"email", req.email}};
		}
		inline void from_json( const nlohmann::json& j, UpdateUserRequest& req) {
			j.at("name").get_to(req.name);
			j.at("email").get_to(req.email);
		}
	}
	struct User {
		int id;
		std::string name;
		std::string email;
	};
	inline void to_json( nlohmann::json& j, const User& user) {
		j = nlohmann::json{{"id", user.id}, {"name", user.name},{"email",user.email}};

	}
	inline void from_json( const nlohmann::json& j, User& user) {
		j.at("id").get_to(user.id);
		j.at("name").get_to(user.name);
		j.at("email").get_to(user.email);
	}

	namespace Error {
		class UserAlreadyExistsException : public std::runtime_error {
			public:
			explicit UserAlreadyExistsException(const std::string& name) : runtime_error("User '" + name + "' already exists") {};

		};
		class UserNotFoundException : public std::runtime_error {
			public:
			explicit UserNotFoundException(const int id):runtime_error("User id '"+std::to_string(id) +"' not found"){};
		};
	}
};

#endif