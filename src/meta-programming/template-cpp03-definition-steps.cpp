//#region [declarations]
#include <iomanip>
#include <iostream>
#define DISPLAY(X) std::setw(40) << #X << " : " << X
//#endregion

// Declaration only
template <typename T> T fooFunc(T t);
template <typename T1, typename T2> struct FooStruct {
  static T1 bar(T2 t);
  static T1 value;
};

// Generic definition: required before any usage
template <typename T> T fooFunc(T t) { return t * t; }
template <typename T1, typename T2> T1 FooStruct<T1, T2>::bar(T2 t) { return t; }

// Explicit instanciation (usually not necessary)
template int fooFunc<int>(int);
// template int fooFunc(int); // same with deduced template parameter
template <> int FooStruct<int, int>::value; // required to define static variable
template struct FooStruct<int, int>;        // will be instantiated even if not used

// Specialization (the body could be completely different from original declaration)
template <> double fooFunc(double t);
template <typename T1> struct FooStruct<T1, double> {
  static T1 baZ(double t) { return 2 * t; }
};

int main() {
  std::cout << DISPLAY(fooFunc(1)) << '\n';           // use explicitly instantiation
  std::cout << DISPLAY(fooFunc<double>(2.0)) << '\n'; // implicit instantiation
  std::cout << DISPLAY(fooFunc(2.0)) << '\n'; // with template parameter deduction

  std::cout << DISPLAY((FooStruct<int, int>::bar(2))) << '\n';
  std::cout << DISPLAY((FooStruct<int, int>::value)) << '\n';
  std::cout << DISPLAY((FooStruct<int, double>::baZ(2))) << '\n'; // is a completely
  // std::cout << FooStruct<int, double>::value << '\n'; // different specialization
}

// late definition (possible due to early specialization or instanciation)
template <> double fooFunc(double t) { return 2 * t; }
template <> int FooStruct<int, int>::value = 2;