import v1_module; // module != namespace
import v2_module; // requires to define a namespace inside the module
                  // if not, there could be name conflicts
#include <cassert>

int main() {
  assert(v1::foo() == 1);
  assert(v2::foo() == 2);
}