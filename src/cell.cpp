#include "cell.hpp"
#include <cstddef>
#include <stdexcept>
#include <vector>

namespace cell {
    bool Cell::is_basic_cell() {
        if (type == COL_NAME) {
            return false;
        }
        return true;
    }

    Cell::Cell() {
        type = EMPTY;
        is_empty = true;
    }

    Cell::Cell(const std::string &str, col_type type) {
        this->type = type;
        value = str;
    }

    Cell::Cell(std::vector<std::byte> &bytes) {
        type = BYTES;
        value = bytes;
    }

    Cell::Cell(int i) {
        type = INT32;
        value = i;
    }

    Cell::Cell(bool b) {
        type = BOOL;
        value = b;
    }

    std::string &Cell::get_string() { return std::get<std::string>(value); }
    int Cell::get_int() { return std::get<int>(value); }

    col_type Cell::get_cell_type() const {
        return type;
    }

    Cell Cell::copy_and_increment() {
        if (this->type != INT32) {
            throw std::runtime_error("Cannot have autoincrement on non int value");
        }
        Cell new_cell = Cell(std::get<int>(this->value) + 1);
        return new_cell;
    }

    bool Cell::get_bool() { return std::get<bool>(value); }

    std::vector<std::byte> &Cell::get_bytes() { return std::get<std::vector<std::byte> >(value); }

    bool Cell::operator==(const Cell &cell) const {
        if (this->get_cell_type() == cell.get_cell_type()) {
            if (this->value == cell.value) {
                return true;
            }
            return false;
        }
        return false;
    }
} // namespace cell
