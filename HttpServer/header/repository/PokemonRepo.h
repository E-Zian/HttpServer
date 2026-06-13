#pragma
#ifndef POKEMONREPO_H
#define POKEMONREPO_H

#include "database/Database.h"
#include "model/PokemonModel.h"
#include <vector>

class PokemonRepo{
	using Pokemon = PokemonModel::Pokemon;
public:
	explicit PokemonRepo(SQLite::Database& db) :db_{ db } {};

	std::optional<Pokemon> createPokemonById(const Pokemon &newPokemon) const;

	std::optional<Pokemon> getPokemonById(int id) const;

	std::optional<Pokemon> updatePokemonById(int id, Pokemon newPokemon) const;

	bool deletePokemonById(int id) const;

	std::optional<std::vector<Pokemon>> getAllPokemon() const;

private:
	SQLite::Database& db_;
};


#endif