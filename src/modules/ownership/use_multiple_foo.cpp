import module1; // module != namespace
import module2; // requires to define a namespace inside the module
                // if not, there could be name conflicts
#include <cassert>

int main() {
  assert(::foo1() == 1);
  assert(::foo2() == 2);
  assert(ns_module1::foo() == 1);
  assert(ns_module2::foo() == 2);
}