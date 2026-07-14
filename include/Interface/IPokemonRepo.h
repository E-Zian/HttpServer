#pragma once
#ifndef IPOKEMONREPO_H
#define IPOKEMONREPO_H

#include "model/PokemonModel.h"
#include <optional>
#include <vector>

class IPokemonRepo {
public:
	[[nodiscard]] virtual std::optional<PokemonModel::Pokemon> createPokemon(const PokemonModel::DTO::CreatePokemonRequest& newPokemon) const = 0;

	[[nodiscard]] virtual std::optional<PokemonModel::Pokemon> getPokemonById(int id) const = 0;

	[[nodiscard]] virtual std::optional<PokemonModel::Pokemon> updatePokemon(const PokemonModel::Pokemon& updatePokemonRequest) const = 0;

	[[nodiscard]] virtual bool deletePokemonById(int id) const = 0;

	[[nodiscard]] virtual std::optional<std::vector<PokemonModel::Pokemon>> getAllPokemon() const = 0;

	virtual ~IPokemonRepo() = default;
};

#endif