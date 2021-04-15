//#region [declarations]
#include <iostream>
//#endregion

// declaration only
template <typename T> T fooFunction(T t);

// definition: required before any usage
template <typename T> T fooFunction(T t) { return t * t; }

// explicit instanciation (not necessary)
template int fooFunction<int>(int);
// template int fooFunction(int); // same with deduced template parameter

int main() {
  fooFunction(1);           // use explicitly instantiated version
  fooFunction<double>(2.0); // implicit instantiation
  fooFunction(2.0);         // (same with template parameter deduction)
}
