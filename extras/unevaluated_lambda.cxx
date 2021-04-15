int main() {

  using x = decltype([] {
    return 1;
  }()); // requires C++20 : error: lambda expression in an unevaluated operand
}