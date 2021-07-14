#include <atomic>
#include <iostream>
#include <thread>

int main() {
  std::atomic<int> a{1};

  std::thread t1([&] { a = a * 3; }); // not atomic: atomic read + ops + atomic write
  std::thread t2([&] { a = a + 1; }); // not atomic: atomic read + ops + atomic write
  std::thread t3([&] { ++a; });       // atomic    : atomic operator++

  t1.join();
  t2.join();
  t3.join();

  std::cout << "a = " << a << std::endl;
}