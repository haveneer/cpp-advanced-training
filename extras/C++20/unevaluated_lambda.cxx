//
// Created by Pascal Havé on 08/02/2021.
//

int main() {

  using x= decltype([] { return 1; }()); // requires C++20 : error: lambda expression in an unevaluated operand
}