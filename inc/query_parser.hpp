#pragma once
#include "db.hpp"
#include <string_view>

namespace parse {
class query_parser {
private:
  // NOTE: math_engine should calculate
  class math_engine {};
  // TODO: It should return array of instructions to run
public:
  std::vector<memdb::instruction> parse(std::string_view query);
};
} // namespace parse
