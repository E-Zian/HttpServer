#include "controller/UserController.h"

#include "Helper.h"
#include "ResponseFactory.h"
#include "model/PokemonModel.h"
#include <charconv>

UserController::UserController(Router &router, const IUserRepo& repo) : ControllerBase(router), repo_{repo} {
    router_.addRoute(Method::GET, "/api/user", [this](const ParsedRequestObject &request) {
        return this->getAllUsers(request);
    });

    router_.addRoute(Method::POST, "/api/user", [this](const ParsedRequestObject &request) {
        return this->createUser(request);
    });

    router_.addRoute(Method::GET, "/api/user/:id", [this](const ParsedRequestObject &request) {
        return this->getUser(request);
    });

    router_.addRoute(Method::PUT, "/api/user/:id", [this](const ParsedRequestObject &request) {
        return this->updateUser(request);
    });

    router_.addRoute(Method::DEL, "/api/user/:id", [this](const ParsedRequestObject &request) {
        return this->deleteUser(request);
    });
}

Response UserController::createUser(const ParsedRequestObject &request) const {
    try {
        Response response{ ResponseFactory::baseSuccessResponse() };

        const nlohmann::json receivedJson = nlohmann::json::parse(request.body);
        if (!receivedJson.contains("user")) {
            return ResponseFactory::failedResponse(HttpStatus::BAD_REQUEST, "No user received");
        }

        const std::optional<UserModel::User> newUser{
            repo_.createUser(receivedJson["user"].get<UserModel::DTO::CreateUserRequest>())
        };

        if (!newUser.has_value()) {
            return ResponseFactory::failedResponse(HttpStatus::BAD_REQUEST, "User email already exists");
        }
        nlohmann::json json;
        json["user"] = *newUser;
        json["message"] = "User created successfully";
        response.body = json.dump();

        response.header["content-length"] = std::to_string(response.body.length());

        return response;

    }  catch (const nlohmann::json::exception& e) {
        Helper::displayError("{}", e.what());
        return ResponseFactory::failedResponse(HttpStatus::BAD_REQUEST, "Invalid JSON body");
    }
    catch (std::exception &e) {
        Helper::displayError("{}", e.what());
        return ResponseFactory::failedResponse(HttpStatus::SERVER_ERROR, "An unexpected error has occurred");
    }
}

Response UserController::getAllUsers(const ParsedRequestObject &request) const {
    try {
        Response response{ ResponseFactory::baseSuccessResponse() };

        const std::optional<std::vector<UserModel::User> > userList{repo_.getAllUser()};

        if (!userList) {
            return ResponseFactory::failedResponse(HttpStatus::SERVER_ERROR,"An unexpected error has occurred");
        }

        nlohmann::json json;

        json["users"] = *userList;
        json["message"] = fmt::format("Total {} users retrieved", userList.value().size());
        response.body = json.dump();

        response.header["content-length"] = std::to_string(response.body.length());

        return response;

    } catch (const std::exception &e) {
        Helper::displayError("{}", e.what());
        return ResponseFactory::failedResponse(HttpStatus::SERVER_ERROR, "An unexpected error has occurred");
    }
}

Response UserController::getUser(const ParsedRequestObject &request) const {
    try {
        Response response{ ResponseFactory::baseSuccessResponse() };

        const std::string& idString{ request.parameterValues.at(":id") };
        int id{};
        const auto [ptr, ec] {
            std::from_chars(idString.data(), idString.data() + idString.size(),
                id)
            };

        if (ec != std::errc{} || ptr != idString.data() + idString.size()) {
            return ResponseFactory::failedResponse(HttpStatus::BAD_REQUEST, "Invalid id data type: {}", std::make_error_code(ec).message());
        }

        const std::optional<UserModel::User> user{repo_.getUserById(id)};

        if (!user) {
            return ResponseFactory::failedResponse(HttpStatus::NOT_FOUND, "User id {} does not exists",id);
        }

        nlohmann::json json;
        json["user"] = *user;
        json["message"] = "User retrieved successfully";

        response.body = json.dump();

        response.header["content-length"] = std::to_string(response.body.length());

        return response;
    } catch (const std::invalid_argument &e) {
        Helper::displayError("{}", e.what());

        return ResponseFactory::failedResponse(HttpStatus::BAD_REQUEST, "Invalid user id");
    } catch (const std::out_of_range &e) {
        Helper::displayError("{}", e.what());

        return ResponseFactory::failedResponse(HttpStatus::BAD_REQUEST, "Invalid user id");
    }
    catch (const std::exception &e) {
        Helper::displayError("{}", e.what());
        return ResponseFactory::failedResponse(HttpStatus::SERVER_ERROR, "An unexpected error has occurred");
    }
}

Response UserController::updateUser(const ParsedRequestObject &request) const {
    try {
        Response response{ ResponseFactory::baseSuccessResponse() };

        const std::string& idString{ request.parameterValues.at(":id") };
        int id{};
        const auto [ptr, ec] {
            std::from_chars(idString.data(), idString.data() + idString.size(),
                id)
            };

        if (ec != std::errc{} || ptr != idString.data() + idString.size()) {
            return ResponseFactory::failedResponse(HttpStatus::BAD_REQUEST, "Invalid id data type: {}", std::make_error_code(ec).message());
        }


        const nlohmann::json receivedJson = nlohmann::json::parse(request.body);

        if (!receivedJson.contains("user")) {
            return ResponseFactory::failedResponse(HttpStatus::BAD_REQUEST, "Invalid user id");
        }

        const UserModel::User updateUserRequest{
            id, receivedJson.at("user").get<UserModel::DTO::UpdateUserRequest>().name,
            receivedJson.at("user").get<UserModel::DTO::UpdateUserRequest>().email
        };

        const std::optional<UserModel::User> updatedUser{repo_.updateUser(updateUserRequest)};

        if (!updatedUser) {
            return ResponseFactory::failedResponse(HttpStatus::NOT_FOUND, "User id {} does not exists", id);
        }

        nlohmann::json json;
        json["user"] = *updatedUser;
        json["message"] = "User updated successfully";
        response.body = json.dump();

        response.header["content-length"] = std::to_string(response.body.length());

        return response;
    } catch (const nlohmann::json::exception& e) {
        Helper::displayError("{}", e.what());
        return ResponseFactory::failedResponse(HttpStatus::BAD_REQUEST, "Invalid JSON body");
    }catch (const std::invalid_argument &e) {
        Helper::displayError("{}", e.what());

        return ResponseFactory::failedResponse(HttpStatus::BAD_REQUEST, "Invalid user id");
    } catch (const std::out_of_range &e) {
        Helper::displayError("{}", e.what());

        return ResponseFactory::failedResponse(HttpStatus::BAD_REQUEST, "Invalid user id");
    }  catch (const std::exception &e) {
        Helper::displayError("{}", e.what());
        return ResponseFactory::failedResponse(HttpStatus::SERVER_ERROR, "An unexpected error has occurred");
    }
}

Response UserController::deleteUser(const ParsedRequestObject &request) const {
    try {
        Response response{ ResponseFactory::baseSuccessResponse() };

        const std::string& idString{ request.parameterValues.at(":id") };
        int id{};
        const auto [ptr, ec] {
            std::from_chars(idString.data(), idString.data() + idString.size(),
                id)
            };

        if (ec != std::errc{} || ptr != idString.data() + idString.size()) {
            return ResponseFactory::failedResponse(HttpStatus::BAD_REQUEST, "Invalid id data type: {}", std::make_error_code(ec).message());
        }


        if (!repo_.deleteUserById(id)) {
            return ResponseFactory::failedResponse(HttpStatus::NOT_FOUND, "User id {} does not exists", id);
        }

        nlohmann::json json;

        json["message"] = fmt::format("User id {} deleted successfully", id);
        response.body = json.dump();

        response.header["content-length"] = std::to_string(response.body.length());

        return response;
    } catch (const std::invalid_argument &) {
        return ResponseFactory::failedResponse(HttpStatus::BAD_REQUEST, "Invalid user id");
    } catch (const std::out_of_range &) {
        return ResponseFactory::failedResponse(HttpStatus::BAD_REQUEST, "Invalid user id");
    } catch (const std::exception &e) {
        Helper::displayError("{}", e.what());
        return ResponseFactory::failedResponse(HttpStatus::SERVER_ERROR, "An unexpected error had occurred in the server");
    }
}
