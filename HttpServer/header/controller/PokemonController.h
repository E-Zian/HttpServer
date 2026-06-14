#pragma once
#ifndef POKEMONCONTROLLER_H
#define POKEMONCONTROLLER_H

#include "Router.h"
#include "ControllerBase.h"
#include "repository/PokemonRepo.h"

class PokemonController :public ControllerBase {
public:
	PokemonController(const Router& router,const PokemonRepo& repo);
private:
	const PokemonRepo& repo_;

	Response getAllPokemon(ParsedRequestObject& request) const;

	Response createPokemon(ParsedRequestObject& request) const;

	Response getPokemon(const ParsedRequestObject& request) const;

	Response updatePokemon(const ParsedRequestObject& request) const;

	Response deletePokemon(const ParsedRequestObject& request) const;
};

#endif 