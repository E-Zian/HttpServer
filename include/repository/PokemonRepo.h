#pragma once
#ifndef POKEMONREPO_H
#define POKEMONREPO_H

#include "../Interface/IPokemonRepo.h"
#include "database/Database.h"
#include "model/PokemonModel.h"
#include <vector>


class PokemonRepo: public IPokemonRepo {
	using Pokemon = PokemonModel::Pokemon;
public:
	explicit PokemonRepo(SQLite::Database& db) :db_{ db } {};

	[[nodiscard]] std::optional<Pokemon> createPokemon(const PokemonModel::DTO::CreatePokemonRequest &newPokemon) const override;

	[[nodiscard]] std::optional<Pokemon> getPokemonById(int id) const override;

	[[nodiscard]] std::optional<Pokemon> updatePokemon(const Pokemon& updatePokemonRequest) const override;

	[[nodiscard]] bool deletePokemonById(int id) const override;

	[[nodiscard]] std::optional<std::vector<Pokemon>> getAllPokemon() const override;

private:
	SQLite::Database& db_;
};


#endif