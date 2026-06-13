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

	std::optional<Pokemon> createPokemonById(Pokemon newPokemon);

	std::optional<Pokemon> getPokemonById(int id);

	std::optional<Pokemon> updatePokemonById(int id, Pokemon newPokemon);

	bool deletePokemonById(int id);

	std::vector<Pokemon> getAllPokemon();


private:
	SQLite::Database& db_;
};


#endif