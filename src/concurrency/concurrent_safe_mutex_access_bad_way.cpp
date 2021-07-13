//#region [Collapse all]
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
//#endregion

int main() {
  std::mutex m;
  std::vector<char> v;
  std::thread ta([&] {
    for (int i = 0; i < 20; ++i) {
      m.lock();
      v.push_back('a');
      m.unlock();
    }
  });
  std::thread tb([&] {
    for (int i = 0; i < 20; ++i) {
      m.lock();
      v.push_back('b');
      m.unlock();
    }
  });
  ta.join();
  tb.join();

  std::string_view result(std::data(v), v.size());
  std::cout << result << std::endl;
}