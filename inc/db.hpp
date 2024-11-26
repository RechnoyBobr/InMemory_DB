#pragma once

#include <memory>
#include <string_view>
#include <unordered_map>
#include <vector>
#include "instruction.hpp"


#include "cell.hpp"

namespace memdb {
    class db;
    class table;

    // TODO: Create iterator for result class
    class result {
    private:
        std::vector<std::vector<cell::Cell> > queried_cols;
        bool valid = true;
        ins::instruction_type type;
        std::string error_msg;

    public:
        result() = default;

        result(ins::instruction_type t);

        result(std::vector<std::vector<cell::Cell> > queried_items);

        // If there was an error
        result(std::string msg);

        std::string &get_error();

        bool is_ok() const;
    };

    class header {
    private:
        std::vector<std::pair<std::string, std::vector<ins::attributes> > > col_names;
        std::vector<std::pair<cell::col_type, cell::Cell> > col_types;

    public:
        header() = default;

        header(std::vector<std::pair<std::string, std::vector<ins::attributes> > > &names,
               std::vector<std::pair<cell::col_type, cell::Cell> > &types);

        std::vector<std::pair<std::string, std::vector<ins::attributes>>> &get_columns_names();

        std::vector<std::pair<cell::col_type, cell::Cell>> &get_columns_types();

        friend table;
    };

    class table {
    private:
        std::vector<std::vector<cell::Cell> > rows = {};
        header cols;
        int row_size = 0;

    public:
        auto &get_ith_col_name(int i);

        void insert_row(std::vector<cell::Cell> row);

        table(header &h);

        friend db;
    };

    class db {
        std::unordered_map<std::string, std::shared_ptr<table> > tables;

    public:
        void check_value(
            cell::Cell &v,
            std::vector<std::pair<cell::col_type, cell::Cell>> &columns_type, std::vector<std::pair<std::string, std::vector<ins
            ::attributes>>> &columns_names, size_t i);

        void insert_value(
            cell::Cell &v, std::vector<cell::Cell> &row_to_insert, size_t i, std::vector<std::pair<cell::col_type, cell::Cell>>
            &columns_type, std::vector<std::vector<cell::Cell>> &rows, std::vector<std::pair<std::string, std::vector<ins::
            attributes>>>
            &columns_names);

        result execute(std::string_view query);

        void load_from_file(std::istream);

        void save_to_file(std::ostream);

        std::string &debug_get_table_ith_col_name(const std::string &name, int i);
    };
} // namespace memdb
