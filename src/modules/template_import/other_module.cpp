export module constant_module; // no instruction before module declaration

import<iostream>; // no instruction before import

export template <typename T> constexpr T pi = T(3.1415926535897932384626L);

#ifndef __clang__
// Not yet supported by clang
export auto print(auto &&set) {
  std::cout << '(';
  for (bool is_first = true; const auto &e : set) {
    if (!is_first)
      std::cout << ", ";
    std::cout << e;
    is_first = false;
  }
  std::cout << ")\n";
};
#endif
