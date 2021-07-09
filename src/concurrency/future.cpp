//#region [Collapse all]
#include <cassert>
#include <chrono>
#include <future>
#include <iostream>
#include <string>
#include <thread>
//#endregion

template <typename R> //
bool is_ready(const std::future<R> &f) {
  return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

std::string do_something_and_return_value() {
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  std::cout << "Thread ready" << std::endl;
  return "Hello";
}

int main() {
  std::future<std::string> future = std::async(do_something_and_return_value);
  std::cout << "Main ready" << std::endl;
  std::cout << "Future is ready: " << is_ready(future) << std::endl;
  std::cout << "Wait for future and get result" << std::endl;
  auto result = future.get(); // wait for result
  std::cout << "Result: " << result << std::endl;
  assert(!future.valid()); // once consumed, std::future is no more valid
}