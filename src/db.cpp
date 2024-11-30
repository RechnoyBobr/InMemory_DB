#include "db.hpp"
#include "query_parser.hpp"

#include <algorithm>
#include <format>
#include <memory>
#include <ranges>
#include <stdexcept>


namespace memdb {
    void db::check_value(cell::Cell &v, std::vector<std::pair<cell::col_type, cell::Cell> > &columns_type,
                         std::vector<std::pair<std::string, std::vector<ins::attributes> > > &columns_names, size_t i) {
        if (v.get_cell_type() == cell::EMPTY &&
            std::ranges::find(columns_names[i].second.begin(), columns_names[i].second.end(), ins::AUTOINCREMENT) ==
            columns_names[i].second.end() &&
            columns_type[i].second.get_cell_type() == cell::EMPTY) {
            throw std::runtime_error(
                "Can't insert empty value to a column with no default value or autoincrement attribute\n");
        }
        if (v.get_cell_type() != cell::EMPTY && v.get_cell_type() != columns_type[i].first) {
            throw std::runtime_error("The types of value and column are mismatched\n");
        }
    }

    void db::insert_value(cell::Cell &v, std::vector<cell::Cell> &row_to_insert, size_t i,
                          std::vector<std::pair<cell::col_type, cell::Cell> > &columns_type,
                          std::vector<std::vector<cell::Cell> > &rows,

                          std::vector<std::pair<std::string, std::vector<ins::attributes> > > &columns_names) {
        if (v.get_cell_type() == cell::EMPTY &&
            std::ranges::find(columns_names[i].second.begin(), columns_names[i].second.end(), ins::AUTOINCREMENT) !=
            columns_names[i].second.end()) {
            if (i == 0) {
                row_to_insert[i] = cell::Cell(0);
            } else {
                row_to_insert[i] = (*rows.rbegin())[i].copy_and_increment();
            }
        } else if (std::ranges::find(columns_names[i].second.begin(), columns_names[i].second.end(), ins::UNIQUE) !=
                   columns_names[i].second.end() ||
                   std::ranges::find(columns_names[i].second.begin(), columns_names[i].second.end(), ins::KEY) !=
                   columns_names[i].second.end()) {
            for (int j = 0; j < rows.size(); j++) {
                if (rows[j][i] == v) {
                    throw std::runtime_error("Column have attributes key/unique");
                }
            }
        } else if (columns_type[i].second.get_cell_type() != cell::EMPTY && v.get_cell_type() == cell::EMPTY) {
            row_to_insert[i] = columns_type[i].second;
        } else {
            row_to_insert[i] = v;
        }
    }

    result db::execute(std::string_view query) {
        std::vector<ins::instruction> res = basic_parser::query_parser::parse(query);
        std::ranges::reverse(res.begin(), res.end());
        std::shared_ptr<table> cur_table;
        std::vector<int> cols_to_select = {};
        std::vector<std::shared_ptr<table> > tables_to_operate_with = {};
        int insert_type = -1;
        result ret_val;
        try {
            for (auto &i: res) {
                switch (i.get_type()) {
                    case ins::CREATE: {
                        header h(i.get_columns_names(), i.get_columns_types());
                        tables[i.get_table_name()] = std::make_shared<table>(table(h));
                        ret_val = result(i.get_type());
                        break;
                    }
                    case ins::INSERT: {
                        std::vector<cell::Cell> ordered_values;
                        std::unordered_map<std::string, cell::Cell> values_by_name;
                        if (i.insert_type == 1) {
                            ordered_values = i.get_ordered_values();
                        } else {
                            values_by_name = i.get_values_by_name();
                        }
                        insert_type = i.insert_type;
                        std::vector<std::pair<cell::col_type, cell::Cell> > columns_type =
                                cur_table->cols.get_columns_types();
                        std::vector<std::pair<std::string, std::vector<ins::attributes> > > columns_names =
                                cur_table->cols.get_columns_names();
                        std::vector<cell::Cell> row_to_insert = std::vector<cell::Cell>(columns_type.size());
                        if (insert_type == 1) {
                            for (size_t j = 0; j < ordered_values.size(); j++) {
                                check_value(ordered_values[j], columns_type, columns_names, j);
                                insert_value(ordered_values[j], row_to_insert, j, columns_type, cur_table->rows,
                                             columns_names);
                            }
                        } else if (insert_type == 2) {
                            for (auto &[k, v]: values_by_name) {
                                size_t index_to_ins =
                                        std::ranges::find_if(
                                            columns_names.begin(), columns_names.end(),
                                            [&k](const std::pair<std::string, std::vector<ins::attributes> > &elem) {
                                                return elem.first == k;
                                            }) -
                                        columns_names.begin();
                                if (index_to_ins == columns_names.size()) {
                                    throw std::runtime_error(std::format("Can't find column with such name: {}", k));
                                }
                                check_value(v, columns_type, columns_names, index_to_ins);
                                insert_value(v, row_to_insert, index_to_ins, columns_type, cur_table->rows,
                                             columns_names);
                            }
                        } else {
                            throw std::runtime_error("Invalid keyword combination.\n");
                        }
                        cur_table->insert_row(row_to_insert);
                        ret_val = result(i.get_type());
                        break;
                    }
                    case ins::TO: {
                        cur_table = this->tables[i.get_table_name()];
                        break;
                    }
                    case ins::SELECT: {
                        std::unordered_map<std::string, std::vector<std::string> > col_to_table_name = i.col_to_tables;
                    }
                    case ins::FROM: {
                        for (auto &name: i.table_names) {
                            tables_to_operate_with.emplace_back(tables[name]);
                        }
                        break;
                    }
                    case ins::WHERE: {
                        std::unique_ptr<std::queue<cell::Cell> > operands = std::make_unique<std::queue<cell::Cell> >(
                            i.operands);
                        std::unique_ptr<std::queue<op::instruction_operator> > operators = std::make_unique<std::queue<
                            op::instruction_operator> >(i.operators);
                        std::vector<table_view> views;
                        // TODO: execute every operator with given operands. If operand of column type then proceed to
                        // executing operator with every cell of that column.
                        // (Optimization: evaluate every constant operations(without need to iterate through columns)

                        break;
                    }
                    default: {
                        throw std::runtime_error("Unsupported");
                    }
                }
            }
        } catch (std::runtime_error &e) {
            ret_val = result(e.what());
        }
        return ret_val;
    }


    result::iterator::iterator(std::vector<table_view> &views, size_t current_col) {
        sources = views;
        this->current_col = current_col;
    }

    bool result::iterator::operator==(const iterator &other) const {
        return current_col == other.current_col;
    }

    bool result::iterator::operator!=(const iterator &other) const {
        return current_col != other.current_col;
    }

    result::iterator &result::iterator::operator++() {
        current_col++;
        return *this;
    }

    std::vector<cell::Cell> *result::iterator::operator->() {
        resulting_vector.clear();
        for (auto &src: sources) {
            if (current_col >= src.size()) {
                continue;
            }
            src.get_result(current_col, resulting_vector);
        }
        return &resulting_vector;
    }

    std::vector<cell::Cell> &result::iterator::operator*() {
        resulting_vector.clear();
        for (auto &src: sources) {
            if (current_col >= src.size()) {
                continue;
            }
            src.get_result(current_col, resulting_vector);
        }
        return resulting_vector;
    }

    result::iterator result::begin() {
        return {result_table_view};
    }

    result::iterator result::end() {
        return {result_table_view, max_size};
    }

    result::result(ins::instruction_type t) {
        type = t;
    }

    result::result(std::vector<table_view> views) {
        this->result_table_view = views;
        for (auto &i: result_table_view) {
            if (max_size < i.size()) {
                max_size = i.size();
            }
        }
    }

    result::result(std::string msg) {
        type = ins::ERROR;
        valid = false;
        this->error_msg = std::move(msg);
    }

    bool result::is_ok() const { return valid; }


    std::string &result::get_error() { return error_msg; }

    header::header(std::vector<std::pair<std::string, std::vector<ins::attributes> > > &names,
                   std::vector<std::pair<cell::col_type, cell::Cell> > &types) {
        this->col_names = names;
        this->col_types = types;
    }

    void table::insert_row(std::vector<cell::Cell> &row) { rows.emplace_back(row); }


    table::table(header &h) { this->cols = h; }

    size_t table_view::size() const {
        return table_src->rows.size();
    }

    table_view::table_view(std::shared_ptr<table> &table_ptr, std::vector<int> &cols) {
        this->table_src = table_ptr;
        this->choosed_columns = cols;
    }

    void table_view::get_result(size_t cur, std::vector<cell::Cell> &result) {
        for (const int i: choosed_columns) {
            result.emplace_back(table_src->rows[cur][i]);
        }
    }


    std::vector<std::pair<std::string, std::vector<ins::attributes> > > &header::get_columns_names() {
        return col_names;
    }

    std::vector<std::pair<cell::col_type, cell::Cell> > &header::get_columns_types() { return col_types; }

    auto &table::get_ith_col_name(int i) { return cols.get_columns_names()[i].first; }

    std::string &db::debug_get_table_ith_col_name(const std::string &name, int i) {
        return tables[name]->get_ith_col_name(i);
    }
} // namespace memdb
