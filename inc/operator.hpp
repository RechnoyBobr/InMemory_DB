#pragma once
#include <cstddef>
#include <memory>
#include <unordered_map>
#include <vector>

#include "cell.hpp"

namespace op {
    class instruction_operator {
        std::shared_ptr<instruction_operator> v1, v2;
        cell::Cell v;

        enum op_type {
            VALUE,
            ADD,
            SUB,
            MUL,
            DIV,
            REM,
            L,
            G,
            LT,
            GT,
            EQ,
            NEQ,
            AND,
            OR,
            INV,
            XOR,
            SIZE,
            NEG,
        };

        op_type type;

    public:
        instruction_operator();

        instruction_operator(cell::Cell &value);

        instruction_operator(std::string &query, instruction_operator value1, instruction_operator value2);

        instruction_operator(std::string &query, instruction_operator value1);

        cell::Cell exec_operator(std::unordered_map<std::string, cell::Cell> &line);

        bool check_for_substitution() const;

        cell::Cell exec_op(std::string &s1, std::string &s2) const;

        cell::Cell exec_op(int i1, int i2);

        cell::Cell exec_op(bool b1, bool b2);

        cell::Cell exec_op(std::vector<std::byte> &bytes1, std::vector<std::byte> &bytes2) const;

        bool is_single() const;
    };
} // namespace op
