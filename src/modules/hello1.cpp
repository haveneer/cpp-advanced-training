module;
#include <iostream>
#include <string_view>
export module hello1;
export void greeter(const std::string_view &name) {
  std::cout << "Hello " << name << "!\n";
}