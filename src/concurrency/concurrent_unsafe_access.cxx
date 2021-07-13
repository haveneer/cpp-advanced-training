//#region [Collapse all]
#include <iostream>
#include <thread>
#include <vector>
//#endregion

int main() {
  std::vector<char> v;
  std::thread ta([&] {
    for (int i = 0; i < 20; ++i)
      v.push_back('a');
  });
  std::thread tb([&] {
    for (int i = 0; i < 20; ++i)
      v.push_back('b');
  });
  ta.join();
  tb.join();

  std::string_view result(std::data(v), v.size());
  std::cout << result << std::endl;
}