#include <version>
#if !defined(__cpp_lib_atomic_shared_ptr)
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_lib_atomic_shared_ptr)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

#ifdef __cpp_lib_atomic_shared_ptr
#define CPP20_STYLE
#else
#define CPP11_STYLE
#endif

#include <atomic>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>

template <typename T> class ConcurrentStack {
private:
  struct Node {
    T value;
    std::shared_ptr<Node> next; // Or Node* to be able
  };                            // to use std::atomic<Node*> in C++11

#ifdef CPP20_STYLE
  std::atomic<std::shared_ptr<Node>> m_head;
#else
  // Before C++20, you have to protect 'm_head' yourself
  std::shared_ptr<Node> m_head;
  mutable std::mutex m_mutex;
#endif

public:
  class reference {
  private:                     // Or Node* to be able
    std::shared_ptr<Node> m_p; // to use std::atomic<Node*> in C++11

  public:
    reference(std::shared_ptr<Node> p) : m_p(p) {}
    T &operator*() { return m_p->value; }
    const T &operator*() const { return m_p->value; }
    T *operator->() { return &m_p->value; }
    const T *operator->() const { return &m_p->value; }
  };

public:
  ConcurrentStack() = default;
  ~ConcurrentStack() = default;
  ConcurrentStack(ConcurrentStack &) = delete;
  void operator=(const ConcurrentStack &) = delete;
  ConcurrentStack(ConcurrentStack &&) = delete;
  void operator=(ConcurrentStack &&) = delete;

public:
  reference find(T val) const {
#ifdef CPP20_STYLE
    auto p = m_head.load(); // Equivalent to C++11 std::atomic_load(&m_head);
#else
    std::unique_lock guard(m_mutex);
    auto p = m_head;
    guard.unlock();
#endif
    while (p && p->value != val)
      p = p->next;
    return reference(std::move(p));
  }

  reference front() const {
#ifdef CPP20_STYLE
    return reference(m_head); // Equivalent to C++11 std::atomic_load(&m_head);
#else
    std::unique_lock guard(m_mutex);
    auto p = m_head;
    guard.unlock();
    return reference(std::move(p));
#endif
  }

  void push(T val) {
    auto p = std::make_shared<Node>();
    p->value = val;
#ifdef CPP20_STYLE
    p->next = m_head;
    while (!m_head.compare_exchange_weak(p->next, p))
      ;
#else
    std::unique_lock guard(m_mutex);
    p->next = m_head;
    m_head = p;
    guard.unlock();
#endif
  }

  void pop() {
#ifdef CPP20_STYLE
    auto p = m_head.load(); // Equivalent to C++11 std::atomic_load(&m_head);
    while (p && !m_head.compare_exchange_weak(p->next, p))
      ;
#else
    std::unique_lock guard(m_mutex);
    if (m_head) {
      m_head->next = {};
      auto p = m_head;
      m_head = p->next;
    }
#endif
  }

  bool is_empty() const {
#ifdef CPP20_STYLE
    bool has_value = static_cast<bool>(m_head.load());
#else
    std::unique_lock guard(m_mutex);
    bool has_value = static_cast<bool>(m_head.get());
#endif
    return !has_value;
  }
};

int main() {
  ConcurrentStack<int> stack;

  auto fut1 = std::async([&stack] {
    for (int i = 1; i <= 100; ++i) {
      std::this_thread::sleep_for(std::chrono::nanoseconds(std::rand() % 1000));
      if (std::rand() % 2 == 0)
        stack.push(i);
      else
        stack.pop();
    }
  });
  auto fut2 = std::async([&stack] {
    for (int i = 1; i <= 100; ++i) {
      std::this_thread::sleep_for(std::chrono::nanoseconds(std::rand() % 1000));
      if (std::rand() % 2 == 0)
        stack.push(-i);
      else
        stack.pop();
    }
  });

  fut1.wait();
  fut2.wait();

  if (!stack.is_empty())
    std::cout << *stack.front() << "\n";
}

#endif