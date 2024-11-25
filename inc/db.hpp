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
  // NOTE: is it important?
  std::vector<op::instruction_operator> operators;
  // If type is create table
  std::vector<std::pair<std::string, std::vector<attributes>>> col_names;
  std::vector<std::pair<col_type, cell::Cell>> col_types;
  std::string table_name;

 public:
  instruction() = default;

  instruction(std::vector<cell::Cell> ops,
              std::vector<op::instruction_operator> op, instruction_type type);
  instruction(
      std::string table_name,
      std::vector<std::pair<std::string, std::vector<attributes>>> names,
      std::vector<std::pair<col_type, cell::Cell>> types);
  instruction_type get_type();
  // Can throw runtime error if type not create
  std::vector<std::pair<std::string, std::vector<attributes>>>
  get_columns_names();
  std::vector<std::pair<col_type, cell::Cell>> get_columns_types();
  std::string get_table_name();
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
 private:
  std::vector<item> queried_cols;
  bool is_ok;
  bool is_empty;

 public:
  result();
  result(std::vector<item>);
};

class header {
 private:
  std::vector<std::pair<std::string, std::vector<attributes>>> col_names;
  std::vector<std::pair<col_type, cell::Cell>> col_types;

 public:
  header(std::vector<std::pair<std::string, std::vector<attributes>>> names,
         std::vector<std::pair<col_type, cell::Cell>> types);
};

// NOTE: main class in db.hpp
class db {
  class table {
   private:
    std::vector<item> rows;
    header cols;
    int row_size;

   public:
    table(header cols);
  };

 public:
  result execute(std::string_view query);
  std::unordered_map<std::string_view, std::unique_ptr<table>> tables;
  void create_table();
};
}  // namespace memdb
