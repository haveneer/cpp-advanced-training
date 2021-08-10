module;

#include <cmath> // cannot be located in module

export module reexported;

double foo();

export {
  int phi = (1 + std::sqrt(5.)) / 2;
  namespace Namespace {
  double baz() { return foo(); }
  } // namespace Namespace
}

double foo() { return std::cos(0); }