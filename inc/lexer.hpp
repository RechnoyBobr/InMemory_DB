#pragma once
#include <string_view>

#include "db.hpp"

namespace basic_lexer {
    // NOTE: lexer should tokenize query string
    // and return vector of tokens to parser for further
    // processing
    class lexer {
    private:
        enum waiting_state { KEYWORD, COMMAND };

        std::unordered_map<std::string_view, ins::instruction_type> converter = {
            {"select", ins::instruction_type::SELECT},
            {"create", ins::instruction_type::CREATE},
            {"delete", ins::instruction_type::DELETE},
            {"update", ins::instruction_type::UPDATE},
            {"where", ins::instruction_type::WHERE},
            {"insert", ins::instruction_type::INSERT},
            {"from", ins::instruction_type::FROM},
            {"set", ins::instruction_type::SET},
            {"to", ins::instruction_type::TO}

        };

    public:
        lexer() = default;

        std::vector<std::pair<ins::instruction_type, std::string> > tokenize(
            std::string_view query);
    };
} // namespace basic_lexer
