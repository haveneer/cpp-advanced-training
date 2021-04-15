// unsupported features should be defined in unsupported_features variables before
// including this file

// #define STR(X) #X
// const char * unsupported_features[] = {STR(#feature_name)};

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <thread>

int main() {
  std::filesystem::path file_path{"unsupported_examples.md"};
  while (std::filesystem::exists("_lock")) { // not a perfect file lock :/
    using namespace std::chrono_literals;
    std::puts("Waiting for file lock");
    std::this_thread::sleep_for(100ms);
  }
  std::ofstream lock("_lock");
  std::ofstream f(file_path, std::ios_base::app);
  for (auto &&t : unsupported_features) {
    f << "* " << __DATE__ << " at " << __TIME__ << " : " << t << " in " << __FILE__
      << '\n';
    std::cout << "Unsupported feature " << t << '\n';
  }
  f.close();
  lock.close();
  std::filesystem::remove("_lock");
}
