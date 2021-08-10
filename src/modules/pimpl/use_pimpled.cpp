import pimpled;

//#region [Collapse all]
#include "type.hpp" // advanced type printing
//#endregion

#include <iostream>

int main() {
  S s;              // S is exported => visible => reachable 
  s.do_stuff();     // OK.
  auto p = s.get(); // OK: pointer to incomplete type.
                    // Impl is required => reachable only
  std::cout << "Type of p is " << type<decltype(p)>() << '\n';
  // auto impl = *s.get(); // ill-formed: use of undefined type 'Impl'.
}