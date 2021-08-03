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
                # -fimplicit-module-maps -fimplicit-module-maps
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
        set_property(TEST ${name} PROPERTY LABELS "Modules")
    endfunction()

    add_module(helloworld_module modular_helloworld/helloworld_module.cpp)
    add_executable(modular_helloworld modular_helloworld/modular_helloworld.cpp modular_helloworld/helloworld_module.cpp)
    add_module_dependencies_and_test(modular_helloworld helloworld_module)

    add_module(helloworld_module2 modular_helloworld2/helloworld_module2.cpp)
    add_executable(helloworld2 modular_helloworld2/modular_helloworld2.cpp modular_helloworld2/helloworld_module2.cpp)
    add_module_dependencies_and_test(helloworld2 helloworld_module2)

    # fatal error: module 'std_config' is defined in both
    #    add_module(module_with_std_import conflicting_modules/module_with_std_import.cpp)
    #    add_executable(main_with_std_import conflicting_modules/main_with_std_import.cpp conflicting_modules/module_with_std_import.cpp)
    #    add_module_dependencies_and_test(main_with_std_import module_with_std_import)

else ()
    message(STATUS "\tC++20 Modules build not supported")
    string(TIMESTAMP DATETIME "%b %d %Y at %H:%M") # use strftime date format
    file(APPEND "${CMAKE_BINARY_DIR}/unsupported_examples.md" "* ${DATETIME} : cannot build modules\n")
endif ()