#pragma once

#include <stdexcept>
#include <string>
#include <variant>
#include <vector>


namespace cell {
    enum col_type { BOOL, INT32, STRING, BYTES, COL_NAME, EMPTY };

    class Cell {
    private:
        std::variant<std::string, bool, int, std::vector<std::byte> > value;
        bool is_empty = false;
        // Only needed if cell is a column name
        col_type type;

    public:
        template<class T>
        T get() {
            return std::get<T>(this->value);
        }

        bool is_basic_cell() {
            if (type == COL_NAME) {
                return false;
            }
            return true;
        }

        Cell() {
            type = EMPTY;
            is_empty = true;
        }

        Cell(const std::string &str, col_type type) {
            this->type = type;
            value = str;
        }

        Cell(std::vector<std::byte> &bytes) {
            type = BYTES;
            value = bytes;
        }

        Cell(int i) {
            type = INT32;
            value = i;
        }

        Cell(bool b) {
            type = BOOL;
            value = b;
        }

        std::string &get_string() { return std::get<std::string>(value); }
        int get_int() { return std::get<int>(value); }

        col_type get_cell_type() const {
            return type;
        }

        Cell copy_and_increment() {
            if (this->type != INT32) {
                throw std::runtime_error("Cannot have autoincrement on non int value");
            }
            Cell new_cell = Cell(std::get<int>(this->value) + 1);
            return new_cell;
        }

        bool get_bool() { return std::get<bool>(value); }

        std::vector<std::byte> &get_bytes() { return std::get<std::vector<std::byte> >(value); }

        bool operator==(const Cell &cell) const {
            if (this->get_cell_type() == cell.get_cell_type()) {
                if (this->value == cell.value) {
                    return true;
                }
                return false;
            }
            return false;
        }
    };
} // namespace cell
