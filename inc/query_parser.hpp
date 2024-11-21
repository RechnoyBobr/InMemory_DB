#pragma once
#include <string_view>

namespace parser {
class query_parser {
private:
  // NOTE: math_engine should calculate
  class math_engine {};
  // TODO: It should return array of instructions to run
  std::vector<> parse(std::string_view query);
};
} // namespace parser
