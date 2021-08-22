#include <version>
#if !defined(__cpp_lib_parallel_algorithm) || !defined(__cpp_concepts)
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_lib_parallel_algorithm), STR(__cpp_concepts)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

/* Subject: count the number of words of a text.
 * Hint: count the number of beginnings
 * Hint: a beginning is a transition between a non-word char and a word char
 */

#include "parallel_word_count.hpp"
#include <benchmark/benchmark.h>
#include <execution>
#include <string>

static void seq_word_count(benchmark::State &state) {
  const std::string text = make_text(10000);
  for (auto _ : state) {
    benchmark::DoNotOptimize(word_count(std::execution::seq, text));
  }
}
BENCHMARK(seq_word_count);

static void par_word_count(benchmark::State &state) {
  const std::string text = make_text(10000);
  for (auto _ : state) {
    benchmark::DoNotOptimize(word_count(std::execution::par, text));
  }
}
BENCHMARK(par_word_count);

static void par_unseq_word_count(benchmark::State &state) {
  const std::string text = make_text(10000);
  for (auto _ : state) {
    benchmark::DoNotOptimize(word_count(std::execution::par_unseq, text));
  }
}
BENCHMARK(par_unseq_word_count);

static void unseq_word_count(benchmark::State &state) {
  const std::string text = make_text(10000);
  for (auto _ : state) {
    benchmark::DoNotOptimize(word_count(std::execution::unseq, text));
  }
}
BENCHMARK(unseq_word_count);

static void naive_word_count(benchmark::State &state) {
  const std::string text = make_text(10000);
  for (auto _ : state) {
    benchmark::DoNotOptimize(naive_word_count(text));
  }
}
BENCHMARK(naive_word_count);

BENCHMARK_MAIN();

#endif
