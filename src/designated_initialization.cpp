//#region [Headers]
#include <iostream>
#include <string>
//#endregion

//#region [Class definitions; expand me]
using Real = float;

struct Real2 {
  Real x, y;
};

struct RobotAgent {
  // RobotAgent() = default; // user-declared ctor incompatible with designated init
  Real position[2];
  Real2 speed;
  int payload;
  std::string name = ""; // default member initialisation is compatible
};
//#endregion

//#region [ostream operator<<]
std::ostream &operator<<(std::ostream &o, const Real2 &p) {
  return o << "{ .x=" << p.x << ", .y=" << p.y << " }";
}

std::ostream &operator<<(std::ostream &o, const RobotAgent &r) {
  std::ostream out(o.rdbuf()); // safe RAII flags restore
  out.precision(2);
  out << "{ .position=[" << r.position[0] << ", " << r.position[1] << "]"
      << ", .speed=" << r.speed << ", .payload=" << r.payload << ", .name=\""
      << r.name << "\" }";
  return o;
}
//#endregion

int main() {
  RobotAgent r1;
  RobotAgent r2{};
  RobotAgent r3{.name = "Wall-E"};
  RobotAgent r4{.speed = {.y = 1000}, .name = "EVE"};

  // Valid C99 examples but invalid in C++20
  // RobotAgent r5{.name = "KO", .payload = 1}; // error: designator order
  //                                            // do not match declaration order
  // RobotAgent r6{.position.y = 1000};         // error: nested designator
  // RobotAgent r7{.payload=1, "Name"};         // error: mixed initialization
  // RobotAgent r8{.position[1]=1};             // error: array designated init

  //#region [Display]
  std::cout << "r1 = " << r1 << '\n';
  std::cout << "r2 = " << r2 << '\n';
  std::cout << "r3 = " << r3 << '\n';
  std::cout << "r4 = " << r4 << '\n';
  //#endregion
}