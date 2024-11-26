#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "cell.hpp"
#include "operator.hpp"


namespace ins {

    enum attributes {
        KEY,
        AUTOINCREMENT,
        UNIQUE,
    };

    enum instruction_type { SELECT, WHERE, INSERT, UPDATE, CREATE, DELETE, FROM, TO, SET, ERROR };

    class instruction {
        instruction_type type;

        // If type WHERE
        std::vector<cell::Cell> operands;
        std::vector<op::instruction_operator> operators;
        // If type is create table
        std::vector<std::pair<std::string, std::vector<attributes>>> col_names;
        std::vector<std::pair<cell::col_type, cell::Cell>> col_types;
        // This works as well for type TO
        std::string table_name;
        // If type is insert
        std::vector<cell::Cell> values_ordered;
        std::unordered_map<std::string, cell::Cell> values_by_name;

    public:
        int insert_type;

        std::vector<cell::Cell> &get_ordered_values();
        std::unordered_map<std::string, cell::Cell> &get_values_by_name();

        instruction() = default;

        instruction(std::vector<cell::Cell> v);

        instruction(const std::string &table_to, instruction_type type);

        instruction(std::unordered_map<std::string, cell::Cell> &v);

        instruction(std::vector<cell::Cell> &ops, std::vector<op::instruction_operator> &op, instruction_type type);

        instruction(std::string &table_name, std::vector<std::pair<std::string, std::vector<attributes>>> &names,
                    std::vector<std::pair<cell::col_type, cell::Cell>> &types);

        instruction_type get_type() const;

        // Can throw runtime error if type not create
        std::vector<std::pair<std::string, std::vector<attributes>>> &get_columns_names();

        std::vector<std::pair<cell::col_type, cell::Cell>> &get_columns_types();

        std::string &get_table_name();
    };
} // namespace ins
