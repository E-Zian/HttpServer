#include "repository/PokemonRepo.h"
#include "Helper.h"

std::optional<PokemonModel::Pokemon> PokemonRepo::createPokemonById(Pokemon newPokemon) {
    try {
        SQLite::Transaction transaction{db_};
        if (!db_.tableExists("pokemons")) {
            throw SQLite::Exception{"Pokemon table not found"};
        }

        SQLite::Statement checkExist{db_,"SELECT * FROM pokemons WHERE name = :name"};
        checkExist.bind(":name", newPokemon.name);

        transaction.commit();
    }catch (std::exception& e) {
        Helper::displayError(e.what());
        return std::nullopt;
    }
}

