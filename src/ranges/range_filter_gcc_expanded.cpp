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

#include <concepts>
#include <iostream>
#include <iterator>
#include <numeric>
#include <optional>
#include <random>
#include <ranges>
#include <type_traits>
#include <functional>
#include <cassert>

namespace __detail {
template <typename _It>
concept __has_arrow = std::input_iterator<_It> &&
    (std::is_pointer_v<_It> || requires(_It __it) { __it.operator->(); });

template <typename _Tp>
concept __boxable = std::copy_constructible<_Tp> && std::is_object_v<_Tp>;

template <__boxable _Tp> struct __box : std::optional<_Tp> {
  using std::optional<_Tp>::optional;

  constexpr __box() noexcept(std::is_nothrow_default_constructible_v<_Tp>) requires
      std::default_initializable<_Tp> : std::optional<_Tp>{std::in_place} {}

  __box(const __box &) = default;
  __box(__box &&) = default;

  using std::optional<_Tp>::operator=;

  // _GLIBCXX_RESOLVE_LIB_DEFECTS
  // 3477. Simplify constraints for semiregular-box
  __box &operator=(const __box &__that) noexcept(
      std::is_nothrow_copy_constructible_v<_Tp>) requires(!std::copyable<_Tp>) {
    if ((bool)__that)
      this->emplace(*__that);
    else
      this->reset();
    return *this;
  }

  __box &operator=(__box &&__that) noexcept(
      std::is_nothrow_move_constructible_v<_Tp>) requires(!std::movable<_Tp>) {
    if ((bool)__that)
      this->emplace(std::move(*__that));
    else
      this->reset();
    return *this;
  }
};

template <typename _Iter, typename _Sent, typename _Pred>
constexpr _Iter find_if(_Iter __first, _Sent __last, _Pred __pred) {
  while (__first != __last && !(bool)std::invoke(__pred, *__first))
    ++__first;
  return __first;
}

template <std::ranges::range _Range> struct _CachedPosition {
  constexpr bool _M_has_value() const { return false; }

  constexpr std::ranges::iterator_t<_Range> _M_get(const _Range &) const {
    assert(false);
    return {};
  }

  constexpr void _M_set(const _Range &,
                        const std::ranges::iterator_t<_Range> &) const {}
};

} // namespace __detail

namespace __adaptor {
// True if the range adaptor _Adaptor can be applied with _Args.
template <typename _Adaptor, typename... _Args>
concept __adaptor_invocable = requires {
  std::declval<_Adaptor>()(std::declval<_Args>()...);
};

// True if the range adaptor non-closure _Adaptor can be partially applied
// with _Args.
template <typename _Adaptor, typename... _Args>
concept __adaptor_partial_app_viable =
    (_Adaptor::_S_arity > 1) && (sizeof...(_Args) == _Adaptor::_S_arity - 1) &&
    (std::constructible_from<std::decay_t<_Args>, _Args> && ...);

template <typename _Adaptor, typename... _Args> struct _Partial;

template <typename _Lhs, typename _Rhs> struct _Pipe;

// The base class of every range adaptor closure.
struct _RangeAdaptorClosure {
  // range | adaptor is equivalent to adaptor(range).
  template <typename _Self, typename _Range>
  requires std::derived_from<std::remove_cvref_t<_Self>, _RangeAdaptorClosure> &&
      __adaptor_invocable<_Self, _Range>
  friend constexpr auto operator|(_Range &&__r, _Self &&__self) {
    return std::forward<_Self>(__self)(std::forward<_Range>(__r));
  }

  // Compose the adaptors __lhs and __rhs into a pipeline, returning
  // another range adaptor closure object.
  template <typename _Lhs, typename _Rhs>
  requires std::derived_from<_Lhs, _RangeAdaptorClosure> &&
      std::derived_from<_Rhs, _RangeAdaptorClosure>
  friend constexpr auto operator|(_Lhs __lhs, _Rhs __rhs) {
    return _Pipe<_Lhs, _Rhs>{std::move(__lhs), std::move(__rhs)};
  }
};

// The base class of every range adaptor non-closure.
//
// The static data member _Derived::_S_arity must contain the total number of
// arguments that the adaptor takes, and the class _Derived must introduce
// _RangeAdaptor::operator() into the class scope via a using-declaration.
template <typename _Derived> struct _RangeAdaptor {
  // Partially apply the arguments __args to the range adaptor _Derived,
  // returning a range adaptor closure object.
  template <typename... _Args>
  requires __adaptor_partial_app_viable<_Derived, _Args...>
  constexpr auto operator()(_Args &&...__args) const {
    return _Partial<_Derived, std::decay_t<_Args>...>{
        std::forward<_Args>(__args)...};
  }
};

// A range adaptor closure that represents partial application of
// the range adaptor _Adaptor with arguments _Args.
template <typename _Adaptor, typename... _Args>
struct _Partial : _RangeAdaptorClosure {
  std::tuple<_Args...> _M_args;

  constexpr _Partial(_Args... __args) : _M_args(std::move(__args)...) {}

  // Invoke _Adaptor with arguments __r, _M_args... according to the
  // value category of the range adaptor closure object.
  template <typename _Range>
  requires __adaptor_invocable<_Adaptor, _Range, const _Args &...>
  constexpr auto operator()(_Range &&__r) const & {
    auto __forwarder = [&__r](const auto &...__args) {
      return _Adaptor{}(std::forward<_Range>(__r), __args...);
    };
    return std::apply(__forwarder, _M_args);
  }

  template <typename _Range>
  requires __adaptor_invocable<_Adaptor, _Range, _Args...>
  constexpr auto operator()(_Range &&__r) && {
    auto __forwarder = [&__r](auto &...__args) {
      return _Adaptor{}(std::forward<_Range>(__r), std::move(__args)...);
    };
    return std::apply(__forwarder, _M_args);
  }

  template <typename _Range>
  constexpr auto operator()(_Range &&__r) const && = delete;
};

// A lightweight specialization of the above primary template for
// the common case where _Adaptor accepts a single extra argument.
template <typename _Adaptor, typename _Arg>
struct _Partial<_Adaptor, _Arg> : _RangeAdaptorClosure {
  _Arg _M_arg;

  constexpr _Partial(_Arg __arg) : _M_arg(std::move(__arg)) {}

  template <typename _Range>
  requires __adaptor_invocable<_Adaptor, _Range, const _Arg &>
  constexpr auto operator()(_Range &&__r) const & {
    return _Adaptor{}(std::forward<_Range>(__r), _M_arg);
  }

  template <typename _Range>
  requires __adaptor_invocable<_Adaptor, _Range, _Arg>
  constexpr auto operator()(_Range &&__r) && {
    return _Adaptor{}(std::forward<_Range>(__r), std::move(_M_arg));
  }

  template <typename _Range>
  constexpr auto operator()(_Range &&__r) const && = delete;
};

template <typename _Lhs, typename _Rhs, typename _Range>
concept __pipe_invocable = requires {
  std::declval<_Rhs>()(std::declval<_Lhs>()(std::declval<_Range>()));
};

// A range adaptor closure that represents composition of the range
// adaptor closures _Lhs and _Rhs.
template <typename _Lhs, typename _Rhs> struct _Pipe : _RangeAdaptorClosure {
  [[no_unique_address]] _Lhs _M_lhs;
  [[no_unique_address]] _Rhs _M_rhs;

  constexpr _Pipe(_Lhs __lhs, _Rhs __rhs)
      : _M_lhs(std::move(__lhs)), _M_rhs(std::move(__rhs)) {}

  // Invoke _M_rhs(_M_lhs(__r)) according to the value category of this
  // range adaptor closure object.
  template <typename _Range>
  requires __pipe_invocable<const _Lhs &, const _Rhs &, _Range>
  constexpr auto operator()(_Range &&__r) const & {
    return _M_rhs(_M_lhs(std::forward<_Range>(__r)));
  }

  template <typename _Range>
  requires __pipe_invocable<_Lhs, _Rhs, _Range>
  constexpr auto operator()(_Range &&__r) && {
    return std::move(_M_rhs)(std::move(_M_lhs)(std::forward<_Range>(__r)));
  }

  template <typename _Range>
  constexpr auto operator()(_Range &&__r) const && = delete;
};
} // namespace __adaptor

template <std::ranges::input_range _Vp,
          std::indirect_unary_predicate<std::ranges::iterator_t<_Vp>> _Pred>
requires std::ranges::view<_Vp> && std::is_object_v<_Pred>
class custom_filter_view
    : public std::ranges::view_interface<custom_filter_view<_Vp, _Pred>> {
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

    friend custom_filter_view;

    using _Vp_iter = std::ranges::iterator_t<_Vp>;

    _Vp_iter _M_current = _Vp_iter();
    custom_filter_view *_M_parent = nullptr;

  public:
    using iterator_concept = decltype(_S_iter_concept());
    using iterator_category = decltype(_S_iter_cat());
    using value_type = std::ranges::range_value_t<_Vp>;
    using difference_type = std::ranges::range_difference_t<_Vp>;

    _Iterator() = default;

    constexpr _Iterator(custom_filter_view *__parent, _Vp_iter __current)
        : _M_current(std::move(__current)), _M_parent(__parent) {}

    constexpr _Vp_iter base() const &requires std::copyable<_Vp_iter> {
      return _M_current;
    }

    constexpr _Vp_iter base() && { return std::move(_M_current); }

    constexpr std::ranges::range_reference_t<_Vp> operator*() const {
      return *_M_current;
    }

    constexpr _Vp_iter operator->()
        const requires __detail::__has_arrow<_Vp_iter> && std::copyable<_Vp_iter> {
      return _M_current;
    }

    constexpr _Iterator &operator++() {
      _M_current = __detail::find_if(std::move(++_M_current),
                                     std::ranges::end(_M_parent->_M_base),
                                     std::ref(*_M_parent->_M_pred));
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
      do
        --_M_current;
      while (!std::invoke(*_M_parent->_M_pred, *_M_current));
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

    friend constexpr std::ranges::range_rvalue_reference_t<_Vp>
    iter_move(const _Iterator &__i) noexcept(
        noexcept(std::ranges::iter_move(__i._M_current))) {
      return std::ranges::iter_move(__i._M_current);
    }

    friend constexpr void
    iter_swap(const _Iterator &__x, const _Iterator &__y) noexcept(
        noexcept(std::ranges::iter_swap(__x._M_current, __y._M_current))) requires
        std::indirectly_swappable<_Vp_iter> {
      std::ranges::iter_swap(__x._M_current, __y._M_current);
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

    constexpr explicit _Sentinel(custom_filter_view *__parent)
        : _M_end(std::ranges::end(__parent->_M_base)) {}

    constexpr std::ranges::sentinel_t<_Vp> base() const { return _M_end; }

    friend constexpr bool operator==(const _Iterator &__x, const _Sentinel &__y) {
      return __y.__equal(__x);
    }
  };

  [[no_unique_address]] __detail::__box<_Pred> _M_pred;
  [[no_unique_address]] __detail::_CachedPosition<_Vp> _M_cached_begin;
  _Vp _M_base = _Vp();

public:
  custom_filter_view() = default;

  constexpr custom_filter_view(_Vp __base, _Pred __pred)
      : _M_pred(std::move(__pred)), _M_base(std::move(__base)) {}

  constexpr _Vp base() const &requires std::copy_constructible<_Vp> {
    return _M_base;
  }

  constexpr _Vp base() && { return std::move(_M_base); }

  constexpr const _Pred &pred() const { return *_M_pred; }

  constexpr _Iterator begin() {
    if (_M_cached_begin._M_has_value())
      return {this, _M_cached_begin._M_get(_M_base)};

    assert(_M_pred.has_value());
    auto __it = __detail::find_if(std::ranges::begin(_M_base),
                                  std::ranges::end(_M_base), std::ref(*_M_pred));
    _M_cached_begin._M_set(_M_base, __it);
    return {this, std::move(__it)};
  }

  constexpr auto end() {
    if constexpr (std::ranges::common_range<_Vp>)
      return _Iterator{this, std::ranges::end(_M_base)};
    else
      return _Sentinel{this};
  }
};

template <typename _Range, typename _Pred>
custom_filter_view(_Range &&, _Pred)
    -> custom_filter_view<std::ranges::views::all_t<_Range>, _Pred>;

namespace custom_views {
namespace __detail {
template <typename _Range, typename _Pred>
concept __can_filter_view = requires {
  custom_filter_view{std::declval<_Range>(), std::declval<_Pred>()};
};
} // namespace __detail

struct _Filter : __adaptor::_RangeAdaptor<_Filter> {
  template <std::ranges::viewable_range _Range, typename _Pred>
  requires __detail::__can_filter_view<_Range, _Pred>
  constexpr auto operator()(_Range &&__r, _Pred &&__p) const {
    return custom_filter_view{std::forward<_Range>(__r), std::forward<_Pred>(__p)};
  }

  using _RangeAdaptor<_Filter>::operator();
  static constexpr int _S_arity = 2;
};

inline constexpr _Filter custom_filter;
} // namespace custom_views

int main() {
  namespace views = std::ranges::views;
  namespace ranges = std::ranges;

  auto v1 = views::iota(1)                                              //
            | custom_views::custom_filter([](auto x) { return x % 2; }) //
            | views::take(10)                                           //
            | views::reverse;
  for (auto &&x : v1 | views::take(4)) {
    std::cout << x << "\n";
  }

  auto v2 = views::iota(1)                                              //
            | views::transform([](auto x) -> int { return 3 * x + 1; }) //
            | custom_views::custom_filter([](auto x) { return x % 2; }) //
            | views::take(10)                                           //
      ;

  for (auto &&x : v2 | views::take(4)) {
    std::cout << x << "\n";
  }
}

#endif