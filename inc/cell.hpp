#pragma once

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
        bool is_basic_cell();

        Cell();

        Cell(const std::string &str, col_type type);

        Cell(int i);

        Cell(bool b);

        Cell(std::vector<std::byte> &bytes);

        // NOTE: if called a function of type different than actual value, an
        // exception should be thrown
        std::string &get_string();

        bool get_bool();

        int get_int();

        col_type get_cell_type() const;

        Cell copy_and_increment();

        std::vector<std::byte> &get_bytes();

        bool operator==(const Cell &cell) const;
    };
} // namespace cell
