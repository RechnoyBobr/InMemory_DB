#pragma once
#include <memory>
#include <string_view>
#include "cell.hpp"
#include "instruction.hpp"


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
            std::shared_ptr<std::vector<ins::instruction>> parse(std::string_view expression);
        };

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
        cell::Cell get_cell(std::string &str, cell::col_type cur_type);

        std::vector<ins::instruction> parse(std::string_view query);
    };
} // namespace basic_parser
