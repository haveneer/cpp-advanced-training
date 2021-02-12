#include <tuple>
#include <type_traits>

int main() {
  // Alignment specification
  alignas(float) unsigned char c[sizeof(float)];
}

