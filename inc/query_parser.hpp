#pragma once
#include <memory>
#include <string_view>

#include "cell.hpp"
#include "db.hpp"

namespace parse {
class query_parser {
private:
  // NOTE:
  class math_engine {
  private:
    enum state {
      START,
      SUBEXPRESSION,

    };

  public:
    // Returns the result of calculated expression
    std::shared_ptr<cell::Cell> parse(std::string_view expression);
  };
  // TODO: It should return array of instructions to run
public:
  std::vector<memdb::instruction> parse(std::string_view query);
};
} // namespace parse
