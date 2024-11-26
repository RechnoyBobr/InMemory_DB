#include "../inc/lexer.hpp"

#include <ranges>
#include <stdexcept>
#include <string_view>

namespace basic_lexer {
    std::vector<std::pair<ins::instruction_type, std::string> > lexer::tokenize(
        std::string_view query) {
        waiting_state current_state = KEYWORD;
        std::string command;
        auto result = std::vector<std::pair<ins::instruction_type, std::string> >(0);
        bool isCreate = false;
        ins::instruction_type type;
        std::string_view delim = " ";
        auto splitted_string = std::views::split(query, delim);
        for (auto word_value_type: splitted_string) {
            auto word = std::string_view(word_value_type);
            if (isCreate && word == "table") {
                isCreate = false;
                current_state = COMMAND;
                type = ins::CREATE;
                continue;
            } else if (isCreate) {
                throw std::runtime_error("Can't recognize a keyword");
            }
            if (current_state == KEYWORD && converter.contains(word)) {
                if (word == "create") {
                    isCreate = true;
                    continue;
                }
                type = converter[word];
                current_state = COMMAND;
            } else if (current_state == KEYWORD) {
                throw std::runtime_error("Can't recognize keyword");
            } else {
                if (converter.contains(word)) {
                    result.emplace_back(type, command);
                    type = converter[word];
                    command = "";
                } else {
                    // NOTE:
                    command += (word);
                    command += " ";
                }
            }
        }
        result.emplace_back(type, std::string_view(command));
        return result;
    }
} // namespace basic_lexer
