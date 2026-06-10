#pragma once
#ifndef POKEMONCONTROLLER_H
#define POKEMONCONTROLLER_H

#include "Router.h"
#include "ControllerBase.h"

class PokemonController :public ControllerBase {
public:
	explicit PokemonController(const Router& router);
private:
};

#endif 