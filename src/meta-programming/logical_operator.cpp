//#region [Collapse all]
#ifdef DEBUG_TYPE
#include "type.hpp" // advanced type printing
#else // clang-format off
#include <typeinfo>
template <typename T> std::string type() { if (std::is_same_v<std::remove_extent_t<std::remove_const_t< std::remove_pointer_t<std::remove_reference_t<T>>>>, T>) return typeid(T).name(); else if (std::is_array_v<T>) return type<std::remove_extent_t<T>>() + "[]"; else if (std::is_const_v<T>) return type<std::remove_const_t<T>>() + " const"; else if (std::is_pointer_v<T>) return type<std::remove_pointer_t<T>>() + "*"; else if (std::is_reference_v<T>) return type<std::remove_reference_t<T>>() + ((std::is_lvalue_reference_v<T>) ? "&" : "") + ((std::is_rvalue_reference_v<T>) ? "&&" : ""); else std::string("cannot decode ") + typeid(T).name(); }
#endif              // clang-format on

#include <iostream>
#include <type_traits>
#include <vector>
//#endregion

// logical operators work on std::integral_constant<bool, ?> not true static bool
template <typename T, typename... Ts>
constexpr bool isAllSame = std::conjunction_v<std::is_same<T, Ts>...>;

template <typename T, typename... Ts>
constexpr bool isAllSame2 = // Morgan's laws is action
    std::negation_v<std::disjunction<std::negation<std::is_same<T, Ts>>...>>;

template <typename T, typename... Ts> // using fold expression (less efficient)
constexpr bool isAllSame3 = (std::is_same_v<T, Ts> && ...);

//#region [Hidden type definition]
using T1 = long;
using T2 = signed long;
using T3 = long int;
using T4 = signed long int;
//#endregion

int main() { // TODO: find 4 basic types of different names but are the same type
  std::cout << "are all same : " << isAllSame<T1, T2, T3, T4> << '\n';
  std::cout << "are all same : " << isAllSame2<T1, T2, T3, T4> << '\n';
  std::cout << "are all same : " << isAllSame3<T1, T2, T3, T4> << '\n';
}