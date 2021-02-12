#ifndef TYPE_HPP
#define TYPE_HPP

#include <string>
#include <typeinfo>

std::string demangle(const char* name);

template <class T>
inline std::string type() {
  typedef typename std::remove_reference<T>::type TR;
  std::string r = demangle(typeid(TR).name());

  if (std::is_const<TR>::value)
    r += " const";
  if (std::is_volatile<TR>::value)
    r += " volatile";
  if (std::is_lvalue_reference<T>::value)
    r += "&";
  else if (std::is_rvalue_reference<T>::value)
    r += "&&";

  return r;
}

template <class T>
inline std::string type(const T&) {
  return type<T>();
}

#ifdef __GNUG__
#include <cstdlib>
#include <memory>
#include <cxxabi.h>

// inline to enable .h only

// Use GCC C++ ABI
// (cf from https://stackoverflow.com/questions/281818/unmangling-the-result-of-stdtype-infoname
//     and  https://stackoverflow.com/questions/28621844/is-there-a-typeid-for-references )
//
inline std::string demangle(const char* name) {

  int status = -4; // some arbitrary value to eliminate the compiler warning

  // enable c++11 by passing the flag -std=c++11 to g++
  std::unique_ptr<char, void(*)(void*)> res {
      abi::__cxa_demangle(name, NULL, NULL, &status),
      std::free
  };

  return (status==0) ? res.get() : name ;
}

#else

// does nothing if not g++ compatible
// TODO use portable boost
// (cf https://stackoverflow.com/questions/38820579/using-auto-type-deduction-how-to-find-out-what-type-the-compiler-deduced
//     https://stackoverflow.com/questions/28621844/is-there-a-typeid-for-references )
//        boost::typeindex::type_id_with_cvr<decltype(x)>().pretty_name()
inline std::string demangle(const char* name) {
    return name;
}

#endif
#endif