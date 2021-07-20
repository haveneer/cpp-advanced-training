// include GTest tools
#include "producer-consumer-ASSERT_DURATION_LE.hpp"
#include "producer-consumer-queue.hpp"
#include "gtest/gtest.h"
#include <algorithm>
#include <list>
#include <random>
//#include <syncstream>
#include <iostream>
#include <thread>
#include <vector>

/***************************************************/

std::mutex display_mutex;
constexpr bool DEBUG_PROCESS = false;

/***************************************************/

class Consumer {
public:
  Consumer(ConcurrentQueue<int> &queue, std::size_t id)
      : m_queue{queue}, m_count{0}, m_id{id} {}
  Consumer(const Consumer &) = delete;
  Consumer(Consumer &&) = default;
  Consumer &operator=(const Consumer &) = delete;
  Consumer &operator=(Consumer &&) = delete;

  virtual ~Consumer() {
    std::lock_guard<std::mutex> lock(display_mutex);
    std::cout << "Consumer #" << m_id << " has popped " << m_count << " values"
              << std::endl;
  }

  void run() {
    bool do_continue = true;
    while (do_continue) {
      auto val = m_queue.pop();
      if constexpr (DEBUG_PROCESS) {
        std::lock_guard<std::mutex> lock(display_mutex);
        std::cout << "Consumer #" << m_id << " pops " << val << std::endl;
      }
      ++m_count;
      do_continue = (val >= 0);
    }
    if constexpr (DEBUG_PROCESS) {
      std::lock_guard<std::mutex> lock(display_mutex);
      std::cout << "Consumer #" << m_id << " done" << std::endl;
    }
  }

private:
  ConcurrentQueue<int> &m_queue;
  std::size_t m_count;
  std::size_t m_id;
};

/***************************************************/

class Producer {
public:
  Producer(ConcurrentQueue<int> &queue, std::size_t id)
      : m_queue{queue}, m_count{0}, m_id{id} {}
  Producer(const Producer &) = delete;
  Producer(Producer &&) = default;
  Producer &operator=(const Producer &) = delete;
  Producer &operator=(Producer &&) = delete;

  virtual ~Producer() {
    std::lock_guard<std::mutex> lock(display_mutex);
    std::cout << "Producer #" << m_id << " has pushed " << m_count << " values"
              << std::endl;
  }

  void run() {
    std::random_device rd;
    std::uniform_int_distribution<int> dist(0, 100);
    bool do_continue = true;
    while (do_continue) {
      int val = dist(rd);
      if constexpr (DEBUG_PROCESS) {
        std::lock_guard<std::mutex> lock(display_mutex);
        std::cout << "Producer #" << m_id << " pushes " << val << std::endl;
      }
      m_queue.push(val);
      ++m_count;
      do_continue = (val != 0);
    }
    if constexpr (DEBUG_PROCESS) {
      std::lock_guard<std::mutex> lock(display_mutex);
      std::cout << "Producer #" << m_id << " done" << std::endl;
    }
  }

private:
  ConcurrentQueue<int> &m_queue;
  std::size_t m_count;
  std::size_t m_id;
};

/***************************************************/

TEST(ProducerConsumerSuit, TestCaseName) {
  ConcurrentQueue<int> queue;

  std::list<Consumer> consumers; // or std::vector with reserve
                                 // (to avoid relocation + pointer invalidation)
  std::vector<std::thread> consumer_threads;
  for (unsigned i = 0; i < 3; ++i) {
    consumers.emplace_back(queue, consumers.size());
    consumer_threads.emplace_back(
        std::thread(&Consumer::run, std::ref(consumers.back())));
  }

  std::list<Producer> producers; // or std::vector + reserve (cf above)
  std::vector<std::thread> producer_threads;
  for (unsigned i = 0; i < 8; ++i) {
    producers.emplace_back(queue, producers.size());
    producer_threads.emplace_back(
        std::thread(&Producer::run, std::ref(producers.back())));
  }

  // Not a gtest standard assertion
  ASSERT_DURATION_LE(1, {
    for (auto &&t : producer_threads)
      t.join();
    for (auto &&c : consumers) { // will stop consumers
      queue.push(-1);
    }
    for (auto &&t : consumer_threads)
      t.join();
  });
}