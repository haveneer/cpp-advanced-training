#include <version>
#if !defined(__cpp_lib_memory_resource)
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_lib_memory_resource)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

#include "logger_resource.hpp"
#include <array>
#include <gtest/gtest.h>
#include <iostream>
#include <limits>
#include <list>
#include <memory_resource>
#include <numeric>
#include <random>
#include <string>
#include <type_traits>
#include <vector>

template <template <typename T> class C, typename Type>
std::size_t test_body(auto default_value) {
  constexpr size_t N = 10;
  using Collection = C<Type>;

  constexpr bool is_polymorphic_collection = std::is_same_v<
      typename Collection::allocator_type,
      std::pmr::polymorphic_allocator<typename Collection::value_type>>;

  std::pmr::unsynchronized_pool_resource sync_pools[N];

  auto build_collection = [&](size_t i) {
    if constexpr (is_polymorphic_collection) {
      return Collection{&sync_pools[i]};
    } else {
      return Collection{};
    }
  };

  Collection collections[N];
  for (size_t i = 0; auto &c : collections) {
    c = build_collection(i);
  }

  std::mt19937 engine{};
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  for (long i = 0; i < 10000; ++i) {
    for (auto &collection : collections) {
      if (collection.size() < 10 || dist(engine) > 0.5)
        collection.emplace_back(default_value);
      else
        collection.erase(collection.begin());
    }
  }

  long long total_jump_median = 0;
  long long total_jump_mean = 0;
  for (auto &collection : collections) {
    std::vector<long long> jumps;
    long long last_addr = 0;
    for (const auto &e : collection) {
      {
        long long addr = reinterpret_cast<long long>(e.data());
        if (last_addr != 0) {
          jumps.emplace_back(std::abs(last_addr - addr));
        }
        last_addr = addr;
      }
    }
    std::sort(jumps.begin(), jumps.end());
    if (size_t size = jumps.size(); size % 2 == 0) {
      total_jump_median += (jumps[size / 2 - 1] + jumps[size / 2]) / 2;
    } else {
      total_jump_median += jumps[size / 2];
    }
    total_jump_mean += std::reduce(jumps.begin(), jumps.end(), 0) / jumps.size();
  }

  return total_jump_median / N;
}

TEST(PoolResource, std_string_in_std_container) {
  auto status = test_body<std::vector, std::string>(
      "A string that beats SSO or Small String Optimization");
  std::cout << status << '\n';
}

TEST(PoolResource, std_string_in_pmr_container) {
  auto status = test_body<std::pmr::vector, std::string>(
      "A string that beats SSO or Small String Optimization");
  std::cout << status << '\n';
}

TEST(PoolResource, pmr_string_in_pmr_container) {
  auto status = test_body<std::pmr::vector, std::pmr::string>(
      "A string that beats SSO or Small String Optimization");
  std::cout << status << '\n';
}

#endif