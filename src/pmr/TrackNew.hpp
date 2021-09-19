#ifndef TRACKNEW_HPP
#define TRACKNEW_HPP
// Inspired by Nicolai M. Josuttis

#include <cstdio>  // for printf() : do not use cout (which can do allocations)
#include <cstdlib> // for C++17 std::malloc(), std::aligned_alloc(), std::free
#include <iostream>
#include <new> // for std::align_val_t

class TrackNew {
public:
  struct Status {
    int numAlloc = 0;   // num malloc calls
    size_t sumSize = 0; // bytes allocated so far

    friend std::ostream &operator<<(std::ostream &o, const Status &status) {
      return o << status.numAlloc << " allocations for " << status.sumSize << " byte"
               << ((status.sumSize > 1) ? "s" : "");
    }
  };

private:
  static inline bool m_do_trace = false; // tracing enabled
  static inline bool inNew = false;      // don't track output inside new overloads
  static inline Status m_status{0, 0};

public:
  static void reset() { // reset new/memory counters
    m_status.numAlloc = 0;
    m_status.sumSize = 0;
  }

  static void trace(bool b) { // enable/disable tracing
    m_do_trace = b;
  }

  // implementation of tracked allocation:
  static void *allocate(std::size_t size, std::size_t align, const char *call) {
    // track and trace the allocation:
    ++m_status.numAlloc;
    m_status.sumSize += size;

    void *p = nullptr;
    if (align == 0) {
      p = std::malloc(size);
    } else {
#ifdef _MSC_VER
      p = _aligned_malloc(size, align);
#else
      p = std::aligned_alloc(align, size); // Not available in MSVC
#endif
    }

    if (m_do_trace) {
      // DON'T use std::cout here because it might allocate memory
      // while we are allocating memory (core dump at best)
      printf("#%d %s ", m_status.numAlloc, call);
      printf("(%zu bytes, ", size);
      if (align > 0) {
        printf("%zu-byte aligned) ", align);
      } else {
        printf("def-aligned) ");
      }
      printf("=> %p (total: %zu bytes)\n", (void *)p, m_status.sumSize);
    }
    return p;
  }

  static Status status() { return m_status; }
};

[[nodiscard]] void *operator new(std::size_t size) {
  return TrackNew::allocate(size, 0, "::new");
}

[[nodiscard]] void *operator new(std::size_t size, std::align_val_t align) {
  return TrackNew::allocate(size, static_cast<size_t>(align), "::new aligned");
}

[[nodiscard]] void *operator new[](std::size_t size) {
  return TrackNew::allocate(size, 0, "::new[]");
}

[[nodiscard]] void *operator new[](std::size_t size, std::align_val_t align) {
  return TrackNew::allocate(size, static_cast<size_t>(align), "::new[] aligned");
}

void operator delete(void *p) noexcept { std::free(p); }
void operator delete(void *p, std::size_t) noexcept { ::operator delete(p); }
void operator delete(void *p, std::align_val_t) noexcept {
#ifdef _MSC_VER
  _aligned_free(p); // Windows API
#else
  std::free(p);                            // C++17 API
#endif
}
void operator delete(void *p, std::size_t, std::align_val_t align) noexcept {
  ::operator delete(p, align);
}

#endif // TRACKNEW_HPP
