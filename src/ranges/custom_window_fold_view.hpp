#ifndef CPP_ADVANCED_TRAINING_SRC_RANGES_CUSTOM_WINDOW_FOLD_VIEW_HPP
#define CPP_ADVANCED_TRAINING_SRC_RANGES_CUSTOM_WINDOW_FOLD_VIEW_HPP

#include "custom_adaptor_details.hpp"
#include <concepts>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <string>

template <std::ranges::input_range Vp, typename Acc>
requires std::ranges::view<Vp>
class window_fold_view
    : public std::ranges::view_interface<window_fold_view<Vp, Acc>> {
private:
  struct Sentinel;

  struct Iterator {
  private:
    static constexpr auto iter_concept_static_helper() {
      if constexpr (std::ranges::bidirectional_range<Vp>)
        return std::bidirectional_iterator_tag{};
      else if constexpr (std::ranges::forward_range<Vp>)
        return std::forward_iterator_tag{};
      else
        return std::input_iterator_tag{};
    }

    static constexpr auto iter_cat_static_helper() {
      using Cat = typename std::iterator_traits<Vp_iter>::iterator_category;
      if constexpr (std::derived_from<Cat, std::bidirectional_iterator_tag>)
        return std::bidirectional_iterator_tag{};
      else if constexpr (std::derived_from<Cat, std::forward_iterator_tag>)
        return std::forward_iterator_tag{};
      else
        return Cat{};
    }

    friend window_fold_view;

    using Vp_iter = std::ranges::iterator_t<Vp>;

    Vp_iter m_current = Vp_iter();
    Vp_iter m_next = Vp_iter();
    window_fold_view *m_parent = nullptr;
    Acc m_acc;

  public:
    using iterator_concept = decltype(iter_concept_static_helper());
    using iterator_category = decltype(iter_cat_static_helper());
    using value_type = std::string;
    using difference_type = std::ranges::range_difference_t<Vp>;

    Iterator() = default;

    constexpr Iterator(window_fold_view *parent, Vp_iter current)
        : m_current(std::move(current)), m_parent(parent) {
      lookup();
    }

    constexpr std::string operator*() const { return m_acc; }

    constexpr Iterator &operator++() {
      m_current = m_next;
      lookup();
      return *this;
    }

    constexpr void operator++(int) { ++*this; }

    constexpr Iterator operator++(int) requires std::ranges::forward_range<Vp> {
      auto tmp = *this;
      ++*this;
      return tmp;
    }

    constexpr Iterator &operator--() requires std::ranges::bidirectional_range<Vp> {
      // backward lookup
      m_acc = {};
      m_next = m_current;
      int n = m_parent->m_n;
      for (int n = m_parent->m_n;
           n > 0 && m_current != std::ranges::begin(m_parent->m_base); --n) {
        m_acc = *--m_current + std::move(m_acc);
      }
      return *this;
    }

    constexpr Iterator
    operator--(int) requires std::ranges::bidirectional_range<Vp> {
      auto tmp = *this;
      --*this;
      return tmp;
    }

    friend constexpr bool
    operator==(const Iterator &x,
               const Iterator &y) requires std::equality_comparable<Vp_iter> {
      return x.m_current == y.m_current;
    }

  private :

      void
      lookup() {
      m_acc = {};
      m_next = m_current;
      int n = m_parent->m_n;
      for (int n = m_parent->m_n;
           n > 0 && m_next != std::ranges::end(m_parent->m_base); --n) {
        m_acc += *m_next++;
      }
    }
  };

  struct Sentinel {
  private:
    std::ranges::sentinel_t<Vp> m_end = std::ranges::sentinel_t<Vp>();

  public:
    Sentinel() = default;

    constexpr explicit Sentinel(window_fold_view *parent)
        : m_end(std::ranges::end(parent->m_base)) {}

    constexpr std::ranges::sentinel_t<Vp> base() const { return m_end; }

    constexpr bool operator==(const Iterator &i) const {
      return i.m_current == m_end;
    }
  };

  int m_n = {};
  Vp m_base = Vp();

public:
  window_fold_view() = default;

  constexpr window_fold_view(Vp base, int n) : m_n(n), m_base(std::move(base)) {}

  constexpr Iterator begin() {
    return {this, std::move(std::ranges::begin(m_base))};
  }

  constexpr auto end() {
    if constexpr (std::ranges::common_range<Vp>)
      return Iterator{this, std::ranges::end(m_base)};
    else
      return Sentinel{this};
  }
};

template <typename Range, typename Acc>
window_fold_view(Range &&, int)
    -> window_fold_view<std::ranges::views::all_t<Range>, Acc>;

namespace custom_views {

template <typename Acc> struct WindowFold : adaptor::_RangeAdaptor<WindowFold<Acc>> {
  template <std::ranges::viewable_range Range>
  constexpr auto operator()(Range &&r, int n) const {
    return window_fold_view<Range, Acc>{std::forward<Range>(r), n};
  }

  // Everything about invocation and pipe plumbing is hidden here
  using adaptor::_RangeAdaptor<WindowFold<Acc>>::operator();
  static constexpr int _S_arity = 2; // plugged with adaptor details
};

template <typename Acc> inline constexpr WindowFold<Acc> window_fold;
} // namespace custom_views

#endif // CPP_ADVANCED_TRAINING_SRC_RANGES_CUSTOM_WINDOW_FOLD_VIEW_HPP
