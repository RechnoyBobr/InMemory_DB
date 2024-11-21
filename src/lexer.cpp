#include "../inc/lexer.hpp"
#include <ranges>
#include <stdexcept>
#include <string_view>

namespace basic_lexer {

std::vector<std::pair<memdb::instruction_type, std::string_view>>
lexer::tokenize(std::string_view query) {
  waiting_state current_state = KEYWORD;
  std::string command = "";
  auto result =
      std::vector<std::pair<memdb::instruction_type, std::string_view>>(0);
  memdb::instruction_type type;
  for (const auto word_value_type : std::views::split(query, " ")) {
    std::string word = std::string(std::string_view(word_value_type));

    if (current_state == KEYWORD && converter.contains(word)) {
      type = converter[word];
      current_state = COMMAND;
    } else if (current_state == KEYWORD) {
      throw new std::runtime_error("Can't recognize keyword");
    } else if (current_state == COMMAND) {
      if (converter.contains(word)) {
        result.emplace_back(type, std::string_view(command));
        type = converter[word];
        command = "";
      } else {
        command += std::string(word);
      }
    }
  }
  return result;
}
} // namespace basic_lexer
