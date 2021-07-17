#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
public:
  enum class Termination { Continue, Immediate, EmptyQueue };

public:
  ThreadPool(int max_concurrency = -1);
  ~ThreadPool();

public:
  ThreadPool(const ThreadPool &) = delete;
  ThreadPool(ThreadPool &&) = delete;
  ThreadPool &operator=(const ThreadPool &) = delete;
  ThreadPool &operator=(ThreadPool &&) = delete;

public:
  void add_task(std::function<void()> task);
  void stop(Termination mode);

private:
  void wait_loop();

private:
  Termination m_termination = Termination::Continue;

  std::mutex m_queue_mutex;
  std::condition_variable m_queue_condition;
  std::queue<std::function<void()>> m_task_queue;
  std::vector<std::thread> m_pool;
};
