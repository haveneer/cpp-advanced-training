#include <version>
#if !defined(__cpp_lib_jthread)
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_lib_jthread)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

//#region [Colorization]
#include <iomanip>
#include <iostream>
#if __has_include("colorize.hpp")
#include "colorize.hpp"
#else
template <typename T> struct Colorize { T x; };
template <typename T>
std::ostream &operator<<(std::ostream &o, const Colorize<T> &t) {
  return o << t.x;
}
template <typename T> Colorize(T &&t) -> Colorize<T>;
#endif
//#endregion

#include <chrono>
#include <thread>

void do_something() {
  std::cout << "Hello from jthread id=" << Colorize{std::this_thread::get_id()}
            << "\n";
  std::this_thread::sleep_for(std::chrono::milliseconds(300));
  std::cout << "Goodbye from jthread id=" << Colorize{std::this_thread::get_id()}
            << "\n";
}

int main() {
  std::cout << "Starting main" << std::hex; // std::hex side effect in all threads
  std::jthread t(do_something);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  std::cout << "See you soon from main id=" << Colorize{std::this_thread::get_id()}
            << "\n";
  // with classical threads, without join(), code stops with 'terminated'
}

#endif