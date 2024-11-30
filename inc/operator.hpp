#pragma once
#include <cstddef>
#include <string_view>
#include <variant>
#include <vector>

namespace op {
    class instruction_operator {
    private:
        enum op_type {
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
        instruction_operator(std::string &query);

        std::variant<std::string, int> exec_op(std::string &s1, std::string &s2) const;

        int exec_op(int i1, int i2);

        bool exec_op(bool b1, bool b2);

        int exec_op(std::vector<std::byte> &bytes1, std::vector<std::byte> &bytes2) const;
        bool is_single() const;
    };
} // namespace op
