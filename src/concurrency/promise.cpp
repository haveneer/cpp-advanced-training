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

void do_something_and_set_promise(std::promise<int> promise) {
  // do computations
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  std::cout << "Thread Computing" << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  promise.set_value(42);
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  std::cout << "Thread Cleaning" << std::endl;
  // do some cleaning stuff
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  // once set, a promise cannot be reused
  // promise.set_value(42); // terminate called after throwing an instance of
  //                        // 'std::future_error'
  //                        //  what():  std::future_error: Promise already satisfied
}

int main() {
  std::promise<int> result_promise;
  auto result_future = result_promise.get_future();
  std::thread t(do_something_and_set_promise, std::move(result_promise));
  std::cout << "Main ready" << std::endl;
  std::cout << "Result is ready: " << is_ready(result_future) << std::endl;
  auto result = result_future.get(); // wait for result
  std::cout << "Result: " << result << std::endl;
  t.join();
  assert(!result_future.valid()); // once consumed, std::future is no more valid
}