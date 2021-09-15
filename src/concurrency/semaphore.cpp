#include <version>
#if !defined(__cpp_lib_semaphore) || !defined(__cpp_lib_syncbuf) || \
    !defined(__cpp_lib_jthread)
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_lib_semaphore), STR(__cpp_lib_syncbuf),
                 STR(__cpp_lib_jthread)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

//#region [Collapse all]
#include <array>
#include <chrono>
#include <iostream>
#include <mutex>
#include <random>
#include <semaphore>
#include <syncstream>
#include <thread>
//#endregion

void do_stuff(const std::string &name, const std::size_t duration,
              const std::size_t pos) {
  std::osyncstream(std::cout)
      << name << " does stuff in slot " << pos << " for " << duration << "_ms\n";
  std::this_thread::sleep_for(std::chrono::milliseconds(duration));
}

auto slot_duration() {
  std::random_device seed;
  std::mt19937 engine{seed()};
  return engine() % 200;
};

int main() {
  std::array names = {"Apu", "Krusty", "Bart", "Homer", "Ned", "Maggie"};
  std::array<bool, 4> slots{};    // initialize array to false
  std::binary_semaphore mutex(1); // mutex made with a semaphore for slot lookup
  std::counting_semaphore semaphore(slots.size());

  auto find_slot = [&] {
    std::size_t pos = 0;
    mutex.acquire();
    for (; slots[pos]; ++pos)
      ;
    if (pos >= slots.size() || slots[pos])
      throw std::runtime_error("No slot found");
    slots[pos] = true;
    mutex.release();
    return pos;
  };

  std::vector<std::jthread> queuers;
  for (auto name : names) {
    queuers.emplace_back([&, name]() {
      semaphore.acquire(); // wait if no slot available
      const auto pos = find_slot();
      const auto duration = slot_duration();
      do_stuff(name, duration, pos);
      mutex.acquire();
      slots[pos] = false;
      mutex.release();
      semaphore.release();
    });
  }
}

#endif