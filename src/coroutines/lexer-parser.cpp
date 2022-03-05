#include <version>
#if !defined(__cpp_lib_coroutine) && __has_include(<experimental/coroutine>)
#define __cpp_lib_coroutine
#endif
#ifndef __cpp_lib_coroutine
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_lib_coroutine)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

//#region [Collapse all]
#if __has_include(<coroutine>)
#include <coroutine>
#elif __has_include(<experimental/coroutine>)
#include <experimental/coroutine>
namespace std {
using namespace std::experimental;
}
#endif
#include <cassert>
#include <cctype>
#include <exception>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <stack>
#include <string>
#include <string_view>
#include <variant>
#include <vector>
using namespace std::string_literals;
//#endregion

template <typename T> struct Generator {
  struct promise_type {
    T m_val;
    std::exception_ptr m_exception;

    Generator get_return_object() { return this; }
    std::suspend_never initial_suspend() { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }
    std::suspend_always yield_value(T &&val) {
      m_val = std::move(val);
      return {};
    }
    std::suspend_always yield_value(const T &val) {
      m_val = val;
      return {};
    }
    void return_void() {}

    // Disallow co_await in generator coroutines.
    void await_transform() = delete;
    static void unhandled_exception() {
      try {
        std::rethrow_exception(std::current_exception());
      } catch (const std::exception &e) {
        std::cout << "Caught exception \"" << e.what() << "\"\n";
      }
    }
  };
  /* -------- Iterator Implementation ---------- */
  struct iterator {
    bool operator!=(const iterator &rhs) { return !m_h_ptr->done(); }
    iterator &operator++() {
      assert(not m_h_ptr->done());
      m_h_ptr->resume();
      return *this;
    }
    const T &operator*() const {
      if (auto e = m_h_ptr->promise().m_exception; e)
        std::rethrow_exception(e);
      return m_h_ptr->promise().m_val;
    }
    std::coroutine_handle<promise_type> *m_h_ptr;
  };
  iterator begin() { return iterator{&m_co_handle}; }
  iterator end() { return iterator{nullptr}; } // to only satisfy for-range API
  /* -------------------------------------------- */
  Generator(promise_type *p)
      : m_co_handle(std::coroutine_handle<promise_type>::from_promise(*p)) {}
  ~Generator() {
    if (m_co_handle)
      m_co_handle.destroy();
  }

  Generator(const Generator &) = delete;
  Generator &operator=(const Generator &) = delete;
  Generator(Generator &&that) noexcept : m_co_handle(that.m_co_handle) {
    that.m_co_handle = nullptr;
  }
  Generator &operator=(Generator &&that) noexcept {
    if (m_co_handle)
      m_co_handle.destroy();
    m_co_handle = that.m_co_handle;
    that.m_co_handle = nullptr;
    return *this;
  }

  std::coroutine_handle<promise_type> m_co_handle;
};

//#region [Operators]
struct Operator {
  virtual int apply(int a, int b) const = 0;
  virtual std::string_view symbol() const = 0;
  virtual std::unique_ptr<Operator> clone() const = 0;
};

struct AddOperator : Operator {
  int apply(int a, int b) const override { return a + b; }
  std::string_view symbol() const override { return "+"; };
  std::unique_ptr<Operator> clone() const override {
    return std::make_unique<AddOperator>();
  }
};

struct SubOperator : Operator {
  int apply(int a, int b) const override { return a - b; }
  std::string_view symbol() const override { return "-"; };
  std::unique_ptr<Operator> clone() const override {
    return std::make_unique<SubOperator>();
  }
};

struct MultOperator : Operator {
  int apply(int a, int b) const override { return a * b; }
  std::string_view symbol() const override { return "*"; };
  std::unique_ptr<Operator> clone() const override {
    return std::make_unique<MultOperator>();
  }
};

struct DivOperator : Operator {
  int apply(int a, int b) const override { return a / b; }
  std::string_view symbol() const override { return "/"; };
  std::unique_ptr<Operator> clone() const override {
    return std::make_unique<DivOperator>();
  }
};

std::ostream &operator<<(std::ostream &o, const std::unique_ptr<Operator> &op) {
  return o << op->symbol();
}

struct Print {};
//#endregion

using Token = std::variant<std::monostate, int, Print, std::unique_ptr<Operator>,
                           std::exception_ptr>;

// Lambda overloader C++17 version
template <typename... Ts> struct overload : Ts... {
  // overload(Ts... ts) : Ts(ts)... {} // can be replaced by CTAD
  using Ts::operator()...;
};
// Custom Template Argument Deduction Rules
template <typename... Ts> overload(Ts...) -> overload<Ts...>;

std::ostream &operator<<(std::ostream &o, const Token &token) {
  std::visit(
      overload{
          [&o](const int &i) { o << "value: " << i; },
          [&o](const Print &i) { o << "print"; },
          [&o](const std::unique_ptr<Operator> &op) { o << "operator: " << op; },
          [&o](const std::exception &e) { o << "Exception: " << e.what(); },
          [](auto) { throw std::runtime_error("Undefined token"); }},
      token);
  return o;
}

Generator<Token> read(const std::string str) {
  std::optional<int> value;
  for (const char &c : str) {
    if (value) {
      if (std::isdigit(c)) {
        value = *value * 10 + (c - '0');
        continue;
      } else {
        co_yield *value;
        value = std::nullopt;
      }
    }

    if (std::isdigit(c) && 'c' != '0') {
      value = (c - '0');
    } else if (std::isspace(c)) {
      if (value) {
        co_yield *value;
        value = std::nullopt;
      }
    } else if (c == ';') {
      co_yield Print{};
    } else {
      switch (c) {
      case '+':
        co_yield std::make_unique<AddOperator>();
        break;
      case '-':
        co_yield std::make_unique<SubOperator>();
        break;
      case '*':
        co_yield std::make_unique<MultOperator>();
        break;
      case '/':
        co_yield std::make_unique<DivOperator>();
        break;
      default:
        co_yield std::make_exception_ptr(
            std::runtime_error("Unexpected char '"s + c + "'"));
        break;
      }
    }
  }
  if (value) {
    co_yield *value;
  }
  co_yield Print{};
}

struct Result {
  std::string expression;
  int value;
};

Generator<Result> parse(Generator<Token> token_stream) {
  std::string expression{};
  std::stack<Result> stack;

  for (auto &&token : token_stream) {
    bool do_print = false;
    std::visit(
        overload{
            [&stack](int v) {
              std::cout << "Push value " << v << '\n';
              stack.push(Result{std::to_string(v), v});
            },
            [&do_print](const Print & /* print */) { do_print = true; },
            [&stack](const std::unique_ptr<Operator> &op) {
              std::cout << "Apply operator " << op->symbol() << '\n';
              if (stack.size() < 2) {
                throw std::runtime_error("Too few arguments to apply operator " +
                                         std::string(op->symbol()));
              } else {
                const Result b = std::move(stack.top());
                stack.pop();
                const Result a = std::move(stack.top());
                stack.pop();
                stack.push(Result{"(" + a.expression + std::string(op->symbol()) +
                                      b.expression + ")",
                                  op->apply(a.value, b.value)});
              }
            },
            [](const std::exception_ptr &e) { std::rethrow_exception(e); },
            [](const std::monostate &) {}},
        token);

    if (do_print) {
      while (!stack.empty()) {
        co_yield stack.top(); // this co_yield cannot be located in an inner function
        stack.pop();
      }
    }
  }
}

int main() {
  std::string data_stream = "13 14 15 + + ; 2 3.";
  auto token_stream = read(data_stream);             // read is a coroutine
  auto expressions = parse(std::move(token_stream)); // parse is a coroutine
  for (const auto &[expression, result] : expressions) {
    std::cout << expression << " = " << result << '\n';
  }
}

#endif