#pragma once

#include <string>
#include <variant>
#include <vector>


namespace cell {

    enum col_type { BOOL, INT32, STRING, BYTES, EMPTY };

    class Cell {
    private:
        std::variant<std::string, bool, int, std::vector<std::byte>> value;
        bool is_empty = false;

    public:
        Cell();

        Cell(const std::string &str);

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

        bool operator==(const Cell & cell) const;
    };
} // namespace cell
