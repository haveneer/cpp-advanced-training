//#region [Collapse all]
#include <algorithm>
#include <atomic>
#include <cstdlib>
#include <iostream>
#include <map>
#include <thread>
#include <utility>
#include <vector>

#define SLOW_DOWN(x)                  \
  x;                                  \
  if constexpr (slow_down_factor > 0) \
    std::this_thread::sleep_for(      \
        std::chrono::nanoseconds(std::rand() % slow_down_factor));

std::ostream &operator<<(std::ostream &o, const std::tuple<int, int, int> &t) {
  return o << "(" << std::get<0>(t) << ", " << std::get<1>(t) << ", "
           << std::get<2>(t) << ")";
}
//#endregion

int main() {
  std::srand(std::time(nullptr));
  std::map<std::tuple<int, int, int>, uint32_t> results;

  constexpr uint32_t slow_down_factor = 0; // 5; // max in nanoseconds

  for (auto count = 100'000; count > 0; --count) {
    std::atomic<int> a{1}, b{0}, c{1};
    // int a{1}, b{0}, c{1};

    auto t1 = std::thread([&]() {
      SLOW_DOWN(a = 2);
      SLOW_DOWN(b = 4);
      SLOW_DOWN(c = 3);
    });

    auto t2 = std::thread([&]() {
      SLOW_DOWN(b += a);
      SLOW_DOWN(c += 4);
      SLOW_DOWN(a += c);
    });

    t1.join();
    t2.join();

    std::tuple<int, int, int> result{a, b, c};
    auto [finder, inserted] = results.insert(std::make_pair(result, 0));
    if (inserted) {
      std::cout << "New order (a, b, c) = " << result << '\n';
    }
    finder->second++;
  }

  std::cout << "----------------------\n";
  std::vector<std::pair<decltype(results)::key_type, decltype(results)::mapped_type>>
      ordered_results;
  std::copy(std::begin(results), std::end(results),
            std::back_inserter(ordered_results));
  std::sort(std::begin(ordered_results), std::end(ordered_results),
            [](auto &&a, auto &&b) { return a.second > b.second; });
  std::cout << ordered_results.size() << " results:\n";
  for (auto [k, v] : ordered_results) {
    std::cout << k << " : " << v << '\n';
  }
}