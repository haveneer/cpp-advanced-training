#ifndef CPP_ADVANCED_TRAINING_SRC_RANGES_CUSTOM_WINDOW_FOLD_VIEW_HPP
#define CPP_ADVANCED_TRAINING_SRC_RANGES_CUSTOM_WINDOW_FOLD_VIEW_HPP

#include "custom_adaptor_details.hpp"
#include <concepts>
#include <iterator>
#include <ranges>
#include <type_traits>

template <std::ranges::input_range _Vp, typename _Acc>
requires std::ranges::view<_Vp>
class window_fold_view
    : public std::ranges::view_interface<window_fold_view<_Vp, _Acc>> {
private:
  struct _Sentinel;

  struct _Iterator {
  private:
    static constexpr auto _S_iter_concept() {
      if constexpr (std::ranges::bidirectional_range<_Vp>)
        return std::bidirectional_iterator_tag{};
      else if constexpr (std::ranges::forward_range<_Vp>)
        return std::forward_iterator_tag{};
      else
        return std::input_iterator_tag{};
    }

    static constexpr auto _S_iter_cat() {
      using _Cat = typename std::iterator_traits<_Vp_iter>::iterator_category;
      if constexpr (std::derived_from<_Cat, std::bidirectional_iterator_tag>)
        return std::bidirectional_iterator_tag{};
      else if constexpr (std::derived_from<_Cat, std::forward_iterator_tag>)
        return std::forward_iterator_tag{};
      else
        return _Cat{};
    }

    friend window_fold_view;

    using _Vp_iter = std::ranges::iterator_t<_Vp>;

    _Vp_iter _M_current = _Vp_iter();
    _Vp_iter _M_next = _Vp_iter();
    window_fold_view *_M_parent = nullptr;
    _Acc _M_acc;

  public:
    using iterator_concept = decltype(_S_iter_concept());
    using iterator_category = decltype(_S_iter_cat());
    using value_type = std::string;
    using difference_type = std::ranges::range_difference_t<_Vp>;

    _Iterator() = default;

    constexpr _Iterator(window_fold_view *__parent, _Vp_iter __current)
        : _M_current(std::move(__current)), _M_parent(__parent) {
      update();
    }

    constexpr std::string operator*() const { return _M_acc; }

    constexpr _Vp_iter operator->()
        const requires details::__has_arrow<_Vp_iter> && std::copyable<_Vp_iter> {
      return _M_acc;
    }

    constexpr _Iterator &operator++() {
      _M_current = _M_next;
      update();
      return *this;
    }

    constexpr void operator++(int) { ++*this; }

    constexpr _Iterator operator++(int) requires std::ranges::forward_range<_Vp> {
      auto __tmp = *this;
      ++*this;
      return __tmp;
    }

    constexpr _Iterator &
    operator--() requires std::ranges::bidirectional_range<_Vp> {
      _M_acc = {};
      _M_next = _M_current;
      int n = _M_parent->_n;
      for (int n = _M_parent->_n;
           n > 0 && _M_current != std::ranges::begin(_M_parent->_M_base); --n) {
        _M_acc = *--_M_current + std::move(_M_acc);
      }
      return *this;
    }

    constexpr _Iterator
    operator--(int) requires std::ranges::bidirectional_range<_Vp> {
      auto __tmp = *this;
      --*this;
      return __tmp;
    }

    friend constexpr bool
    operator==(const _Iterator &__x,
               const _Iterator &__y) requires std::equality_comparable<_Vp_iter> {
      return __x._M_current == __y._M_current;
    }

  private :

      void
      update() {
      _M_acc = {};
      _M_next = _M_current;
      int n = _M_parent->_n;
      for (int n = _M_parent->_n;
           n > 0 && _M_next != std::ranges::end(_M_parent->_M_base); --n) {
        _M_acc += *_M_next++;
      }
    }
  };

  struct _Sentinel {
  private:
    std::ranges::sentinel_t<_Vp> _M_end = std::ranges::sentinel_t<_Vp>();

    constexpr bool __equal(const _Iterator &__i) const {
      return __i._M_current == _M_end;
    }

  public:
    _Sentinel() = default;

    constexpr explicit _Sentinel(window_fold_view *__parent)
        : _M_end(std::ranges::end(__parent->_M_base)) {}

    constexpr std::ranges::sentinel_t<_Vp> base() const { return _M_end; }

    friend constexpr bool operator==(const _Iterator &__x, const _Sentinel &__y) {
      return __y.__equal(__x);
    }
  };

  int _n = {};
  _Vp _M_base = _Vp();

public:
  window_fold_view() = default;

  constexpr window_fold_view(_Vp __base, int n)
      : _n(n), _M_base(std::move(__base)) {}

  constexpr _Iterator begin() {
    return {this, std::move(std::ranges::begin(_M_base))};
  }

  constexpr auto end() {
    if constexpr (std::ranges::common_range<_Vp>)
      return _Iterator{this, std::ranges::end(_M_base)};
    else
      return _Sentinel{this};
  }
};

template <typename _Range, typename _Acc>
window_fold_view(_Range &&, int)
    -> window_fold_view<std::ranges::views::all_t<_Range>, _Acc>;

namespace custom_views {

template <typename _Acc>
struct WindowFold : adaptor::_RangeAdaptor<WindowFold<_Acc>> {
  template <std::ranges::viewable_range _Range>
  constexpr auto operator()(_Range &&__r, int n) const {
    return window_fold_view<_Range, _Acc>{std::forward<_Range>(__r), n};
  }

  using adaptor::_RangeAdaptor<WindowFold<_Acc>>::operator();
  static constexpr int _S_arity = 2;
};

template <typename _Acc> inline constexpr WindowFold<_Acc> window_fold;
} // namespace custom_views

#endif // CPP_ADVANCED_TRAINING_SRC_RANGES_CUSTOM_WINDOW_FOLD_VIEW_HPP
