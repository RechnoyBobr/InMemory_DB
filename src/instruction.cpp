#include "../inc/instruction.hpp"

namespace ins {
    instruction::instruction(const std::unordered_map<std::string, cell::Cell> &v) {
        values_by_name = v;
        type = INSERT;
        insert_type = 2;
    }

    instruction::instruction(const std::queue<cell::Cell> &operands,
                             const std::queue<op::instruction_operator> &operators,
                             instruction_type type) {
        this->operands = operands;
        this->operators = operators;
        this->type = type;
    }

    std::vector<cell::Cell> &instruction::get_ordered_values() {
        return values_ordered;
    }

    std::unordered_map<std::string, cell::Cell> &instruction::get_values_by_name() {
        return values_by_name;
    }

    instruction::instruction(const std::vector<std::string> &col_names) {
        type = FROM;
        table_names = col_names;
        this->type = type;
    }

    instruction::instruction(const std::unordered_map<std::string, std::vector<std::string> > &cols_tables) {
        this->col_to_tables = cols_tables;
        type = SELECT;
    }

    instruction::instruction(const std::vector<cell::Cell> &v) {
        values_ordered = v;
        type = INSERT;
        insert_type = 1;
    }

    // insert_type is equal to -1 to clearly indicate that it isn't supposed to be insert instruction
    instruction::instruction(const std::string &table_to) {
        this->type = TO;
        this->table_name = table_to;
    }


    // If instruction is create table
    instruction::instruction(
        const std::string &table_name,
        const std::vector<std::pair<std::string, std::vector<attributes> > > &names,
        const std::vector<std::pair<cell::col_type, cell::Cell> > &types) {
        this->table_name = table_name;
        this->col_types = types;
        this->col_names = names;
        this->type = CREATE;
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
