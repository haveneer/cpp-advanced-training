if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")

    set(CMAKE_CXX_EXTENSIONS OFF)
    set(PREBUILT_MODULE_PATH ${CMAKE_BINARY_DIR}/modules)
    function(add_module name)
        file(MAKE_DIRECTORY ${PREBUILT_MODULE_PATH})
        separate_arguments(CXX_FLAGS UNIX_COMMAND "${CMAKE_CXX_FLAGS}")
        add_custom_target(${name}.pcm
                COMMAND
                ${CMAKE_CXX_COMPILER}
                ${CXX_FLAGS}
                -std=c++20
                -stdlib=libc++
                -pthread # due to enabled pthread
                -fmodules
                -c
                ${CMAKE_CURRENT_SOURCE_DIR}/${ARGN}
                -Xclang -emit-module-interface
                -o ${PREBUILT_MODULE_PATH}/${name}.pcm
                COMMENT "Build module ${name}"
                )
    endfunction()

    function(add_module_dependencies_and_test name)
        add_dependencies(${name} ${ARGN}.pcm)
        target_compile_options(${name} PRIVATE -fmodules)
        target_compile_options(${name} PRIVATE -fbuiltin-module-map)
        target_compile_options(${name} PRIVATE -fimplicit-module-maps)
        target_compile_options(${name} PRIVATE -fprebuilt-module-path=${PREBUILT_MODULE_PATH})
        message(STATUS "\tAdd module exemple ${name}")
        add_test(NAME ${name} COMMAND ${name})
    endfunction()

    add_module(helloworld4 helloworld4.cpp)
    add_executable(main4 main4.cpp helloworld4.cpp)
    add_module_dependencies_and_test(main4 helloworld4)

    add_module(helloworld3 helloworld3.cpp)
    add_executable(main3 main3.cpp helloworld3.cpp)
    add_module_dependencies_and_test(main3 helloworld3)

    # fatal error: module 'std_config' is defined in both
    #    add_module(greetings2 greetings2.cpp)
    #    add_executable(main2 main2.cpp greetings2.cpp)
    #    add_module_dependencies_and_test(main2 greetings2)

    # fatal error: module 'std_config' is defined in both
    #    add_module(hello1 hello1.cpp)
    #    add_executable(main1 main1.cpp hello1.cpp)
    #    add_module_dependencies_and_test(main1 hello1)
else ()
    message(WARNING "C++20 Modules build not supported")
endif ()