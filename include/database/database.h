#pragma once
#ifndef DATABASE_H
#define DATABASE_H

#include <SQLiteCpp/SQLiteCpp.h>
#include <string>

class DataBase {
public:
    explicit DataBase(std::string path);
       
    SQLite::Database& get() { return db_; }

private:
	SQLite::Database db_;


};

#endif 
