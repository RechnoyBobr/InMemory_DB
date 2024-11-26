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

        std::unordered_map<std::string_view, memdb::instruction_type> converter = {
            {"select", memdb::instruction_type::SELECT},
            {"create", memdb::instruction_type::CREATE},
            {"delete", memdb::instruction_type::DELETE},
            {"update", memdb::instruction_type::UPDATE},
            {"where", memdb::instruction_type::WHERE},
            {"insert", memdb::instruction_type::INSERT},
            {"from", memdb::instruction_type::FROM},
            {"set", memdb::instruction_type::SET},
            {"to", memdb::instruction_type::TO}

        };

    public:
        lexer() = default;

        std::vector<std::pair<memdb::instruction_type, std::string> > tokenize(
            std::string_view query);
    };
} // namespace basic_lexer
