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

/* Subject:
 * The challenge: several threads try to find a given number (or modulo) resulting
 * from a random generation.
 * Each thread has a different random seed.
 * The first thread which find the given number has to stop other threads and to
 * return to the main thread its id.
 */

#include <chrono>
#include <future>
#include <iostream>
#include <latch>
#include <random>
#include <syncstream>
#include <thread>
#include <vector>

int main() {
  std::random_device rd;

  constexpr int nthread{10};

  std::vector<std::jthread> threads; // could be reserved to nthread
  //                                 //  but not to show movable threads

  std::promise<unsigned> promise;
  std::stop_source source;
  std::latch latch{nthread + 1};
  std::chrono::steady_clock::time_point start;

  for (unsigned i{0}; i < nthread; ++i) {
    threads.emplace_back(
        // use rd() to get a random result; seed = i for a mostly deterministic one
        [&, seed = i /*rd()*/](const std::stop_token &st, // to stop other threads
                               std::promise<unsigned> &promise, // to set the winner
                               unsigned id) {                   // id of this thread
          std::default_random_engine engine{seed};
          latch.arrive_and_wait();
          unsigned round = 0;
          while (engine() % 20092021) {
            // only check stop_requested sometimes to show delayed answer behavior
            if ((++round % 10000000 == 0) && st.stop_requested()) {
              std::osyncstream(std::cout) << "Thread " << id << " leaves after "
                                          << round << " rounds" << std::endl;
              return;
            }
          }
          try {
            promise.set_value(id);
            source.request_stop();
          } catch (...) {
            std::osyncstream(std::cout)
                << "Thread " << id << " is too late" << std::endl;
          }
        },
        source.get_token(), std::ref(promise), i);
  }

  latch.arrive_and_wait();
  start = std::chrono::steady_clock::now();
  auto winner = promise.get_future().get();
  auto duration = std::chrono::duration<double, std::milli>(
                      std::chrono::steady_clock::now() - start)
                      .count();
  std::cout << "The winner is " << winner << " after " << duration << "_ms"
            << std::endl;
}

#endif