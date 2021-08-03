export module helloworld_module; // define module name and export it
import<iostream>;
export void hello(const char *name = "world") {
  std::cout << "Hello " << name << "! (from a C++20 module)\n";
}