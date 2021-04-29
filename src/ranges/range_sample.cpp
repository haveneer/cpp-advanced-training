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

//#region [Collapse all]
#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <ranges>
#include <string>
#include <vector>
//#endregion

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

template <typename Class, typename Value> auto projector(Value Class::*field) {
  return [field](const Class &c) { return c.*field; };
}

int main() {
  namespace ranges = std::ranges;
  namespace views = std::views;

  // You cannot modify original data
  const std::vector persons = {
      Person{"John", 25, 1.82, 75.},   Person{"Emy", 28, 1.82, 65.},
      Person{"Paul", 28, 1.85, 60.},   Person{"Julie", 35, 1.70, 50.},
      Person{"Raph", 32, 1.69, 55.},   Person{"Phil", 32, 1.82, 80.},
      Person{"Elodie", 24, 1.62, 50.}, Person{"Simon", 25, 1.82, 60.},
      Person{"Alice", 20, 1.72, 60.},  Person{"Eddy", 27, 1.82, 85.},
  };

  auto println = [](auto &&x) { std::cout << x << '\n'; };
  auto is_selected_imc = [](const Person &p) {
    const auto v = imc(p);
    return (18 <= v && v <= 25);
  };
  auto is_selected_age = [](const Person &p) { return p.age <= 30; };

  std::cout << "------ Preamble ------\n";
  {
    std::cout.precision(3);
    for (auto i = std::begin(persons), e = std::end(persons); i != e;) {
      std::cout << *i++;
      if (i != e)
        std::cout << "   " << *i++;
      std::cout << '\n';
    }
  }
  std::cout << "------ Question 1 ------\n";
  { //
    ranges::for_each(persons | views::filter([](auto &p) { return p.age <= 30; }),
                     println, projector(&Person::name));
  }
  std::cout << "------ Question 2 ------\n";
  { //
    ranges::for_each(views::filter(is_selected_imc)(persons), println);
  }
  std::cout << "------ Question 3 ------\n";
  {
    std::vector tmp = persons;
    const auto begin = std::begin(tmp);
    const auto pivot = ranges::next(begin, 4, std::end(tmp));

    ranges::nth_element(tmp, pivot, std::greater<>(), projector(&Person::poids));
    const auto total = std::accumulate( // not available in ranges::
        begin, pivot, 0, [](const auto &a, const Person &b) { return a + b.poids; });
    std::cout << "Total = " << total << '\n';
  }
  std::cout << "------ Question 4 ------\n";
  {
    std::vector<Person> tmp;
    ranges::copy(persons                               //
                     | views::filter(is_selected_imc)  //
                     | views::filter(is_selected_age), //
                 std::back_inserter(tmp));
    const auto begin = std::begin(tmp);
    const auto pivot = ranges::next(begin, 4, std::end(tmp));
    ranges::nth_element(tmp, pivot, std::greater<>(), projector(&Person::poids));
    const auto total = std::accumulate( // not available in ranges::
        begin, pivot, 0, [](const auto &a, const Person &b) { return a + b.poids; });
    std::cout << "Total = " << total << '\n';
    ranges::for_each(tmp.begin(), pivot, println, projector(&Person::name));
  }
}

#endif