export module module2;

int bar() { return 2; }

export namespace module2 {
int foo() { return bar(); }
} // namespace module2