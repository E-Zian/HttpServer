#include "repository/UserRepo.h"

#include "Helper.h"

std::optional<UserRepo::User> UserRepo::createUser(const User &newUser) const {
    try {
        SQLite::Transaction transaction{db_};
        SQLite::Statement checkExist{db_, "SELECT COUNT(*) FROM users WHERE id = :id"};
        checkExist.bind(":id", newUser.id);

        checkExist.executeStep();

        if (checkExist.getColumn(0).getInt() >= 1) {
            throw SQLite::Exception{"User already exists"};
        }

        SQLite::Statement createStatement{db_, "INSERT INTO users (name,email) VALUES (:name,:email)"};
        createStatement.bind(":name", newUser.name);
        createStatement.bind(":email", newUser.email);

        createStatement.executeStep();

        transaction.commit();

        return newUser;
    } catch (SQLite::Exception &exception) {
        Helper::displayError("{}", exception.what());
        return std::nullopt;
    }
}

std::optional<std::vector<UserRepo::User> > UserRepo::getAllUser() const {
    try {
        std::vector<UserRepo::User> users;

        SQLite::Statement fetchStatement{db_, "SELECT * FROM users"};
        while (fetchStatement.executeStep()) {
            users.emplace_back(fetchStatement.getColumn(0).getInt(), fetchStatement.getColumn(1).getText(),
                               fetchStatement.getColumn(2).getText());
        }

        return users;
    } catch (SQLite::Exception &exception) {
        Helper::displayError("{}",exception.what());
        return std::nullopt;
    }
}

std::optional<UserRepo::User> UserRepo::getUserById(const int id) const {
    try {
        SQLite::Statement fetchStatement{db_, "SELECT id, name, email FROM users WHERE id = :id"};
        fetchStatement.bind(":id", id);

        if (!fetchStatement.executeStep()) {
            throw SQLite::Exception{"User does not exist"};
        }

        UserRepo::User user{
            fetchStatement.getColumn(0).getInt(),
            fetchStatement.getColumn(1).getText(),
            fetchStatement.getColumn(2).getText()
        };

        return user;
    } catch (SQLite::Exception &exception) {
        Helper::displayError("{}",exception.what());
        return std::nullopt;
    }
}

std::optional<UserRepo::User> UserRepo::updateUserById(const int id, const User &newUser) const {
    try {
        SQLite::Transaction transaction{db_};

        SQLite::Statement checkExist{db_,"SELECT COUNT(*) FROM users WHERE id = :id"};
        checkExist.bind(":id", id);
        checkExist.executeStep();
        if (checkExist.getColumn(0).getInt() ==0) {
            throw SQLite::Exception{"User does not exist"};
        }

        SQLite::Statement updateStatement{db_, "UPDATE users SET name = :name, email = :email WHERE id = :id"};
        updateStatement.bind(":name", newUser.name);
        updateStatement.bind(":email", newUser.email);
        updateStatement.bind(":id", id);

        updateStatement.executeStep();

        transaction.commit();

        return newUser;

    }catch (SQLite::Exception &exception) {
        Helper::displayError("{}",exception.what());
        return std::nullopt;
    }
}

bool UserRepo::deleteUserById(const int id) const {
    try {
        SQLite::Transaction transaction{db_};
        SQLite::Statement checkExist{db_, "SELECT COUNT(*) FROM users WHERE id = :id"};
        checkExist.bind(":id", id);
        checkExist.executeStep();

        if (checkExist.getColumn(0).getInt() ==0) {
            throw SQLite::Exception{"User does not exist"};
        }

        SQLite::Statement deleteStatement{db_, "DELETE FROM users WHERE id =:id"};
        deleteStatement.bind(":id", id);

        deleteStatement.exec();

        transaction.commit();

        return true;

    }catch (SQLite::Exception &exception) {
        Helper::displayError("{}",exception.what());
        return false;
    }
}
