#include "../inc/operator.hpp"

#include <stdexcept>
#include <variant>

namespace op {
instruction_operator::instruction_operator(std::string_view query) {
  if (query.contains("+")) {
    type = ADD;
  } else if (query.contains("-")) {
    type = SUB;
  } else if (query.contains("*")) {
    type = MUL;
  } else if (query.contains("/")) {
    type = DIV;
  } else if (query.contains("%")) {
    type = REM;
  } else if (query.contains("<=")) {
    type = LT;
  } else if (query.contains(">=")) {
    type = GT;
  } else if (query.contains("<")) {
    type = L;
  } else if (query.contains(">")) {
    type = G;
  } else if (query.contains("!=")) {
    type = NEQ;
  } else if (query.contains("=")) {
    type = EQ;
  } else if (query.contains("&&")) {
    type = AND;
  } else if (query.contains("||")) {
    type = OR;
  } else if (query.contains("|")) {
    type = SIZE;
  } else if (query.contains("^")) {
    type = XOR;
  }
}

std::variant<std::string_view, int> instruction_operator::exec_op(
    std::string s1, std::string s2) {
  switch (type) {
    case L:
      return s1 < s2;
    case G:
      return s1 > s2;
    case LT:
      return s1 <= s2;
    case GT:
      return s1 >= s2;
    case EQ:
      return s1 == s2;
    case NEQ:
      return s1 != s2;
    case SIZE:
      return static_cast<int>(s1.size());
    case ADD:
      return std::string_view(s1 + s2);
    default:
      throw std::runtime_error("String type does not support that operation");
  }
}
int instruction_operator::exec_op(std::vector<std::byte> bytes1,
                                  std::vector<std::byte> bytes2) {
  switch (type) {
    case L:
      return bytes1 < bytes2;
    case G:
      return bytes1 > bytes2;
    case LT:
      return bytes1 <= bytes2;
    case GT:
      return bytes1 >= bytes2;
    case EQ:
      return bytes1 == bytes2;
    case NEQ:
      return bytes1 != bytes2;
    case SIZE:
      return bytes1.size();
    default:
      throw std::runtime_error(
          "Array of bytes does not support that operation");
  }
};
int instruction_operator::exec_op(int i1, int i2) {
  switch (type) {
    case ADD:
      return i1 + i2;
    case SUB:
      return i1 - i2;
    case MUL:
      return i1 * i2;
    case DIV:
      return i1 / i2;
    case REM:
      return i1 % i2;
    case L:
      return i1 < i2;
    case G:
      return i1 > i2;
    case GT:
      return i1 >= i2;
    case LT:
      return i1 <= i2;
    case EQ:
      return i1 == i2;
    case NEQ:
      return i1 != i2;
    default:
      throw std::runtime_error("Int type does not support that operation");
  }
}

bool instruction_operator::exec_op(bool b1, bool b2) {
  switch (type) {
    case L:
      return b1 < b2;
    case G:
      return b1 > b2;
    case LT:
      return b1 <= b2;
    case GT:
      return b1 >= b2;
    case EQ:
      return b1 == b2;
    case NEQ:
      return b1 != b2;
    case AND:
      return b1 && b2;
    case OR:
      return b1 || b2;
    case INV:
      return !b1;
    case XOR:
      return b1 ^ b2;
    default:
      throw std::runtime_error("Bool does not support that operation");
  }
}

}  // namespace op
//
