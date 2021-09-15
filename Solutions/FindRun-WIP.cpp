#include <version>
#if !defined(__cpp_lib_jthread)
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_lib_jthread)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

/* Subject:
 * A hash challenge (https://fr.wikipedia.org/wiki/Preuve_de_travail),
 * requires to compute a hash of an object + a seed so that there are 'n' zeros at
 * the end.
 * Write a multithreaded program where the search space is split on them. The first
 * thread which finds a solution has to call a stop_request for all other thread.
 */

int main() {}

#endif