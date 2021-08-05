import module_with_std_import;
import<iostream>; // import in main
                  // fatal error: module 'std_config' is defined in both
                  // https://github.com/clangd/clangd/issues/797
int main() {}
