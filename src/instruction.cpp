#include "../inc/instruction.hpp"

namespace ins {
    instruction::instruction(std::unordered_map<std::string, cell::Cell> &v) {
        values_by_name = std::move(v);
        type = INSERT;
        insert_type = 2;
    }

    std::vector<cell::Cell> &instruction::get_ordered_values() {
        return values_ordered;
    }

    std::unordered_map<std::string, cell::Cell> &instruction::get_values_by_name() {
        return values_by_name;
    }

    instruction::instruction(std::vector<cell::Cell> v) {
        values_ordered = std::move(v);
        type = INSERT;
        insert_type = 1;
    }
// insert_type is equal to -1 to clearly indicate that it isn't supposed to be insert instruction
    instruction::instruction(const std::string &table_to, instruction_type type) {
        this->type = type;
        this->table_name = table_to;
        this->insert_type = -1;
    }

    instruction::instruction(std::vector<cell::Cell> &ops,
                             std::vector<op::instruction_operator> &op,
                             instruction_type type) {
        this->operands = std::move(ops);
        this->operators = std::move(op);
        this->type = type;
        this->insert_type = -1;
    }

    // If instruction is create table
    instruction::instruction(
        std::string &table_name,
        std::vector<std::pair<std::string, std::vector<attributes> > > &names,
        std::vector<std::pair<cell::col_type, cell::Cell> > &types) {
        this->table_name = table_name;
        this->col_types = types;
        this->col_names = names;
        this->type = CREATE;
        this->insert_type = -1;
    }

    std::vector<std::pair<std::string, std::vector<attributes> > > &instruction::get_columns_names() {
        return col_names;
    }

    std::vector<std::pair<cell::col_type, cell::Cell> > &instruction::get_columns_types() {
        return col_types;
    }

    instruction_type instruction::get_type() const { return type; }

    std::string &instruction::get_table_name() { return table_name; }
}
