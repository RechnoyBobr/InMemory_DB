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
        SET,
        ERROR
    };

    class instruction {
        instruction_type type;

        // If type WHERE
        std::vector<cell::Cell> operands;
        std::vector<op::instruction_operator> operators;
        // If type is create table
        std::vector<std::pair<std::string, std::vector<attributes> > > col_names;
        std::vector<std::pair<col_type, cell::Cell> > col_types;
        std::string table_name;
        // If type is insert
        std::vector<cell::Cell> values_ordered;
        std::unordered_map<std::string, cell::Cell> values_by_name;
        int insert_type;

    public:
        instruction() = default;

        instruction(std::vector<cell::Cell> v);

        instruction(std::unordered_map<std::string, cell::Cell> v);

        instruction(std::vector<cell::Cell> ops,
                    std::vector<op::instruction_operator> op, instruction_type type);

        instruction(
            std::string table_name,
            std::vector<std::pair<std::string, std::vector<attributes> > > names,
            std::vector<std::pair<col_type, cell::Cell> > types);

        instruction_type get_type() const;

        // Can throw runtime error if type not create
        std::vector<std::pair<std::string, std::vector<attributes> > >
        &get_columns_names();

        std::vector<std::pair<col_type, cell::Cell> > &get_columns_types();

        std::string &get_table_name();
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
        std::string error_msg;

    public:
        result() = default;

        result(instruction_type t);

        result(std::vector<item> queried_items);

        // If there was an error
        result(std::string msg);

        std::string &get_error();

        bool is_ok() const;
    };

    class header {
    private:
        std::vector<std::pair<std::string, std::vector<attributes> > > col_names;
        std::vector<std::pair<col_type, cell::Cell> > col_types;

    public:
        header() = default;

        header(std::vector<std::pair<std::string, std::vector<attributes> > > names,
               std::vector<std::pair<col_type, cell::Cell> > types);

        auto &get_columns_names();

        auto &get_columns_types();
    };

    class table {
    private:
        std::vector<item> rows;
        header cols;
        int row_size = 0;

    public:
        auto &get_ith_col_name(int i);

        table(header h);
    };

    class db {
        std::unordered_map<std::string, std::unique_ptr<table> > tables;

    public:
        result execute(std::string_view query);

        std::string &debug_get_table_ith_col_name(const std::string& name, int i);
    };
} // namespace memdb
