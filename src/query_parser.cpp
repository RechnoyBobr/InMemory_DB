#include "../inc/query_parser.hpp"

#include <stdexcept>
#include <string>
#include <string_view>
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
                ret_value = cell::Cell(trimmed_str);
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
                    throw std::runtime_error("Can't recognize default value");
                }
            }
            case cell::col_type::EMPTY: {
                return {};
            }
            default: {
                throw std::runtime_error("There is no such type. The error is bogus\n");
            }
        }
        return ret_value;
    }

    std::vector<ins::instruction> query_parser::parse(std::string_view query) {
        basic_lexer::lexer l = basic_lexer::lexer();
        auto lexed_commands = l.tokenize(query);
        // TODO: there is quite a large mess.
        // the comma are only met in attributes or different cols
        std::vector<ins::instruction> result = std::vector<ins::instruction>(0);
        parser_state cur_state = TABLE_NAME;
        for (const auto &pair: lexed_commands) {
            ins::instruction_type type = pair.first;
            std::string params = pair.second;
            switch (type) {
                case ins::instruction_type::CREATE: {
                    size_t ind = 0;
                    auto col_names = std::vector<std::pair<std::string, std::vector<ins::attributes>>>(0);
                    std::string table_name;
                    std::vector<std::pair<cell::col_type, cell::Cell>> types = {};
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
                        }
                        if (params[ind] == ')' || params[ind] == ',') {
                            cell::col_type cur_type;
                            if (tmp.contains('\"')) {
                                cur_type = cell::col_type::STRING;
                            } else if (tmp.contains('x')) {
                                cur_type = cell::col_type::BYTES;
                            } else if (std::isalpha(tmp[0])) {
                                cur_type = cell::col_type::BOOL;
                            } else if (tmp[0] == '-' || tmp[0] == '+' || std::isdigit(tmp[0])) {
                                cur_type = cell::col_type::INT32;
                            } else {
                                cur_type = cell::col_type::EMPTY;
                            }
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
                        } else if (params[ind] != ' ') {
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
                    std::vector<std::string> columns_from;
                    std::string tmp;
                    while (ind < params.size()) {
                        if (params[ind] == ',') {
                            columns_from.emplace_back(tmp);
                        } else if (params[ind] != ' ') {
                            tmp += params[ind];
                        }
                        ind++;
                    }
                    if (tmp != "") {
                        columns_from.emplace_back(tmp);
                    }
                    break;
                }
                case ins::WHERE: {
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
        } else if (input == "bool") {
            return cell::col_type::BOOL;
        } else if (input.contains("string")) {
            return cell::col_type::STRING;
        } else if (input.contains("byte")) {
            return cell::col_type::BYTES;
        } else {
            throw std::runtime_error("can't recognize column type");
        }
    }

    std::vector<ins::instruction> query_parser::math_engine::parse(std::string_view expression) {
        std::vector<ins::instruction> result;
        std::vector<std::pair<op::instruction_operator, std::pair<cell::Cell, cell::Cell>>> values;
        int ind = 0;
        int subexpression_start = 0;
        state current_state = START;
        int bracket_cnt = 0;
        while (ind != expression.size()) {
            // This will evaluate expression in the brackets.
            char cur = expression[ind];
            if (cur == '(' && current_state == START) {
                current_state = SUBEXPRESSION;
                subexpression_start = ind + 1;
            } else if (cur == '(') {
                bracket_cnt++;
            } else if (cur == ')' && current_state == SUBEXPRESSION) {
                if (!bracket_cnt) {
                    // Extend vector with result
                    auto temp = parse(expression.substr(subexpression_start, ind - 1));
                    result.reserve(result.size() + distance(temp.begin(), temp.end()));
                    result.insert(result.end(), temp.begin(), temp.end());
                    current_state = START;
                } else {
                    bracket_cnt--;
                }
            }

            // Means that we need to find a size of strings or bytes.
            if (ind < expression.size() && cur == '|' && expression[ind + 1] != '|') {
                if (expression[ind + 1] == '\"') {
                    std::string str_to_get_size;
                    ind += 2;
                    while (expression[ind] != '\"') {
                        str_to_get_size += expression[ind];
                    }
                }
            }
            ind++;
        }
        return result;
    }
} // namespace basic_parser
