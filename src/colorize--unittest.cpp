#include <gtest/gtest.h>
#include <iostream>
#include <map>
#include <sstream>
#include <tuple>
#include <vector>

#include "colorize.hpp"

struct Object {
  Object() { std::cout << "Object default ctor " << Colorize{this} << std::endl; }

  Object(const Object &obj) {
    std::cout << "Object copy ctor " << Colorize{&obj} << " -> " << Colorize{this}
              << std::endl;
  }
  void operator=(const Object &obj) {
    std::cout << "Object copy =" << Colorize{&obj} << " -> " << Colorize{this}
              << std::endl;
  }
  Object(Object &&obj) {
    std::cout << "Object move ctor " << Colorize{&obj} << " -> " << Colorize{this}
              << std::endl;
  }
  void operator=(Object &&obj) {
    std::cout << "Object move =" << Colorize{&obj} << " -> " << Colorize{this}
              << std::endl;
  }
  ~Object() { std::cout << "Object destroy " << Colorize{this} << std::endl; }
};

std::ostream &operator<<(std::ostream &o, const Object &obj) {
  return o << "Object{" << Colorize{&obj} << "}";
}

TEST(colorize, runOk){
  auto x = Object();
  auto y = x;
}
