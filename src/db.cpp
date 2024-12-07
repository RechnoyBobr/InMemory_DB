#include "db.hpp"
#include "query_parser.hpp"

#include <algorithm>
#include <format>
#include <memory>
#include <ranges>
#include <stack>
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
            if (rows.size() == 0) {
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
            row_to_insert[i] = v;
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
        std::vector<std::shared_ptr<table> > tables_to_operate_with = {};
        // First one is the value of the columns
        op::instruction_operator instruction_root;
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
                            ordered_values = std::vector<cell::Cell>(cur_table->cols.get_columns_names().size());
                        }
                        insert_type = i.insert_type;
                        std::vector<std::pair<cell::col_type, cell::Cell> > columns_type =
                                cur_table->cols.get_columns_types();
                        std::vector<std::pair<std::string, std::vector<ins::attributes> > > columns_names =
                                cur_table->cols.get_columns_names();
                        std::vector<cell::Cell> row_to_insert = std::vector<cell::Cell>(columns_type.size());
                        if (insert_type == -1) {
                            throw std::runtime_error("Invalid keyword combination.\n");
                        }
                        if (insert_type == 2) {
                            for (auto &[k, v]: values_by_name) {
                                size_t index_to_ins = cur_table->cols.col_name_to_ind[k];
                                ordered_values[index_to_ins] = v;
                            }
                        }
                        for (size_t j = 0; j < ordered_values.size(); j++) {
                            check_value(ordered_values[j], columns_type, columns_names, j);
                            insert_value(ordered_values[j], row_to_insert, j, columns_type, cur_table->rows,
                                         columns_names);
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
                        // TODO: Debug the whole thing.
                        std::unordered_map<std::string, std::vector<std::string> > col_to_table_name = i.col_to_tables;
                        std::vector<std::string> cols_to_select = {};
                        std::vector<int> rows_to_select = {};
                        size_t max = 0;
                        for (auto j: tables_to_operate_with) {
                            if (j->rows.size() > max) {
                                max = j->rows.size();
                            }
                        }
                        for (int j = 0; j < max; j++) {
                            for (auto t: tables_to_operate_with) {
                                std::unordered_map<std::string, cell::Cell> cur_row = {};
                                if (j < t->rows.size()) {
                                    for (int c = 0; c < t->rows[j].size(); c++) {
                                        std::string col_name = t->cols.get_columns_names()[c].first;
                                        cur_row[col_name] = t->rows[j][c];
                                    }
                                    cell::Cell result = instruction_root.exec_operator(cur_row);
                                    if (result.get<bool>()) {
                                        rows_to_select.emplace_back(j);
                                    }
                                }
                            }
                        }
                        std::vector<table_view> resulting_vector;
                        int streak = 0;
                        int prev = -1;
                        std::vector<std::vector<int> > rows_from_tables = std::vector<std::vector<int> >(
                            tables_to_operate_with.size());
                        std::vector<std::vector<int> > cols_from_tables = std::vector<std::vector<
                            int> >(tables_to_operate_with.size());
                        for (auto &[k,v]: col_to_table_name) {
                            for (auto &str: v) {
                                int ind = std::ranges::find(tables_to_operate_with.begin(),
                                                            tables_to_operate_with.end(),
                                                            this->tables[str]) - tables_to_operate_with.begin();
                                auto current_table = tables_to_operate_with[ind];
                                cols_from_tables[ind].emplace_back(current_table->cols.col_name_to_ind[k]);
                            }
                        }
                        for (int row_n: rows_to_select) {
                            if (row_n == prev) {
                                streak++;
                            } else {
                                streak = 0;
                            }
                            // If we have 2 same values then it means that  operations from first table was true and from second table too.
                            rows_from_tables[streak].emplace_back(row_n);
                            prev = row_n;
                        }
                        for (int j = 0; j < rows_from_tables.size(); j++) {
                            resulting_vector.emplace_back(tables_to_operate_with[j], rows_from_tables[j],
                                                          cols_from_tables[j]);
                        }
                        ret_val = result(resulting_vector);
                    }
                    case ins::FROM: {
                        for (auto &name: i.table_names) {
                            tables_to_operate_with.emplace_back(tables[name]);
                        }
                        break;
                    }
                    case ins::WHERE: {
                        instruction_root = i.operators;
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

    void db::load_from_file(std::istream &stream) {
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
        max_size = 0;
        for (auto &i: this->result_table_view) {
            if (i.choosed_rows.size() > max_size) {
                max_size = i.choosed_rows.size();
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
        int ind = 0;
        for (auto &i: col_names) {
            col_name_to_ind[i.first] = ind;
            ind++;
        }
        this->col_types = types;
    }

    void table::insert_row(std::vector<cell::Cell> &row) { rows.emplace_back(row); }


    table::table(header &h) { this->cols = h; }

    size_t table_view::size() const {
        return table_src->rows.size();
    }

    table_view::table_view(std::shared_ptr<table> &table_ptr, std::vector<int> &rows, std::vector<int> &cols) {
        this->table_src = table_ptr;
        this->choosed_columns = cols;
        this->choosed_rows = rows;
    }

    void table_view::get_result(size_t cur, std::vector<cell::Cell> &result) {
        for (const auto &j: choosed_columns) {
            result.emplace_back(table_src->rows[choosed_rows[cur]][j]);
        }
    }


    std::vector<std::pair<std::string, std::vector<ins::attributes> > > &header::get_columns_names() {
        return col_names;
    }

    std::vector<std::pair<cell::col_type, cell::Cell> > &header::get_columns_types() { return col_types; }

    auto &table::get_ith_col_name(int i) { return cols.get_columns_names()[i].first; }

    std::string cell_to_output(cell::Cell &x) {
        std::unordered_map<int, std::string> hex_to_string = {
            {10, "A"}, {11, "B"}, {12, "C"}, {13, "D"}, {14, "E"}, {15, "F"}
        };
        switch (x.get_cell_type()) {
            case cell::BOOL: {
                if (x.get<bool>()) {
                    return "true";
                }
                return "false";
                break;
            }
            case cell::INT32: {
                return std::to_string(x.get<int>());
                break;
            }
            case cell::BYTES: {
                std::string ret_val = "0x";
                for (std::byte b: x.get<std::vector<std::byte> >()) {
                    int first_byte = static_cast<int>(b) / 16;
                    int second_byte = static_cast<int>(b) % 16;
                    if (first_byte >= 10) {
                        ret_val += hex_to_string[first_byte];
                    } else {
                        ret_val += std::to_string(first_byte);
                    }
                    if (second_byte >= 10) {
                        ret_val += hex_to_string[second_byte];
                    } else {
                        ret_val += std::to_string(second_byte);
                    }
                }
                return ret_val;
                break;
            }
            case cell::STRING: {
                return x.get<std::string>();
                break;
            }
            default: {
                throw std::runtime_error(
                    "Cannot convert empty cell to string. Error is bogus as there shouldn't be any empty cells in table");
                break;
            }
        }
    }

    void db::save_to_file(std::ostream &stream) {

    }

    std::string &db::debug_get_table_ith_col_name(const std::string &name, int i) {
        return tables[name]->get_ith_col_name(i);
    }
} // namespace memdb
