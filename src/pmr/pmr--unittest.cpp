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

#include "TrackNew.hpp"
#include <array>
#include <gtest/gtest.h>
#include <iostream>
#include <memory_resource>
#include <string>
#include <vector>
#include <list>
#include <type_traits>

template <template <typename T> class Collection, typename Type>
TrackNew::Status test_body(auto default_value) {

  constexpr bool is_polymorphic_collection =
      std::is_same_v<typename Collection<Type>::allocator_type,
                     std::pmr::polymorphic_allocator<Type>>;

  // buffer is a static array allocated on the stack
  std::array<std::byte, 200'000> buffer;

  // an instance of monotonic_buffer_resource
  // using memory allocated on the stack through buffer
  std::pmr::monotonic_buffer_resource pool{buffer.data(), buffer.size()};

  TrackNew::reset();

  auto collection = [&] {
    if constexpr (is_polymorphic_collection) {
      return Collection<Type>{&pool};
    } else {
      return Collection<Type>();
    }
  }();

  for (int i = 0; i < 1000; ++i) {
    collection.emplace_back(default_value);
  }

  return TrackNew::status();
}

TEST(PMR, std_string_in_vector_should_use_many_new) {
  TrackNew::Status status = test_body<std::vector, std::string>(
      "A string that beats SSO or Small String Optimization");
  EXPECT_GT(status.numAlloc, 0);
  EXPECT_GT(status.sumSize, 0);
  std::cout << status << '\n';
}

TEST(PMR, std_string_in_pmr_vector_should_use_new) {
  TrackNew::Status status = test_body<std::pmr::vector, std::string>(
      "A string that beats SSO or Small String Optimization");
  EXPECT_GT(status.numAlloc, 0);
  EXPECT_GT(status.sumSize, 0);
  std::cout << status << '\n';
}

TEST(PMR, std_string_in_pmr_vector_should_not_use_new) {
  TrackNew::Status status = test_body<std::pmr::vector, std::pmr::string>(
      "A string that beats SSO or Small String Optimization");
  EXPECT_EQ(status.numAlloc, 0);
  EXPECT_EQ(status.sumSize, 0);
  std::cout << status << '\n';
}

#endif