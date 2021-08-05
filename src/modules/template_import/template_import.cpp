import template_module;
import constant_module;

int main() {
  constexpr unsigned size = 5;
  Vector<double> v(size);
  for (int incr = 1; auto &x : v) {
    x = incr++;
  }
  Vector<double> w = pi<double> * v;

#ifndef __clang__
  print(w);
#endif
}