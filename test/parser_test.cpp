#include "db.hpp"
#include <gtest/gtest.h>

#include "../inc/query_parser.hpp"

TEST(parser_tests, create_table_test) {
    basic_parser::query_parser parser = basic_parser::query_parser();
    auto res = parser.parse(
        "create table beaver ({autoincrement, key} name : string = \"Fofan\", "
        "age:int32 = +12, {unique}has_eaten_wood: bool = false, byte_number: "
        "bytes[8] "
        "= 0x000001)");
    EXPECT_EQ(res[0].get_type(), ins::instruction_type::CREATE);
    auto names = res[0].get_columns_names();
    auto types = res[0].get_columns_types();
    EXPECT_EQ(names[0].first, "name");
    EXPECT_EQ(names[0].second[0], ins::attributes::AUTOINCREMENT);
    EXPECT_EQ(types[0].second.get_string(), "Fofan");
    EXPECT_EQ(types[1].second.get_int(), 12);
    EXPECT_EQ(*(types[3].second.get_bytes().end() - 1), std::byte(1));
}


TEST(parser_test, select_where_test) {
    basic_parser::query_parser parser;
    auto res = parser.parse("select user.id, user.password_hash where |id| % 2 = 0");
    EXPECT_EQ(res[0].get_type(), ins::SELECT);
    EXPECT_EQ(res[1].get_type(), ins::WHERE);
}
