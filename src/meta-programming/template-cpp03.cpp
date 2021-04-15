//#region [declarations]
#include <iostream>
//#endregion

template <typename T> // #A
class FooClass {
  // use T as a type
};

template <typename T> // #B
void fooFunction() {
  // use T as a type
}

template <typename T> // #C
void fooFunction(T t) {
  // use T as a type
}

int main() {
  FooClass<int> foo_int; // #A (T is explicitly int)
  fooFunction<int>();    // #B (T is explicitly int)
  fooFunction(1);        // #C (T is implicitly int, deduced from argument)
}