export module helloworld4;
import<cstdio>;
import<iostream>;
#include <string> // https://github.com/clangd/clangd/issues/797
export void hello(const char * name = "world") {
  //
  std::cout << "Hello " << name << "!" << std::endl;
}