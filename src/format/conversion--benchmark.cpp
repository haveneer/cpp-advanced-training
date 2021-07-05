//#region [Collapse all]
#include <benchmark/benchmark.h>
#include <charconv>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>
//#endregion

//#region [Compiler dependent <format> header]
#include <fmt/core.h>
#include <fmt/format.h>
#include <version>
#ifdef __cpp_lib_format
#include <format>
#else
namespace std {
using namespace fmt;
}
#endif
//#endregion

constexpr size_t buffer_size = 24;

struct Data {
  std::vector<double> values;

  auto begin() const { return values.begin(); }
  auto end() const { return values.end(); }

  Data() : values(1'000'000) {
    std::mt19937 gen;
    std::uniform_real_distribution<double> dist(0, 1000);
    std::generate(values.begin(), values.end(), [&]() { return dist(gen); });
  }
} data;

struct Digester {
  benchmark::State &state;
  unsigned digest = 0;

  explicit Digester(benchmark::State &s) : state(s) {}

  ~Digester() noexcept(false) {
    state.SetItemsProcessed(state.iterations() * data.values.size());
    benchmark::DoNotOptimize(digest);
  }

  // Computes a digest of data. It is used both to prevent compiler from
  // optimizing away the benchmarked code (could be used to check results)
  // The overhead is less than 2.5% compared to just DoNotOptimize.
  inline unsigned compute_digest(std::string_view data) {
    unsigned digest = 0;
    for (char c : data)
      digest += c;
    return digest;
  }

  inline void add(std::string_view s) {
    //    if (digest == 0) // to debug results
    //      std::cout << this << " : " << s << "\n";
    digest += compute_digest(s);
    // benchmark::DoNotOptimize(s);
  }
};

void sprintf(benchmark::State &state) {
  auto dc = Digester(state);
  for (auto s : state) {
    for (auto value : data) {
      char buffer[buffer_size];
      unsigned size = std::sprintf(buffer, "%.16g", value);
      dc.add({buffer, size});
    }
  }
}
BENCHMARK(sprintf);

void std_ostringstream(benchmark::State &state) {
  auto dc = Digester(state);
  std::ostringstream os;
  for (auto s : state) {
    for (auto value : data) {
      os.str(std::string());
      os << std::setprecision(16) << value;
      auto s = os.str(); // C++20 .view(); .str() otherwise
      dc.add(s);
    }
  }
}
BENCHMARK(std_ostringstream);

void fmt_format(benchmark::State &state) {
  auto dc = Digester(state);
  for (auto s : state) {
    for (auto value : data) {
      std::string s = std::format("{:.16g}", value);
      dc.add(s);
    }
  }
}
BENCHMARK(fmt_format);

void fmt_format_to(benchmark::State &state) {
  auto dc = Digester(state);
  for (auto s : state) {
    for (auto value : data) {
      char buffer[buffer_size];
      auto end = std::format_to(buffer, "{:.16g}", value);
      unsigned size = end - buffer;
      dc.add({buffer, size});
    }
  }
}
BENCHMARK(fmt_format_to);

void std_to_string(benchmark::State &state) {
  auto dc = Digester(state);
  for (auto s : state) {
    for (auto value : data) {
      std::string s = std::to_string(value);
      dc.add(s);
    }
  }
}
BENCHMARK(std_to_string);

#ifdef my_cpp_to_chars_on_double
void std_to_chars(benchmark::State &state) {
  auto dc = Digester(state);

  for (auto s : state) {
    for (auto value : data) {
      char buffer[buffer_size];
      auto res = std::to_chars(buffer, buffer + sizeof(buffer), value,
                               std::chars_format::general, 16);
      unsigned size = res.ptr - buffer;
      dc.add({buffer, size});
    }
  }
}
BENCHMARK(std_to_chars);
#endif

BENCHMARK_MAIN();
