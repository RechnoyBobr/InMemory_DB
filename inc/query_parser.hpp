#pragma once
#include <memory>
#include <string_view>
#include <unordered_map>

#include "db.hpp"

namespace basic_parser {
class query_parser {
 private:
  class math_engine {
   private:
    enum state {
      START,
      SUBEXPRESSION,

    };

   public:
    // Returns the result of calculated expression
    std::shared_ptr<std::vector<memdb::instruction>> parse(
        std::string_view expression);
  };
  enum parser_state {
    VAR_NAME,
    VAR_TYPE,
    ATTRIBUTES,
    DEF_VAL,
    TABLE_NAME,
  };
  memdb::col_type parse_type(std::string_view input);
  // TODO: It should return array of instructions to run
 public:
  std::vector<memdb::instruction> parse(std::string_view query);
};
}  // namespace basic_parser
