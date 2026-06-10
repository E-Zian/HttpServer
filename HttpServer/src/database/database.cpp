#include "database/database.h"

namespace {
	void createUserTable(SQLite::Database& db) {
        db.exec(
            "CREATE TABLE IF NOT EXISTS users ("
            "id     INTEGER PRIMARY KEY AUTOINCREMENT,"
            "name   TEXT    NOT NULL,"
            "email  TEXT    NOT NULL UNIQUE,"
            "age    INTEGER NOT NULL"
            ")"
        );
	}

    void createPokemonTable(SQLite::Database& db) {
        db.exec(
            "CREATE TABLE IF NOT EXISTS pokemons ("
            "id     INTEGER PRIMARY KEY AUTOINCREMENT,"
            "name   TEXT    NOT NULL"
            ")"
        );
    }


}

DataBase::DataBase(const std::string path) :db_{ path,SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE } {
    createUserTable(db_);
    createPokemonTable(db_);
}

