#ifndef CPP_ADVANCED_TRAINING_INCLUDE_COLORIZE_HPP
#define CPP_ADVANCED_TRAINING_INCLUDE_COLORIZE_HPP

#if defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
#define USE_COLOR
#endif

#include <iostream>
#include <map>
#include <memory>
#include <tuple>

template <typename T> struct Colorize {
  T p;

  static auto to_color(const T &p) {
    if (!_to_color) {
      _last_ptr_color = static_cast<uint32_t>(std::hash<T>{}(p)); // seed
      _to_color = std::make_unique<typename decltype(_to_color)::element_type>();
    }
    const auto color = Color{(_last_ptr_color >> 16u) & 0xFF,
                             (_last_ptr_color >> 8u) & 0xFF, _last_ptr_color & 0xFF};
    auto [iter, inserted] = _to_color->insert({p, color});
    if (inserted) {
      do {
        _last_ptr_color = 1664525 * _last_ptr_color + 1013904223; // Knuth & Lewis
      } while (((_last_ptr_color >> 16u) & 0xFF) + ((_last_ptr_color >> 8u) & 0xFF) +
                   (_last_ptr_color & 0xFF) <
               400);
    }
    return iter->second;
  }

  static auto count() { return (_to_color) ? _to_color->size() : 0; }

private:
  using Color = std::tuple<uint8_t, uint8_t, uint8_t>;
  static inline std::unique_ptr<std::map<T, Color>> _to_color;
  static inline uint32_t _last_ptr_color = 0x05caf0;
};

// Custom CTAD (required by CLANG but not by GCC)
template <typename T> Colorize(T &&t) -> Colorize<T>;

template <typename T>
inline std::ostream &operator<<(std::ostream &o, const Colorize<T> &c) {
#ifdef USE_COLOR
  auto color = Colorize<T>::to_color(c.p);
  char command[20] = {0};
  std::snprintf(command, sizeof(command), "\033[38;2;%d;%d;%dm", std::get<0>(color),
                std::get<1>(color), std::get<2>(color));
  return o << command << c.p << "\033[00m";
#else
  return o << c.p;
#endif
}

#endif // CPP_ADVANCED_TRAINING_INCLUDE_COLORIZE_HPP
