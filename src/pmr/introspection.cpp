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

#include <cstdint>

// Special memory management :
// https://en.cppreference.com/w/cpp/header/memory_resource
// see also:
// https://stackoverflow.com/questions/38010544/polymorphic-allocator-when-and-why-should-i-use-it
// https://www.bfilipek.com/2020/06/pmr-hacking.html
// https://www.bfilipek.com/2020/08/pmr-dbg.html

// later: use https://github.com/google/benchmark

#include <array>
#include <iomanip>
#include <iostream>
#include <list>
#include <memory_resource>
#include <set>
#include <string>
#include <vector>

template <std::size_t N, typename Container>
void print_buffer(const std::string_view &header,
                  const std::array<std::uint8_t, N> &buffer,
                  const Container &container) {
  std::cout
      << "---------------------------------------------------------------------\n";
  std::cout << header << "\n";
  std::cout << "---------------------\n";
  for (const auto &e : container) {
    std::string hint =
        (&e < buffer.data()) || (&e >= buffer.data() + N) ? " (out of buffer)" : "";
    std::cout << "Item @ " << static_cast<const void *>(&e) << hint << "\n";
  }
  std::cout
      << "---------------------------------------------------------------------\n";
  std::cout << std::setw(18) << "Buffer"
            << "\n";
  const std::size_t line_size = 16;
  for (std::size_t i = 0; i < N; ++i) {
    if (i % line_size == 0) {
      const void *addr = &(buffer[i]);
      std::cout << std::setw(18) << std::hex << addr << " ->";
    }
    std::cout << " " << std::setfill('0') << std::hex << std::setw(2)
              << (int)buffer[i] << std::setfill(' ');
    if (i % line_size == line_size - 1) {
      std::cout << "\n";
    }
  }
}

// ------------------------------------------------>%
int main() {
  using namespace std::string_literals;
  std::array<std::uint8_t, 96> buffer{};
  std::pmr::monotonic_buffer_resource rsrc(buffer.data(), buffer.size());

  // Choose one of these datastructures (list, vector, set) to observe
  // buffer evolution while inserting items

  //    std::pmr::list<uint8_t> container{&rsrc};
  //    auto hint = [&container] { return ""s; };

  std::pmr::vector<uint8_t> container{&rsrc};
  auto hint = [&container, memo = (uint8_t *)nullptr]() mutable {
    if (!container.empty() && memo != container.data()) {
      memo = container.data();
      return " (has been resized)"s;
    } else {
      return ""s;
    }
  };

  //  std::pmr::set<uint8_t> container{&rsrc};
  //  auto hint = [&container] { return ""s; };

  print_buffer("New container" + hint(), buffer, container);
  container.insert(container.end(), 0x1);
  print_buffer("After insert 1" + hint(), buffer, container);
  container.insert(container.end(), 0x2);
  print_buffer("After insert 2" + hint(), buffer, container);
  container.insert(container.end(), 0x3);
  print_buffer("After insert 3" + hint(), buffer, container);
  container.insert(container.end(), 0x4);
  print_buffer("After insert 4" + hint(), buffer, container);
  container.insert(container.end(), 0x5);
  print_buffer("After insert 5" + hint(), buffer, container);
}

#endif