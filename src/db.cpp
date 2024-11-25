#include "../inc/db.hpp"

#include <memory>
#include <stdexcept>

#include "../inc/query_parser.hpp"

namespace memdb {
result db::execute(std::string_view query) {
  basic_parser::query_parser parser;
  auto res = parser.parse(query);
  try {
    for (auto i : res) {
      switch (i.get_type()) {
        case CREATE: {
          tables[i.get_table_name()] = std::make_unique<table>(
              table(header(i.get_columns_names(), i.get_columns_types())));
          break;
        }
        default: {
          throw std::runtime_error("Unsupported");
        }
      }
    }
  }
}
instruction::instruction(std::vector<cell::Cell> ops,
                         std::vector<op::instruction_operator> op,
                         instruction_type type) {
  this->operands = ops;
  this->operators = op;
  this->type = type;
}
// If instruction is create table
instruction::instruction(
    std::string table_name,
    std::vector<std::pair<std::string, std::vector<attributes>>> names,
    std::vector<std::pair<col_type, cell::Cell>> types) {
  this->table_name = table_name;
  this->col_types = types;
  this->col_names = names;
  this->type = CREATE;
}
std::vector<std::pair<std::string, std::vector<attributes>>>
instruction::get_columns_names() {
  return col_names;
}

std::vector<std::pair<col_type, cell::Cell>> instruction::get_columns_types() {
  return col_types;
}
instruction_type instruction::get_type() { return type; }
std::string instruction::get_table_name() { return table_name; }
}  // namespace memdb
