import pimpled_module;

//#region [Collapse all]
#ifdef DEBUG_TYPE
#include "type.hpp" // advanced type printing
#else // clang-format off
#include <typeinfo>
template <typename T> std::string type() { if (std::is_same_v<std::remove_extent_t<std::remove_const_t< std::remove_pointer_t<std::remove_reference_t<T>>>>, T>) return typeid(T).name(); else if (std::is_array_v<T>) return type<std::remove_extent_t<T>>() + "[]"; else if (std::is_const_v<T>) return type<std::remove_const_t<T>>() + " const"; else if (std::is_pointer_v<T>) return type<std::remove_pointer_t<T>>() + "*"; else if (std::is_reference_v<T>) return type<std::remove_reference_t<T>>() + ((std::is_lvalue_reference_v<T>) ? "&" : "") + ((std::is_rvalue_reference_v<T>) ? "&&" : ""); else return std::string("cannot decode ") + typeid(T).name(); }
#endif              // clang-format on
//#endregion

#include <iostream>

int main() {
  S s;
  s.do_stuff();     // OK.
  auto p = s.get(); // OK: pointer to incomplete type.
  std::cout << "Type of p is " << type<decltype(p)>() << '\n';
  // auto impl = *s.get(); // ill-formed: use of undefined type 'Impl'.
}