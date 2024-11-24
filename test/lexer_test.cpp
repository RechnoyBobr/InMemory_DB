#include "../inc/db.hpp"
#include "../inc/lexer.hpp"
#include "gtest/gtest.h"
TEST(lexer_tests, first_test) {
  basic_lexer::lexer lex = basic_lexer::lexer();
  auto res = lex.tokenize(
      std::string_view("select users.id, bebra.hpp where bob = builder"));
  EXPECT_EQ(res[0].first, memdb::instruction_type::SELECT);
  EXPECT_EQ(res[1].first, memdb::instruction_type::WHERE);
}
