// unsupported features should be defined in unsupported_features variables before
// including this file

#define STR(X) #X
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string_view>
#include <thread>

std::string_view this_file_reference = __FILE__; // this include gets reference root

std::string_view remove_common_prefix(std::string_view file,
                                      std::string_view reference) {
  auto [i1, i2] = std::mismatch(file.begin(), file.end(), //
                                reference.begin(), reference.end());
  auto prefix_len = std::distance(file.begin(), i1);
  file.remove_prefix(prefix_len);
  return file;
}

#define REPORT_FEATURES(...)                                                     \
  int main() {                                                                   \
    std::filesystem::path file_path{"unsupported_examples.md"};                  \
    while (std::filesystem::exists("_lock")) { /* not a perfect file lock */     \
      using namespace std::chrono_literals;                                      \
      std::puts("Waiting for file lock");                                        \
      std::this_thread::sleep_for(100ms);                                        \
    }                                                                            \
    std::ofstream lock("_lock");                                                 \
    std::ofstream f(file_path, std::ios_base::app);                              \
    f << "* " << __DATE__ << " at " << __TIME__ << " : unsupported ( ";          \
    std::cout << "Unsupported feature set ( ";                                    \
    for (auto &&t : __VA_ARGS__) {                                               \
      f << t << ' ';                                                             \
      std::cout << t << ' ';                                                     \
    }                                                                            \
    f << ") in " << remove_common_prefix(__FILE__, this_file_reference) << '\n'; \
    std::cout << ")";                                                           \
                                                                                 \
    f.close();                                                                   \
    lock.close();                                                                \
    std::filesystem::remove("_lock");                                            \
  }
