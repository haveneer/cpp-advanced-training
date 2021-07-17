#pragma once

//#region [Colorization]
#include <iomanip>
#include <iostream>
#include <sstream>
#if __has_include("colorize.hpp")
#include "colorize.hpp"
#else
template <typename T> struct Colorize { T x; };
template <typename T>
std::ostream &operator<<(std::ostream &o, const Colorize<T> &t) {
  return o << t.x;
}
template <typename T> Colorize(T &&t) -> Colorize<T>;
#endif
//#endregion

struct SyncCout {
  SyncCout() : oss{} {
    std::ostream o(oss.rdbuf()); // safe RAII flags restore
    o << std::hex << "[" << Colorize{std::this_thread::get_id()} << "] ";
  }
  ~SyncCout() {
    oss << '\n';
    std::scoped_lock lock(m_mutex);
    std::cout << oss.str();
  }
  template <typename T> auto &operator<<(T &&val) { return oss << val; }
  std::ostringstream oss;
  static inline std::mutex m_mutex;
};
