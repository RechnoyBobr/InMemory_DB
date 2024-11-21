#pragma once

#include "cell.hpp"
#include <string_view>
#include <unordered_map>
#include <vector>

namespace memdb {
enum col_type { bool_type, int32, string, bytes };
class instruction {
private:
};
class item {
  std::vector<cell::Cell> row;

public:
  item(std::vector<cell::Cell> r);
  // TODO: create helpful methods
};

class db {
  class table {
    std::vector<item> rows;
    std::vector<std::pair<std::string_view, col_type>> cols;
  };

public:
  void execute(std::string_view query);
  std::unordered_map<std::string_view, table> tables;
};
} // namespace memdb
