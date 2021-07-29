// Conditionals with omitted operands aka "elvis operator"

auto read() -> int { return 1; }

auto default_value() -> int { return 2; }

auto read_or_default() -> int {
  // expr1 ?: expr2
  // is equivalent to
  // expr1 ? expr1 : expr2
  return read() ?: default_value(); // don't repeat read() as "then" return value
}

int main() {
  // Trivial test
  return 1 - read_or_default();
}