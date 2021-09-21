//#region [Collapse all]
// Inspired from cppreference
#include <chrono>
#include <future>
#include <iostream>
//#endregion

int main() {
  std::promise<void> ready_promise;
  std::shared_future<void> ready_future(ready_promise.get_future());
  std::promise<void> t1_ready_promise, t2_ready_promise;

  std::chrono::time_point<std::chrono::high_resolution_clock> start;

  auto fun =
      [&start, ready_future](
          std::promise<void> &promise) -> std::chrono::duration<double, std::milli> {
    promise.set_value();
    ready_future.wait(); // waits for the signal from main()
    return std::chrono::high_resolution_clock::now() - start;
  };

  auto fut1 = t1_ready_promise.get_future();
  auto fut2 = t2_ready_promise.get_future();

  auto result1 = std::async(std::launch::async, fun, std::ref(t1_ready_promise));
  auto result2 = std::async(std::launch::async, fun, std::ref(t2_ready_promise));

  fut1.wait(); // wait for the threads to become ready
  fut2.wait();

  start = std::chrono::high_resolution_clock::now(); // the threads are ready,
                                                     // start the clock
  // signal the threads to go
  ready_promise.set_value();

  std::cout << "Thread 1 received the signal " << result1.get().count()
            << " ms after start\n"
            << "Thread 2 received the signal " << result2.get().count()
            << " ms after start\n";
}