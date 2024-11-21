#pragma once

#include "db.hpp"
#include <vector>

namespace res {
// Result of execute operation

// TODO: Create iterator for result class
class result {
  // WARN: I'm unsure if it's good idea to have vector of cells as a result
  std::vector<memdb::item> result;

  bool is_ok();
};

} // namespace res
