//#region [declarations]
#include <iostream>
//#endregion

template <typename T> // #A
class MyContainer {
  // use T as a type
};

template <typename T> // #B
void genericFunction() {
  // use T as a type
}

template <typename T> // #C
void genericFunction(T t) {
  // use T as a type
}

int main() {
  MyContainer<int> foo_int; // #A (T is explicitly int)
  genericFunction<int>();   // #B (T is explicitly int)
  genericFunction(1);       // #C (T is implicitly int, deduced from argument)
}