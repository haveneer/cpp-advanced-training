//#region [Declarations]
/***********************************************************************
 * algebraT.cc : an example of function algebra with differenciation    *
 * Author : Pascal Havé (hpwxf@haveneer.com)                           *
 ***********************************************************************/
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#ifdef DEBUG_TYPE
#include "type.hpp" // advanced type printing
#else // clang-format off
#include <typeinfo>
template <typename T> std::string type() { if (std::is_same_v<std::remove_extent_t<std::remove_const_t< std::remove_pointer_t<std::remove_reference_t<T>>>>, T>) return typeid(T).name(); else if (std::is_array_v<T>) return type<std::remove_extent_t<T>>() + "[]"; else if (std::is_const_v<T>) return type<std::remove_const_t<T>>() + " const"; else if (std::is_pointer_v<T>) return type<std::remove_pointer_t<T>>() + "*"; else if (std::is_reference_v<T>) return type<std::remove_reference_t<T>>() + ((std::is_lvalue_reference_v<T>) ? "&" : "") + ((std::is_rvalue_reference_v<T>) ? "&&" : ""); else std::string("cannot decode ") + typeid(T).name(); }
#endif              // clang-format on
//#endregion

// Enable static optimization of expressions
#define OPTIMIZE

typedef double Real;

// CTRP : "Curious Template Resursive Pattern" en tant que d'Interface statique
template <typename Function> class FunctionT {
public:
  FunctionT() = default;
  /* no virtual */ ~FunctionT() = default;

public:
  const Function &cast() const { return static_cast<const Function &>(*this); }
};

// Forward declarations
class Zero;
class One;
class Identity;
class Constant;
template <typename Function> class Sinus;
template <typename Function> class Cosinus;
template <typename Function1, typename Function2> class Add;
template <typename Function1, typename Function2> class Mult;

// Traits d'optimisation
template <typename Function> struct Opt { typedef Function Type; };

// Constante symbolique 0
class Zero : public FunctionT<Zero> {
public:
  typedef Zero DiffType;

public:
  Zero() = default;
  inline Real operator()(Real x) const { return 0; }
  DiffType diff() const;
  std::ostream &print(std::ostream &o) const {
    o << '0';
    return o;
  }
};

// Constante symbolique 1
class One : public FunctionT<One> {
public:
  typedef Zero DiffType;

public:
  One() = default;
  inline Real operator()(Real x) const { return 1; }
  DiffType diff() const;
  std::ostream &print(std::ostream &o) const {
    o << '1';
    return o;
  }
};

// Constante quelconque
class Constant : public FunctionT<Constant> {
public:
  typedef Zero DiffType;

public:
  explicit Constant(Real v) : m_v(v) {}
  inline Real operator()(Real x) const { return m_v; }
  DiffType diff() const;
  std::ostream &print(std::ostream &o) const {
    o << m_v;
    return o;
  }

public:
  Real value() const { return m_v; }

public: /* Données internes */
  const Real m_v;
};

// Identité
class Identity : public FunctionT<Identity> {
public:
  typedef One DiffType;

public:
  Identity() = default;
  inline Real operator()(Real x) const { return x; }
  DiffType diff() const;
  std::ostream &print(std::ostream &o) const {
    o << 'x';
    return o;
  }
};

// Addition
template <typename Function1, typename Function2>
class Add : public FunctionT<Add<Function1, Function2>> {
public:
  typedef typename Opt<Add<typename Function1::DiffType,
                           typename Function2::DiffType>>::Type DiffType;

public:
  Add(Function1 f, Function2 g) : m_f(std::move(f)), m_g(std::move(g)) {}
  inline Real operator()(Real x) const { return m_f(x) + m_g(x); }
  DiffType diff() const;
  std::ostream &print(std::ostream &o) const {
    o << '(';
    m_f.print(o);
    o << ") + (";
    m_g.print(o);
    o << ')';
    return o;
  }

public: /* Données internes */
  const Function1 m_f;
  const Function2 m_g;
};

// Multiplication
template <typename Function1, typename Function2>
class Mult : public FunctionT<Mult<Function1, Function2>> {
public:
  typedef typename Opt<
      Add<typename Opt<Mult<Function1, typename Function2::DiffType>>::Type,
          typename Opt<Mult<typename Function1::DiffType, Function2>>::Type>>::Type
      DiffType;

public:
  Mult(Function1 f, Function2 g) : m_f(std::move(f)), m_g(std::move(g)) {}
  inline Real operator()(Real x) const { return m_f(x) * m_g(x); }
  DiffType diff() const;
  std::ostream &print(std::ostream &o) const {
    o << '(';
    m_f.print(o);
    o << ") * (";
    m_g.print(o);
    o << ')';
    return o;
  }

public: /* Données internes */
  const Function1 m_f;
  const Function2 m_g;
};

// Inverse
template <typename Function> class Inverse : public FunctionT<Inverse<Function>> {
public:
  typedef typename Opt<Mult<
      Constant,
      typename Opt<Mult<typename Function::DiffType,
                        Inverse<typename Opt<Mult<Function, Function>>::Type>>>::
          Type>>::Type DiffType;

public:
  Inverse(Function f) : m_f(std::move(f)) {}
  inline Real operator()(Real x) const { return 1. / m_f(x); }
  DiffType diff() const;
  std::ostream &print(std::ostream &o) const {
    o << "inv(";
    m_f.print(o);
    o << ")";
    return o;
  }

public: /* Données internes */
  const Function m_f;
};

// Puissance
template <int n, typename Function> class Pow : public FunctionT<Pow<n, Function>> {
public:
  typedef typename Opt<
      Mult<Constant, typename Opt<Mult<typename Opt<Pow<n - 1, Function>>::Type,
                                       typename Function::DiffType>>::Type>>::Type
      DiffType;

public:
  Pow(Function f) : m_f(std::move(f)) {}
  inline Real operator()(Real x) const { return std::pow(m_f(x), n); }
  DiffType diff() const;
  // std::ostream & print(std::ostream & o) const { o << '('; m_f.print(o); o <<
  // ") ^" << n; return o; }
  std::ostream &print(std::ostream &o) const {
    o << "std::pow(";
    m_f.print(o);
    o << "," << n << ")";
    return o;
  }

public: /* Données internes */
  const Function m_f;
};

// Sinus
template <typename Function> class Sinus : public FunctionT<Sinus<Function>> {
public:
  typedef typename Opt<Mult<Cosinus<Function>, typename Function::DiffType>>::Type
      DiffType;

public:
  Sinus(Function f) : m_f(std::move(f)) {}
  inline Real operator()(Real x) const { return std::sin(m_f(x)); }
  DiffType diff() const;
  std::ostream &print(std::ostream &o) const {
    o << "sin(";
    m_f.print(o);
    o << ')';
    return o;
  }

public: /* Données internes */
  const Function m_f;
};

// Cosinus
template <typename Function> class Cosinus : public FunctionT<Cosinus<Function>> {
public:
  typedef typename Opt<Mult<
      Constant,
      typename Opt<Mult<Sinus<Function>, typename Function::DiffType>>::Type>>::Type
      DiffType;

public:
  Cosinus(Function f) : m_f(std::move(f)) {}
  inline Real operator()(Real x) const { return std::cos(m_f(x)); }
  DiffType diff() const;
  std::ostream &print(std::ostream &o) const {
    o << "cos(";
    m_f.print(o);
    o << ')';
    return o;
  }

public: /* Données internes */
  const Function m_f;
};

// Quelques fonctions pour une interface utilisateur plus commode
template <typename Function> Sinus<Function> sin(const FunctionT<Function> &f) {
  return Sinus<Function>(f.cast());
}
template <typename Function> Cosinus<Function> cos(const FunctionT<Function> &f) {
  return Cosinus<Function>(f.cast());
}
// Des opérateurs primaires
template <typename Function1, typename Function2>
Add<Function1, Function2> operator+(const FunctionT<Function1> &f,
                                    const FunctionT<Function2> &g) {
  return Add<Function1, Function2>(f.cast(), g.cast());
}
template <typename Function1, typename Function2>
typename Opt<Mult<Function1, Function2>>::Type
operator*(const FunctionT<Function1> &f, const FunctionT<Function2> &g) {
  return Mult<Function1, Function2>(f.cast(), g.cast());
}
template <typename Function1, typename Function2>
typename Opt<Mult<Function1, Inverse<Function2>>>::Type
operator/(const FunctionT<Function1> &f, const FunctionT<Function2> &g) {
  return f.cast() * Inverse<Function2>(g.cast());
}

// Autres opérateurs associés
template <typename Function>
typename Opt<Add<Function, Constant>>::Type operator+(const FunctionT<Function> &f,
                                                      const Real &a) {
  return Add<Function, Constant>(f.cast(), Constant(a));
}
template <typename Function>
typename Opt<Mult<Constant, Function>>::Type
operator*(const Real &a, const FunctionT<Function> &f) {
  return Mult<Constant, Function>(Constant(a), f.cast());
}
template <typename Function>
typename Opt<Add<Function, Constant>>::Type operator-(const FunctionT<Function> &f,
                                                      const Real &a) {
  return Add<Function, Constant>(f.cast(), Constant(-a));
}
template <typename Function>
typename Opt<Mult<Constant, Inverse<Function>>>::Type
operator/(const Real &a, const FunctionT<Function> &f) {
  return Constant(a) * Inverse<Function>(f.cast());
}
template <typename Function>
typename Opt<Mult<Function, Constant>>::Type operator/(const FunctionT<Function> &f,
                                                       const Real &a) {
  return f.cast() * Constant(1. / a);
}

// Des opérations déduits
template <typename Function>
typename Opt<Add<Function, Constant>>::Type operator+(const Real &a,
                                                      const FunctionT<Function> &f) {
  return f.cast() + a;
}
template <typename Function>
typename Opt<Mult<Constant, Function>>::Type operator*(const FunctionT<Function> &f,
                                                       const Real &a) {
  return a * f.cast();
}
template <typename Function>
typename Opt<Add<Function, Constant>>::Type operator-(const Real &a,
                                                      const FunctionT<Function> &f) {
  return f.cast() - a;
}
template <typename Function>
typename Opt<Mult<Constant, Function>>::Type
operator-(const FunctionT<Function> &f) {
  return Constant(-1) * f.cast();
}
template <typename Function1, typename Function2>
typename Opt<Add<Function1, typename Opt<Mult<Constant, Function2>>::Type>>::Type
operator-(const FunctionT<Function1> &f, const FunctionT<Function2> &g) {
  return f + (-g);
}
template <int n, typename Function>
typename Opt<Pow<n, Function>>::Type pow(const FunctionT<Function> &f) {
  return Pow<n, Function>(f.cast());
}

// Les calculs symbolique de dérivées
One Identity::diff() const { return One(); }
Zero Constant::diff() const { return Zero(); }
Zero Zero::diff() const { return Zero(); }
Zero One::diff() const { return Zero(); }
template <typename Function>
typename Sinus<Function>::DiffType Sinus<Function>::diff() const {
  return cos(m_f) * m_f.diff();
}
template <typename Function>
typename Cosinus<Function>::DiffType Cosinus<Function>::diff() const {
  return -(sin(m_f) * m_f.diff());
}
template <typename Function1, typename Function2>
typename Add<Function1, Function2>::DiffType
Add<Function1, Function2>::diff() const {
  return m_f.diff() + m_g.diff();
}
template <typename Function1, typename Function2>
typename Mult<Function1, Function2>::DiffType
Mult<Function1, Function2>::diff() const {
  return m_f * m_g.diff() + m_f.diff() * m_g;
}
template <typename Function>
typename Inverse<Function>::DiffType Inverse<Function>::diff() const {
  return -(m_f.diff() / (m_f * m_f));
}
template <int n, typename Function>
typename Pow<n, Function>::DiffType Pow<n, Function>::diff() const {
  return n * (pow<n - 1>(m_f) * m_f.diff());
}

// Opérateur de dérivation
template <typename Function> typename Function::DiffType d(const Function &f) {
  return f.diff();
}

// Affichage d'une expression
template <typename Function>
std::ostream &operator<<(std::ostream &o, const FunctionT<Function> &f) {
  return f.cast().print(o);
}

#ifdef OPTIMIZE
// Simplification statique de d'arbre d'évaluation
// Uniquement la transformation; les dérivations et évaluations viennent sont
// déduites des expressions transformées

// Règle: F+0 = F
template <typename Function>
class Add<Function, Zero> : public FunctionT<Add<Function, Zero>> {
public:
  Add(const Function &f, const Zero &g) : m_f(f) {}

public:
  operator Function() { return m_f; }

public: /* Données internes */
  const Function m_f;
};
template <typename Function> struct Opt<Add<Function, Zero>> {
  typedef Function Type;
};

// Règle: 0+F = F
template <typename Function>
class Add<Zero, Function> : public FunctionT<Add<Zero, Function>> {
public:
  Add(const Zero &f, Function g) : m_g(std::move(g)) {}

public:
  operator Function() { return m_g; }

public: /* Données internes */
  const Function m_g;
};
template <typename Function> struct Opt<Add<Zero, Function>> {
  typedef Function Type;
};

// Règle: F*1 = F
template <typename Function>
class Mult<Function, One> : public FunctionT<Mult<Function, One>> {
public:
  Mult(Function f, const One &g) : m_f(std::move(f)) {}

public:
  operator Function() { return m_f; }

public: /* Données internes */
  const Function m_f;
};
template <typename Function> struct Opt<Mult<Function, One>> {
  typedef Function Type;
};

// Règle: 1*F = F
template <typename Function>
class Mult<One, Function> : public FunctionT<Mult<One, Function>> {
public:
  Mult(const One &f, const Function &g) : m_g(g) {}

public:
  operator Function() { return m_g; }

public: /* Données internes */
  const Function m_g;
};
template <typename Function> struct Opt<Mult<One, Function>> {
  typedef Function Type;
};

// Règle: C1 * C2 = C3
template <>
class Mult<Constant, Constant> : public FunctionT<Mult<Constant, Constant>> {
public:
  Mult(const Constant &f, const Constant &g) : m_v(f.value() * g.value()) {}

public:
  operator Constant() { return Constant(m_v); }

public: /* Données internes */
  const Real m_v;
};
template <> struct Opt<Mult<Constant, Constant>> { typedef Constant Type; };
template <> struct Opt<Mult<Constant, One>> { typedef Constant Type; };
template <> struct Opt<Mult<Constant, Zero>> { typedef Zero Type; };

// Règle: 0 * F = 0
template <typename Function>
class Mult<Zero, Function> : public FunctionT<Mult<Zero, Function>> {
public:
  Mult(const Zero &f, const Function &g) {}

public:
  operator Zero() { return Zero(); }
};
template <typename Function> struct Opt<Mult<Zero, Function>> { typedef Zero Type; };

// Règle: F * 0 = 0
template <typename Function>
class Mult<Function, Zero> : public FunctionT<Mult<Function, Zero>> {
public:
  Mult(const Function &f, const Zero &g) {}

public:
  operator Zero() { return Zero(); }
};
template <typename Function> struct Opt<Mult<Function, Zero>> { typedef Zero Type; };

// Règle: F^1 = F
template <typename Function>
class Pow<1, Function> : public FunctionT<Pow<1, Function>> {
public:
  Pow(const Function &f) : m_f(f) {}

public:
  operator Function() { return m_f; }

public: /* Données internes */
  const Function m_f;
};
template <typename Function> struct Opt<Pow<1, Function>> { typedef Function Type; };

// Règle: F^0 = 1
template <typename Function>
class Pow<0, Function> : public FunctionT<Pow<0, Function>> {
public:
  Pow(const Function &f) {}

public:
  operator One() { return One(); }
};
template <typename Function> struct Opt<Pow<0, Function>> { typedef One Type; };

// Règle: C * (F * G) = F * (G * C)
template <typename Function1, typename Function2>
class Mult<Constant, Mult<Function1, Function2>>
    : public FunctionT<Mult<Constant, Mult<Function1, Function2>>> {
public:
  Mult(const Constant &c, const Mult<Function1, Function2> &g)
      : m_c(c), m_f(g.m_f), m_g(g.m_g) {}

public:
  operator typename Opt<
      Mult<Function1, typename Opt<Mult<Function2, Constant>>::Type>>::Type() {
    return m_f * (m_g * m_c);
  }

public: /* Données internes */
  const Constant m_c;
  const Function1 m_f;
  const Function2 m_g;
};
template <typename Function1, typename Function2>
struct Opt<Mult<Constant, Mult<Function1, Function2>>> {
  typedef typename Opt<
      Mult<Function1, typename Opt<Mult<Function2, Constant>>::Type>>::Type Type;
};
// Règle: (F * G) * H = F * (G * H)
template <typename Function1, typename Function2, typename Function3>
class Mult<Mult<Function1, Function2>, Function3>
    : public FunctionT<Mult<Mult<Function1, Function2>, Function3>> {
public:
  Mult(const Mult<Function1, Function2> &f, const Function3 &g)
      : m_f(f.m_f), m_g(f.m_g), m_h(g) {}

public:
  operator typename Opt<
      Mult<Function1, typename Opt<Mult<Function2, Function3>>::Type>>::Type() {
    return m_f * (m_g * m_h);
  }

public: /* Données internes */
  const Function1 m_f;
  const Function2 m_g;
  const Function3 m_h;
};
template <typename Function1, typename Function2, typename Function3>
struct Opt<Mult<Mult<Function1, Function2>, Function3>> {
  typedef typename Opt<
      Mult<Function1, typename Opt<Mult<Function2, Function3>>::Type>>::Type Type;
};
// Règle: C * F = F * C
template <typename Function>
class Mult<Constant, Function> : public FunctionT<Mult<Constant, Function>> {
public:
  Mult(const Constant &c, Function g) : m_c(c), m_g(std::move(g)) {}

public:
  operator typename Opt<Mult<Function, Constant>>::Type() { return m_g * m_c; }

public: /* Données internes */
  const Constant m_c;
  const Function m_g;
};
template <typename Function> struct Opt<Mult<Constant, Function>> {
  typedef typename Opt<Mult<Function, Constant>>::Type Type;
};

#endif /* OPTIMIZE */

template <typename Function> void plot(const Function &f, const char *filename) {
  std::ofstream o(filename);
  const Real xmin = -1;
  const Real xmax = +1;
  const int n = (1u << 16u);

  Real sum = 0;
  const Real dx = (xmax - xmin) / n;
  for (int i = 0; i <= n; ++i) {
    const Real x = xmin + i * dx;
    sum += f(x);
    // o << x << ' ' << f(x) << '\n';
  }
  // std::cout << "Sum is " << sum << std::endl;
  // auto-close when out of scope
}

// C style function of f=(pow<4>(2*x+1)-pow<3>(x-1)/(1+x+pow<3>(x/2.)))
// extracted from the algebra.cc example
double fC(double x) {
  return (std::pow(((2) * (x)) + (1), 4)) +
         ((-1) * ((std::pow((x) + (-1), 3)) *
                  (1. / (((x) + (1)) + (std::pow((0.5) * (x), 3))))));
}

// C style function of d(f) extracted from the algebra.cc example
double dfC(double x) {
  return (((4) * (std::pow(((2) * (x)) + (1), 3))) *
          ((((0) * (x)) + ((2) * (1))) + (0))) +
         (((0) * ((std::pow((x) + (-1), 3)) *
                  (1. / (((x) + (1)) + (std::pow((0.5) * (x), 3)))))) +
          ((-1) *
           (((((3) * (std::pow((x) + (-1), 2))) * ((1) + (0))) *
             (1. / (((x) + (1)) + (std::pow((0.5) * (x), 3))))) +
            ((std::pow((x) + (-1), 3)) *
             (((-1) * (((1) + (0)) + (((3) * (std::pow((0.5) * (x), 2))) *
                                      (((0) * (x)) + ((0.5) * (1)))))) *
              (1. / (std::pow(((x) + (1)) + (std::pow((0.5) * (x), 3)), 2))))))));
}

// C style function of d(f) extracted from the algebraT.cpp example without
// OPTIMIZE
double dfC2(double x) {
  return ((4) *
          ((std::pow(((2) * (x)) + (1), 3)) * ((((2) * (1)) + ((0) * (x))) + (0)))) +
         (((-1) *
           (((std::pow((x) + (-1), 3)) *
             ((-1) * ((((1) + (0)) + ((3) * ((std::pow((x) * (0.5), 2)) *
                                             (((x) * (0)) + ((1) * (0.5)))))) *
                      (1. / ((((x) + (1)) + (std::pow((x) * (0.5), 3))) *
                             (((x) + (1)) + (std::pow((x) * (0.5), 3)))))))) +
            (((3) * ((std::pow((x) + (-1), 2)) * ((1) + (0)))) *
             (1. / (((x) + (1)) + (std::pow((x) * (0.5), 3))))))) +
          ((0) * ((std::pow((x) + (-1), 3)) *
                  (1. / (((x) + (1)) + (std::pow((x) * (0.5), 3)))))));
}

// C style function of d(f) extracted from the algebraT.cpp example with
// OPTIMIZE
double dfC3(double x) {
  return ((std::pow(((x) * (2)) + (1), 3)) * (8)) +
         (((std::pow((x) + (-1), 3)) *
           (((1) + ((std::pow((x) * (0.5), 2)) * (1.5))) *
            ((1. / ((((x) + (1)) + (std::pow((x) * (0.5), 3))) *
                    (((x) + (1)) + (std::pow((x) * (0.5), 3))))) *
             (1)))) +
          ((std::pow((x) + (-1), 2)) *
           ((1. / (((x) + (1)) + (std::pow((x) * (0.5), 3)))) * (-3))));
}

int main() {
  Identity x;

  auto g = sin(x);
  auto dg = d(g);
  std::cout << "g(1) = " << std::setw(8) << g(1.) << ";\t g'(1) = " << std::setw(8)
            << dg(1.) << " d(g) = " << d(g) << std::endl;
  auto h = pow<4>(x);
  std::cout << "h(2) = " << std::setw(8) << h(2) << ";\t h'(2) = " << std::setw(8)
            << d(h)(2.) << " d(h) = " << d(h) << std::endl;
  // auto f = (pow<4>(2 * x + 1) - pow<3>(x - 1) / (1 + x + pow<3>(x / 2.)));
  auto f = (sin(8 * x - 1)) / cos(x);
  std::cout << "f(2) = " << std::setw(8) << f(2) << ";\t f'(2) = " << std::setw(8)
            << d(f)(2.) << std::endl;
  std::cout << "  f  is " << f << std::endl;
  std::cout << "d(f) is " << d(f) << std::endl;

  std::cout << "Sizeofs : "
            << "\n\t  x=" << sizeof(x) << "\n\t  g=" << sizeof(g)
            << "\n\t dg=" << sizeof(dg) << "\n\t  h=" << sizeof(h)
            << "\n\tddh=" << sizeof(d(d(h))) << "\n\t  f=" << sizeof(f)
            << "\n\t df=" << sizeof(d(f)) << "\n";

  //   plot(f,"fT.dat");
  //   plot(d(f),"dfT.dat");

  std::cout << "type of f is " << type<decltype(f)>() << '\n';
  std::cout << "type of d(f) is " << type<decltype(d(f))>() << '\n';

  plot(fC, "fT.dat");
  plot(dfC3, "dfT.dat");
}