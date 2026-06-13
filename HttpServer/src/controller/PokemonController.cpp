#include "controller/PokemonController.h"

PokemonController::PokemonController(const Router& router,const PokemonRepo& repo):ControllerBase(router),repo_(repo) {

}