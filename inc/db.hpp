#pragma once

#include <string_view>
#include <unordered_map>
#include <vector>

#include "cell.hpp"
#include "operator.hpp"

namespace memdb {

enum col_type { bool_type, int32, string, bytes };

enum instruction_type {
  SELECT,
  WHERE,
  INSERT,
  UPDATE,
  CREATE,
  DELETE,
  FROM,
  TO,
  SET
};

class instruction {
 private:
  instruction_type type;
  std::vector<cell::Cell> operands;
  std::vector<op::instruction_operator> operators;
  // TODO: imagine a way to represent operators. (they should be overloaded for
  // compatible types)
 public:
  cell::Cell evaluate();
};

class item {
  std::vector<cell::Cell> row;

 public:
  item(std::vector<cell::Cell> r);
  // TODO: create helpful methods
};

// TODO: Create iterator for result class
class result {
  // WARN: I'm unsure if it's good idea to have vector of cells as a result
  std::vector<item> result;

  bool is_ok();
};

// NOTE: main class in db.hpp
class db {
  class table {
    std::vector<item> rows;
    std::vector<std::pair<std::string_view, col_type>> cols;
  };

 public:
  result execute(std::string_view query);
  std::unordered_map<std::string_view, table> tables;
};
}  // namespace memdb
