#ifndef CPP_ADVANCED_TRAINING_SRC_RANGES_CUSTOM_TAKE_VIEW_HPP
#define CPP_ADVANCED_TRAINING_SRC_RANGES_CUSTOM_TAKE_VIEW_HPP

#include <ranges>

namespace views = std::ranges::views;
namespace ranges = std::ranges;

template <ranges::input_range R>
requires ranges::view<R>
class custom_take_view : public ranges::view_interface<custom_take_view<R>> {
private:
  R m_base{};
  std::iter_difference_t<ranges::iterator_t<R>> m_count{};
  ranges::iterator_t<R> m_iter{std::begin(m_base)};

public:
  custom_take_view() = default;

  constexpr custom_take_view(R base,
                             std::iter_difference_t<ranges::iterator_t<R>> count)
      : m_base(base), m_count(count), m_iter(std::begin(m_base)) {}

  constexpr R base() const & { return m_base; }
  constexpr R base() && { return std::move(m_base); }

  constexpr auto begin() const { return m_iter; }
  constexpr auto end() const { return std::next(m_iter, m_count); }

  constexpr auto size() const requires ranges::sized_range<const R> {
    const auto s = ranges::size(m_base);
    const auto c = static_cast<decltype(s)>(m_count);
    return s < c ? 0 : s - c;
  }
};

// C++17 CTAD
template <class R>
custom_take_view(R &&base, std::iter_difference_t<ranges::iterator_t<R>>)
    -> custom_take_view<ranges::views::all_t<R>>;

namespace details {
struct custom_take_range_adaptor_closure {
  std::size_t m_count;
  constexpr custom_take_range_adaptor_closure(std::size_t count) : m_count(count) {}

  template <ranges::viewable_range R> //
  constexpr auto operator()(R &&r) const {
    return custom_take_view(std::forward<R>(r), m_count);
  }
};

struct custom_take_range_adaptor {
  template <ranges::viewable_range R>
  constexpr auto operator()(R &&r,
                            std::iter_difference_t<ranges::iterator_t<R>> count) {
    return custom_take_view(std::forward<R>(r), count);
  }

  constexpr auto operator()(std::size_t count) {
    return custom_take_range_adaptor_closure(count);
  }
};

template <ranges::viewable_range R>
constexpr auto operator|(R &&r, custom_take_range_adaptor_closure const &a) {
  return a(std::forward<R>(r));
}
} // namespace details

namespace custom_views {
details::custom_take_range_adaptor custom_take; // this is a factory object
}

#endif // CPP_ADVANCED_TRAINING_SRC_RANGES_CUSTOM_TAKE_VIEW_HPP
