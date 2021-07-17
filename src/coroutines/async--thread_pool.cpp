#include "async--thread_pool.hpp"

ThreadPool::ThreadPool(int max_concurrency) {
  if (max_concurrency < 0)
    max_concurrency = std::thread::hardware_concurrency();
  for (int i = 0; i < max_concurrency; ++i) {
    m_pool.emplace_back(&ThreadPool::wait_loop, this);
  }
}

ThreadPool::~ThreadPool() { stop(Termination::Immediate); }

void ThreadPool::wait_loop() {
  std::function<void()> task;

  while (true) {
    {
      std::unique_lock lock(m_queue_mutex);
      m_queue_condition.wait(lock, [this] {
        return !m_task_queue.empty() || m_termination != Termination::Continue;
      });
      if (m_termination == Termination::Immediate ||
          (m_termination == Termination::EmptyQueue && m_task_queue.empty())) {
        return;
      }
      task = m_task_queue.front();
      m_task_queue.pop();
      lock.unlock();
      task();
    }
  }
}

void ThreadPool::add_task(std::function<void()> task) {
  std::unique_lock lock(m_queue_mutex);
  m_task_queue.push(task);
  lock.unlock();
  m_queue_condition.notify_one();
}

void ThreadPool::stop(ThreadPool::Termination mode) {
  std::unique_lock lock(m_queue_mutex);
  m_termination = mode;
  lock.unlock();
  m_queue_condition.notify_all();
  for (auto &t : m_pool) {
    t.join();
  }
  m_pool.clear();
}
