#pragma once
#ifndef POKEMONREPO_H
#define POKEMONREPO_H

#include "database/Database.h"
#include "model/PokemonModel.h"
#include <vector>


class PokemonRepo{
	using Pokemon = PokemonModel::Pokemon;
public:
	explicit PokemonRepo(SQLite::Database& db) :db_{ db } {};

	[[nodiscard]] std::optional<Pokemon> createPokemon(const PokemonModel::DTO::CreatePokemonRequest &newPokemon) const;

	[[nodiscard]] std::optional<Pokemon> getPokemonById(int id) const;

	[[nodiscard]] std::optional<Pokemon> updatePokemon(const Pokemon& updatePokemonRequest) const;

	[[nodiscard]] bool deletePokemonById(int id) const;

	[[nodiscard]] std::optional<std::vector<Pokemon>> getAllPokemon() const;

private:
	SQLite::Database& db_;
};


#endif