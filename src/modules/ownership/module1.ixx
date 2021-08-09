export module module1;

int bar() { return 1; }

export int foo1() { return bar(); }
export namespace ns_module1 {
int foo() { return bar(); }
} // namespace ns_module1