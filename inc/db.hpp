#include "cell.hpp"
#include <concepts>
#include <string>
#include <type_traits>
#include <vector>

namespace memdb {
enum col_type { bool_type, int32, string, bytes };
class db {
  class table {
    // Some array of cells goes here
    std::vector<std::string> cols;
    class item {
    private:
      std::vector<cell::Cell> row;
      std::vector<std::string> row_names;
      std::vector<col_type> types;

    public:
      item(std::vector<cell::Cell> r);
    };
  };

  void execute(std::string query);
  std::vector<table> tables;
};
} // namespace memdb
