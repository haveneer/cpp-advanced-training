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
#include <algorithm>
#include <concepts>
#include <iomanip>
#include <iostream>
#include <locale>
#include <ranges>
#include <string>
#include <vector>
//#endregion

namespace ranges = std::ranges;

template <ranges::range Range, class Proj = std::identity>
void quicksort(Range &&range, Proj proj = {}) {
  if (ranges::distance(range) <= 1) {
    return;
  }
  auto first = std::begin(range);
  auto last = std::end(range);
  auto pivot = proj(*ranges::next(first, ranges::distance(range) / 2, last));

  auto predicate1 = [pivot](const auto &e) { return e < pivot; };
  std::ranges::subrange tail1 = ranges::partition(range, predicate1, proj);
  auto predicate2 = [pivot](const auto &e) { return e <= pivot; };
  std::ranges::subrange tail2 = ranges::partition(tail1, predicate2, proj);

  quicksort(ranges::subrange(first, tail1.begin()), proj);
  quicksort(tail2, proj);
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
      Person{"John", 25, 1.82, 75.},   Person{"Emy", 28, 1.82, 65.},
      Person{"Paul", 28, 1.85, 60.},   Person{"Julie", 35, 1.70, 50.},
      Person{"Raph", 32, 1.69, 55.},   Person{"Phil", 32, 1.82, 80.},
      Person{"Elodie", 24, 1.62, 50.}, Person{"Simon", 25, 1.82, 60.},
      Person{"Alice", 20, 1.72, 60.},  Person{"Eddy", 27, 1.82, 85.},
  };

  //#region [Collapse all]
  auto tail =
      ranges::partition(persons, [](const Person &p) { return p.age <= 30; });

  std::cout << "Partition by age <= 30:\n";
  ranges::copy(std::begin(persons), std::begin(tail),
               std::ostream_iterator<Person>(std::cout, "\n"));
  std::cout << "*\n";
  ranges::copy(tail, std::ostream_iterator<Person>(std::cout, "\n"));
  std::cout << "\n";
  //#endregion

  quicksort(persons, [](const Person &p) { return imc(p); });

  std::cout << "Result:\n";
  ranges::copy(persons, std::ostream_iterator<Person>(std::cout, "\n"));
  std::cout << '\n';
}
#endif