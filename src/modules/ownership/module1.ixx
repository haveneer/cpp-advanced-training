export module module1;

int bar() { return 1; }

export namespace module1 {
int foo() { return bar(); }
} // namespace module1