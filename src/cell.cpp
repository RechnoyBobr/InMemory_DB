#include "../inc/cell.hpp"
#include <string>
#include <string_view>

namespace cell {
Cell::Cell(std::string_view &str) { value = str; }
Cell::Cell(std::vector<std::byte> &bytes) { value = bytes; }
Cell::Cell(int i) { value = i; }
Cell::Cell(bool b) { value = b; }
} // namespace cell
