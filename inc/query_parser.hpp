#pragma once
#include <string_view>
#include <unordered_map>
#include "cell.hpp"
#include "instruction.hpp"


namespace basic_parser {
    class query_parser {
    private:
        enum parser_state {
            VAR_NAME,
            VAR_TYPE,
            ATTRIBUTES,
            DEF_VAL,
            TABLE_NAME,
        };

        static cell::col_type parse_type(std::string &input);

        // TODO: It should return array of instructions to run
    public:
        class math_engine {
            std::string op_chars = "|&^=!<>-+*/%";
            std::unordered_map<std::string, int> priorities = {
                {"(", 0}, {"||", 1}, {"&&", 2}, {"^^", 3}, {"=", 4}, {"!=", 4}, {"<", 5}, {"<=", 5}, {">", 5},
                {">=", 5}, {"-", 6}, {"+", 6}, {"*", 7}, {"/", 7}, {"%", 7}, /*if number is negative*/{"~", 8}, {"|", 8}

            };

            enum state {
                START,
                SUBEXPRESSION,
            };

            int get_num_from_string;

        public:
            // Returns the result of calculated expression
            std::string to_postfix(std::string &expr);
        };

        static cell::Cell get_cell(std::string &str, cell::col_type cur_type);

        static std::vector<ins::instruction> parse(std::string_view query);
    };
} // namespace basic_parser
