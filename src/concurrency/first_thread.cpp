//#region [Collapse all]
#include <iostream>
#include <thread>
//#endregion

int main() {
  // To create a new thread you have to provide a callable object (non blocking)
  std::thread t([] { std::cout << "Hello from thread" << std::endl; });
  
  // Continue main thread
  std::cout << "Hello from main" << std::endl;
  
  // Main thread waits for thread t (blocking)
  t.join();
  
  // Thread t has been joined; main continues its way
  std::cout << "By from main" << std::endl;
}