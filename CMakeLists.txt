cmake_minimum_required(VERSION 3.12)
project(cpp-advanced-training)
option(ENABLE_STATIC_ANALYSIS "Enable static analysis" OFF)
set(CPP_TRAINING_SOURCE_DIR ${PROJECT_SOURCE_DIR}) # to locate main root source in sub-projects

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-DDEBUG_TYPE)
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

# to enable shared lib in WIN32
# cf    https://github.com/google/googletest/tree/master/googletest#incorporating-into-an-existing-cmake-project
#   and https://stackoverflow.com/questions/12540970/how-to-make-gtest-build-mdd-instead-of-mtd-by-default-using-cmake
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) # 

add_subdirectory(third_party/googletest)

#------------------------------------------------------

add_executable(manage_test_crash tools/manage_test_crash.c)

message(STATUS "Detect file to compile using globbing")
file(GLOB SRCS1 RELATIVE "${CMAKE_SOURCE_DIR}" "src/*.cpp")
file(GLOB SRCS2 RELATIVE "${CMAKE_SOURCE_DIR}" "Solutions/*.cpp")
file(GLOB SRCS3 RELATIVE "${CMAKE_SOURCE_DIR}" "Curiosity/*.cpp")
file(GLOB SRCS4 RELATIVE "${CMAKE_SOURCE_DIR}" "extras/*.cpp")
file(GLOB SRCS5 RELATIVE "${CMAKE_SOURCE_DIR}" "gtest/*--unittest.cpp")
file(GLOB SRCS6 RELATIVE "${CMAKE_SOURCE_DIR}" "gtest/samples/*--unittest.cpp")
set(SRCS ${SRCS1} ${SRCS2} ${SRCS3} ${SRCS4} ${SRCS5} ${SRCS6})

include_directories(include)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -pedantic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused -Wno-unused-parameter")
    # To focus only on useful warnings 
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wunused-function -Wno-unused-variable -Wno-unused-parameter -Wno-unused-value -Wno-unused-result -Wno-unused-but-set-variable")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -pedantic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused -Wno-unused-parameter")
    # To focus only on useful warnings 
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-member-function -Wunused-private-field -Wno-unused-variable -Wno-unused-parameter -Wno-unused-value -Wno-unused-result -Wno-unused-but-set-variable")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /Zc:__cplusplus")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "PGI")
    set(PGI TRUE)
    set(CMAKE_CXX_STANDARD 17) # C++20 requires an updated compiler (not yet available) 
endif ()

if (NOT WIN32)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    link_libraries(Threads::Threads)
endif()

set(EXCLUDED)
if (MSVC)
    list(APPEND EXCLUDED)
    if (NOT DONT_VS_EXCLUDE)
        list(APPEND EXCLUDED)
    endif ()
endif ()

if (PGI)
    # compile time errors
    list(APPEND EXCLUDED)
    # runtime errors
    list(APPEND EXCLUDED)
endif()


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
            string(REGEX MATCH "--.*\\.cpp$" SKIP_COMPANION ${SRC})
            if (SKIP_COMPANION)
                continue()
            else ()
                string(REGEX REPLACE ".cpp$" "" TEST ${SRC})
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

        if (NEED_GTEST)
            target_link_libraries(${EXE} gtest gtest_main gmock)
            target_include_directories(${EXE} PUBLIC ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
            target_include_directories(${EXE} PUBLIC ${gmock_SOURCE_DIR}/include ${gmock_SOURCE_DIR})
            set_property(TEST ${TEST} PROPERTY LABELS "GTest")
        endif ()
    endif ()
endforeach ()
