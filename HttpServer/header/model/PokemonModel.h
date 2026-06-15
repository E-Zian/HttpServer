#pragma
#ifndef POKEMONMODEL_H
#define POKEMONMODEL_H

#include <string>
#include <nlohmann/json.hpp>

namespace PokemonModel {
	struct Pokemon {
		int id;
		std::string name;
	};

	inline void to_json( nlohmann::json& json, const PokemonModel::Pokemon& pokemon) {
		json = nlohmann::json{ {"id", pokemon.id}, {"name", pokemon.name}};
	}

	inline void from_json(const nlohmann::json& json, PokemonModel::Pokemon& pokemon) {
		json.at("id").get_to(pokemon.id);
		json.at("name").get_to(pokemon.name);
	}

	namespace DTO {
		struct CreatePokemonRequest {
			std::string name;
		};

		inline void to_json( nlohmann::json& json, const PokemonModel::DTO::CreatePokemonRequest& createPokemonRequest) {
			json = nlohmann::json{ {"name", createPokemonRequest.name}};
		}

		inline void from_json(const nlohmann::json& json, PokemonModel::DTO::CreatePokemonRequest& createPokemonRequest) {
			json.at("name").get_to(createPokemonRequest.name);
		}
		struct UpdatePokemonRequest {
			std::string name;
		};
		inline void to_json( nlohmann::json& json, const PokemonModel::DTO::UpdatePokemonRequest& updatePokemonRequest) {
			json = nlohmann::json{ {"name", updatePokemonRequest.name}};
		}

		inline void from_json(const nlohmann::json& json, PokemonModel::DTO::UpdatePokemonRequest& updatePokemonRequest) {
			json.at("name").get_to(updatePokemonRequest.name);
		}
	}
	namespace Error {
		class PokemonAlreadyExistsException : public std::runtime_error {
		public:
			explicit PokemonAlreadyExistsException(const std::string& name)
				: std::runtime_error("Pokemon '" + name + "' already exists") {}
		};
		class PokemonNotFoundException : public std::runtime_error {
			public:
			explicit PokemonNotFoundException(const int id):runtime_error("Pokemon with id '"+std::to_string(id)+"' not found"){};
		};
	}
};


#endif