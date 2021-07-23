template <typename T1, typename T2> // main declaration
struct Converter {                  // for any two types T1 and T2
  Converter() = default;
  T2 convert(const T1 &t1);
};

template <typename T1, typename T2>           // default implementation
T2 Converter<T1, T2>::convert(const T1 &t1) { //
  //#region [implementation]
  return t1;
  //#endregion
}

template <typename T>    // specialization for same types
struct Converter<T, T> { //
  Converter() = default;
  T convert(const T &t) { return t; }
};

int main() {
  Converter<int, double> int_to_double;
  int_to_double.convert(1);

  Converter<int, int> int_to_int;
  int_to_int.convert(1);
}