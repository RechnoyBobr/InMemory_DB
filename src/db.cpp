#include "../inc/db.hpp"

#include <string_view>

#include "../inc/query_parser.hpp"

namespace memdb {
// result db::execute(std::string_view query) {
//   // TODO: finish that method
//   parse::query_parser parser;
//   auto res = parser.parse(query);
//
//   for (auto i : res) {
//     // Should be something like this
//     this->execute_single_instruction(res);
//   }
//
//   return last_result;
// }
//
instruction::instruction(std::vector<cell::Cell> ops,
                         std::vector<op::instruction_operator> op,
                         instruction_type type) {
  this->operands = ops;
  this->operators = op;
  this->type = type;
}
}  // namespace memdb
