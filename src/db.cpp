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
        std::vector<std::pair<std::string, std::vector<attributes> > > names,
        std::vector<std::pair<col_type, cell::Cell> > types) {
        this->table_name = table_name;
        this->col_types = types;
        this->col_names = names;
        this->type = CREATE;
    }

    std::vector<std::pair<std::string, std::vector<attributes> > >
    instruction::get_columns_names() {
        return col_names;
    }

    std::vector<std::pair<col_type, cell::Cell> > instruction::get_columns_types() {
        return col_types;
    }

    instruction_type instruction::get_type() { return type; }

    std::string instruction::get_table_name() { return table_name; }

    result::result(instruction_type t) {
        type = t;
    }

    result::result(std::string_view msg) {
        valid = false;
        this->error_msg = msg;
    }

    bool result::is_ok() { return valid; }

    // WARN: it'd be impossible to iterate over cols without knowing of their type
    // or atleast I think so. (Find some better way to do so)
    result::result(std::vector<item> queried_items) {
        this->queried_cols = queried_items;
    }

    std::string_view result::get_error() { return error_msg; }

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

    std::string &db::debug_get_table_ith_col_name(std::string name, int i) {
        return tables[name]->get_ith_col_name(i);
    }
} // namespace memdb
