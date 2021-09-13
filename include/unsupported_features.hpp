// unsupported features should be defined in unsupported_features variables before
// including this file

#define STR(X) #X
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string_view>
#include <sys/stat.h>
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

inline bool file_exists(const std::string &filename) {
  struct stat buffer;
  // return std::filesystem::exists(name); // not portable with NVC 21.5
  return (stat(filename.c_str(), &buffer) == 0);
}

struct UnsupportedReportContext {
  UnsupportedReportContext(std::string_view date, std::string_view time) {
    while (file_exists("_lock")) { /* not a perfect file lock */
      using namespace std::chrono_literals;
      std::puts("Waiting for file lock");
      std::this_thread::sleep_for(100ms);
    }
    lock.open("_lock");
    f.open(file_path, std::ios_base::app);
    f << "* " << date << " at " << time << " : ";
  }

  ~UnsupportedReportContext() {
    f.close();
    lock.close();
    std::filesystem::remove("_lock");
  }

  std::filesystem::path file_path{"unsupported_examples.md"};
  std::ofstream lock;
  std::ofstream f;
};

#define REPORT_FEATURES(...)                                                    \
  int main() {                                                                  \
    UnsupportedReportContext context{__DATE__, __TIME__};                       \
    context.f << "unsupported set ( ";                                          \
    std::cout << "Unsupported feature set ( ";                                  \
    for (auto &&t : __VA_ARGS__) {                                              \
      context.f << t << ' ';                                                    \
      std::cout << t << ' ';                                                    \
    }                                                                           \
    context.f << ") in " << remove_common_prefix(__FILE__, this_file_reference) \
              << '\n';                                                          \
    std::cout << ")";                                                           \
  }                                                                             \
  struct dummy // to avoid warning about "extra ';'" when using this macro
