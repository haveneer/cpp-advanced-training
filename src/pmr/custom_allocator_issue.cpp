//#region [my_allocator]
// Using skeleton from https://howardhinnant.github.io/allocator_boilerplate.html#new
//     + a few "optimizations"
// boilerplate will be filled by the compiler using
//     std::allocator_traits<my_allocator<T>>::construct(...)

#include <cstddef>

template <typename T> class my_allocator {
public:
  using value_type = T;

  my_allocator() noexcept {}
  template <class U> my_allocator(const my_allocator<U> &) noexcept {}

  // Use pointer if pointer is not a value_type*
  [[nodiscard]] value_type *allocate(std::size_t n) {
    return static_cast<value_type *>(::operator new(n * sizeof(value_type)));
  }

  // Use pointer if pointer is not a value_type*
  void deallocate(value_type *p, std::size_t) noexcept { ::operator delete(p); }
};

template <class T, class U>
bool operator==(const my_allocator<T> &, const my_allocator<U> &) noexcept {
  return true;
}

template <class T, class U>
bool operator!=(const my_allocator<T> &x, const my_allocator<U> &y) noexcept {
  return !(x == y);
}
//#endregion

#include <vector>

int main() {
  auto v1 = std::vector<int, std::allocator<int>>();
  auto v2 = std::vector<int, my_allocator<int>>();
  auto v = v1; // OK
  // v = v2; // error: no match for 'operator=' (operand types are 'std::vector<int,
  //         // std::allocator<int> >' and 'std::vector<int, my_allocator<int> >')
}