// Funny examples Extracted from https://wg21.link/std20 section D.5
// and additional comments from https://youtu.be/KJW_DLaVXIY

void example1() {
  // C++20 says: "Postfix ++ and -- expressions and prefix ++ and -- expressions of
  // volatile-qualified arithmetic and pointer types are deprecated"
  volatile int velociraptor;
  ++velociraptor; // deprecated: is this a single memory operation, two or ?
}

void example2() {
  // C++20 says: "Certain assignments where the left operand is a volatile-qualified
  // non-class type are deprecated"
  int neck, tail;
  volatile int brachiosaur;
  brachiosaur = neck; // OK (a volatile store)
  tail = brachiosaur; // OK (a volatile load)

  // deprecated: does this access brachiosaur once or twice ?
  tail = brachiosaur = neck;
  // deprecated: does this access brachiosaur once or twice ?
  brachiosaur += neck;
  // OK , a volatile load, an addition and a volatile store
  brachiosaur = brachiosaur + neck;
}

namespace example3 {
// C++20 says: "A function type with a parameter with volatile-qualified type or with
// a volatile-qualified return type is deprecated"

// deprecated: a volatile return type has no meaning
volatile struct amber jurassic();

// deprecated: volatile parameters are not meaningful to the caller;
// volatile only applies with the function
void trex(volatile short left_arm, volatile short right_arm);

// OK, the pointer isn't volatile; the data it points to is (same for reference)
void fly(volatile struct pterosaur *pteranodon);
} // namespace example3

namespace example4 {
// C++20 says: "A structured binding of a volatile-qualified type is deprecated."

struct linhenykus {
  short forelimb;
};
void park(linhenykus alvarezsauroid) {
  // deprecated: does the binding copy the forelimb?
  volatile auto [what_is_this] = alvarezsauroid;
  // ...
}

} // namespace example4

int main() {}