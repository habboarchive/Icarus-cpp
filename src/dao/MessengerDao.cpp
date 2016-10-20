/**
* Icarus - A multi-platform C++ server
*
* Copyright 2016 Alex "Quackster" Miller
*
* Licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License
* (see https://creativecommons.org/licenses/by-nc-sa/4.0/, or LICENSE.txt for a full license
*/
#include "stdafx.h"

#include "dao/MessengerDao.h"
#include "boot/Icarus.h"

bool MessengerDao::exists(std::string query) {

    bool output = false;
    std::shared_ptr<MySQLConnection> connection = Icarus::getDatabaseManager()->getConnectionPool()->borrow();

    try {
        std::shared_ptr<sql::Connection> sqlConnection = connection->sqlConnection;
        std::shared_ptr<sql::Statement> statement = std::shared_ptr<sql::Statement>(sqlConnection->createStatement());
        std::shared_ptr<sql::ResultSet> resultSet = std::shared_ptr<sql::ResultSet>(statement->executeQuery(query));
        output = resultSet->next();
    }
    catch (sql::SQLException &e) {
        Icarus::getDatabaseManager()->printException(e, __FILE__, __FUNCTION__, __LINE__);
    }

    Icarus::getDatabaseManager()->getConnectionPool()->unborrow(connection);

    return output;
}

std::map<int, MessengerUser*> *MessengerDao::getFriends(int user_id) {

    std::map<int, MessengerUser*> *friends = new std::map<int, MessengerUser*>();
    std::shared_ptr<MySQLConnection> connection = Icarus::getDatabaseManager()->getConnectionPool()->borrow();

    try {

        std::shared_ptr<sql::Connection> sqlConnection = connection->sqlConnection;
        std::shared_ptr<sql::Statement> statement = std::shared_ptr<sql::Statement>(sqlConnection->createStatement());
        std::shared_ptr<sql::ResultSet> resultSet = std::shared_ptr<sql::ResultSet>(statement->executeQuery("SELECT * FROM messenger_friendships WHERE (sender = " + std::to_string(user_id) + ") OR (receiver = " + std::to_string(user_id) + ")"));

        while (resultSet->next()) {

            MessengerUser *friend_;

            if (resultSet->getInt("sender") != user_id) {
                friend_ = new MessengerUser(resultSet->getInt("sender"));
            }
            else {
                friend_ = new MessengerUser(resultSet->getInt("receiver"));
            }

            friends->insert(std::make_pair(friend_->getDetails()->getId(), friend_));
        }

    }
    catch (sql::SQLException &e) {
        Icarus::getDatabaseManager()->printException(e, __FILE__, __FUNCTION__, __LINE__);
    }

    Icarus::getDatabaseManager()->getConnectionPool()->unborrow(connection);

    return friends;
}

std::map<int, MessengerUser*> *MessengerDao::getRequests(int user_id) {

    std::map<int, MessengerUser*> *friends = new std::map<int, MessengerUser*>();
    std::shared_ptr<MySQLConnection> connection = Icarus::getDatabaseManager()->getConnectionPool()->borrow();

    try {

        std::shared_ptr<sql::Connection> sql_connection = connection->sqlConnection;
        std::shared_ptr<sql::Statement> statement = std::shared_ptr<sql::Statement>(sql_connection->createStatement());
        std::shared_ptr<sql::ResultSet> result_set = std::shared_ptr<sql::ResultSet>(statement->executeQuery("SELECT * FROM messenger_requests WHERE to_id = " + std::to_string(user_id)));

        while (result_set->next()) {

            MessengerUser *friend_ = new MessengerUser(result_set->getInt("from_id"));
            friends->insert(std::make_pair(friend_->getDetails()->getId(), friend_));
        }

    }
    catch (sql::SQLException &e) {
        Icarus::getDatabaseManager()->printException(e, __FILE__, __FUNCTION__, __LINE__);
    }

    Icarus::getDatabaseManager()->getConnectionPool()->unborrow(connection);

    return friends;

}

std::vector<int> MessengerDao::search(std::string query) {

    std::vector<int> results;
    std::shared_ptr<MySQLConnection> connection = Icarus::getDatabaseManager()->getConnectionPool()->borrow();

    try {

        std::shared_ptr<sql::Connection> sql_connection = connection->sqlConnection;
        std::shared_ptr<sql::PreparedStatement> statement = std::shared_ptr<sql::PreparedStatement>(sql_connection->prepareStatement("SELECT id FROM users WHERE username LIKE ? LIMIT 30")); {
            statement->setString(1, "%" + query + "%");
        }

        std::shared_ptr<sql::ResultSet> result_set = std::shared_ptr<sql::ResultSet>(statement->executeQuery());

        while (result_set->next()) {
            results.push_back(result_set->getInt("id"));
        }

    }
    catch (sql::SQLException &e) {
        Icarus::getDatabaseManager()->printException(e, __FILE__, __FUNCTION__, __LINE__);
    }

    Icarus::getDatabaseManager()->getConnectionPool()->unborrow(connection);

    return results;
}

bool MessengerDao::newRequest(int fromId, int toId) {

    bool success = false;

    if (!MessengerDao::exists("SELECT * FROM messenger_requests WHERE to_id = '" + std::to_string(toId) + "' AND from_id = '" + std::to_string(fromId) + "'") &&
        !MessengerDao::exists("SELECT * FROM messenger_requests WHERE to_id = '" + std::to_string(toId) + "' AND from_id = '" + std::to_string(fromId) + "'")) {


        std::shared_ptr<MySQLConnection> connection = Icarus::getDatabaseManager()->getConnectionPool()->borrow();

        try {

            std::shared_ptr<sql::Connection> sql_connection = connection->sqlConnection;
            std::shared_ptr<sql::PreparedStatement> statement = std::shared_ptr<sql::PreparedStatement>(sql_connection->prepareStatement("INSERT INTO messenger_requests (to_id, from_id) VALUES (?, ?)")); {
                statement->setInt(1, toId);
                statement->setInt(2, fromId);
            }

            statement->execute();
            success = true;

        }
        catch (sql::SQLException &e) {
            Icarus::getDatabaseManager()->printException(e, __FILE__, __FUNCTION__, __LINE__);
        }

        Icarus::getDatabaseManager()->getConnectionPool()->unborrow(connection);

    }

    return success;
}

bool MessengerDao::removeRequest(int fromId, int toId) {

    std::shared_ptr<MySQLConnection> connection = Icarus::getDatabaseManager()->getConnectionPool()->borrow();

    try {
        std::shared_ptr<sql::Connection> sql_connection = connection->sqlConnection;
        std::shared_ptr<sql::Statement> statement = std::shared_ptr<sql::Statement>(sql_connection->createStatement());
        statement->execute("DELETE FROM messenger_requests WHERE from_id = " + std::to_string(fromId) + " AND to_id = " + std::to_string(toId));
    }
    catch (sql::SQLException &e) {
        Icarus::getDatabaseManager()->printException(e, __FILE__, __FUNCTION__, __LINE__);
    }

    Icarus::getDatabaseManager()->getConnectionPool()->unborrow(connection);

    return false;
}


bool MessengerDao::removeFriend(int friendId, int userId) {

    std::shared_ptr<MySQLConnection> connection = Icarus::getDatabaseManager()->getConnectionPool()->borrow();

    try {
        std::shared_ptr<sql::Connection> sql_connection = connection->sqlConnection;
        std::shared_ptr<sql::Statement> statement = std::shared_ptr<sql::Statement>(sql_connection->createStatement());
        statement->execute("DELETE FROM messenger_friendships WHERE(sender = " + std::to_string(userId) + " AND receiver = " + std::to_string(friendId) + ") OR (receiver = " + std::to_string(userId) + " AND sender = " + std::to_string(friendId) + ")");
    }
    catch (sql::SQLException &e) {
        Icarus::getDatabaseManager()->printException(e, __FILE__, __FUNCTION__, __LINE__);
    }

    Icarus::getDatabaseManager()->getConnectionPool()->unborrow(connection);

    return false;
}

bool MessengerDao::newFriend(int sender, int receiver) {

    std::shared_ptr<MySQLConnection> connection = Icarus::getDatabaseManager()->getConnectionPool()->borrow();

    try {

        std::shared_ptr<sql::Connection> sql_connection = connection->sqlConnection;
        std::shared_ptr<sql::PreparedStatement> statement = std::shared_ptr<sql::PreparedStatement>(sql_connection->prepareStatement("INSERT INTO messenger_friendships (sender, receiver) VALUES (?, ?)")); {
            statement->setInt(1, sender);
            statement->setInt(2, receiver);
        }

        statement->execute();

    }
    catch (sql::SQLException &e) {
        Icarus::getDatabaseManager()->printException(e, __FILE__, __FUNCTION__, __LINE__);
    }

    Icarus::getDatabaseManager()->getConnectionPool()->unborrow(connection);

    return false;
}