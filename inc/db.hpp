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
private:
  std::vector<cell::Cell> row;

public:
  item(std::vector<cell::Cell> r);
  // TODO: create helpful methods
};

// TODO: Create iterator for result class
class result {
private:
  std::vector<item> queried_cols;
  bool valid = true;
  instruction_type type;
  std::string_view error_msg;

public:
  result() = default;
  result(std::vector<item> queried_items);
  // If there was an error
  result(std::string_view msg);
  std::string_view get_error();
  bool is_ok();
};

class header {
private:
  std::vector<std::pair<std::string, std::vector<attributes>>> col_names;
  std::vector<std::pair<col_type, cell::Cell>> col_types;

public:
  header() = default;
  header(std::vector<std::pair<std::string, std::vector<attributes>>> names,
         std::vector<std::pair<col_type, cell::Cell>> types);
  auto get_columns_names();
  auto get_columns_types();
};

class table {
private:
  std::vector<item> rows;
  header cols;
  int row_size;

public:
  auto get_ith_col_name(int i);
  table(header h);
};

class db {
public:
  result execute(std::string_view query);
  std::unordered_map<std::string_view, std::unique_ptr<table>> tables;
  std::string debug_get_table_ith_col_name(std::string name, int i);
};
} // namespace memdb
