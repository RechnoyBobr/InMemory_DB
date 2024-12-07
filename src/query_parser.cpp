#include "../inc/query_parser.hpp"

#include <format>
#include <queue>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

#include "../inc/lexer.hpp"
#include "operator.hpp"

namespace basic_parser {
    cell::Cell query_parser::get_cell(std::string &str, cell::col_type cur_type) {
        cell::Cell ret_value;
        switch (cur_type) {
            case cell::col_type::BYTES: {
                std::vector<std::byte> res = {};
                bool flag = false;
                int cnt_ch = 0;
                int cur_byte = 0;
                for (char ch: str) {
                    if (flag) {
                        cur_byte <<= 4;
                        std::string char_str;
                        char_str += ch;
                        cur_byte += std::stoi(char_str, nullptr, 16);
                        cnt_ch++;
                    }
                    if (ch == 'x') {
                        flag = true;
                    }
                    if (cnt_ch == 2) {
                        res.emplace_back(std::byte(cur_byte));
                        cur_byte = 0;
                        cnt_ch = 0;
                    }
                }
                if (str.size() % 2 != 0) {
                    res.emplace_back(std::byte(cur_byte));
                }
                ret_value = cell::Cell(res);
                break;
            }
            case cell::col_type::STRING: {
                // trim the quotes
                std::string trimmed_str = str.substr(1, str.size() - 2);
                ret_value = cell::Cell(trimmed_str, cell::STRING);
                break;
            }
            case cell::col_type::INT32: {
                ret_value = cell::Cell(std::stoi(str));
                break;
            }
            case cell::col_type::BOOL: {
                if (str == "true") {
                    ret_value = cell::Cell(true);
                } else if (str == "false") {
                    ret_value = cell::Cell(false);
                } else {
                    throw std::runtime_error("Can't recognize cell value");
                }
                break;
            }
            case cell::col_type::EMPTY: {
                break;
            }
            case cell::col_type::COL_NAME: {
                ret_value = cell::Cell(str, cell::COL_NAME);
                break;
            }
            default: {
                throw std::runtime_error("There is no such type. The error is bogus\n");
            }
        }
        return ret_value;
    }

    cell::Cell query_parser::get_cell(std::string &str) {
        cell::col_type type = get_cell_type_by_value(str);
        return get_cell(str, type);
    }

    cell::col_type query_parser::get_cell_type_by_value(std::string tmp) {
        cell::col_type cur_type;
        if (tmp.contains('\"')) {
            cur_type = cell::col_type::STRING;
        } else if (tmp.contains('x')) {
            cur_type = cell::col_type::BYTES;
        } else if (tmp == "true" || tmp == "false") {
            cur_type = cell::col_type::BOOL;
        } else if (tmp[0] == '-' || tmp[0] == '+' || std::isdigit(tmp[0])) {
            cur_type = cell::col_type::INT32;
        } else if (!tmp.empty()) {
            // If type is not recognized program will consider it being column name
            // And maybe it will throw an error of unknown column name
            cur_type = cell::col_type::COL_NAME;
        } else {
            cur_type = cell::col_type::EMPTY;
        }
        return cur_type;
    }

    std::vector<ins::instruction> query_parser::parse(std::string_view query) {
        basic_lexer::lexer l = basic_lexer::lexer();
        auto lexed_commands = l.tokenize(query);
        std::vector<ins::instruction> result = std::vector<ins::instruction>(0);
        parser_state cur_state = TABLE_NAME;
        std::vector<std::string> select_col_names;
        bool select_without_dot = false;
        for (const auto &pair: lexed_commands) {
            ins::instruction_type type = pair.first;
            std::string params = pair.second;
            switch (type) {
                case ins::instruction_type::CREATE: {
                    size_t ind = 0;
                    auto col_names = std::vector<std::pair<std::string, std::vector<ins::attributes> > >(0);
                    std::string table_name;
                    std::vector<std::pair<cell::col_type, cell::Cell> > types = {};
                    std::string tmp;
                    std::vector<ins::attributes> attr;
                    cell::Cell default_val;
                    cell::col_type cur_type;

                    while (ind < params.size()) {
                        if (params[ind] == '(') {
                            table_name = tmp;
                            tmp = "";
                            cur_state = VAR_NAME;
                        } else if (params[ind] == '{') {
                            cur_state = ATTRIBUTES;
                        } else if (params[ind] == ':') {
                            cur_state = VAR_TYPE;
                            col_names.emplace_back(tmp, attr);
                            attr.clear();
                            tmp = "";
                        } else if (params[ind] == '=') {
                            cur_state = DEF_VAL;
                            cur_type = parse_type(tmp);
                            tmp = "";
                        } else if ((params[ind] == ',' || params[ind] == '}') && cur_state == ATTRIBUTES) {
                            if (tmp == "key") {
                                attr.emplace_back(ins::attributes::KEY);
                            } else if (tmp == "autoincrement") {
                                attr.emplace_back(ins::attributes::AUTOINCREMENT);
                            } else if (tmp == "unique") {
                                attr.emplace_back(ins::attributes::UNIQUE);
                            } else {
                                throw std::runtime_error("Can't recognize the attribute");
                            }
                            tmp = "";
                            if (params[ind] == '}') {
                                cur_state = VAR_NAME;
                            }
                        } else if (params[ind] == '}') {
                            cur_state = VAR_NAME;
                        } else if (params[ind] == ',' || params[ind] == ')') {
                            if (cur_state == VAR_TYPE) {
                                // if there is no default value

                                types.emplace_back(parse_type(tmp), cell::Cell());
                                tmp = "";
                            } else if (cur_state == DEF_VAL) {
                                if (tmp.empty()) {
                                    default_val = cell::Cell();
                                } else {
                                    default_val = get_cell(tmp, cur_type);
                                }
                                types.emplace_back(cur_type, default_val);
                                tmp = "";
                            }
                            cur_state = VAR_NAME;
                        } else if (params[ind] != ' ') {
                            tmp += params[ind];
                        }
                        ind++;
                    }
                    ins::instruction table_create = ins::instruction(table_name, col_names, types);
                    result.emplace_back(table_create);
                    break;
                }

                case ins::instruction_type::INSERT: {
                    int ind = 0;
                    std::string tmp;
                    std::unordered_map<std::string, cell::Cell> values_by_name;
                    std::vector<cell::Cell> values_by_order;
                    std::string column_name;
                    ins::instruction insert_c;
                    int insert_type = 1;
                    while (ind < params.size()) {
                        if (params[ind] == '=') {
                            // Then we should save column name
                            column_name = tmp;
                            tmp = "";
                        } else if (params[ind] == ')' || params[ind] == ',') {
                            cell::col_type cur_type = get_cell_type_by_value(tmp);
                            cell::Cell c;
                            if (!tmp.empty()) {
                                c = get_cell(tmp, cur_type);
                            }
                            if (column_name.empty()) {
                                // Then pass the values vector
                                values_by_order.emplace_back(c);
                            } else {
                                insert_type = 2;
                                values_by_name[column_name] = c;
                            }
                            tmp = "";
                        } else if (params[ind] != ' ' && params[ind] != '(') {
                            tmp += params[ind];
                        }
                        ind++;
                    }
                    if (insert_type == 1) {
                        result.emplace_back(values_by_order);
                    } else {
                        result.emplace_back(values_by_name);
                    }
                    break;
                }
                case ins::TO: {
                    int ind = 0;
                    std::string table_to;
                    while (ind < params.size()) {
                        if (params[ind] != ' ') {
                            table_to += params[ind];
                        }
                        ind++;
                    }
                    result.emplace_back(table_to, ins::TO);
                    break;
                }
                case ins::SELECT: {
                    int ind = 0;
                    std::unordered_map<std::string, std::vector<std::string> > cols_to_tables;
                    std::string tmp;
                    std::string col_name;
                    std::string table_name;
                    bool is_col = false;
                    while (ind < params.size()) {
                        if (params[ind] == ',') {
                            if (!is_col && !table_name.empty()) {
                                select_col_names.emplace_back(table_name);
                                table_name = "";
                                ind++;
                                continue;
                            }
                            if (cols_to_tables.contains(col_name)) {
                                cols_to_tables[col_name].emplace_back(table_name);
                            } else {
                                cols_to_tables[col_name] = std::vector<std::string>(1, table_name);
                            }
                            col_name = "";
                            table_name = "";
                        } else if (params[ind] == '.') {
                            is_col = true;
                        } else if (is_col && params[ind] != ' ') {
                            col_name += params[ind];
                        } else if (params[ind] != ' ') {
                            table_name += params[ind];
                        } else if (params[ind] == ' ' && table_name != "") {
                            // When there is no table prefixes like table.column
                            select_col_names.emplace_back(table_name);
                            select_without_dot = true;
                        }

                        ind++;
                    }
                    if (!tmp.empty()) {
                        if (cols_to_tables.contains(col_name)) {
                            cols_to_tables[col_name].emplace_back(table_name);
                        } else {
                            cols_to_tables[col_name] = std::vector(1, table_name);
                        }
                    }
                    if (!cols_to_tables.empty()) {
                        result.emplace_back(cols_to_tables);
                    }
                    break;
                }
                case ins::FROM: {
                    std::unordered_map<std::string, std::vector<std::string> > cols_to_tables;
                    std::vector<std::string> table_names = {};
                    std::string tmp;
                    for (char param: params) {
                        if (param == ',') {
                            table_names.emplace_back(tmp);
                            tmp = "";
                        } else if (param != ' ') {
                            tmp += param;
                        }
                    }
                    if (tmp != "") {
                        if (select_without_dot) {
                            for (auto &x: select_col_names) {
                                cols_to_tables[x].emplace_back(tmp);
                            }
                            result.emplace_back(cols_to_tables);
                        }
                        table_names.emplace_back(tmp);
                    }
                    result.emplace_back(table_names);
                    break;
                }
                case ins::WHERE: {
                    math_engine engine;
                    std::string postfix_expr = engine.to_postfix(params);
                    const std::string op_chars = engine.op_chars;
                    int i = 0;
                    std::string tmp;

                    // queue for operands
                    std::stack<cell::Cell> operands;
                    std::stack<op::instruction_operator> ops;
                    op::instruction_operator v1 = op::instruction_operator();
                    op::instruction_operator v2;
                    while (i < postfix_expr.size()) {
                        if (!op_chars.contains(postfix_expr[i]) && postfix_expr[i] != ' ') {
                            tmp += postfix_expr[i];
                        } else if (postfix_expr[i] != ' ') {
                            // It's an operator
                            cell::Cell c1 = get_cell(tmp);
                            if (tmp == "") {
                                v2 = ops.top();
                                ops.pop();
                            } else {
                                v2 = op::instruction_operator(c1);
                            }
                            tmp = "";
                            tmp += postfix_expr[i];
                            if (i < postfix_expr.size() - 1 && engine.priorities.contains(tmp + postfix_expr[i + 1])) {
                                tmp += postfix_expr[i + 1];
                                i++;
                            }

                            if (tmp == "~" || tmp == "|" || tmp == "!") {
                                ops.emplace(tmp, v2);
                            } else {
                                ops.emplace(tmp, ops.top(), v2);
                            }

                            tmp = "";
                        }
                        i++;
                    }
                    op::instruction_operator res = ops.top();
                    while (!ops.empty()) {
                        ops.pop();
                    }
                    result.emplace_back(res, type);
                    break;
                }
                case ins::UPDATE: {
                    int ind = 0;
                    std::string tmp;
                    while (ind < params.size()) {
                        if (params[ind] != ' ') {
                            tmp += params[ind];
                        }
                        ind++;
                    }
                    result.emplace_back(tmp, ins::UPDATE);
                    break;
                }
                case ins::DELETE: {
                    int ind = 0;
                    std::string tmp;
                    while (ind < params.size()) {
                        if (params[ind] != ' ') {
                            tmp += params[ind];
                        }
                        ind++;
                    }
                    result.emplace_back(tmp, ins::DELETE);
                    break;
                }
                case ins::SET: {

                    break;
                }


                default:
                    throw std::runtime_error("There was an error during the lexing process");
            }
        }
        return result;
    }

    cell::col_type query_parser::parse_type(std::string &input) {
        if (input == "int32") {
            return cell::col_type::INT32;
        }
        if (input == "bool") {
            return cell::col_type::BOOL;
        }
        if (input.contains("string")) {
            return cell::col_type::STRING;
        }
        if (input.contains("byte")) {
            return cell::col_type::BYTES;
        }
        throw std::runtime_error("can't recognize column type");
    }


    std::string query_parser::math_engine::to_postfix(std::string &expr) {
        std::string result;
        std::stack<std::string> ops = std::stack<std::string>();
        std::string tmp_op;
        std::string tmp;
        std::string prev;
        bool is_last_ch_op = false;
        int i = 0;
        bool is_string = false;
        while (i < expr.size()) {
            if (!op_chars.contains(expr[i]) && expr[i] != '(' && expr[i] != ')' && expr[i] != ' ') {
                while (i < expr.size() && !op_chars.contains(expr[i]) && expr[i] != ')') {
                    if (expr[i] == '\"') {
                        is_string = !is_string;
                    }
                    if (expr[i] != ' ' || is_string) {
                        tmp += expr[i];
                    }
                    i++;
                }
                result += tmp + " ";
                tmp = "";
                is_last_ch_op = false;
            }
            if (expr[i] == '(') {
                ops.emplace("(");
            } else if (expr[i] == ')') {
                while (!ops.empty() && ops.top() != "(") {
                    result += ops.top() + " ";
                    ops.pop();
                }
                // Delete ( from stack
                ops.pop();
            } else if (op_chars.contains(expr[i])) {
                tmp_op = expr[i];
                if (i < expr.size() - 1 && priorities.contains(tmp_op + expr[i + 1])) {
                    tmp_op += expr[i + 1];
                    i++;
                } else if (is_last_ch_op && expr[i] == '-') {
                    tmp_op = "~";
                } else if (is_last_ch_op && result.at(result.size() - 2) != '|') {
                    throw std::runtime_error("There are 2 consecutive operators");
                }

                is_last_ch_op = true;
                while (!ops.empty() && priorities[ops.top()] >= priorities[tmp_op]) {
                    result += ops.top() + " ";
                    ops.pop();
                }
                if (result.empty() || result.at(result.size() - 2) != '|' || tmp_op != "|") {
                    ops.emplace(tmp_op);
                }
                tmp_op = "";
            }
            i++;
        }
        while (!ops.empty()) {
            result += ops.top() + " ";
            ops.pop();
        }
        return result;
    }
} // namespace basic_parser
