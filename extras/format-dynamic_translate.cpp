//#region [Collapse all]
#include <chrono>
#include <cmath>
#include <iostream>
#include <string_view>
//#endregion

//#region [Compiler dependent <format> header]
#include <fmt/chrono.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <version>
#ifdef __cpp_lib_format
#include <format>
#else
namespace std {
using namespace fmt;
}
#endif
//#endregion

enum class Language { Uk, Fr, Es, De, Count };
constexpr int language_count = static_cast<int>(Language::Count);
enum class Message { Opening, Addition };

constexpr std::string_view messages[][language_count] = {
    {"Hello {0}", "Salut {0}", "Hola {0}", "Hallo {0}"},
    {"The average of {0} and {1} is {2}.", "La moyenne de {0} et {1} est {2}.",
     "El promedio de {0} y {1} es {2}.",
     "{2:.6f} ist der Durchschnitt von {0} und {1}."}};

template <Message Id, int... langs, typename... Args>
std::string select_match(std::integer_sequence<int, langs...>, Language language,
                         Args... args) {
  constexpr int id = static_cast<int>(Id);
  const int lang = static_cast<int>(language);
  std::string result;
  (((langs == lang) ? (void)(result = std::format(messages[id][langs], args...))
                    : (void)0),
   ...);
  return result;
}

template <Message Id, typename... Args> //
auto translate(Language lang, Args... args) {
  return select_match<Id>(std::make_integer_sequence<int, language_count>(), lang,
                          args...);
}

int main() {
  Language language = Language::Fr;

  std::cout << translate<Message::Opening>(language, "John") << std::endl;
  
  // TODO in following comment replace 3. by 3 and see what happens
  std::cout << translate<Message::Addition>(language, 1., 2., 3.) << std::endl;
}