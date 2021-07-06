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

//#region [Collapse all]
#include <chrono>
#include <thread>
//#endregion

int main() {
  using namespace std::chrono_literals;

  std::jthread nonInterruptible([] {
    for (int counter{0}; counter < 10; ++counter) {
      std::this_thread::sleep_for(0.21s);
      std::cout << "NonInterruptible thread counter=" << counter << "\n";
    }
  });

  std::jthread interruptible([](std::stop_token stopToken) { // HINT stop_token HERE
    for (int counter{0}; counter < 10; ++counter) {
      std::this_thread::sleep_for(0.20s);
      if (stopToken.stop_requested())
        return;
      std::cout << "Interruptible thread counter=" << counter << "\n";
    }
  });

  std::this_thread::sleep_for(1s);
  interruptible.request_stop();    // ask for stop: 🤝
  nonInterruptible.request_stop(); // ask for stop: 🙉🙈
  std::cout << "Main thread completed" << std::endl;
}

#endif