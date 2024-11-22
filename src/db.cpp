#include "../inc/db.hpp"
#include "../inc/query_parser.hpp"
#include <string_view>

namespace memdb {
result db::execute(std::string_view query) {
  // TODO: finish that method
  parse::query_parser parser;
  auto res = parser.parse(query);

  for (auto i : res) {
    // Should be something like this
    this->execute_single_instruction(res);
  }

  return last_result;
}
} // namespace memdb
