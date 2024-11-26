#include "../inc/db.hpp"

#include <memory>
#include <stdexcept>

#include "../inc/query_parser.hpp"

namespace memdb {
    result db::execute(std::string_view query) {
        basic_parser::query_parser parser;
        auto res = parser.parse(query);
        result ret_val;
        try {
            for (auto &i: res) {
                switch (i.get_type()) {
                    case CREATE: {
                        tables[i.get_table_name()] = std::make_unique<table>(
                            table(header(i.get_columns_names(), i.get_columns_types())));
                        ret_val = result(i.get_type());
                        break;
                    }
                    case INSERT: {
                        if (i.)
                    }
                    default: {
                        throw std::runtime_error("Unsupported");
                    }
                }
            }
        } catch (std::runtime_error e) {
            ret_val = result(e.what());
        }
        return ret_val;
    }

    instruction::instruction(std::unordered_map<std::string, cell::Cell> v) {
        values_by_name = std::move(v);
        type = INSERT;
        insert_type = 2;
    }

    instruction::instruction(std::vector<cell::Cell> v) {
        values_ordered = std::move(v);
        type = INSERT;
        insert_type = 2;
    }


    instruction::instruction(std::vector<cell::Cell> ops,
                             std::vector<op::instruction_operator> op,
                             instruction_type type) {
        this->operands = std::move(ops);
        this->operators = std::move(op);
        this->type = type;
    }

    // If instruction is create table
    instruction::instruction(
        std::string table_name,
        std::vector<std::pair<std::string, std::vector<attributes> > > names,
        std::vector<std::pair<col_type, cell::Cell> > types) {
        this->table_name = table_name;
        this->col_types = types;
        this->col_names = names;
        this->type = CREATE;
    }

    std::vector<std::pair<std::string, std::vector<attributes>>> &instruction::get_columns_names() {
        return col_names;
    }

    std::vector<std::pair<col_type, cell::Cell>> &instruction::get_columns_types() {
        return col_types;
    }

    instruction_type instruction::get_type() const { return type; }

    std::string &instruction::get_table_name() { return table_name; }

    result::result(instruction_type t) {
        type = t;
    }

    result::result(std::string msg) {
        type = ERROR;
        valid = false;
        this->error_msg = std::move(msg);
    }

    bool result::is_ok() const { return valid; }

    // WARN: it'd be impossible to iterate over cols without knowing of their type
    // or atleast I think so. (Find some better way to do so)
    result::result(std::vector<item> queried_items) {
        this->queried_cols = std::move(queried_items);
        type = SELECT;
    }

    std::string &result::get_error() { return error_msg; }

    header::header(
        std::vector<std::pair<std::string, std::vector<attributes> > > names,
        std::vector<std::pair<col_type, cell::Cell> > types) {
        this->col_names = names;
        this->col_types = types;
    }

    table::table(header h) { this->cols = h; }

    auto &header::get_columns_names() { return col_names; }
    auto &header::get_columns_types() { return col_types; }

    auto &table::get_ith_col_name(int i) {
        return cols.get_columns_names()[i].first;
    }

    std::string &db::debug_get_table_ith_col_name(const std::string& name, int i) {
        return tables[name]->get_ith_col_name(i);
    }
} // namespace memdb
