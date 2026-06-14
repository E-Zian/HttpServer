#include "repository/PokemonRepo.h"
#include "Helper.h"

std::optional<PokemonModel::Pokemon> PokemonRepo::createPokemon(const PokemonModel::DTO::CreatePokemonRequest &newPokemon) const {
    try {
        SQLite::Transaction transaction{db_};

        SQLite::Statement checkExist{db_, "SELECT COUNT(*) FROM pokemons WHERE name = :name"};
        checkExist.bind(":name", newPokemon.name);
        checkExist.executeStep();
        if (checkExist.getColumn(0).getInt() >= 1) {
            throw SQLite::Exception{"Pokemon already exists"};
        }

        SQLite::Statement createStatement{db_, "INSERT INTO pokemons (name) VALUES(:name)"};
        createStatement.bind(":name", newPokemon.name);

        createStatement.exec();

        Pokemon createdPokemon{db_.getLastInsertRowid(), newPokemon.name};
        transaction.commit();

        return createdPokemon;
    } catch (const SQLite::Exception &e) {
        Helper::displayError("{}",e.what());
        return std::nullopt;
    }
}

std::optional<PokemonRepo::Pokemon> PokemonRepo::getPokemonById(const int id) const {
    try {
        SQLite::Statement query{db_, "SELECT id, name FROM pokemons WHERE id = :id"};
        query.bind(":id", id);

        if (!query.executeStep()) {
            throw SQLite::Exception{"Pokemon does not exist"};
        }
        Pokemon newPokemon{query.getColumn(0).getInt(), query.getColumn(1).getText()};

        return newPokemon;
    } catch (const SQLite::Exception &e) {
        Helper::displayError("{}",e.what());
        return std::nullopt;
    }
}

std::optional<PokemonRepo::Pokemon> PokemonRepo::updatePokemon(const Pokemon& updatePokemonRequest) const {
    try {
        SQLite::Transaction transaction{db_};

        SQLite::Statement checkExist(db_, "SELECT COUNT(*) FROM pokemons WHERE id = :id");
        checkExist.bind(":id", updatePokemonRequest.id);
        checkExist.executeStep();
        if (checkExist.getColumn(0).getInt() == 0) {
            throw SQLite::Exception{"Pokemon does not exists"};
        }

        SQLite::Statement updateStatement{db_, "UPDATE pokemons SET name = :name WHERE id = :id"};
        updateStatement.bind(":name", updatePokemonRequest.name);
        updateStatement.bind(":id", updatePokemonRequest.id);

        updateStatement.exec();

        transaction.commit();
        return updatePokemonRequest;
    } catch (const SQLite::Exception &e) {
        Helper::displayError("{}",e.what());
        return std::nullopt;
    }
}

bool PokemonRepo::deletePokemonById(const int id) const {
    try {
        SQLite::Transaction transaction{db_};

        SQLite::Statement checkExist{db_, "SELECT COUNT(*) FROM pokemons WHERE id = :id"};
        checkExist.bind(":id", id);
        checkExist.executeStep();
        if (checkExist.getColumn(0).getInt() == 0) {
            throw SQLite::Exception{"Pokemon does not exists"};
        }

        SQLite::Statement deleteStatement{db_, "DELETE FROM pokemons WHERE id=:id"};
        deleteStatement.bind(":id", id);

        deleteStatement.exec();

        transaction.commit();

        return true;
    } catch (const SQLite::Exception &e) {
        Helper::displayError("{}",e.what());

        return false;
    }
}

std::optional<std::vector<PokemonRepo::Pokemon> > PokemonRepo::getAllPokemon() const {
    try {
        std::vector<PokemonRepo::Pokemon> pokemons;
        SQLite::Statement query{db_, "SELECT id,name FROM pokemons"};

        while (query.executeStep()) {
            pokemons.emplace_back(
                query.getColumn(0).getInt(),
                query.getColumn(1).getText()
            );
        }

        return pokemons;
    } catch (const SQLite::Exception &e) {
        Helper::displayError("{}",e.what());
        return std::nullopt;
    }
}
