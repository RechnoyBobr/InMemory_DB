#pragma once

#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace cell {
class Cell {
private:
  std::variant<std::string_view, bool, int, std::vector<std::byte>> value;

public:
  Cell() = default;
  Cell(std::string_view &str);
  Cell(int i);
  Cell(bool i);
  Cell(std::string x);
  Cell(std::vector<std::byte> &bytes);
  // NOTE: if called a function of type different than actual value, an
  // exception should be thrown
  std::string_view get_string();
  bool get_bool();
  int get_int();
  std::vector<std::byte> get_bytes();
};
} // namespace cell
