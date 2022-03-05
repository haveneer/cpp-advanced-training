cmake_minimum_required(VERSION 3.12)
project(cpp-advanced-training)
option(ENABLE_STATIC_ANALYSIS "Enable static analysis" OFF)
option(BENCHMARK_ONLY "Only enables benchmark list" OFF)
option(SANITIZE "Enable sanitize feature" OFF) # could be THREAD or MEMORY
set(CPP_TRAINING_SOURCE_DIR ${PROJECT_SOURCE_DIR}) # to locate main root source in sub-projects

# This message also avoid warning : "Manually-specified variables were not used by the project"
message(STATUS "Using toolchain file: ${CMAKE_TOOLCHAIN_FILE}") # used for conan
message(STATUS "Current binary directory: ${CMAKE_BINARY_DIR}")

add_definitions(-DDEBUG_TYPE) # TODO could be replaced by using __has_include in .cpp
add_definitions(-DSAFE_RUN) # add workaround for explicitly buggy example (or on the edge behavior)

#------------------------------------------------------

if (ENABLE_STATIC_ANALYSIS)
    # search for clang-tidy (while be used while adding library or executable)
    find_program(CLANG_TIDY clang-tidy)
    if (CLANG_TIDY)
        set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY})
        message(STATUS "Static analysis enabled while compiling.")
    else ()
        message(FATAL_ERROR "Static analysis requested but not available.")
    endif ()
endif ()

#------------------------------------------------------

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -pedantic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused -Wno-unused-parameter")
    if (SANITIZE STREQUAL "THREAD")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
    elseif (SANITIZE STREQUAL "MEMORY")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    elseif (NOT SANITIZE STREQUAL "OFF")
        message(FATAL_ERROR "Bad SANITIZE value; should be THREAD, MEMORY or OFF")
    endif ()
    # To focus only on useful warnings 
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wunused-function -Wno-unused-variable -Wno-unused-parameter -Wno-unused-value -Wno-unused-result -Wno-unused-but-set-variable")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -pedantic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused -Wno-unused-parameter")
    if (SANITIZE STREQUAL "THREAD")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
    elseif (SANITIZE STREQUAL "MEMORY")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    elseif (NOT SANITIZE STREQUAL "OFF")
        message(FATAL_ERROR "Bad SANITIZE value; should be THREAD, MEMORY or OFF")
    endif ()
    # To focus only on useful warnings 
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-member-function -Wunused-private-field -Wno-unused-variable -Wno-unused-parameter -Wno-unused-value -Wno-unused-result -Wno-unused-but-set-variable")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /Zc:__cplusplus")
    if (NOT SANITIZE STREQUAL "OFF")
        message(FATAL_ERROR "SANITIZE feature not support with ${CMAKE_CXX_COMPILER_ID}")
    endif ()
elseif (CMAKE_CXX_COMPILER_ID MATCHES "PGI" OR CMAKE_CXX_COMPILER_ID MATCHES "NVHPC")
    set(PGI TRUE)
    if (NOT SANITIZE STREQUAL "OFF")
        message(FATAL_ERROR "SANITIZE feature not support with ${CMAKE_CXX_COMPILER_ID}")
    endif ()
endif ()


IF (PGI)
    include(CheckCXXCompilerFlag)
    # CMake 3.21 does not know the compile flags to use to enable it
    # look at /usr/share/cmake-3.*/Modules/Compiler/PGI-CXX.cmake
    set(CMAKE_CXX20_STANDARD_COMPILE_OPTION --std=c++20)
    check_cxx_compiler_flag(${CMAKE_CXX20_STANDARD_COMPILE_OPTION} HAS_PGI_CXX20_FLAG)
    if (HAS_PGI_CXX20_FLAG)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX20_STANDARD_COMPILE_OPTION}")
    else ()
        message(FATAL_ERROR "language dialect CXX20 not supported")
    endif ()
else ()
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif ()

#------------------------------------------------------

find_package(TBB QUIET)
if (TBB_FOUND)
    message(STATUS "TBB v${TBB_VERSION} found")
else ()
    message(STATUS "TBB not found")
endif ()

#------------------------------------------------------

if (NOT WIN32)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    link_libraries(Threads::Threads)
endif ()

#------------------------------------------------------
message(STATUS "Custom compiler feature checks")
# Check compiler support (when __cpp_* feature is not available)
include(CheckCXXSourceCompiles)

# https://en.cppreference.com/w/cpp/language/function_template#Abbreviated_function_template
check_cxx_source_compiles("auto f(auto a, auto b) { return a + b; } int main() { return f(1, -1); }"
        HAS_CXX_FEATURE_ABBREVIATED_FUNCTION_TEMPLATE)
if (HAS_CXX_FEATURE_ABBREVIATED_FUNCTION_TEMPLATE)
    ADD_COMPILE_DEFINITIONS(my_cpp_abbreviated_function_template)
endif ()

# https://en.cppreference.com/w/cpp/memory/shared_ptr/make_shared (in 4-Notes)
check_cxx_source_compiles("#include <memory>\n int main() { auto a = std::make_shared<int[10]>(); }"
        HAS_CXX_FEATURE_SHARED_PTR_ARRAYS)
if (HAS_CXX_FEATURE_SHARED_PTR_ARRAYS)
    ADD_COMPILE_DEFINITIONS(my_cpp_lib_shared_ptr_arrays)
endif ()

check_cxx_source_compiles("#include <chrono>\n int main() { std::chrono::year_month_day check_this; }"
        HAS_CXX_FEATURE_CHRONO_YMD)
if (HAS_CXX_FEATURE_CHRONO_YMD)
    ADD_COMPILE_DEFINITIONS(my_cpp_chrono_ymd)
endif ()

# Some charconv is incompletely implemented 
check_cxx_source_compiles("
#include <charconv>
int main() {
  char buffer[24];
  auto res = std::to_chars(buffer, buffer + sizeof(buffer), 1. / 3,
                           std::chars_format::general, 16);
  unsigned size = res.ptr - buffer;
}" HAS_CXX_FEATURE_TO_CHARS_ON_DOUBLE)
if (HAS_CXX_FEATURE_TO_CHARS_ON_DOUBLE)
    ADD_COMPILE_DEFINITIONS(my_cpp_to_chars_on_double)
endif ()

# __cpp_lib_atomic_wait is not well defined on AppleClang 12
check_cxx_source_compiles("
#include <atomic>
int main() {
  std::atomic<bool> v;
  v.wait(false);
  v.notify_one();
}" HAS_CXX_FEATURE_ATOMIC_WAIT)
if (HAS_CXX_FEATURE_ATOMIC_WAIT)
    ADD_COMPILE_DEFINITIONS(my_cpp_lib_atomic_wait)
endif ()

# work around for MSVC 19.29 which seems to have a bug when range loop depends on a structured binding values
check_cxx_source_compiles("
#include <vector>
struct S {
  int a;
  std::vector<int> v;
};
int main() {
  for (const auto &[a, v] = S{}; auto x : v);
}" HAS_CXX_FEATURE_RANGE_BASED_FOR_LOOP_WITH_STRUCTURED_BINDING_INIT)
if (HAS_CXX_FEATURE_RANGE_BASED_FOR_LOOP_WITH_STRUCTURED_BINDING_INIT)
    ADD_COMPILE_DEFINITIONS(my_cpp_feature_range_based_for_loop_with_structured_binding_init)
endif ()

check_cxx_source_compiles("
int main() {
  using T = decltype([] {});
}" HAS_CXX_FEATURE_LAMBDA_IN_UNEVALUATED_CONTEXT)
if (HAS_CXX_FEATURE_LAMBDA_IN_UNEVALUATED_CONTEXT)
    ADD_COMPILE_DEFINITIONS(my_cpp_feature_lambda_in_unevaluated_context)
endif ()

check_cxx_source_compiles("
int main() {
  if constexpr (requires { true; });
}" HAS_CXX_FEATURE_IF_CONSTEXPR_REQUIRES)
if (HAS_CXX_FEATURE_IF_CONSTEXPR_REQUIRES)
    ADD_COMPILE_DEFINITIONS(my_cpp_feature_if_constexpr_requires)
endif ()

check_cxx_source_compiles("
template <class T> T::R f1();
int main() { }
" HAS_CXX_FEATURE_LESS_TYPENAME)
if (HAS_CXX_FEATURE_LESS_TYPENAME)
    ADD_COMPILE_DEFINITIONS(my_cpp_feature_less_typename)
endif ()

# Check good support of span
# better than __cpp_lib_span with clang++-14
check_cxx_source_compiles("
#include <string>
#include <vector>
#include <span>

int main() {
  std::vector v1 = {1, 2, 3, 4};
  std::span sp1{v1};                          
  std::string v3{\"1234\"};
  std::span sp3{v3};
}
" HAS_CXX_GOOD_SPAN_SUPPORT)
if (HAS_CXX_GOOD_SPAN_SUPPORT)
    ADD_COMPILE_DEFINITIONS(my_cpp_feature_good_span_support)
endif ()

#------------------------------------------------------

add_executable(manage_test_crash tools/manage_test_crash.c)

message(STATUS "Detect file to compile using globbing")
file(GLOB_RECURSE SRCS1 RELATIVE "${CMAKE_SOURCE_DIR}" "src/*.cpp")
# Modules examples use a dedicated configuration
file(GLOB_RECURSE MODULE_SRCS RELATIVE "${CMAKE_SOURCE_DIR}" "src/modules/*.cpp")
list(REMOVE_ITEM SRCS1 ${MODULE_SRCS})
file(GLOB_RECURSE SRCS2 RELATIVE "${CMAKE_SOURCE_DIR}" "Solutions/*.cpp")
file(GLOB_RECURSE SRCS3 RELATIVE "${CMAKE_SOURCE_DIR}" "Curiosity/*.cpp")
file(GLOB_RECURSE SRCS4 RELATIVE "${CMAKE_SOURCE_DIR}" "extras/*.cpp")
file(GLOB_RECURSE SRCS5 RELATIVE "${CMAKE_SOURCE_DIR}" "gtest/*--unittest.cpp")
set(SRCS ${SRCS1} ${SRCS2} ${SRCS3} ${SRCS4} ${SRCS5})

include_directories(include)

#------------------------------------------------------

message(STATUS "Googletest configuration")
# to enable shared lib in WIN32
# cf    https://github.com/google/googletest/tree/master/googletest#incorporating-into-an-existing-cmake-project
#   and https://stackoverflow.com/questions/12540970/how-to-make-gtest-build-mdd-instead-of-mtd-by-default-using-cmake
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) # 
add_subdirectory(third_party/googletest)

#------------------------------------------------------

message(STATUS "Google benchmark configuration")
OPTION(BENCHMARK_ENABLE_TESTING "" OFF)
add_subdirectory(third_party/benchmark)

#------------------------------------------------------

if (NOT PGI)
    message(STATUS "FMT configuration")
    add_subdirectory(third_party/fmt)
    link_libraries(fmt::fmt)
endif ()

#------------------------------------------------------

set(EXCLUDED)
if (MSVC)
    list(APPEND EXCLUDED src/ranges/range_adaptors1.cpp src/ranges/range_filter_gcc_expanded.cpp src/ranges/range_algorithm.cpp) # FIXME should be fixed for MVSC 19.29
    if (DEFINED ENV{GITHUB_ACTIONS})
        # infinite execution loop on GHA (but Ok with TravisCI ?)
        list(APPEND EXCLUDED src/concepts/concept_setup.cpp)
        list(APPEND EXCLUDED src/coroutines/inlined-coroutines.cpp) # Execution failure; should be debugged later
        list(APPEND EXCLUDED src/coroutines/async--unittest.cpp) # Strange timeout; should be debugged later
        list(APPEND EXCLUDED src/format/minimal-custom-formatter.cpp) # internal lib error (with MSVC 19.29)
        list(APPEND EXCLUDED src/format/format-other-functions.cpp) # timeout with MSVC 19.29
        list(APPEND EXCLUDED src/pmr/monotonic--unittest.cpp) # fatal error C1001: Internal compiler (MSVC 19.29)
    endif ()
    if (NOT DONT_VS_EXCLUDE)
        list(APPEND EXCLUDED)
    endif ()
endif ()

if (PGI)
    # compile time errors (about lib::fmt)
    file(GLOB_RECURSE _SRCS RELATIVE "${CMAKE_SOURCE_DIR}" "src/format/*.cpp")
    list(APPEND EXCLUDED ${_SRCS} extras/format-dynamic_translate.cpp)
    # other files
    list(APPEND EXCLUDED
            src/concurrency/combinatory_result.cpp
            src/concepts/concept_usage_showcase.cpp
            src/concepts/legal_constraints.cpp
            src/concepts/case_disjunction.cpp
            )
    # dev WIP files
    list(APPEND EXCLUDED
            src/concurrency/combinatory_result2.cpp
            )
    # runtime errors
    list(APPEND EXCLUDED
            src/parallel-stl/parallel_insight.cpp
            src/spaceship/spaceship_migration_issue1.cpp
            src/spaceship/spaceship_migration_issue2.cpp
            )
endif ()
#------------------------------------------------------

set(BENCHMARK_LIST)
list(APPEND BENCHMARK_LIST
        src/check_support__full_display.cpp
        src/concurrency/incrementation--benchmark.cpp
        src/concurrency/pingpong--benchmark.cpp
        src/coroutines/concurrency--benchmark.cpp
        src/format/conversion--benchmark.cpp
        src/coroutines/massive-usage.cpp
        src/parallel-stl/parallel_insight.cpp
        src/parallel-stl/sort_executions.cpp
        src/pmr/container--benchmark.cpp
        src/source_location.cpp # just for testing MSVC output
        Solutions/parallel_word_count_bench--benchmark.cpp
        src/pmr/monotonic--unittest.cpp
        src/pmr/pool--unittest.cpp
        )

# Excluded for thread sanitizer
set(SANITIZE_THREAD_EXCLUSION_LIST
        src/concurrency/combinatory_result2.cpp # local test and too long
        src/concurrency/incrementation--benchmark.cpp # contains intentional unsafe operations
        src/coroutines/async--unittest.cpp    # FIXME need check
        src/parallel-stl/parallel_insight.cpp # pb inside TBB
        src/parallel-stl/sort_executions.cpp  # pb inside TBB
        src/parallel-stl/transform.cpp        # pb inside TBB
        src/parallel-stl/trivial_parallel.cpp # pb inside TBB
        Solutions/parallel_word_count_bench--benchmark.cpp # pb inside TBB
        Solutions/parallel_word_count_test--unittest.cpp   # pb inside TBB
        )

# Excluded for memory sanitizer
set(SANITIZE_MEMORY_EXCLUSION_LIST
        src/concurrency/combinatory_result2.cpp # local test and too long
        src/analysis/dangling.cpp        # contains intentional unsafe operations 
        extras/nodiscard.cpp             # FIXME need check
        src/coroutines/massive-usage.cpp # FIXME need check
        src/parallel-stl/parallel_insight.cpp # pb inside TBB
        src/parallel-stl/sort_executions.cpp  # pb inside TBB
        src/parallel-stl/transform.cpp        # pb inside TBB
        src/parallel-stl/trivial_parallel.cpp # pb inside TBB
        Solutions/parallel_word_count_bench--benchmark.cpp # pb inside TBB
        Solutions/parallel_word_count_test--unittest.cpp   # pb inside TBB
        )

#------------------------------------------------------

enable_testing()
foreach (SRC ${SRCS})
    list(FIND EXCLUDED ${SRC} IS_EXCLUDED)
    list(FIND BENCHMARK_LIST ${SRC} IS_BENCHMARK)
    list(FIND SANITIZE_THREAD_EXCLUSION_LIST ${SRC} IS_SANITIZE_THREAD_EXCLUDED)
    list(FIND SANITIZE_MEMORY_EXCLUSION_LIST ${SRC} IS_SANITIZE_MEMORY_EXCLUDED)
    if (IS_EXCLUDED GREATER_EQUAL 0)
        message(STATUS "\tExclude example ${SRC}")
    elseif (BENCHMARK_ONLY AND IS_BENCHMARK LESS 0)
        # message(STATUS "\tSkip non benchmark example ${SRC}")
    elseif (SANITIZE STREQUAL "THREAD" AND IS_SANITIZE_THREAD_EXCLUDED GREATER_EQUAL 0)
        message(STATUS "\tSkip thread sanitize excluded example ${SRC}")
    elseif (SANITIZE STREQUAL "MEMORY" AND IS_SANITIZE_MEMORY_EXCLUDED GREATER_EQUAL 0)
        message(STATUS "\tSkip memory sanitize excluded example ${SRC}")
    else ()
        set(SRCS ${SRC})

        set(TYPE "example")
        string(REGEX MATCH "--unittest\\.cpp$" NEED_GTEST ${SRC})
        if (NEED_GTEST)
            set(TYPE "google test")
            string(REGEX REPLACE "--unittest\\.cpp$" "" TEST ${SRC})
            file(GLOB GTEST_EXTRAS_PATH RELATIVE "${CMAKE_SOURCE_DIR}" "${TEST}.*")
        else ()
            string(REGEX MATCH "--benchmark\\.cpp$" NEED_GBENCH ${SRC})
            if (NEED_GBENCH)
                set(TYPE "google benchmark")
                string(REGEX REPLACE "--benchmark\\.cpp$" "" EXTRA_SRC_BASE ${SRC})
                file(GLOB GTEST_EXTRAS_PATH RELATIVE "${CMAKE_SOURCE_DIR}" "${EXTRA_SRC_BASE}.*")
                set(TEST "${EXTRA_SRC_BASE}--benchmark")
            else ()
                string(REGEX MATCH "--.*\\.cpp$" SKIP_COMPANION ${SRC})
                if (SKIP_COMPANION)
                    continue()
                else ()
                    string(REGEX REPLACE ".cpp$" "" TEST ${SRC})
                endif ()
            endif ()
        endif ()
        file(GLOB EXTRAS_PATH RELATIVE "${CMAKE_SOURCE_DIR}" "${TEST}--*.*")

        string(REGEX MATCH "/parallel" NEED_PARALLEL ${SRC}) # in directory or filename
        if (NEED_PARALLEL)
            set(TYPE "parallel example")
        endif ()

        set(EXTRAS)
        foreach (S ${EXTRAS_PATH} ${GTEST_EXTRAS_PATH})
            if (NOT S IN_LIST SRCS)
                get_filename_component(EXTRA ${S} NAME)
                list(APPEND SRCS ${S})
                list(APPEND EXTRAS ${EXTRA})
            endif ()
        endforeach ()

        if (EXTRAS)
            message(STATUS "\tAdd ${TYPE} ${SRC} with extras: ${EXTRAS}")
        else ()
            message(STATUS "\tAdd ${TYPE} ${SRC}")
        endif ()

        string(REGEX REPLACE "\\.cpp$" "" EXE_PATH ${SRC})
        string(REGEX REPLACE "^src/" "" EXE_PATH ${EXE_PATH})
        get_filename_component(EXE ${EXE_PATH} NAME)

        add_executable(${EXE} ${SRCS})
        set_target_properties(${EXE} PROPERTIES SUFFIX .exe)
        # OUTPUT_NAME on Windows cannot be a full path
        #   https://gitlab.kitware.com/cmake/cmake/-/issues/22133
        # make generator does not create full path 
        # if (WIN32)
        string(REPLACE "/" "__" EXE_PATH ${EXE_PATH})
        # endif()
        set_target_properties(${EXE} PROPERTIES OUTPUT_NAME ${EXE_PATH})

        string(REGEX REPLACE "^src/" "" TEST ${TEST})
        set(WILL_FAIL_FOUND 0)
        string(FIND ${SRC} "-will_fail" WILL_FAIL_FOUND)
        if (${WILL_FAIL_FOUND} EQUAL "-1")
            add_test(NAME ${TEST} COMMAND ${EXE})
        else ()
            add_test(NAME ${TEST} COMMAND manage_test_crash ${EXE})
            set_tests_properties(${TEST} PROPERTIES WILL_FAIL TRUE)
        endif ()

        if (NEED_GBENCH)
            target_link_libraries(${EXE} benchmark::benchmark)
            target_include_directories(${EXE} PUBLIC ${benchmark_SOURCE_DIR}/include ${benchmark_SOURCE_DIR})
            set(NEED_GTEST 1)
        endif ()
        if (NEED_GTEST)
            target_link_libraries(${EXE} gtest gtest_main gmock)
            target_include_directories(${EXE} PUBLIC ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
            target_include_directories(${EXE} PUBLIC ${gmock_SOURCE_DIR}/include ${gmock_SOURCE_DIR})
            set_property(TEST ${TEST} PROPERTY LABELS "GTest")
        endif ()
        if (NEED_PARALLEL)
            if (TBB_FOUND)
                target_compile_definitions(${EXE} PUBLIC WITH_TBB)
                target_link_libraries(${EXE} TBB::tbb)
            else ()
                message(WARNING "Parallel TBB required but not found")
            endif ()
        endif ()
        if (IS_BENCHMARK)
            set_property(TEST ${TEST} PROPERTY LABELS "Benchmark")
        endif ()
    endif ()
endforeach ()

message(STATUS "Special processing for module examples")
add_subdirectory(src/modules)