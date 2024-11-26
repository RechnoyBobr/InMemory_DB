#include "../inc/query_parser.hpp"

#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "../inc/lexer.hpp"

namespace basic_parser {
    cell::Cell query_parser::get_cell(std::string &tmp, memdb::col_type cur_type) {
        cell::Cell ret_value;
        switch (cur_type) {
            case memdb::col_type::bytes: {
                std::vector<std::byte> res = {};
                bool flag = false;
                int cnt_ch = 0;
                int cur_byte = 0;
                for (char ch: tmp) {
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
                if (tmp.size() % 2 != 0) {
                    res.emplace_back(std::byte(cur_byte));
                }
                ret_value = cell::Cell(res);
                break;
            }
            case memdb::col_type::string: {
                // trim the quotes
                std::string trimmed_str = tmp.substr(1, tmp.size() - 2);
                ret_value = cell::Cell(trimmed_str);
                break;
            }
            case memdb::col_type::int32: {
                ret_value = cell::Cell(std::stoi(tmp));
                break;
            }
            case memdb::col_type::bool_type: {
                if (tmp == "true") {
                    ret_value = cell::Cell(true);
                } else if (tmp == "false") {
                    ret_value = cell::Cell(false);
                } else {
                    throw std::runtime_error("Can't recognize default value");
                }
            }
        }
    }

    std::vector<memdb::instruction> query_parser::parse(std::string_view query) {
        basic_lexer::lexer l = basic_lexer::lexer();
        auto lexed_commands = l.tokenize(query);
        // TODO: there is quite a large mess.
        // the comma are only met in attributes or different cols
        std::vector<memdb::instruction> result = std::vector<memdb::instruction>(0);
        parser_state cur_state = TABLE_NAME;
        for (const auto &pair: lexed_commands) {
            memdb::instruction_type type = pair.first;
            std::string params = pair.second;
            switch (type) {
                case memdb::CREATE: {
                    size_t ind = 0;
                    auto col_names =
                            std::vector<std::pair<std::string, std::vector<memdb::attributes> > >(
                                0);
                    std::string table_name;
                    std::vector<std::pair<memdb::col_type, cell::Cell> > types = {};
                    std::string tmp;
                    std::vector<memdb::attributes> attr;
                    cell::Cell default_val;
                    memdb::col_type cur_type;

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
                        } else if ((params[ind] == ',' || params[ind] == '}') &&
                                   cur_state == ATTRIBUTES) {
                            if (tmp == "key") {
                                attr.emplace_back(memdb::attributes::KEY);
                            } else if (tmp == "autoincrement") {
                                attr.emplace_back(memdb::attributes::AUTOINCREMENT);
                            } else if (tmp == "unique") {
                                attr.emplace_back(memdb::attributes::UNIQUE);
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
                                    break;
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
                    memdb::instruction table_create =
                            memdb::instruction(table_name, col_names, types);
                    result.emplace_back(table_create);
                    break;
                }

                case memdb::INSERT: {
                    int ind = 0;
                    std::string tmp;
                    std::unordered_map<std::string, cell::Cell> values_by_name;
                    std::vector<cell::Cell> values_by_order;
                    std::string column_name;
                    memdb::instruction insert_c;
                    int insert_type = 1;
                    while (ind < query.size()) {
                        if (query[ind] == '=') {
                            //Then we should save column name
                            column_name = tmp;
                            tmp = "";
                        }
                        if (query[ind] == ')' || query[ind] == ',') {
                            memdb::col_type cur_type;
                            if (tmp.contains('\"')) {
                                cur_type = memdb::col_type::string;
                            } else if (tmp.contains('x')) {
                                cur_type = memdb::col_type::bytes;
                            } else if (std::isalpha(tmp[0])) {
                                cur_type = memdb::col_type::bool_type;
                            } else {
                                cur_type = memdb::col_type::int32;
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
                        } else if (query[ind] != ' ') {
                            tmp += query[ind];
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

                default:
                    throw std::runtime_error("There was an error during the lexing process");
            }
        }
        return result;
    }

    memdb::col_type query_parser::parse_type(std::string &input) {
        if (input == "int32") {
            return memdb::col_type::int32;
        } else if (input == "bool") {
            return memdb::col_type::bool_type;
        } else if (input.contains("string")) {
            return memdb::col_type::string;
        } else if (input.contains("byte")) {
            return memdb::col_type::bytes;
        } else {
            throw std::runtime_error("can't recognize column type");
        }
    }

    std::shared_ptr<std::vector<memdb::instruction> >
    query_parser::math_engine::parse(std::string_view expression) {
        std::shared_ptr<std::vector<memdb::instruction> > result;
        memdb::instruction tmp;
        int ind = 0;
        int subexpression_start = 0;
        state current_state = START;
        int bracket_cnt = 0;
        while (ind != expression.size()) {
            // TODO: rework parse method. Split it to 2 parts. One should parse the
            // operation and the other the data.
            char cur = expression[ind];
            if (cur == '(' && current_state == START) {
                current_state = SUBEXPRESSION;
                subexpression_start = ind + 1;
            } else if (cur == '(') {
                bracket_cnt++;
            } else if (cur == ')' && current_state == SUBEXPRESSION) {
                if (!bracket_cnt) {
                    result = parse(expression.substr(subexpression_start, ind - 1));
                    current_state = START;
                } else {
                    bracket_cnt--;
                }
            }
        }
        return result;
    }
} // namespace basic_parser
