#define MACRO // no effect on MACRO in preproc_citadel module
import preproc_citadel;

#include <cassert>

int main() {
  assert(foo() == 1);
}