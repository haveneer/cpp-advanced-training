//#region [declarations]
#include <iostream>
#define DISPLAY(x) std::cout << #x << " = " << x << '\n';
//#endregion

int main() {
  auto sumInt = [](int a, int b) { return a + b; };
  auto sumAuto = [](auto a, auto b) { return a + b; };
  auto sumDeclAuto = [](auto a, decltype(a) b) { return a + b; };
  auto sumT = []<typename T>(T a, T b) { return a + b; };

  DISPLAY(sumInt(true, 41));      // TODO: what are the outputs ?
  DISPLAY(sumAuto(true, 41));     //
  DISPLAY(sumDeclAuto(true, 41)); // HINT: what is the type of 'a' ?
  // DISPLAY(sumT(*/ true, 41));  // error: no match for call
  //                              //   to '(main()::<lambda(T, T)>) (bool, int)'
}