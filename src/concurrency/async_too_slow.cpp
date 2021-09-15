//#region [Collapse all]
#include <chrono>
#include <future>
#include <iostream>
#include <thread>
using namespace std::chrono_literals;

double ms_from(const std::chrono::steady_clock::time_point &start) {
  return std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() -
                                                   start)
      .count();
}
//#endregion

int main() {
  auto start = std::chrono::steady_clock::now();
  std::async(std::launch::async, [&] {
    std::cout << "Hello from f at " << ms_from(start) << "_ms\n";
    //#region [Surprise !]
    std::this_thread::sleep_for(100ms); // HINT: this is not the answer
    //#endregion
  });
  std::async(std::launch::async,
             [&] { std::cout << "Hello from g at " << ms_from(start) << "_ms\n"; });
}