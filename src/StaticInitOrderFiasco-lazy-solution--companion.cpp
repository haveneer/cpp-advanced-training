//#region [Headers]
#include <cassert>
#include <type_traits>
//#endregion

// a runtime function
int square(int n) { 
  assert(!std::is_constant_evaluated());
  return n * n;
}

int &staticCompanion() { // will initialize its static data at first call
  static auto private_static_data = square(5); 
  return private_static_data;
}