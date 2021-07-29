#include <iostream>

union vec4 {
  float array[4];
  struct {
    float x, y, z, w;
  };
};

int main() {
  auto v = vec4{};
  v.x = 5;
  v.y = 6;
  v.array[2] = 7;

  std::cout << "v.x = " << v.x << '\n';
  std::cout << "v.y = " << v.y << '\n';
  std::cout << "v.z = " << v.z << '\n';
}