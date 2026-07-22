#pragma once
#ifndef DATABASE_H
#define DATABASE_H

#include <SQLiteCpp/SQLiteCpp.h>
#include <string>
#include <mutex>

class DataBase {
public:
    explicit DataBase(std::string path);
       
    SQLite::Database& getDb() { return db_; }

    std::mutex& getMutex() {
        return mutex_;
    }
private:
	SQLite::Database db_;
    std::mutex mutex_{};

};

#endif 
