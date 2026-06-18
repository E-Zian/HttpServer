#include "controller/PokemonController.h"
#include "ResponseFactory.h"

#include "Helper.h"


PokemonController::PokemonController(const Router &router, const PokemonRepo &repo) : ControllerBase(router),
    repo_(repo) {
    router_.addRoute(Method::GET, "/api/pokemon/getAll", [this](const ParsedRequestObject &req) {
        return this->getAllPokemon(req);
    });

    router_.addRoute(Method::POST, "/api/pokemon/create", [this](const ParsedRequestObject &req) {
        return this->createPokemon(req);
    });

    router_.addRoute(Method::GET, "/api/pokemon/get/:id", [this](const ParsedRequestObject &req) {
        return this->getPokemon(req);
    });

    router_.addRoute(Method::POST, "/api/pokemon/update/:id", [this](const ParsedRequestObject &req) {
        return this->updatePokemon(req);
    });

    router_.addRoute(Method::DEL, "/api/pokemon/delete/:id", [this](const ParsedRequestObject &req) {
    return this->deletePokemon(req);
});
}

Response PokemonController::getAllPokemon(const ParsedRequestObject &request) const {
    try {
        Response response{HttpStatus::OK, {}, {}};

        nlohmann::json json;
        const std::optional<std::vector<PokemonModel::Pokemon> > pokemons{repo_.getAllPokemon()};

        if (!pokemons) {
            return ResponseFactory::serverError("An unexpected error had occurred in the server");;
        }

        json["pokemons"] = *pokemons;
        json["message"] = fmt::format("Total Pokemons Retrieved {}", pokemons->size());

        response.body = json.dump();
        response.header["Content-Type"] = "application/json";
        response.header["Content-Length"] = std::to_string(response.body.size());

        return response;
    } catch (const std::exception &e) {
        Helper::displayError("{}", e.what());
        return ResponseFactory::serverError("An unexpected error had occurred in the server");
    }
}

Response PokemonController::createPokemon(const ParsedRequestObject &request) const {
    try {
        Response response{HttpStatus::OK, {}, {}};

        nlohmann::json receivedJson{nlohmann::json::parse(request.body)};

        if (!receivedJson.contains("pokemon")) {
            return ResponseFactory::badRequest("No pokemon was passed");
        }

        const std::optional<PokemonModel::Pokemon> newPokemon{
            repo_.createPokemon(receivedJson["pokemon"].get<PokemonModel::DTO::CreatePokemonRequest>())
        };

        if (!newPokemon) {
            return ResponseFactory::serverError("An unexpected error had occurred in the server");
        }

        nlohmann::json json;
        json["pokemon"] = *newPokemon;
        json["message"] = "Pokemon created successfully";

        response.body = json.dump();
        response.header["Content-Type"] = "application/json";
        response.header["Content-Length"] = std::to_string(response.body.size());

        return response;
    } catch (const nlohmann::json::parse_error &e) {
        Helper::displayError("{}", e.what());
        return ResponseFactory::badRequest("Invalid JSON");

    } catch (const nlohmann::json::exception &e) {
        Helper::displayError("{}", e.what());
        return ResponseFactory::badRequest("Invalid pokemon data");

    }catch (const PokemonModel::Error::PokemonAlreadyExistsException& e) {
        Helper::displayError("{}", e.what());
        return ResponseFactory::badRequest("{}", e.what());

    }catch (const std::exception &e) {
        Helper::displayError("{}", e.what());
        return ResponseFactory::serverError("An unexpected error had occurred in the server");
    }
}

Response PokemonController::getPokemon(const ParsedRequestObject &request) const {
    try {
        Response response{HttpStatus::OK, {}, {}};

        if (!request.parameterValues.contains(":id")) {
            return ResponseFactory::badRequest("No pokemon id was passed");
        }
        int id{std::stoi(request.parameterValues.at(":id"))};

        const std::optional<PokemonModel::Pokemon> pokemon{repo_.getPokemonById(id)};

        if (!pokemon) {
            return ResponseFactory::notFound("Pokemon id {} not found", id);
        }

        nlohmann::json json;
        json["pokemon"] = *pokemon;
        json["message"] = "Pokemon found successfully";
        response.body = json.dump();
        response.header["Content-Type"] = "application/json";
        response.header["Content-Length"] = std::to_string(response.body.size());

        return response;

    } catch (const std::invalid_argument& e) {
        Helper::displayError("{}", e.what());

        return ResponseFactory::badRequest("Invalid pokemon id");

    } catch (const std::out_of_range& e) {
        Helper::displayError("{}", e.what());

        return ResponseFactory::badRequest("Invalid pokemon id");

    }catch (const std::exception &e) {
        Helper::displayError("{}", e.what());
        return ResponseFactory::serverError("An unexpected error had occurred in the server");
    }
}

Response PokemonController::updatePokemon(const ParsedRequestObject &request) const {
    try {
        Response response{HttpStatus::OK, {}, {}};

        if (!request.parameterValues.contains(":id")) {
            return ResponseFactory::badRequest("No pokemon id was passed");
        }
        const int id{std::stoi(request.parameterValues.at(":id"))};
        const nlohmann::json receivedJson{nlohmann::json::parse(request.body)};

        if (!receivedJson.contains("pokemon")) {
            return ResponseFactory::badRequest("No pokemon was passed");
        }
        const auto updatePokemonRequest{receivedJson["pokemon"].get<PokemonModel::DTO::UpdatePokemonRequest>()};

        const PokemonModel::Pokemon updatedPokemonRequest{id, updatePokemonRequest.name};

        const std::optional<PokemonModel::Pokemon> updatedPokemon{repo_.updatePokemon(updatedPokemonRequest)};

        if (!updatedPokemon) {
            return ResponseFactory::notFound("Pokemon id {} not found", id);
        }

        nlohmann::json json;
        json["pokemon"] = *updatedPokemon;
        json["message"] = "Pokemon updated successfully";
        response.body = json.dump();
        response.header["Content-Type"] = "application/json";
        response.header["Content-Length"] = std::to_string(response.body.size());

        return response;
    } catch (const std::invalid_argument &) {
        return ResponseFactory::badRequest("Invalid pokemon id");

    } catch (const std::out_of_range &) {
        return ResponseFactory::badRequest("Invalid pokemon id");

    } catch (const nlohmann::json::exception &e) {
        Helper::displayError("{}", e.what());
        return ResponseFactory::badRequest("Invalid request data");

    }catch (PokemonModel::Error::PokemonNotFoundException& e) {
        Helper::displayError("{}", e.what());
        return ResponseFactory::badRequest("{}", e.what());
    }
    catch (const std::exception &e) {
        Helper::displayError("{}", e.what());
        return ResponseFactory::serverError("An unexpected error had occurred in the server");
    }
}

Response PokemonController::deletePokemon(const ParsedRequestObject &request) const {
    try {
        Response response{HttpStatus::OK, {}, {}};

        if (!request.parameterValues.contains(":id")) {
            return ResponseFactory::badRequest("No pokemon id was passed");
        }

        const int id{std::stoi(request.parameterValues.at(":id"))};

        if (!repo_.deletePokemonById(id)) {
            return ResponseFactory::notFound("pokemon with id {} not found", id);
        }

        nlohmann::json json;
        json["message"] = fmt::format("Pokemon id {} deleted successfully", id);
        response.body = json.dump();
        response.header["Content-Type"] = "application/json";
        response.header["Content-Length"] = std::to_string(response.body.size());

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
