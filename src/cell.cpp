#include "../inc/cell.hpp"

#include <string_view>

namespace cell {
Cell::Cell() { is_empty = true; }
Cell::Cell(std::string str) { value = str; }
Cell::Cell(std::vector<std::byte> &bytes) { value = bytes; }
Cell::Cell(int i) { value = i; }
Cell::Cell(bool b) { value = b; }
}  // namespace cell
