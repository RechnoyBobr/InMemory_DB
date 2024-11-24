#pragma once

#include <memory>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "cell.hpp"
#include "operator.hpp"

namespace memdb {

enum col_type { bool_type, int32, string, bytes };

enum attributes {
  KEY,
  AUTOINCREMENT,
  UNIQUE,
};

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
  // Vector of columns names if type is create table
  std::vector<std::string> names;
  // NOTE: is it important?
  std::vector<op::instruction_operator> operators;
  // If type is create table then we should keep vectors of columns names and
  // types
 public:
  instruction() = default;

  instruction(std::vector<cell::Cell> ops,
              std::vector<op::instruction_operator> op, instruction_type type);

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
    std::vector<std::pair<std::string_view, std::pair<col_type, attributes>>>
        cols;
  };

 public:
  result execute(std::string_view query);
  std::unordered_map<std::string_view, std::unique_ptr<table>> tables;
  void create_table();
};
}  // namespace memdb
