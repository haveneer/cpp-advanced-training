//#region [Collapse all]
#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <numeric>
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

int main() {
  // You cannot modify original data
  const std::vector persons = {
      Person{"John", 25, 1.82, 75.},   Person{"Emy", 28, 1.82, 65.},
      Person{"Paul", 28, 1.85, 60.},   Person{"Julie", 35, 1.70, 50.},
      Person{"Raph", 32, 1.69, 55.},   Person{"Phil", 32, 1.82, 80.},
      Person{"Elodie", 24, 1.62, 50.}, Person{"Simon", 25, 1.82, 60.},
      Person{"Alice", 20, 1.72, 60.},  Person{"Eddy", 27, 1.82, 85.},
  };

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
  {
    std::for_each(persons.begin(), persons.end(), [](const Person &p) {
      if (p.age <= 30) {
        std::cout << p.name << '\n';
      }
    });
  }
  std::cout << "------ Question 2 ------\n";
  {
    std::copy_if(persons.begin(), persons.end(),
                 std::ostream_iterator<Person>{std::cout, "\n"},
                 [](const Person &p) {
                   const auto v = imc(p);
                   return (18 <= v && v <= 25);
                 });
  }
  std::cout << "------ Question 3 ------\n";
  {
    std::vector sorted_persons = persons;
    const auto begin = std::begin(sorted_persons);
    const auto end = std::end(sorted_persons);
    const auto max_distance = std::distance(begin, end);
    const auto pivot = std::next(begin, std::min(4l, max_distance));

    std::nth_element(begin, pivot, end, [](const Person &a, const Person &b) {
      return a.poids > b.poids;
    });

    const auto total = std::accumulate(
        begin, pivot, 0, [](const auto &a, const Person &b) { return a + b.poids; });
    std::cout << "Total = " << total << '\n';
  }
  std::cout << "------ Question 4 ------\n";
  {
    auto is_selected_imc = [](const Person &p) {
      const auto v = imc(p);
      return (18 <= v && v <= 25);
    };
    auto is_selected_age = [](const Person &p) { return p.age <= 30; };

    std::vector<Person> tmp;
    std::copy_if(persons.begin(), persons.end(), std::back_inserter(tmp),
                 is_selected_imc);
    auto split_point =
        std::remove_if(std::begin(tmp), std::end(tmp), std::not_fn(is_selected_age));
    tmp.erase(split_point, std::end(tmp)); // remove useless elements (not necessary)
    //                                     // WARNING: this invalidates iterators
    //#region [Debug print]
    // std::for_each(tmp.begin(), tmp.end(),
    //              [](const Person &p) { std::cout << p << '\n'; });
    //#endregion
    const auto begin = std::begin(tmp);
    const auto end = std::end(tmp);
    const auto max_distance = std::distance(begin, end);
    const auto pivot = std::next(tmp.begin(), std::min(4l, max_distance));
    std::nth_element(begin, pivot, end, [](const Person &a, const Person &b) {
      return a.poids > b.poids;
    });
    const auto total = std::accumulate(
        begin, pivot, 0, [](const auto &a, const Person &b) { return a + b.poids; });
    std::cout << "Total = " << total << '\n';
    std::for_each(tmp.begin(), pivot,
                  [](const Person &p) { std::cout << p.name << '\n'; });
  }
}