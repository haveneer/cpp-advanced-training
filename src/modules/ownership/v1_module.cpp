export module v1_module;

int bar() { return 1; }

export namespace v1 {
int foo() { return bar(); }
} // namespace v1