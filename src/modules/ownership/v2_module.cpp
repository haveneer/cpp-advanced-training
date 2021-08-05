export module v2_module;

int bar() { return 2; }

export namespace v2 {
int foo() { return bar(); }
} // namespace v2