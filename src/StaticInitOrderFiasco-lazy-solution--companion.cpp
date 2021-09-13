int square(int n) { // a function
  return n * n;
}

int &staticCompanion() {
  static auto private_static_data = square(5); // will be init at first call
  return private_static_data;
}