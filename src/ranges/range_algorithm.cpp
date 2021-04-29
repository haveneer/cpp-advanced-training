#include <version>
#ifndef __cpp_lib_ranges
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_lib_ranges)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

//#region [Headers]
//#region [Type display]
#ifdef DEBUG_TYPE
#include "type.hpp" // advanced type printing
#else // clang-format off
#include <typeinfo>
template <typename T> std::string type() { if (std::is_same_v<std::remove_extent_t<std::remove_const_t< std::remove_pointer_t<std::remove_reference_t<T>>>>, T>) return typeid(T).name(); else if (std::is_array_v<T>) return type<std::remove_extent_t<T>>() + "[]"; else if (std::is_const_v<T>) return type<std::remove_const_t<T>>() + " const"; else if (std::is_pointer_v<T>) return type<std::remove_pointer_t<T>>() + "*"; else if (std::is_reference_v<T>) return type<std::remove_reference_t<T>>() + ((std::is_lvalue_reference_v<T>) ? "&" : "") + ((std::is_rvalue_reference_v<T>) ? "&&" : ""); else std::string("cannot decode ") + typeid(T).name(); }
#endif              // clang-format on
//#endregion
#include <algorithm>
#include <concepts>
#include <forward_list>
#include <iomanip>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>

auto print = [](auto &&a, auto &&b) {
  std::cout << a << " on " << b << "\n";
  return true;
};
//#endregion

namespace ranges = std::ranges;

template <ranges::range Range, class Proj = std::identity>
void quicksort(Range &&range, Proj proj = {}) {
  if (ranges::distance(range) <= 1) {
    return;
  }
  auto first = std::begin(range);
  auto last = std::end(range);
  auto pivot = ranges::next(first, ranges::distance(range) / 2, last);

  if constexpr (requires { first + 1; }) {
    static const auto printOnce = print("Random access algo", type<Range>());
    ranges::nth_element(range, pivot, ranges::less{}, proj);
    quicksort(ranges::subrange(first, pivot), proj);
    quicksort(ranges::subrange(pivot + 1, last), proj);
  } else {
    static const auto printOnce = print("Forward only access algo", type<Range>());
    auto predicate1 = [ref = proj(*pivot)](const auto &e) { return e < ref; };
    std::ranges::subrange tail1 = ranges::partition(range, predicate1, proj);
    auto predicate2 = [ref = proj(*pivot)](const auto &e) { return e <= ref; };
    std::ranges::subrange tail2 = ranges::partition(tail1, predicate2, proj);
    quicksort(ranges::subrange(first, tail1.begin()), proj);
    quicksort(tail2, proj);
  }
}

struct Person {
  std::string name;
  int age;
  float taille;
  float poids;
};

auto imc(const Person &p) { return p.poids / (p.taille * p.taille); }

// Use this custom operator to 'print' a Person object
std::ostream &operator<<(std::ostream &o, const Person &p) {
  return o << "Person{" << std::setw(8) << ("\"" + p.name + "\"") << ","
           << std::setw(3) << p.age << ", " << std::setw(4) << p.taille << ","
           << std::setw(3) << p.poids << " imc=" << std::setw(4) << imc(p) << "}";
}

int main() {
  std::vector persons = {
      // HINT try with vector and forward_list
      Person{"John", 25, 1.82, 75.},   Person{"Emy", 28, 1.82, 65.},
      Person{"Paul", 28, 1.85, 60.},   Person{"Julie", 35, 1.70, 50.},
      Person{"Raph", 32, 1.69, 55.},   Person{"Phil", 32, 1.82, 80.},
      Person{"Elodie", 24, 1.62, 50.}, Person{"Simon", 25, 1.82, 60.},
      Person{"Alice", 20, 1.72, 60.},  Person{"Eddy", 27, 1.82, 85.},
  };

  //#region [Partition demo]
  if (0) {
    auto tail =
        ranges::partition(persons, [](const Person &p) { return p.age <= 30; });

    std::cout << "Partition by age <= 30:\n";
    ranges::copy(std::begin(persons), std::begin(tail),
                 std::ostream_iterator<Person>(std::cout, "\n"));
    std::cout << "*\n";
    ranges::copy(tail, std::ostream_iterator<Person>(std::cout, "\n"));
    std::cout << "\n";
  }
  //#endregion

  quicksort(persons, [](const Person &p) { return imc(p); });

  std::cout << "Result:\n";
  ranges::copy(persons, std::ostream_iterator<Person>(std::cout, "\n"));
}

#endif