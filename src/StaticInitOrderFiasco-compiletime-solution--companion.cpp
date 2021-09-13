#include <version>
#if defined(__cpp_constinit)

// a compile time computable function
// (also ok with a consteval function)
consteval int square(int n) { 
  return n * n;
}

// a static data always computed at compile time
// NOTE a constexpr variable will be computed at compile time
//      but not exportable to another compilation unit
constinit auto staticCompanion = square(5); 


#endif