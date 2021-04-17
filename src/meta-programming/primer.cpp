//#region [Collapse all]
#include <iostream>
//#endregion

// ROUTINE GENERANT UN WARNING D'AFFICHAGE
template <int I> struct WarnAff {};

// AFFICHAGE CONDITIONNEL
template <int I, int OK, int LAST> struct PrintIF {
  static inline void eval() {}
};

template <int I> struct PrintIF<I, 1, 1> {
  static inline void eval() {
    WarnAff<I> IS_A_PRIME_NUMBER;
    std::cout << I << " is a prime number\n";
  }
};

template <int I> struct PrintIF<I, 0, 1> {
  static inline void eval() { /* WarnAff<I> IS_NOT_A_PRIME_NUMBER; */
  }
};

// LES FONCTIONS DE TESTS GENERIQUES ET PARTIELS

// BOUCLE DICHOTOMIQUE (en profondeur logarithmique)
template <int I, int TEST, int MIN, int MAX, int LAST> struct TESTPRIME_PART {
  enum { midsup = (MAX + MIN + 1) / 2 };
  enum { midinf = (MAX + MIN - 1) / 2 };
  enum { int_OK = MIN * MIN <= I };
  enum {
    cont2 =
        TESTPRIME_PART<I, (midsup == MAX), // static_cast to disable warnings
                       static_cast<unsigned>(midsup) * static_cast<unsigned>(int_OK),
                       static_cast<unsigned>(MAX) * static_cast<unsigned>(int_OK),
                       0>::test
  };
  enum {
    cont1 =
        TESTPRIME_PART<I * cont2, (midinf == MIN), // static_cast to disable warnings
                       static_cast<unsigned>(MIN) * static_cast<unsigned>(int_OK),
                       static_cast<unsigned>(midinf) * static_cast<unsigned>(int_OK),
                       0>::test
  };

  enum { test = cont1 }; // comme (cont1 != 0) && (cont2 != 0)
  static inline void eval() { PrintIF<I, test, LAST>::eval(); }
};

template <int MIN, int MAX, int LAST> struct TESTPRIME_PART<0, 0, MIN, MAX, LAST> {
  enum { test = 0 };
  static inline void eval() {}
};
template <int MIN, int MAX, int LAST> struct TESTPRIME_PART<0, 1, MIN, MAX, LAST> {
  enum { test = 0 };
  static inline void eval() {}
};

template <int I, int MIN, int MAX, int LAST>
struct TESTPRIME_PART<I, 1, MIN, MAX, LAST> {
  enum { test = (I % MIN != 0) };
  static inline void eval() { PrintIF<I, test, LAST>::eval(); }
};

template <int I, int LAST> struct TESTPRIME_PART<I, 1, 0, 0, LAST> {
  enum { test = 1 };
  static inline void eval() {}
};
template <int I, int LAST> struct TESTPRIME_PART<I, 0, 0, 0, LAST> {
  enum { test = 1 };
  static inline void eval() {}
};

// EN CAS D'APPEL INDIVIDUEL
template <int I> struct TESTPRIME {
  static inline void eval() { TESTPRIME_PART<I, 0, 2, I, 1>::eval(); }
};

/****** BOUCLAGE DES TESTS *******/
template <int TEST, int MIN, int MAX> struct NEXTPRIME {
  enum { midsup = (MAX + MIN + 1) / 2 };
  enum { midinf = (MAX + MIN - 1) / 2 };

  static inline void eval() {
    NEXTPRIME<midinf == MIN, MIN, midinf>::eval();
    NEXTPRIME<midsup == MAX, midsup, MAX>::eval();
  }
};

template <int MIN, int MAX> struct NEXTPRIME<1, MIN, MAX> {
  enum { go = ((MIN % 2) ? 1 : 0) };

  static inline void eval() {
    TESTPRIME_PART<MIN * go, 2 == (MIN - 1), 2, MIN - 1, 1>::eval();
  }
};

template <int MIN, int MAX> struct RANGE_TESTPRIME {
  static inline void eval() { NEXTPRIME<MIN == MAX, MIN, MAX>::eval(); }
};

int main() {
  TESTPRIME<647>::eval();
  RANGE_TESTPRIME<10000, 10100>::eval();
}