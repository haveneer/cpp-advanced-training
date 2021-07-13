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

#include <condition_variable>
#include <mutex>
#include <thread>

// std::cout needs also to be protected
// HINT This is why in previous examples I used std::cerr in thread
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

int main() {
  std::mutex mutex;
  bool ready = false; // need to be protected
  std::condition_variable cv;

  auto start = std::chrono::system_clock::now();

  std::thread t([&] {
    // std::this_thread::sleep_for(std::chrono::seconds(1));
    SyncCout{} << "Thread OK";
    std::unique_lock lock(mutex); // always a std::unique_lock
    cv.wait(lock, [&ready] { return ready == true; });
    SyncCout{} << "Thread END";
  });

  using namespace std::chrono_literals;
  std::this_thread::sleep_for(1s); // HINT to show that the order
                                   // notify/wait doesn't matter
  {
    std::scoped_lock guard(mutex); // C++17 CTAD
    ready = true;
  }

  cv.notify_one();
  SyncCout{} << "Main OK";

  t.join();

  std::chrono::duration<double> dur = std::chrono::system_clock::now() - start;
  SyncCout{} << "Duration: " << dur.count() << " seconds";
}
