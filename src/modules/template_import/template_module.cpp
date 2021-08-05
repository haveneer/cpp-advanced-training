module;

#include <cassert> // C-like include are not modularized
#include <cstddef>

export module template_module;

// Not exported section : no 'details' namespace required

// Not exported; usually hidden in a 'details' namespace
template <typename T> decltype(auto) accessor(T *ptr, std::size_t i, std::size_t n) {
  assert(i < n); // Use C++14 decltype(auto) tricks to be DRY on const/non-const ops
  return ptr[i]; // Better usage when applied to Self* inside the class
}

// Exported symbols

export template <typename T> struct Iterator {
  const T &operator*() const { return *ptr; }
  T &operator*() { return *ptr; }
  Iterator &operator++() {
    ++ptr;
    return *this;
  }
  T *ptr;
  bool operator!=(const Iterator<T> &that) const { return this->ptr != that.ptr; }
};

export template <typename T> class Vector {
public:
  Vector(int n = 0) : m_size(n), m_data(new T[n]) {}
  Vector(const Vector<T> &v) = delete;
  Vector(Vector<T> &&v);
  Vector<T> &operator=(const Vector<T> &v) = delete;
  Vector<T> &operator=(Vector<T> &&v) = delete;
  virtual ~Vector() { delete[] m_data; }

public:
  inline int size() const { return m_size; }
  T &operator()(int i) { return accessor(m_data, i, m_size); }
  const T &operator()(int i) const { return accessor(m_data, i, m_size); }
  Vector<T> operator*(T a) const;

public:
  Iterator<T> begin() { return {m_data}; }
  Iterator<T> end() { return {m_data + m_size}; }

private:
  int m_size;
  T *m_data;
};

export template <typename T>
Vector<T> operator*(T a, const Vector<T> &v)
//#region [Workaround]
// clang-format off
#ifdef __clang__
    { return v * a; }
#else
    ;
#endif
// clang-format on
//#endregion

/***************** definitions *******************/

template <typename T> Vector<T>::Vector(Vector<T> &&v) : m_size(v.m_size) {
  m_data = v.m_data;
  v.m_data = nullptr;
  v.m_size = 0;
}

template <typename T> Vector<T> Vector<T>::operator*(T a) const {
  const Vector<T> &v = *this;
  Vector<T> newv(m_size);
  for (int i = 0; i < m_size; ++i)
    newv(i) = a * v(i);
  return newv;
}

//#region [Workaround]
#ifndef __clang__
template <typename T> Vector<T> operator*(T a, const Vector<T> &v) { return v * a; }
#endif
//#endregion