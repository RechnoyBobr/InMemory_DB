#pragma once

#include <string_view>
#include <variant>
#include <vector>

namespace cell {
class Cell {
private:
  std::variant<std::string_view, bool, int, std::vector<std::byte>> value;

public:
  // NOTE: I don't know whether it should be default or not.
  Cell() = default;
  // NOTE: if called a function of type different than actual value, an
  // exception should be thrown
  std::string_view get_string();
  bool get_bool();
  int get_int();
  std::vector<std::byte> get_bytes();
};
} // namespace cell
