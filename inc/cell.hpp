#pragma once

#include <string_view>
#include <variant>

namespace cell {
class Cell {
private:
  std::variant<std::string_view, bool, int, std::byte *> value;

public:
  // NOTE: I don't know whether it should be default or not.
  Cell() = default;
};
} // namespace cell
