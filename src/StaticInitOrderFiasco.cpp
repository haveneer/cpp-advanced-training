#include <iostream>

extern int staticCompanion;
auto staticMain = staticCompanion; // use a static data in a foreign compilation unit

int main() { // What happens ?
  std::cout << "staticMain: " << staticMain << '\n';
}