#include <gtest/gtest.h>
#include <iostream>
#include "query_parser.hpp"


TEST(math_engine_test, bool_test) {
    std::string expr = "false || (false && true) && (true || false)";
    basic_parser::query_parser::math_engine engine;
    std::string result = engine.to_postfix(expr);
    EXPECT_EQ(result, "false false true && true false || && || ");
}
TEST(math_engine_test, int_test) {
    std::string expr = "12 * -3 / (45+8 % 2)";
    basic_parser::query_parser::math_engine engine;
    std::string result = engine.to_postfix(expr);
    EXPECT_EQ(result, "12 3 ~ * 45 8 2 % + / ");
}
TEST(math_engine_test, string_test) {
    std::string expr = "|\"afdvxcv\"|+1";
    basic_parser::query_parser::math_engine engine;
    std::string result = engine.to_postfix(expr);
    ASSERT_EQ(result, "\"afdvxcv\" | 1 + ");
}
