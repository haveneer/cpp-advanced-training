#include <version>
#if !defined(__cpp_lib_syncbuf) || !defined(__cpp_lib_jthread)
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_lib_syncbuf), STR(__cpp_lib_jthread)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

#include <iostream>
#include <syncstream>
#include <thread>

void sayHello(std::string name) {
  std::osyncstream(std::cout) << "Hello from " << name << '\n';
}

int main() {
  std::jthread t0(sayHello, "t0");
  std::jthread t1(sayHello, "t1");
  std::jthread t2(sayHello, "t2");
  std::jthread t3(sayHello, "t3");
  std::jthread t4(sayHello, "t4");
  std::jthread t5(sayHello, "t5");
  std::jthread t6(sayHello, "t6");
  std::jthread t7(sayHello, "t7");
  std::jthread t8(sayHello, "t8");
  std::jthread t9(sayHello, "t9");
}

#endif