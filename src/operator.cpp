#include "../inc/operator.hpp"

#include <stdexcept>

#include "cell.hpp"

namespace op {
    instruction_operator::instruction_operator(cell::Cell &value1) {
        v = value1;
        v1 = nullptr;
        v2 = nullptr;
        type = VALUE;
    }

    instruction_operator::instruction_operator() {
        v1 = nullptr;
        v2 = nullptr;
        v = cell::Cell();
        type = VALUE;
    }

    instruction_operator::instruction_operator(std::string &query, instruction_operator value1,
                                               instruction_operator value2) {
        v1 = std::make_shared<instruction_operator>(value1);
        v2 = std::make_shared<instruction_operator>(value2);
        v = cell::Cell();
        if (query.contains("+")) {
            type = ADD;
        } else if (query.contains("-")) {
            type = SUB;
        } else if (query.contains("*")) {
            type = MUL;
        } else if (query.contains("/")) {
            type = DIV;
        } else if (query.contains("%")) {
            type = REM;
        } else if (query.contains("<=")) {
            type = LT;
        } else if (query.contains(">=")) {
            type = GT;
        } else if (query.contains("<")) {
            type = L;
        } else if (query.contains(">")) {
            type = G;
        } else if (query.contains("!=")) {
            type = NEQ;
        } else if (query.contains("=")) {
            type = EQ;
        } else if (query.contains("!")) {
            type = INV;
        } else if (query.contains("&&")) {
            type = AND;
        } else if (query.contains("||")) {
            type = OR;
        } else if (query.contains("|")) {
            type = SIZE;
        } else if (query.contains("^")) {
            type = XOR;
        } else if (query.contains("~")) {
            type = NEG;
        }
    }

    instruction_operator::instruction_operator(std::string &query, instruction_operator value1) {
        v1 = std::make_shared<instruction_operator>(value1);
        if (query.contains("~")) {
            type = INV;
        } else if (query.contains("|")) {
            type = SIZE;
        } else if (query.contains("!")) {
            type = NEG;
        }
    }


    cell::Cell instruction_operator::exec_op(
        std::string &s1, std::string &s2) const {
        switch (type) {
            case VALUE:
                return cell::Cell(s1, cell::STRING);
            case L:
                return s1 < s2;
            case G:
                return s1 > s2;
            case LT:
                return s1 <= s2;
            case GT:
                return s1 >= s2;
            case EQ:
                return s1 == s2;
            case NEQ:
                return s1 != s2;
            case SIZE:
                return static_cast<int>(s1.size());
            case ADD: {
                std::string res = s1 + s2;
                return cell::Cell(res, cell::STRING);
            }
            default:
                return cell::Cell(s1, cell::STRING);
        }
    }

    cell::Cell instruction_operator::exec_op(std::vector<std::byte> &bytes1,
                                             std::vector<std::byte> &bytes2) const {
        switch (type) {
            case L:
                return bytes1 < bytes2;
            case G:
                return bytes1 > bytes2;
            case LT:
                return bytes1 <= bytes2;
            case GT:
                return bytes1 >= bytes2;
            case EQ:
                return bytes1 == bytes2;
            case NEQ:
                return bytes1 != bytes2;
            case SIZE:
                return static_cast<int>(bytes1.size());
            default:
                throw std::runtime_error(
                    "Array of bytes does not support that operation");
        }
    };

    cell::Cell instruction_operator::exec_op(int i1, int i2) {
        switch (type) {
            case ADD:
                return i1 + i2;
            case SUB:
                return i1 - i2;
            case MUL:
                return i1 * i2;
            case DIV:
                return i1 / i2;
            case REM:
                return i1 % i2;
            case L:
                return i1 < i2;
            case G:
                return i1 > i2;
            case GT:
                return i1 >= i2;
            case LT:
                return i1 <= i2;
            case EQ:
                return i1 == i2;
            case NEQ:
                return i1 != i2;
            default:
                throw std::runtime_error("Int type does not support that operation");
        }
    }

    cell::Cell instruction_operator::exec_op(bool b1, bool b2) {
        switch (type) {
            case L:
                return b1 < b2;
            case G:
                return b1 > b2;
            case LT:
                return b1 <= b2;
            case GT:
                return b1 >= b2;
            case EQ:
                return b1 == b2;
            case NEQ:
                return b1 != b2;
            case AND:
                return b1 && b2;
            case OR:
                return b1 || b2;
            case INV:
                return !b1;
            case XOR:
                return b1 ^ b2;
            default:
                throw std::runtime_error("Bool does not support that operation");
        }
    }

    cell::Cell instruction_operator::exec_operator(std::unordered_map<std::string, cell::Cell> &line) {
        if (type == VALUE) {
            return v;
        }

        cell::Cell first_val;
        first_val = v1->exec_operator(line);
        if (first_val.get_cell_type() == cell::COL_NAME) {
            if (!line.contains(first_val.get<std::string>())) {
                throw std::runtime_error("There are no such column name in the table\n");
            }
            first_val = line[first_val.get<std::string>()];
        }

        cell::Cell second_val;
        bool is_single_op = false;
        if (v2 != nullptr) {
            second_val = v2->exec_operator(line);

            if (first_val.get_cell_type() != second_val.get_cell_type() && !this->is_single()) {
                throw std::runtime_error("Can't cast one type to another");
            }
        } else {
            is_single_op = true;
        }
        switch (first_val.get_cell_type()) {
            case cell::INT32: {
                if (is_single_op) {
                    second_val = cell::Cell(0);
                }
                return exec_op(first_val.get<int>(), second_val.get<int>());
            }
            case cell::BOOL: {
                if (is_single_op) {
                    second_val = cell::Cell(false);
                }
                return exec_op(first_val.get<bool>(), second_val.get<bool>());
            }
            case cell::BYTES: {
                if (is_single_op) {
                    std::vector<std::byte> empty_vec = std::vector<std::byte>(0);
                    second_val = cell::Cell(empty_vec);
                }
                std::vector<std::byte> first = first_val.get<std::vector<std::byte> >();
                std::vector<std::byte> second = second_val.get<std::vector<std::byte> >();

                return exec_op(first, second);
            }
            case cell::STRING: {
                std::string s1 = first_val.get<std::string>();
                std::string s2 = "";
                if (!is_single_op) {
                    s2 = second_val.get<std::string>();
                }
                return exec_op(s1, s2);
            }
            default: {
                throw std::runtime_error("Can't execute operation on empty value or column name\n");
            }
        }
    }

    bool instruction_operator::is_single() const {
        if (type == INV || type == SIZE || type == NEG) {
            return true;
        }
        return false;
    }
} // namespace op
//
