export module module2;

int bar() { return 2; }

export int foo2() { return bar(); }
export namespace ns_module2 {
int foo() { return bar(); }
} // namespace ns_module2
