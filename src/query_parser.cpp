#include "../inc/query_parser.hpp"

#include <memory>
#include <string_view>
#include <vector>

#include "../inc/lexer.hpp"

namespace parse {
std::vector<memdb::instruction> query_parser::parse(std::string_view query) {
  basic_lexer::lexer l = basic_lexer::lexer();
  auto lexed_commands = l.tokenize(query);
  // TODO: finish method
}

std::shared_ptr<cell::Cell>
query_parser::math_engine::parse(std::string_view expression) {
  std::shared_ptr<cell::Cell> result;
  int ind = 0;
  int subexpression_start = 0;
  state current_state = START;
  int bracket_cnt = 0;
  memdb::col_type type;
  while (ind != expression.size()) {
    char cur = expression[ind];
    if (cur == '(' && current_state == START) {
      current_state = SUBEXPRESSION;
      subexpression_start = ind + 1;
    } else if (cur == '(') {
      bracket_cnt++;
    } else if (cur == ')' && current_state == SUBEXPRESSION) {
      if (!bracket_cnt) {
        result = parse(expression.substr(subexpression_start, ind - 1));
        current_state = START;
      } else {
        bracket_cnt--;
      }
    } else if (cur == '\"' && current_state) {
      type = memdb::col_type::string;
      std::string r = "";
      ind++;
      cur = expression[ind];
      while (cur != '\"') {
        r += cur;
        ind++;
        cur = expression[ind];
      }
      result = std::make_shared<cell::Cell>(cell::Cell(r));
    } else if (cur == '|' && ind != expression.size() - 1 &&
               expression[ind + 1] != '|') {
      // Means that it needs to return size of string or bytes
      type = memdb::col_type::int32;
    }
  }
  return result;
}
} // namespace parse
