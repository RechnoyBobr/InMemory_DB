#include "db.hpp"
#include "query_parser.hpp"

#include <algorithm>
#include <format>
#include <memory>
#include <stdexcept>


namespace memdb {
    void db::check_value(cell::Cell &v,
                         std::vector<std::pair<cell::col_type, cell::Cell> > &columns_type,
                         std::vector<std::pair<std::string, std::vector<ins::attributes> > > &columns_names, size_t i) {
        if (v.get_cell_type() == cell::EMPTY && std::ranges::find(
                columns_names[i].second.begin(), columns_names[i].second.end(),
                ins::AUTOINCREMENT) ==
            columns_names[i].second.end() && columns_type[i].second.get_cell_type() == cell::EMPTY) {
            throw std::runtime_error(
                "Can't insert empty value to a column with no default value or autoincrement attribute\n");
        }
        if (v.get_cell_type() != cell::EMPTY && v.get_cell_type() != columns_type[i].first) {
            throw std::runtime_error("The types of value and column are mismatched\n");
        }
    }

    void db::insert_value(
        cell::Cell &v, std::vector<cell::Cell> &row_to_insert, size_t i,
        std::vector<std::pair<cell::col_type, cell::Cell> > &columns_type,
        std::vector<std::vector<cell::Cell> > &rows,

        std::vector<std::pair<std::string, std::vector<ins::attributes> > > &columns_names
    ) {
        if (v.get_cell_type() == cell::EMPTY && std::ranges::find(
                columns_names[i].second.begin(), columns_names[i].second.end(),
                ins::AUTOINCREMENT) != columns_names[i].second.end()) {
            if (i == 0) {
                row_to_insert[i] = cell::Cell(0);
            } else {
                row_to_insert[i] = (*rows.rbegin())[i].copy_and_increment();
            }
        } else if (columns_type[i].second.get_cell_type() != cell::EMPTY && v.get_cell_type() ==
                   cell::EMPTY) {
            row_to_insert[i] = columns_type[i].second;
        }
        row_to_insert[i] = v;
    }

    result db::execute(std::string_view query) {
        basic_parser::query_parser parser;
        std::vector<ins::instruction> res = parser.parse(query);
        std::vector<cell::Cell> ordered_values;
        std::unordered_map<std::string, cell::Cell> values_by_name;
        int insert_type = -1;
        result ret_val;
        try {
            for (auto &i: res) {
                switch (i.get_type()) {
                    case ins::CREATE: {
                        header h(i.get_columns_names(), i.get_columns_types());
                        tables[i.get_table_name()] =
                                std::make_shared<table>(table(h));
                        ret_val = result(i.get_type());
                        break;
                    }
                    case ins::INSERT: {
                        if (i.insert_type == 1) {
                            ordered_values = i.get_ordered_values();
                        } else {
                            values_by_name = i.get_values_by_name();
                        }
                        insert_type = i.insert_type;
                        ret_val = result(i.get_type());
                        break;
                    }
                    case ins::TO: {
                        auto cur_table = this->tables[i.get_table_name()];
                        std::vector<std::pair<cell::col_type, cell::Cell> > columns_type = cur_table->cols.
                                get_columns_types();
                        std::vector<std::pair<std::string, std::vector<ins::attributes> > > columns_names = cur_table->
                                cols.get_columns_names();
                        std::vector<cell::Cell> row_to_insert = std::vector<cell::Cell>(columns_type.size());
                        if (insert_type == 1) {
                            for (size_t i = 0; i < ordered_values.size(); i++) {
                                check_value(ordered_values[i], columns_type, columns_names, i);
                                insert_value(ordered_values[i], row_to_insert, i, columns_type, cur_table->rows,
                                             columns_names
                                );
                            }
                        } else if (insert_type == 2) {
                            for (auto &[k,v]: values_by_name) {
                                size_t index_to_ins = std::ranges::find_if(columns_names.begin(), columns_names.end(),
                                                                           [&k](const std::pair<std::string, std::vector
                                                                       <
                                                                           ins::attributes> > &elem) {
                                                                               return elem.first == k;
                                                                           }) - columns_names.begin();
                                if (index_to_ins == columns_names.size()) {
                                    throw std::runtime_error(std::format("Can't find column with such name: {}", k));
                                }
                                check_value(v, columns_type, columns_names, index_to_ins);
                                insert_value(v, row_to_insert, index_to_ins, columns_type, cur_table->rows,
                                             columns_names
                                );
                            }
                        } else {
                            throw std::runtime_error("Invalid keyword combination.\n");
                        }
                        cur_table->insert_row(row_to_insert);
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

    result::result(ins::instruction_type t) { type = t; }

    result::result(std::string msg) {
        type = ins::ERROR;
        valid = false;
        this->error_msg = std::move(msg);
    }

    bool result::is_ok() const { return valid; }

    // WARN: it'd be impossible to iterate over cols without knowing of their type
    // or atleast I think so. (Find some better way to do so)
    result::result(std::vector<std::vector<cell::Cell> > queried_items) {
        this->queried_cols = std::move(queried_items);
        type = ins::SELECT;
    }

    std::string &result::get_error() { return error_msg; }

    header::header(std::vector<std::pair<std::string, std::vector<ins::attributes> > > &names,
                   std::vector<std::pair<cell::col_type, cell::Cell> > &types) {
        this->col_names = names;
        this->col_types = types;
    }

    void table::insert_row(std::vector<cell::Cell> row) {
        rows.emplace_back(row);
    }


    table::table(header &h) { this->cols = h; }

    std::vector<std::pair<std::string, std::vector<ins::attributes> > > &header::get_columns_names() {
        return col_names;
    }

    std::vector<std::pair<cell::col_type, cell::Cell> > &header::get_columns_types() { return col_types; }

    auto &table::get_ith_col_name(int i) { return cols.get_columns_names()[i].first; }

    std::string &db::debug_get_table_ith_col_name(const std::string &name, int i) {
        return tables[name]->get_ith_col_name(i);
    }
} // namespace memdb
