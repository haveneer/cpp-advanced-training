export module preproc_citadel;

export int foo() {
#ifdef MACRO
  return -1;
#else
  return 1;
#endif
}