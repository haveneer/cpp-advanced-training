/***********************************************************************
 * algebra.cc : an example of function algebra with differenciation    *
 * This version uses virtual mechanism.                                *
 *                                                                     *
 * Author : Pascal Havé (hpwxf@haveneer.com)                           *
 ***********************************************************************/

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>

// Pré-déclaration de l'interface
class IFunction;

typedef double Real;

#include <memory>
using IFunctionPtr = std::shared_ptr<const IFunction>;

// Wrapper pour l'interface pour manipuler des objets sans s'occuper de
// l'allocation dynamique
class Function {
public:
  Function(const IFunction *f) : m_f{f} {}
  Function(IFunctionPtr f) : m_f(std::move(f)) {}
  virtual ~Function() = default;

public:
  operator IFunctionPtr() const { return m_f; }

public:
  Real operator()(const Real x) const;
  Function diff() const;
  std::ostream &print(std::ostream &o) const;
  int memory_size() const;

private:
  const IFunctionPtr m_f;
};

// Interface de fonctions
class IFunction {
public:
  virtual ~IFunction() = default;
  virtual Real eval(Real x) const = 0;
  virtual Function diff() const = 0;
  virtual std::ostream &print(std::ostream &o) const = 0;
  virtual int memory_size() const = 0;
};

// Identité
class Identity : public IFunction {
public:
  Identity() = default;
  Real eval(Real x) const final { return x; }
  Function diff() const final;
  std::ostream &print(std::ostream &o) const final {
    o << 'x';
    return o;
  }
  int memory_size() const final { return sizeof(*this); }
};

// Constante
class Constant : public IFunction {
public:
  explicit Constant(Real v) : m_v(v) {}
  Real eval(Real x) const final { return m_v; }
  Function diff() const final;
  std::ostream &print(std::ostream &o) const final {
    o << m_v;
    return o;
  }
  int memory_size() const final { return sizeof(*this); }

private:
  const Real m_v;
};

// Sinus
class Sinus : public IFunction {
public:
  Sinus(IFunctionPtr f) : m_f(std::move(f)) {}
  Real eval(Real x) const final { return std::sin(m_f->eval(x)); }
  Function diff() const final;
  std::ostream &print(std::ostream &o) const final {
    o << "sin(";
    m_f->print(o);
    o << ')';
    return o;
  }
  int memory_size() const final { return sizeof(*this) + m_f->memory_size(); }

private:
  const IFunctionPtr m_f;
};

// Cosinus
class Cosinus : public IFunction {
public:
  Cosinus(IFunctionPtr f) : m_f(std::move(f)) {}
  Real eval(Real x) const final { return std::cos(m_f->eval(x)); }
  Function diff() const final;
  std::ostream &print(std::ostream &o) const final {
    o << "cos(";
    m_f->print(o);
    o << ')';
    return o;
  }
  int memory_size() const final { return sizeof(*this) + m_f->memory_size(); }

private:
  const IFunctionPtr m_f;
};

// Addition
class Add : public IFunction {
public:
  Add(IFunctionPtr f, IFunctionPtr g) : m_f(std::move(f)), m_g(std::move(g)) {}
  Real eval(Real x) const final { return m_f->eval(x) + m_g->eval(x); }
  Function diff() const final;
  std::ostream &print(std::ostream &o) const final {
    o << '(';
    m_f->print(o);
    o << ") + (";
    m_g->print(o);
    o << ')';
    return o;
  }
  int memory_size() const final {
    return sizeof(*this) + m_f->memory_size() + m_g->memory_size();
  }

private:
  const IFunctionPtr m_f, m_g;
};

// Multiplication
class Mult : public IFunction {
public:
  Mult(IFunctionPtr f, IFunctionPtr g) : m_f(std::move(f)), m_g(std::move(g)) {}
  Real eval(Real x) const final { return m_f->eval(x) * m_g->eval(x); }
  Function diff() const final;
  std::ostream &print(std::ostream &o) const final {
    o << '(';
    m_f->print(o);
    o << ") * (";
    m_g->print(o);
    o << ')';
    return o;
  }
  int memory_size() const final {
    return sizeof(*this) + m_f->memory_size() + m_g->memory_size();
  }

private:
  const IFunctionPtr m_f, m_g;
};

// Inverse
class Inverse : public IFunction {
public:
  Inverse(IFunctionPtr f) : m_f(std::move(f)) {}
  Real eval(Real x) const final { return 1. / m_f->eval(x); }
  Function diff() const final;
  std::ostream &print(std::ostream &o) const final {
    o << "inv(";
    m_f->print(o);
    o << ")";
    return o;
  }
  int memory_size() const final { return sizeof(*this) + m_f->memory_size(); }

private:
  const IFunctionPtr m_f;
};

// Puissance
class Pow : public IFunction {
public:
  Pow(IFunctionPtr f, const int n) : m_f(std::move(f)), m_n(n) {
    if (n == 0)
      throw std::exception();
  }
  Real eval(Real x) const final { return std::pow(m_f->eval(x), m_n); }
  Function diff() const final;
  // std::ostream & print(std::ostream & o) const { o << '('; m_f->print(o); o
  // << ") ^" << m_n; return o; }
  std::ostream &print(std::ostream &o) const final {
    o << "std::pow(";
    m_f->print(o);
    o << "," << m_n << ")";
    return o;
  }
  int memory_size() const final { return sizeof(*this) + m_f->memory_size(); }

private:
  const IFunctionPtr m_f;
  const int m_n;
};

// Impl�mentation de l'op�rateur d'�valuation
Real Function::operator()(const Real x) const { return m_f->eval(x); }
Function Function::diff() const { return m_f->diff(); }
std::ostream &Function::print(std::ostream &o) const { return m_f->print(o); }
int Function::memory_size() const { return sizeof(*this) + m_f->memory_size(); }

// Quelques fonctions pour l'interface utilisateur
Function sin(const Function &f) { return Function(new Sinus(f)); }
Function cos(const Function &f) { return Function(new Cosinus(f)); }
// Des opérateurs primaires
Function operator+(const Function &f, const Function &g) { return new Add(f, g); }
Function operator*(const Function &f, const Function &g) { return new Mult(f, g); }
Function operator/(const Function &f, const Function &g) {
  return f * Function(new Inverse(g));
}
// Autres opérateurs associés
Function operator+(const Function &f, const Real &a) {
  return new Add(f, Function(new Constant(a)));
}
Function operator*(const Real &a, const Function &f) {
  return new Mult(Function(new Constant(a)), f);
}
Function operator/(const Real &a, const Function &f) {
  return a * Function(new Inverse(f));
}
Function operator-(const Function &f, const Real &a) {
  return new Add(f, Function(new Constant(-a)));
}
// Des opérations déduits
Function operator+(const Real &a, const Function &f) { return f + a; }
Function operator*(const Function &f, const Real &a) { return a * f; }
Function operator/(const Function &f, const Real &a) { return f * (1. / a); }
Function operator-(const Real &a, const Function &f) { return f - a; }
Function operator-(const Function &f) { return -1 * f; }
Function operator-(const Function &f, const Function &g) { return f + (-g); }
// Exemple d'optimisation sur l'opérateur ^
Function operator^(const Function &f,
                   const int &n) { // ATTN: opérateur de basse priorité
  if (n == 0)
    return new Constant(1);
  else if (n == 1)
    return f;
  else
    return new Pow(f, n);
}

// Les calculs de dérivées symboliques
Function Identity::diff() const { return new Constant(1); }
Function Constant::diff() const { return new Constant(0); }
Function Sinus::diff() const { return cos(m_f) * m_f->diff(); }
Function Cosinus::diff() const { return -sin(m_f) * m_f->diff(); }
Function Add::diff() const { return m_f->diff() + m_g->diff(); }
Function Mult::diff() const {
  return m_f->diff() * Function(m_g) + Function(m_f) * m_g->diff();
}
Function Inverse::diff() const { return -m_f->diff() / Function(new Pow(m_f, 2)); }
Function Pow::diff() const {
  // m_n != 0 par construction
  if (m_n == 1)
    return m_f->diff();
  else
    return m_n * (Function(m_f) ^ (m_n - 1)) * m_f->diff();
}

// Opérateur de dérivation
Function d(const Function &f) { return f.diff(); }

// Affichage d'une expression
std::ostream &operator<<(std::ostream &o, const Function &f) { return f.print(o); }

void plot(const Function &f, const char *filename) {
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
  std::cout << "Sum is " << sum << std::endl;
  // auto-close when out of scope
}

int main() {
  Function x(new Identity());

  Function g = sin(x);
  Function dg = d(g);
  std::cout << "g(1) = " << std::setw(8) << g(1) << ";\t g'(1) = " << std::setw(8)
            << dg(1) << std::endl;
  Function h = x ^ 4;
  std::cout << "h(2) = " << std::setw(8) << h(2) << ";\t h'(2) = " << std::setw(8)
            << d(d(h))(2.) << std::endl;
  // Function f = (sin(8*x-1)^3.) / cos(x^2.);
  Function f = (((2 * x + 1) ^ 4) - ((x - 1) ^ 3) / (1 + x + ((x / 2.) ^ 3)));
  std::cout << "f(2) = " << std::setw(8) << f(2) << ";\t f'(2) = " << std::setw(8)
            << d(f)(2.) << std::endl;
  std::cout << "  f  is " << f << std::endl;
  std::cout << "d(f) is " << d(f) << std::endl;

  std::cout << "Sizeofs : "
            << "\n\t  x=" << (x).memory_size() << "\n\t  g=" << (g).memory_size()
            << "\n\t dg=" << (dg).memory_size() << "\n\t  h=" << (h).memory_size()
            << "\n\tddh=" << (d(d(h))).memory_size()
            << "\n\t  f=" << (f).memory_size() << "\n\t df=" << (d(f)).memory_size()
            << "\n";

  plot(f, "f.dat");
  plot(d(f), "df.dat");
}