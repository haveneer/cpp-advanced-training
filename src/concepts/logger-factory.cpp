#include <version>
#if !defined(__cpp_concepts) || !defined(__cpp_lib_concepts)
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_concepts), STR(__cpp_lib_concepts)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

//#region [Collapse all]
#ifdef DEBUG_TYPE
#include "type.hpp" // advanced type printing
#else // clang-format off
#include <typeinfo>
template <typename T> std::string type() { if (std::is_same_v<std::remove_extent_t<std::remove_const_t< std::remove_pointer_t<std::remove_reference_t<T>>>>, T>) return typeid(T).name(); else if (std::is_array_v<T>) return type<std::remove_extent_t<T>>() + "[]"; else if (std::is_const_v<T>) return type<std::remove_const_t<T>>() + " const"; else if (std::is_pointer_v<T>) return type<std::remove_pointer_t<T>>() + "*"; else if (std::is_reference_v<T>) return type<std::remove_reference_t<T>>() + ((std::is_lvalue_reference_v<T>) ? "&" : "") + ((std::is_rvalue_reference_v<T>) ? "&&" : ""); else return std::string("cannot decode ") + typeid(T).name(); }
#endif              // clang-format on

#include <concepts>
#include <fstream>
#include <iostream>
#include <string>
//#endregion

//#region [Definitions]
template <typename T>
concept Printable = requires(std::ostream &o, T t) {
  o << t;
};

template <typename Support> class Logger {
public:
  template <typename... Args>
  requires std::constructible_from<Support, Args...> //
  Logger(Args &&...args) : m_support{std::forward<Args...>(args)...} {
    std::cout << "Build ";
    auto decorator = [&]<typename X>(X &) -> decltype(auto) {
      return std::cout << "'" << type<X>() << "' ";
    };
    decorator(m_support) << "support using ( ";
    (decorator(args), ...) << ") argument(s)\n";
  }
  // clang-format off
  template <typename... Args>
  requires(Printable<Args> &&...) //
  void operator()(Args... args) {
    ((m_support << " " << std::forward<Args>(args)), ...) << '\n';
  }
  // clang-format on
private:
  Support m_support;
};

struct PrintableStruct {
  int data;

  friend std::ostream &operator<<(std::ostream &o, const PrintableStruct &s) {
    return o << "PrintableStruct{.data=" << s.data << "}";
  }
};

struct NotPrintableStruct {
  int data;
};
//#endregion

// clang-format off
int main() {
  //Logger<std::ofstream> log("file.txt");
  Logger<std::ostream &> log(std::cout);

  // without concept: error: use of deleted function 'std::basic_ostream<_CharT, _Traits>::basic_ostream
  //    with concept: error: no matching function for call to 'Logger<std::basic_ostream<char> >::Logger(std::ostream&)'
  //                  note: constraints not satisfied ( + constraint details)
  // Logger<std::ostream> log(std::cout); 

  std::ostream &o{std::cout};

  log(1, " + ", 1, " is ", 2);
  log(PrintableStruct{42});
  
  // without concept : error: 500 lines !
  //    with concept : error: no match for call to '(Logger<std::basic_ostream<char>&>) (int, NotPrintableStruct)'
  //                   note: constraints not satisfied ( + constraint details)
  // log(1, NotPrintableStruct{666});
}
// clang-format on

#endif