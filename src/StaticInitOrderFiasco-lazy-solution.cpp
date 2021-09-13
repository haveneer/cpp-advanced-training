#include <iostream>

int &staticCompanion();
auto staticMain = staticCompanion(); // call a runtime function to init global data

int main() { // What happens ?
  std::cout << "staticMain: " << staticMain << '\n';
}