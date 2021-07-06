//#region [Collapse all]
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
#include <thread>
//#endregion

struct CallableObject {
  CallableObject() { //
    std::cout << "New CallableObject " << Colorize{this} << "\n";
  }
  ~CallableObject() { //
    std::cout << "Delete CallableObject " << Colorize{this} << "\n";
  }
  CallableObject(CallableObject &&that) {
    std::cout << "Move CallableObject " << Colorize{&that} << " -> "
              << Colorize{this} << "\n";
  }
  CallableObject(const CallableObject &) = delete;
  void operator=(const CallableObject &) = delete;
  void operator=(CallableObject &&) = delete;

  void operator()(const std::string &greeting) {
    std::cout << greeting << " from CallableObject " << Colorize{this} << "\n";
  }
};

int main() {
  // To create a new thread you have to provide a callable object (non blocking)
  std::thread t(CallableObject{}, "Hello"); // arguments are passed to the call

  // Continue main thread
  std::cout << "Hello from main" << std::endl;

  // Main thread consider the management of the thread
  t.join(); // waits for it
  // exclusive OR
  // t.detach(); // forgets if (continue without waiting)
  // TODO What happens if we use detach ?
  // TODO What happens if neither join nor detach ?

  // Thread t has been joined; main continues its way
  std::cout << "By from main" << std::endl;
}