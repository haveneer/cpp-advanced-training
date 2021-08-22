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
#include <gtest/gtest.h>

TEST(word_beginning, space_to_non_space_should_be_a_word_beginning) {
  EXPECT_TRUE(is_word_beginning(' ', 'a'));
  EXPECT_TRUE(is_word_beginning('\t', '1'));
  EXPECT_TRUE(is_word_beginning('\n', '.'));
}

TEST(word_beginning, non_space_to_space_should_not_be_a_word_beginning) {
  EXPECT_FALSE(is_word_beginning('a', ' '));
  EXPECT_FALSE(is_word_beginning('1', '\t'));
  EXPECT_FALSE(is_word_beginning('.', '\n'));
}

TEST(word_beginning, space_to_space_should_not_be_a_word_beginning) {
  EXPECT_FALSE(is_word_beginning(' ', ' '));
  EXPECT_FALSE(is_word_beginning('\n', '\t'));
  EXPECT_FALSE(is_word_beginning('\n', ' '));
}

TEST(word_beginning, non_space_to_non_space_should_not_be_a_word_beginning) {
  EXPECT_FALSE(is_word_beginning('a', 'b'));
  EXPECT_FALSE(is_word_beginning('a', '1'));
  EXPECT_FALSE(is_word_beginning('\'', 's'));
  EXPECT_FALSE(is_word_beginning('t', ','));
  EXPECT_FALSE(is_word_beginning('.', 'A'));
}

TEST(word_count, empty_string_should_have_no_word) {
  EXPECT_EQ(word_count(std::execution::seq, ""), 0);
  EXPECT_EQ(naive_word_count(""), 0);
}

TEST(word_count, space_string_should_have_no_word) {
  EXPECT_EQ(word_count(std::execution::seq, " \n\t"), 0);
  EXPECT_EQ(naive_word_count(" \n\t"), 0);
}

TEST(word_count, one_letter_string_should_have_one_word) {
  EXPECT_EQ(word_count(std::execution::seq, "Z"), 1);
  EXPECT_EQ(naive_word_count("Z"), 1);
}

TEST(word_count, one_word_string_starting_with_spaces_should_have_one_word) {
  EXPECT_EQ(word_count(std::execution::seq, "  Z"), 1);
  EXPECT_EQ(naive_word_count("  Z"), 1);
}

TEST(word_count, one_word_string_finishing_with_spaces_should_have_one_word) {
  EXPECT_EQ(word_count(std::execution::seq, "1   "), 1);
  EXPECT_EQ(naive_word_count("1   "), 1);
}

TEST(word_count, these_static_texts_should_have_five_words) {
  EXPECT_EQ(word_count(std::execution::seq, "1 2 3 4 5"), 5);
  EXPECT_EQ(word_count(std::execution::seq, "C++ is very powerful !"), 5);
  EXPECT_EQ(word_count(std::execution::seq, " Spaces  do  \n not matter, but\n"), 5);
  EXPECT_EQ(word_count(std::execution::seq, "ponctuations are words ! !"), 5);
  EXPECT_EQ(naive_word_count("ponctuations are words ! !"), 5);
}

TEST(word_count, these_generated_texts_should_have_tousand_words) {
  constexpr std::size_t nword = 1000;
  std::string text = make_text(nword, false);
  EXPECT_EQ(word_count(std::execution::seq, text), nword);
  EXPECT_EQ(word_count(std::execution::par, text), nword);
  EXPECT_EQ(word_count(std::execution::par_unseq, text), nword);
  EXPECT_EQ(word_count(std::execution::unseq, text), nword);
  EXPECT_EQ(naive_word_count(text), nword);
}

#endif