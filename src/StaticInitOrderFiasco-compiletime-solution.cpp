#include <iostream>

extern constinit int staticCompanion;
/* constinit */ auto staticMain = staticCompanion;

int main() { // What happens ?
  std::cout << "staticMain: " << staticMain << '\n';
}