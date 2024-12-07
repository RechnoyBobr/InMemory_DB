#include <gtest/gtest.h>
#include <iostream>
#include "db.hpp"

TEST(db_tests, create_table_test) {
    memdb::db Database;
    auto result = Database.execute(
            "create table ({autoincrement, key}) id: int32, {unique} name:string[20], password_hash :bytes[8] = 0xdeadbeef)");
    EXPECT_EQ("Table name can't be empty\n", result.get_error());
    EXPECT_EQ(result.is_ok(), false);
    result = Database.execute(
            "create table users({autoincrement, key}) id: int32, {unique} name:string[20], password_hash :bytes[8] = 0xdeadbeef)");
    EXPECT_EQ(result.is_ok(), true);
}

TEST(db_tests, insert_select_tables_test) {
    memdb::db Database;
    auto result = Database.execute(
            "create table users ({autoincrement, key}) id: int32, {unique} name:string[20], password_hash :bytes[8] = 0xdeadbeef)");
    EXPECT_EQ(result.is_ok(), true);
    Database.execute("insert (,\"dima\",) to users");
    Database.execute("insert (name=\"vasya\", password_hash=0xbeefdead) to users");
    Database.execute("insert (,\"oleg\",) to users");
    result = Database.execute("select name, id from users where |name| % 2 = 0");
    if (result.is_ok()) {
        for (auto &row: result) {
            std::cout << row[0].get<int>() << " " << row[1].get<std::string>() << "\n";
        }
    }
}