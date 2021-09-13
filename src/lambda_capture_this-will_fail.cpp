//#region [Includes]
#include <array>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
//#endregion

//#region [Colorization]
#include <iomanip>
#include <iostream>
#if __has_include("colorize.hpp")
#include "colorize.hpp"
#else
template <typename T> struct Colorize { T x; };
template <typename T>
std::ostream &operator<<(std::ostream &o, const Colorize<T> &t) {
  return o << t.x;
}
template <typename T> Colorize(T &&t) -> Colorize<T>;
#endif
//#endregion

class MyFactory {
  //#region [ctors and dtor]
public:
  MyFactory() : m_name(make_name()) {
    std::cout << m_name << " default ctor" << std::endl;
  }
  MyFactory(const MyFactory &f) : m_name(make_name()) {
    std::cout << m_name << " copy ctor from " << Colorize{&f} << std::endl;
  }
  MyFactory(MyFactory &&f) noexcept : m_name(make_name()) {
    std::cout << m_name << " move ctor from " << Colorize{&f} << std::endl;
  }
  ~MyFactory() { std::cout << m_name << " dtor" << std::endl; }
  void operator=(const MyFactory &) = delete;
  void operator=(MyFactory &&) = delete;

private:
  std::string make_name() {
    std::ostringstream oss;
    oss << "MyFactory " << Colorize{this};
    return oss.str();
  }
  //#endregion

public:
  auto makeOne() const {
    return [=] {
      std::cout << "I have been built by " //
                << std::flush << m_name << std::endl;
    };
  }

private:
  std::string m_name;
};

//#region [Custom memory management]
std::array<std::byte, sizeof(MyFactory)> buffer;
#ifdef my_cpp_feature_lambda_in_unevaluated_context
using Deleter = decltype([](MyFactory *ptr) { // Define a type from a lambda
  ptr->MyFactory::~MyFactory();               // It can be assigned and copied
  buffer.fill(std::byte{0xFF});               // Deleter d{}; is valid !
});
#else
struct Deleter {
  void operator()(MyFactory *ptr) {
    ptr->MyFactory::~MyFactory();
    buffer.fill(std::byte{0xFF});
  }
};
#endif
auto makeFactory() {
  return std::unique_ptr<MyFactory, Deleter>{new (buffer.data()) MyFactory};
}
//#endregion

auto buildFactoryAndMakeOne() {
  std::unique_ptr factory = makeFactory();
  return factory->makeOne();
}

int main() {
  auto f = buildFactoryAndMakeOne();
  f();
}