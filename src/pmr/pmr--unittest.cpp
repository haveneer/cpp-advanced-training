#include "TrackNew.hpp"
#include <array>
#include <gtest/gtest.h>
#include <iostream>
#include <memory_resource>
#include <string>

template <template <typename T> class Collection, typename Type>
TrackNew::Status test_body(auto default_value) {
  TrackNew::reset();

  // buffer is a static array allocated on the stack
  std::array<std::byte, 200'000> buffer;

  // an instance of monotonic_buffer_resource
  // using memory allocated on the stack through buffer
  std::pmr::monotonic_buffer_resource pool{buffer.data(), buffer.size()};

  Collection<Type> collection{&pool};

  for (int i = 0; i < 1000; ++i) {
    collection.emplace_back("A string that beats SSO or Small String Optimization");
  }

  return TrackNew::status();
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
