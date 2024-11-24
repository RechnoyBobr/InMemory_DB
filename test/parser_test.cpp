#include <gtest/gtest.h>

#include "../inc/query_parser.hpp"

TEST(parser_tests, create_table_test) {
  basic_parser::query_parser parser = basic_parser::query_parser();
  parser.parse(
      "create table beaver ({autoincrement, key} name : string = \"Fofan\", "
      "age:int32, {unique}has_eaten_wood: bool = false, byte_number: bytes[8] "
      "= 0x000001)");
}
