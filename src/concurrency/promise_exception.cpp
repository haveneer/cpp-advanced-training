//#region [Collapse all]
#include <future>
#include <iostream>
#include <thread>
#include <vector>
//#endregion

void do_something_and_set_promise(std::promise<int> promise) {
  std::vector<int> v(10, 0);
  try {
    for (size_t i = 0; i <= 10; ++i)
      v.at(i) = i;
  } catch (...) {
    std::cout << "Catch exception in thread" << std::endl;
    promise.set_exception(std::current_exception());
    // promise.set_exception( // could be a custom exception
    //    std::make_exception_ptr(std::runtime_error("Something goes wrong")));
  }
  std::cout << "End of thread" << std::endl;
}

int main() {
  std::promise<int> result_promise;
  auto result_future = result_promise.get_future();
  std::thread t(do_something_and_set_promise, std::move(result_promise));
  try {
    auto result = result_future.get(); // wait for result
    std::cout << "Result: " << result << std::endl;
  } catch (std::exception &e) {
    std::cout << "Exception from thread: " << e.what() << std::endl;
  }
  t.join();
}