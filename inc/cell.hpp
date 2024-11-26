#pragma once

#include <string>
#include <variant>
#include <vector>

namespace cell {
    class Cell {
    private:
        std::variant<std::string, bool, int, std::vector<std::byte> > value;
        bool is_empty = false;

    public:
        Cell();

        Cell(std::string str);

        Cell(int i);

        Cell(bool i);

        Cell(std::vector<std::byte> &bytes);

        // NOTE: if called a function of type different than actual value, an
        // exception should be thrown
        std::string &get_string();

        bool get_bool();

        int get_int();

        std::vector<std::byte> &get_bytes();
    };
} // namespace cell
