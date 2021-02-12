// Reqquires C++20

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