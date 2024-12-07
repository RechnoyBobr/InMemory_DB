#pragma once

#include <queue>
#include <string>
#include <unordered_map>
#include <vector>
#include "cell.hpp"
#include "operator.hpp"


namespace memdb {
    class db;
}

namespace ins {
    enum attributes {
        KEY,
        AUTOINCREMENT,
        UNIQUE,
    };

    enum instruction_type { SELECT, WHERE, INSERT, UPDATE, CREATE, DELETE, FROM, TO, SET, ERROR };

    class instruction {
        instruction_type type;
        // If type is FROM
        std::vector<std::string> table_names;
        // If type WHERE
        op::instruction_operator operators;
        // If type is create table
        std::vector<std::pair<std::string, std::vector<attributes> > > col_names;
        std::vector<std::pair<cell::col_type, cell::Cell> > col_types;
        // This works as well for type TO
        std::string table_name;
        // If type is insert
        std::vector<cell::Cell> values_ordered;
        std::unordered_map<std::string, cell::Cell> values_by_name;
        // If type is select
        std::unordered_map<std::string, std::vector<std::string> > col_to_tables;

    public:
        int insert_type = -1;

        std::vector<cell::Cell> &get_ordered_values();

        std::unordered_map<std::string, cell::Cell> &get_values_by_name();

        instruction() = default;

        instruction(const std::vector<std::string> &col_names);

        instruction(const std::unordered_map<std::string, std::vector<std::string> > &cols_tables);


        instruction(const std::vector<cell::Cell> &v);

        instruction(const std::string &table_to, instruction_type type);

        instruction(const std::unordered_map<std::string, cell::Cell> &v);

        instruction(op::instruction_operator operators,
                    instruction_type type);


        instruction(const std::string &table_name,
                    const std::vector<std::pair<std::string, std::vector<attributes> > > &names,
                    const std::vector<std::pair<cell::col_type, cell::Cell> > &types);

        instruction_type get_type() const;

        // Can throw runtime error if type not create
        std::vector<std::pair<std::string, std::vector<attributes> > > &get_columns_names();

        std::vector<std::pair<cell::col_type, cell::Cell> > &get_columns_types();

        std::string &get_table_name();

        friend memdb::db;
    };
} // namespace ins
