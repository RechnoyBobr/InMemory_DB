#include "cell.hpp"
#include <cstddef>
#include <stdexcept>
#include <vector>

namespace cell {
    Cell::Cell() { is_empty = true; }
    Cell::Cell(const std::string &str) { value = str; }
    Cell::Cell(std::vector<std::byte> &bytes) { value = bytes; }
    Cell::Cell(int i) { value = i; }
    Cell::Cell(bool b) { value = b; }
    std::string &Cell::get_string() { return std::get<std::string>(value); }
    int Cell::get_int() { return std::get<int>(value); }

    col_type Cell::get_cell_type() const {
        if (is_empty) {
            return EMPTY;
        }
        if (std::holds_alternative<int>(value)) {
            return INT32;
        }
        if (std::holds_alternative<std::string>(value)) {
            return STRING;
        }
        if (std::holds_alternative<bool>(value)) {
            return BOOL;
        }
        if (std::holds_alternative<std::vector<std::byte>>(value)) {
            return BYTES;
        }
        throw std::runtime_error("There is an error with cell type\n");
    }

    Cell Cell::copy_and_increment() {
        if (this->get_cell_type() != INT32) {
            throw std::runtime_error("Cannot have autoincrement on non int value");
        }
        Cell new_cell = Cell(std::get<int>(this->value) + 1);
        return new_cell;
    }

    bool Cell::get_bool() { return std::get<bool>(value); }

    std::vector<std::byte> &Cell::get_bytes() { return std::get<std::vector<std::byte>>(value); }
} // namespace cell
