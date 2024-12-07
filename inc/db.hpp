#pragma once

#include <memory>
#include <string_view>
#include <unordered_map>
#include <vector>
#include "instruction.hpp"


#include "cell.hpp"

namespace memdb {
    class table_view;
    class db;
    class table;
    class result;


    class header {
        std::vector<std::pair<std::string, std::vector<ins::attributes> > > col_names;
        std::vector<std::pair<cell::col_type, cell::Cell> > col_types;
        std::unordered_map<std::string, int> col_name_to_ind;

    public:
        header() = default;

        header(std::vector<std::pair<std::string, std::vector<ins::attributes> > > &names,
               std::vector<std::pair<cell::col_type, cell::Cell> > &types);

        std::vector<std::pair<std::string, std::vector<ins::attributes> > > &get_columns_names();

        std::vector<std::pair<cell::col_type, cell::Cell> > &get_columns_types();

        friend table;
        friend db;
    };

    class table {
        std::vector<std::vector<cell::Cell> > rows = {};
        header cols;
        int row_size = 0;

    public:
        auto &get_ith_col_name(int i);

        void insert_row(std::vector<cell::Cell> &row);

        table(header &h);

        friend db;
        friend table_view;
        friend result;
    };

    class table_view {
        std::shared_ptr<table> table_src = nullptr;
        std::vector<int> choosed_rows;
        std::vector<int> choosed_columns;
        friend result;

    public:
        table_view() = default;

        size_t size() const;

        table_view(std::shared_ptr<table> &table_ptr, std::vector<int> &rows, std::vector<int> &cols);

        void get_result(size_t cur, std::vector<cell::Cell> &result);
    };


    class result {
        std::vector<table_view> result_table_view;
        bool valid = true;
        ins::instruction_type type;
        std::string error_msg;
        size_t max_size = 0;

    public:
        class iterator {
            using iterator_category = std::forward_iterator_tag;
            using value_type = std::vector<cell::Cell>;
            using difference_type = std::ptrdiff_t;
            using pointer = value_type *;
            using reference = value_type &;
            size_t current_col = 0;
            std::vector<cell::Cell> resulting_vector = {};
            std::vector<table_view> sources;

        public:
            iterator() = default;

            iterator(std::vector<table_view> &views, size_t current_col = 0);

            bool operator==(const iterator &other) const;

            bool operator!=(const iterator &) const;

            iterator &operator++();

            std::vector<cell::Cell> *operator->();

            std::vector<cell::Cell> &operator*();
        };

        result() = default;

        iterator begin();

        iterator end();

        result(ins::instruction_type t);

        result(std::vector<table_view> views);

        // If there was an error
        result(std::string msg);

        std::string &get_error();

        bool is_ok() const;
    };


    class db {
        std::unordered_map<std::string, std::shared_ptr<table> > tables;

    public:
        void check_value(cell::Cell &v, std::vector<std::pair<cell::col_type, cell::Cell> > &columns_type,
                         std::vector<std::pair<std::string, std::vector<ins::attributes> > > &columns_names, size_t i);

        void insert_value(cell::Cell &v, std::vector<cell::Cell> &row_to_insert, size_t i,
                          std::vector<std::pair<cell::col_type, cell::Cell> > &columns_type,
                          std::vector<std::vector<cell::Cell> > &rows,
                          std::vector<std::pair<std::string, std::vector<ins::attributes> > > &columns_names);

        result execute(std::string_view query);

        void load_from_file(std::istream &stream);

        void save_to_file(std::ostream &stream);

        std::string &debug_get_table_ith_col_name(const std::string &name, int i);
    };
} // namespace memdb
