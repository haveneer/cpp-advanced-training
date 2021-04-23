#include <algorithm>
#include <iostream>
#include <list>

int main() {
  std::list<int> container = {51, 25,   54, 78, 52, 1,   45, 78,
                              42, 1245, 1,  48, 1,  245, 4,  5};
  std::sort(container.begin(), container.end());

  for (auto &&x : container) {
    std::cout << x << ' ';
  }
}