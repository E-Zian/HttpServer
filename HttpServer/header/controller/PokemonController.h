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
};

#endif 