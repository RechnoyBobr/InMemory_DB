#include "db.hpp"
#include <string_view>

namespace basic_lexer {
// NOTE: lexer should tokenize query string
// and return vector of tokens to parser for further
// processing
class lexer {
  std::unordered_map<std::string_view, memdb::instruction_type> converter = {
      {"SELECT", memdb::instruction_type::SELECT},
      {"CREATE", memdb::instruction_type::CREATE},
      {"DELETE", memdb::instruction_type::DELETE},
      {"UPDATE", memdb::instruction_type::UPDATE},
      {"WHERE", memdb::instruction_type::WHERE},
      {"INSERT", memdb::instruction_type::INSERT},
  };

  std::vector<memdb::instruction_type> tokenize(std::string_view query);
};

} // namespace basic_lexer
