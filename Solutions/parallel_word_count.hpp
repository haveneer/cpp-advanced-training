#pragma once

/* Subject: count the number of words of a text.
 * Hint: count the number of beginnings
 * Hint: a beginning is a transition between a non-word char and a word char
 */

#include <algorithm>
#include <cctype>
#include <execution>
#include <functional>
#include <string_view>

inline bool is_word_beginning(char left, char right) {
  // we consider all non-spaces as word chars
  return std::isspace(left) && !std::isspace(right);
}

std::size_t naive_word_count(std::string_view s) {
  if (s.empty())
    return 0;
  std::size_t wc = (!std::isspace(s.front()) ? 1 : 0);
  for (std::size_t i = 1; i < s.size(); ++i) {
    if (is_word_beginning(s[i - 1], s[i]))
      ++wc;
  }
  return wc;
}

template <typename Policy>
concept CompatiblePolicy = std::is_execution_policy_v<Policy>;

std::size_t word_count(CompatiblePolicy auto execution_policy, std::string_view s) {
  if (s.empty())
    return 0;

  // If the first character is not a space, then it's the beginning of a word.
  std::size_t wc = (!std::isspace(s.front()) ? 1 : 0);

  wc += std::transform_reduce(execution_policy,
                              // "Left" input:  s[0], s[1], ..., s[s.size() - 2]
                              s.begin(), s.end() - 1,
                              // "Right" input: s[1], s[2], ..., s[s.size() - 1],
                              s.begin() + 1,
                              std::size_t(0), // Initial value for reduction.

                              std::plus<std::size_t>(), // Binary reduction op.
                              is_word_beginning // Binary transform op: Return
                                                // 1 when we hit a new word.
  );

  return wc;
}

#include <random>

inline std::string make_text(std::size_t n, bool reproductible = true) {
  unsigned int seed = std::mt19937::default_seed;
  if (!reproductible) {
    seed = std::random_device{}();
  }
  std::mt19937 engine{seed}; // non thread-safe random generator

  std::vector<std::size_t> lengths(n);
  std::generate(std::begin(lengths), std::end(lengths),
                [&engine, dist = std::uniform_int_distribution<>(1, 10)]() mutable {
                  return dist(engine);
                });

  auto get_non_space =
      [&engine, dist = std::uniform_int_distribution<>(33, 126)]() mutable -> char {
    return dist(engine);
  };

  auto get_spaces = [&engine,
                     dist = std::uniform_int_distribution<>(1, 5)]() mutable {
    constexpr char spaces[] = {' ', '\t', '\n'};
    std::string space_string;
    for (int c = 1; c <= dist(engine); ++c)
      space_string += spaces[dist(engine) % 3];
    return space_string;
  };

  return // all-in-one !
      std::transform_reduce(
          std::begin(lengths), std::end(lengths),
          std::string{},                                    // init for reduction
          [&](const std::string &a, const std::string &b) { // reduction
            return a + get_spaces() + b;
          },
          [&](std::size_t s) -> std::string { // transformation
            std::string text(s, ' ');
            std::generate(text.begin(), text.end(), get_non_space);
            return text;
          });
}
