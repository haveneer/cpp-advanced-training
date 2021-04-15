#include <version>
#ifndef __cpp_lib_jthread
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#define STR(X) #X
static const char *unsupported_features[] = {STR(__cpp_lib_jthread)};
#include "unsupported_features.hpp"
#else
#error "Unsupported feature"
#endif
//#endregion
#else

#include <thread>

void do_something(std::stop_token st) {
  puts("start");
  while (!st.stop_requested()) {
    // do anything
  }
  puts("stop");
}

int main() {
  std::jthread t{do_something};

  // will do automatically
  // t.request_stop();
  // t.join();

  // with classical threads, without join(), code stops with 'terminated'
}
#endif