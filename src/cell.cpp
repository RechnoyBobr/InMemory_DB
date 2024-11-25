#include "../inc/cell.hpp"

#include <cstddef>
#include <string_view>

namespace cell {
Cell::Cell() { is_empty = true; }
Cell::Cell(std::string str) { value = str; }
Cell::Cell(std::vector<std::byte> &bytes) { value = bytes; }
Cell::Cell(int i) { value = i; }
Cell::Cell(bool b) { value = b; }
std::string &Cell::get_string() { return std::get<std::string>(value); }
int Cell::get_int() { return std::get<int>(value); }
bool Cell::get_bool() { return std::get<bool>(value); }
std::vector<std::byte> &Cell::get_bytes() {
  return std::get<std::vector<std::byte>>(value);
}
}  // namespace cell
