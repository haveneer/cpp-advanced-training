#include <version>
#if __has_include(<experimental/coroutine>)
#define __cpp_lib_coroutine
#endif
#ifndef __cpp_lib_coroutine
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_lib_coroutine)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

#include "async--synccout.hpp"
#include "async--server.hpp"
#include "async--thread_pool.hpp"
#include <gtest/gtest.h>
#include <list>

struct Foo {
  void method() {
    auto execution_order = counter++;
    SyncCout{} << "Thread #Foo done at " << execution_order;
  }
  std::atomic<int> &counter;
};

TEST(Async, ThreadPoolWorkFlow) {
  ThreadPool pool;
  std::atomic<int> counter;

  auto start = std::chrono::steady_clock::now();
  for (int i = 0; i < 17; ++i)
    pool.add_task([i, &counter] {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      auto execution_order = counter++;
      SyncCout{} << "Thread #" << i << " done at " << execution_order;
    });

  Foo foo{counter};
  pool.add_task(std::bind(&Foo::method, &foo));

  pool.stop(ThreadPool::Termination::EmptyQueue);

  std::chrono::duration<double> duration = std::chrono::steady_clock::now() - start;
  std::cout << "duration: " << duration.count() << '\n';
}

TEST(Async, server) {
  ThreadPool pool(4);
  Context context{pool};

  SyncCout{} << "Enter main";
  std::list<Task> tasks;
  SyncCout{} << "New Echo Stream";
  tasks.emplace_back(tcp_echo_server(Source{"Src1"}, Destination{"Dst1"}, context));
  SyncCout{} << "Check state on " << Colorize{&tasks.back()} << " "
             << Colorize{tasks.back().m_co_handle.address()} << " "
             << tasks.back().m_co_handle.done();
  SyncCout{} << "New Echo Stream";
  tasks.emplace_back(tcp_echo_server(Source{"Src2"}, Destination{"Dst2"}, context));
  SyncCout{} << "Check state on " << Colorize{&tasks.back()} << " "
             << Colorize{tasks.back().m_co_handle.address()} << " "
             << tasks.back().m_co_handle.done();
  assert((tasks.begin())->m_co_handle != (std::next(tasks.begin()))->m_co_handle);

  SyncCout{} << "Wait loop";
  while (not tasks.empty()) {
    SyncCout{} << "• (remaining=" << tasks.size() << ")";
    for (auto i = tasks.begin(); i != tasks.end(); ++i) {
      const Task &task = *i;
      if (task.done()) {
        SyncCout{} << "## Task " << Colorize{task.m_co_handle.address()} << " done";
        i = tasks.erase(i); // or tasks.erase(i++);
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  SyncCout{} << Colorize<std::thread::id>::count() << " threads have been built";
  SyncCout{}
      << Colorize<decltype(std::declval<Task::handle_type>().address())>::count()
      << " coroutine handles have been built";

  SyncCout{} << "Exit main";
}

#endif