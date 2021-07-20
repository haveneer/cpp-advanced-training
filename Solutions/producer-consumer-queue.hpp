#pragma once
#include <cassert>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

template <typename T> //
class ConcurrentQueue {
public:
  ConcurrentQueue() = default;
  ConcurrentQueue(const ConcurrentQueue &) = delete;
  ConcurrentQueue(ConcurrentQueue &&) = delete;
  ConcurrentQueue &operator=(const ConcurrentQueue &) = delete;
  ConcurrentQueue &operator=(ConcurrentQueue &&) = delete;
  ~ConcurrentQueue() = default;

public:
  void push(const T &item) {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_condition_full.wait(lock, [this]() { return m_queue.size() < CAPACITY; });
    m_queue.push(item);
    lock.unlock(); // early free lock
    m_condition_empty.notify_one();
  }

  T pop() {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_condition_empty.wait(lock, [this]() { return m_queue.size() > 0; });
    assert(!m_queue.empty());
    auto val = m_queue.front();
    m_queue.pop();
    lock.unlock(); // early free lock
    m_condition_full.notify_one();
    return val;
  }

private:
  std::mutex m_mutex;
  std::queue<T> m_queue;
  std::condition_variable m_condition_empty; // 2 condition_variable + notify_one
  std::condition_variable m_condition_full;  // or 1 condition_variable + notify_all
  const static std::size_t CAPACITY = 32;
};
