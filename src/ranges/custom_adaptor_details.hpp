#ifndef CPP_ADVANCED_TRAINING_SRC_RANGES_CUSTOM_ADAPTOR_DETAILS_HPP
#define CPP_ADVANCED_TRAINING_SRC_RANGES_CUSTOM_ADAPTOR_DETAILS_HPP

#include <concepts>
#include <iterator>
#include <ranges>
#include <type_traits>

namespace adaptor {
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
} // namespace adaptor

#endif // CPP_ADVANCED_TRAINING_SRC_RANGES_CUSTOM_ADAPTOR_DETAILS_HPP
