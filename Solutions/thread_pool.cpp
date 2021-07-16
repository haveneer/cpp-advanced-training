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

// #region [Collapse all]
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>
#include <functional>
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

std::mutex queue_mutex;
std::condition_variable queue_condition;
std::queue<std::function<void()>> task_queue;

enum class Termination { Continue, Immediate, EmptyQueue };
Termination termination = Termination::Continue;

void thread_wait_loop() {
  std::function<void()> task;

  while (true) {
    {
      std::unique_lock lock(queue_mutex);
      queue_condition.wait(lock, [] {
        return !task_queue.empty() || termination != Termination::Continue;
      });
      if (termination == Termination::Immediate ||
          (termination == Termination::EmptyQueue && task_queue.empty())) {
        return;
      }
      task = task_queue.front();
      task_queue.pop();
      lock.unlock();
      task();
    }
  }
}

void add_task(std::function<void()> task) {
  std::unique_lock lock(queue_mutex);
  task_queue.push(task);
  lock.unlock();
  queue_condition.notify_one();
}

void stop_pool(Termination mode) {
  std::unique_lock lock(queue_mutex);
  termination = mode;
  lock.unlock();
  queue_condition.notify_all();
}

int main() {
  const unsigned int max_concurrency = std::thread::hardware_concurrency();
  std::cout << "max concurrency = " << max_concurrency << '\n';
  std::vector<std::thread> pool;
  for (unsigned int i = 0; i < max_concurrency; ++i) {
    pool.push_back(std::thread(thread_wait_loop));
  }

  auto start = std::chrono::steady_clock::now();
  for (int i = 0; i < 17; ++i)
    add_task([] {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      SyncCout{} << "Done";
    });

  // add_task(std::bind(&Some_Class::Some_Method, &Some_object));

  stop_pool(Termination::EmptyQueue);
  for (auto &t : pool) {
    t.join();
  }

  std::chrono::duration<double> duration = std::chrono::steady_clock::now() - start;
  std::cout << "duration: " << duration.count() << '\n';
}