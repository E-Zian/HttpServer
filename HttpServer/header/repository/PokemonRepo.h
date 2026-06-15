#pragma
#ifndef POKEMONREPO_H
#define POKEMONREPO_H

#include "database/Database.h"
#include "model/PokemonModel.h"
#include <vector>
class PokemonAlreadyExistsException : public std::runtime_error {
public:
	explicit PokemonAlreadyExistsException(const std::string& name)
		: std::runtime_error("Pokemon '" + name + "' already exists") {}
};

class PokemonRepo{
	using Pokemon = PokemonModel::Pokemon;
public:
	explicit PokemonRepo(SQLite::Database& db) :db_{ db } {};

	std::optional<Pokemon> createPokemon(const PokemonModel::DTO::CreatePokemonRequest &newPokemon) const;

	std::optional<Pokemon> getPokemonById(int id) const;

	std::optional<Pokemon> updatePokemon(const Pokemon& updatePokemonRequest) const;

	bool deletePokemonById(int id) const;

	std::optional<std::vector<Pokemon>> getAllPokemon() const;

private:
	SQLite::Database& db_;
};


#endif