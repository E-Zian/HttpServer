#include "controller/UserController.h"

#include "Helper.h"
#include "ResponseFactory.h"
#include "model/PokemonModel.h"

UserController::UserController(const Router &router, const UserRepo &repo) : ControllerBase(router), repo_{repo} {
    router_.addRoute(Method::GET, "/api/user/getAll", [this](const ParsedRequestObject &request) {
        return this->getAllUsers(request);
    });

    router_.addRoute(Method::POST, "/api/user/create", [this](const ParsedRequestObject &request) {
        return this->createUser(request);
    });

    router_.addRoute(Method::GET, "/api/user/get/:id", [this](const ParsedRequestObject &request) {
        return this->getUser(request);
    });

    router_.addRoute(Method::GET, "/api/user/update/:id", [this](const ParsedRequestObject &request) {
        return this->updateUser(request);
    });

    router_.addRoute(Method::GET, "/api/user/delete/:id", [this](const ParsedRequestObject &request) {
        return this->deleteUser(request);
    });
}

Response UserController::createUser(const ParsedRequestObject &request) const {
    try {
        Response response;

        const nlohmann::json receivedJson{nlohmann::json::parse(request.body)};
        if (!receivedJson.contains("user")) {
            return ResponseFactory::badRequest("No user received");
        }

        const std::optional<UserModel::User> newUser{
            repo_.createUser(receivedJson["user"].get<UserModel::DTO::CreateUserRequest>())
        };

        if (!newUser.has_value()) {
            return ResponseFactory::serverError("An unexpected error had occurred in the server");
        }
        nlohmann::json json;
        json["user"] = *newUser;
        json["message"] = "User created successfully";
        response.body = json.dump();

        response.status = HttpStatus::OK;
        response.header["content-type"] = "application/json";
        response.header["content-length"] = std::to_string(response.body.length());
        response.header["connection"] = "close";

        return response;
    } catch (const UserModel::Error::UserAlreadyExistsException &e) {
        Helper::displayError("{}", e.what());
        return ResponseFactory::badRequest("{}", e.what());
    } catch (std::exception &e) {
        Helper::displayError("{}", e.what());
        return ResponseFactory::serverError("An unexpected error has occurred");
    }
}

Response UserController::getAllUsers(const ParsedRequestObject &request) const {
    try {
        Response response;

        const std::optional<std::vector<UserModel::User> > userList{repo_.getAllUser()};

        if (!userList) {
            return ResponseFactory::serverError("An unexpected error has occurred");
        }

        nlohmann::json json;

        json["users"] = *userList;
        json["message"] = fmt::format("Total {} users retrieved", userList.value().size());
        response.body = json.dump();

        response.status = HttpStatus::OK;
        response.header["content-type"] = "application/json";
        response.header["content-length"] = std::to_string(response.body.length());
        response.header["connection"] = "close";

        return response;
    } catch (const std::exception &e) {
        Helper::displayError("{}", e.what());
        return ResponseFactory::serverError("An unexpected error has occurred");
    }
}

Response UserController::getUser(const ParsedRequestObject &request) const {
    try {
        Response response;
        const int id{std::stoi(request.parameterValues.at(":id"))};

        const std::optional<UserModel::User> user{repo_.getUserById(id)};

        if (!user) {
            return ResponseFactory::serverError("An unexpected error has occurred");
        }

        nlohmann::json json;
        json["user"] = *user;
        json["message"] = "User retrieved successfully";

        response.body = json.dump();

        response.status = HttpStatus::OK;
        response.header["content-type"] = "application/json";
        response.header["content-length"] = std::to_string(response.body.length());
        response.header["connection"] = "close";

        return response;
    } catch (const std::invalid_argument &e) {
        Helper::displayError("{}", e.what());

        return ResponseFactory::badRequest("Invalid user id");
    } catch (const std::out_of_range &e) {
        Helper::displayError("{}", e.what());

        return ResponseFactory::badRequest("Invalid user id");
    }catch (const UserModel::Error::UserNotFoundException& e) {
        Helper::displayError("{}", e.what());
        return ResponseFactory::badRequest("{}", e.what());

    }
    catch (const std::exception &e) {
        Helper::displayError("{}", e.what());
        return ResponseFactory::serverError("An unexpected error has occurred");
    }
}

Response UserController::updateUser(const ParsedRequestObject &request) const {
    try {
        Response response;
        const int id{std::stoi(request.parameterValues.at(":id"))};

        const nlohmann::json receivedJson{nlohmann::json::parse(request.body)};

        if (!receivedJson.contains("user")) {
            return ResponseFactory::badRequest("Invalid user id");
        }

        const UserModel::User updateUserRequest{
            id, receivedJson.at("user").get<UserModel::DTO::UpdateUserRequest>().name,
            receivedJson.at("user").get<UserModel::DTO::UpdateUserRequest>().email
        };

        const std::optional<UserModel::User> updatedUser{repo_.updateUser(updateUserRequest)};

        if (!updatedUser) {
            return ResponseFactory::serverError("An unexpected error has occurred");
        }
        nlohmann::json json;
        json["user"] = *updatedUser;
        json["message"] = "User updated successfully";
        response.body = json.dump();

        response.status = HttpStatus::OK;
        response.header["content-type"] = "application/json";
        response.header["content-length"] = std::to_string(response.body.length());
        response.header["connection"] = "close";

        return response;
    } catch (const std::invalid_argument &e) {
        Helper::displayError("{}", e.what());

        return ResponseFactory::badRequest("Invalid user id");
    } catch (const std::out_of_range &e) {
        Helper::displayError("{}", e.what());

        return ResponseFactory::badRequest("Invalid user id");
    } catch (const UserModel::Error::UserNotFoundException &e) {
        Helper::displayError("{}", e.what());

        return ResponseFactory::badRequest("User id '{}' not found", e.what());
    } catch (const std::exception &e) {
        Helper::displayError("{}", e.what());
        return ResponseFactory::serverError("An unexpected error has occurred");
    }
}

Response UserController::deleteUser(const ParsedRequestObject &request) const {
    try {
        Response response;
        const int id{std::stoi(request.parameterValues.at(":id"))};

        if (!repo_.deleteUserById(id)) {
            return ResponseFactory::badRequest("Invalid user id");
        }

        nlohmann::json json;

        json["message"] = fmt::format("User id {} deleted successfully", id);
        response.body = json.dump();

        response.status = HttpStatus::OK;
        response.header["content-type"] = "application/json";
        response.header["content-length"] = std::to_string(response.body.length());
        response.header["connection"] = "close";

        return response;
    } catch (const std::invalid_argument &) {
        return ResponseFactory::badRequest("Invalid pokemon id");
    } catch (const std::out_of_range &) {
        return ResponseFactory::badRequest("Invalid pokemon id");
    } catch (const std::exception &e) {
        Helper::displayError("{}", e.what());
        return ResponseFactory::serverError("An unexpected error had occurred in the server");
    }
}
