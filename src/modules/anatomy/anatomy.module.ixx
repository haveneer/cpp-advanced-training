module;

#include <cmath> // C header: cannot be imported as a module

export module anatomy.module;

import useful.module;

export import reexported; // cannot be located below (should be first)
export using Type = int;
export class Class;
export Type function(Class *);
export int data = 1;
export namespace Namespace {
/* more exported things */
double bar();
} // namespace Namespace

// Not exported things (but isolated in this module)

double foo() { return std::sin(0); }

// Definitions

class Class {};