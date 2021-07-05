cmake_minimum_required(VERSION 3.12)
project(cpp-advanced-training)
option(ENABLE_STATIC_ANALYSIS "Enable static analysis" OFF)
set(CPP_TRAINING_SOURCE_DIR ${PROJECT_SOURCE_DIR}) # to locate main root source in sub-projects

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
        HAS_CXX_FEATURE_ABBREVIATED_FUNCTION_TEMPLATE)
if (HAS_CXX_FEATURE_ABBREVIATED_FUNCTION_TEMPLATE)
    ADD_COMPILE_DEFINITIONS(my_cpp_abbreviated_function_template)
endif ()

check_cxx_source_compiles("#include <chrono>\n int main() { std::chrono::year_month_day check_this; }"
        HAS_CXX_FEATURE_CHRONO_YMD)
if (HAS_CXX_FEATURE_CHRONO_YMD)
    ADD_COMPILE_DEFINITIONS(my_cpp_chrono_ymd)
endif ()

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

#------------------------------------------------------

add_executable(manage_test_crash tools/manage_test_crash.c)

message(STATUS "Detect file to compile using globbing")
file(GLOB_RECURSE SRCS1 RELATIVE "${CMAKE_SOURCE_DIR}" "src/*.cpp")
file(GLOB_RECURSE SRCS2 RELATIVE "${CMAKE_SOURCE_DIR}" "Solutions/*.cpp")
file(GLOB_RECURSE SRCS3 RELATIVE "${CMAKE_SOURCE_DIR}" "Curiosity/*.cpp")
file(GLOB_RECURSE SRCS4 RELATIVE "${CMAKE_SOURCE_DIR}" "extras/*.cpp")
file(GLOB_RECURSE SRCS5 RELATIVE "${CMAKE_SOURCE_DIR}" "gtest/*--unittest.cpp")
set(SRCS ${SRCS1} ${SRCS2} ${SRCS3} ${SRCS4} ${SRCS5})

include_directories(include)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -pedantic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused -Wno-unused-parameter")
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    # To focus only on useful warnings 
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wunused-function -Wno-unused-variable -Wno-unused-parameter -Wno-unused-value -Wno-unused-result -Wno-unused-but-set-variable")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -pedantic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused -Wno-unused-parameter")
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    # To focus only on useful warnings 
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-member-function -Wunused-private-field -Wno-unused-variable -Wno-unused-parameter -Wno-unused-value -Wno-unused-result -Wno-unused-but-set-variable")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /Zc:__cplusplus")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "PGI")
    set(PGI TRUE)
    set(CMAKE_CXX_STANDARD 17) # C++20 requires an updated compiler (not yet available) 
endif ()

#------------------------------------------------------

# to enable shared lib in WIN32
# cf    https://github.com/google/googletest/tree/master/googletest#incorporating-into-an-existing-cmake-project
#   and https://stackoverflow.com/questions/12540970/how-to-make-gtest-build-mdd-instead-of-mtd-by-default-using-cmake
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) # 

add_subdirectory(third_party/googletest)
OPTION(BENCHMARK_ENABLE_TESTING "" OFF)
add_subdirectory(third_party/benchmark)

#------------------------------------------------------

add_subdirectory(third_party/fmt)
link_libraries(fmt::fmt)

#------------------------------------------------------

if (NOT WIN32)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    link_libraries(Threads::Threads)
endif ()

set(EXCLUDED)
if (MSVC)
    if (DEFINED ENV{GITHUB_ACTIONS})
        # infinite execution loop on GHA (but Ok with TravisCI ?)
        list(APPEND EXCLUDED src/concepts/concept_setup.cpp)
        list(APPEND EXCLUDED src/ranges/range_adaptors1.cpp src/ranges/range_algorithm.cpp src/ranges/range_filter_gcc_expanded.cpp) # FIXME should be fixed for MVSC 19.29
        list(APPEND EXCLUDED src/coroutines/inlined-coroutines.cpp) # Execution failure; should be debugged later
        list(APPEND EXCLUDED src/coroutines/async_server.cpp) # Strange timeout; should be debugged later
        list(APPEND EXCLUDED src/format/minimal-custom-formatter.cpp) # internal lib error (with MSVC 19.29)
        list(APPEND EXCLUDED src/format/format-other-functions.cpp) # timeout with MSVC 19.29
    endif ()
    if (NOT DONT_VS_EXCLUDE)
        list(APPEND EXCLUDED)
    endif ()
endif ()

if (PGI)
    # compile time errors
    list(APPEND EXCLUDED)
    # runtime errors
    list(APPEND EXCLUDED)
endif ()

enable_testing()
foreach (SRC ${SRCS})
    get_filename_component(SHORT_SRC ${SRC} NAME)
    string(REGEX REPLACE ".cpp$" ".exe" EXE ${SHORT_SRC})
    list(FIND EXCLUDED ${SRC} IS_EXCLUDED)
    if (IS_EXCLUDED GREATER_EQUAL 0)
        message(STATUS "\tExclude example ${SRC}")
    else ()
        set(SRCS ${SRC})

        set(TYPE "example")
        string(REGEX MATCH "--unittest\\.cpp$" NEED_GTEST ${SRC})
        if (NEED_GTEST)
            set(TYPE "google test")
            string(REGEX REPLACE "--unittest.cpp$" "" TEST ${SRC})
            file(GLOB GTEST_EXTRAS_PATH RELATIVE "${CMAKE_SOURCE_DIR}" "${TEST}.*")
        else ()
            string(REGEX MATCH "--benchmark\\.cpp$" NEED_GBENCH ${SRC})
            if (NEED_GBENCH)
                set(TYPE "google benchmark")
                string(REGEX REPLACE "--benchmark.cpp$" "" TEST ${SRC})
                file(GLOB GTEST_EXTRAS_PATH RELATIVE "${CMAKE_SOURCE_DIR}" "${TEST}.*")
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

        add_executable(${EXE} ${SRCS})

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
    endif ()
endforeach ()
