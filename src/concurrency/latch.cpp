#include <version>
#ifndef __cpp_lib_latch
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_lib_latch)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

//#region [Collapse all]
#include <iostream>
#include <latch>
#include <mutex>
#include <thread>
#include <vector>
//#endregion

std::latch workDone(6);

void synchronizedOut(const std::string &s) {
  static std::mutex coutMutex;
  std::lock_guard<std::mutex> lo(coutMutex);
  std::cout << s;
}

class Worker {
public:
  Worker(std::string n) : name(n) {}

  void operator()() {
    synchronizedOut(name + ": Work done!\n");
    workDone.arrive_and_wait(); // equivalent to count_down(1); wait();
    synchronizedOut(name + ": Good bye!\n");
  }

private:
  std::string name;
};

int main() {
  std::vector<std::thread> ts;
  for (int i = 1; i <= 6; ++i) {
    ts.emplace_back(Worker("t" + std::to_string(i)));
  }

  for (auto &&t : ts) {
    t.join();
  }
}

#endif