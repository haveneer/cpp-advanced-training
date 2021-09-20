#include <version>
#if !defined(__cpp_lib_latch) || !defined(__cpp_lib_syncbuf) || \
    !defined(__cpp_lib_jthread)
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_lib_latch), STR(__cpp_lib_syncbuf),
                 STR(__cpp_lib_jthread)});
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

  constexpr int nthread{10};         // parameter for the number of threads
  std::vector<std::jthread> threads; // could be reserved to nthread
  //                                 //  but not to show movable threads

  std::promise<unsigned> promise; // to set the winner (only once)
  std::stop_source source;        // to stop other threads
  std::latch latch{nthread + 1};  // to sync all threads start

  for (unsigned i{0}; i < nthread; ++i) {
    threads.emplace_back(
        // use rd() to get a random result; seed = i for a mostly deterministic one
        [&, seed = i /*rd()*/](const std::stop_token &st, // to stop other threads
                               std::promise<unsigned> &promise, // to set the winner
                               unsigned id) {                   // id of this thread
          std::default_random_engine engine{seed};
          latch.arrive_and_wait(); // all threads will start challenge together
          unsigned round = 0;
          while (engine() % 20092021) {
            // only check stop_requested sometimes to show delayed answer behavior
            if ((++round % 10000000 == 0) && st.stop_requested()) {
              std::osyncstream(std::cout) << "Thread " << id << " leaves after "
                                          << round << " rounds" << std::endl;
              return;
            }
          }
          try { // an exception may occur if promise is already set
            promise.set_value(id); // first try to be the winner
            source.request_stop(); // then stop other threads
          } catch (...) {          // if an exception occurs, this is a 'late' winner
            std::osyncstream(std::cout)
                << "Thread " << id << " is too late" << std::endl;
          }
        },
        source.get_token(), std::ref(promise), i);
  }

  latch.arrive_and_wait(); // release last lock for latch on threads
  auto start = std::chrono::steady_clock::now(); // will time of first completion
  auto winner = promise.get_future().get();      // effective get data
  auto duration = std::chrono::duration<double, std::milli>(
                      std::chrono::steady_clock::now() - start)
                      .count();
  std::cout << "The winner is thread #" << winner << " after " << duration << "_ms"
            << std::endl;
}

#endif