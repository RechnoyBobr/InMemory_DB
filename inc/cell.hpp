#pragma once

#include <string>
#include <variant>

namespace cell {
class Cell {
private:
  std::variant<std::string, bool, int, std::byte *> value;

public:
  Cell() = default;
};
} // namespace cell
