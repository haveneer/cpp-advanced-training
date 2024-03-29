# For a portable module configuration, follow https://gitlab.kitware.com/cmake/cmake/-/issues/18355
set(TEST_MODULE_PREFIX "src/modules")

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
        AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "11.0.0"
        AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "12.0.0")
    set(HAS_CXX_MODULES_SUPPORT TRUE)

    set(CMAKE_CXX_EXTENSIONS OFF)
    # set_property(GLOBAL PROPERTY GLOBAL_DEPENDS_DEBUG_MODE 1)

    function(add_module_executable name)
        # GCC requires to compile all at once (AFAIK)
        separate_arguments(CXX_FLAGS UNIX_COMMAND "${CMAKE_CXX_FLAGS}")
        list(TRANSFORM ARGN PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
        add_custom_target(${name} ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${name}.exe)
        add_custom_command(
                OUTPUT
                ${CMAKE_CURRENT_BINARY_DIR}/${name}.exe
                COMMAND
                ${CMAKE_CXX_COMPILER} ${CXX_FLAGS}
                -I ${CMAKE_SOURCE_DIR}/include
                -std=c++20 -fmodules-ts
                -x c++ ${ARGN} -o ${CMAKE_CURRENT_BINARY_DIR}/${name}.exe
                DEPENDS ${ARGN}
                COMMENT "Build modular executable ${name}"
        )
        message(STATUS "\tAdd module example ${TEST_MODULE_PREFIX}/${name}")
        add_test(NAME ${TEST_MODULE_PREFIX}/${name} COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${name}.exe)
        set_property(TEST ${TEST_MODULE_PREFIX}/${name} PROPERTY LABELS "Modules")
    endfunction()

    function(add_modules name)
        # Do nothing for GCC (done in 'all at once' compilation)
    endfunction()

    function(add_system_modules name)
        separate_arguments(CXX_FLAGS UNIX_COMMAND "${CMAKE_CXX_FLAGS}")
        foreach (mod ${ARGN})
            if (NOT TARGET ${mod}.gcm)
                file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${mod}_finder.h "#include <${mod}>\n")
                execute_process( # use special gcc option : -H (https://gcc.gnu.org/onlinedocs/gcc/Preprocessor-Options.html)
                        COMMAND ${CMAKE_CXX_COMPILER} "-H" "${CMAKE_CURRENT_BINARY_DIR}/${mod}_finder.h"
                        RESULT_VARIABLE _RESULT
                        ERROR_VARIABLE mod_path
                        )
                if (NOT _RESULT EQUAL 0)
                    message(FATAL_ERROR "Cannot locate system header <${mod}>")
                endif ()
                string(REGEX MATCH "[^\n]*" mod_path ${mod_path})
                string(SUBSTRING ${mod_path} 2 -1 mod_path)
                add_custom_target(${mod}.gcm DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/gcm.cache/${mod_path}.gcm")
                add_custom_command(
                        OUTPUT
                        ${CMAKE_CURRENT_BINARY_DIR}/gcm.cache/${mod_path}.gcm
                        COMMAND
                        ${CMAKE_CXX_COMPILER} ${CXX_FLAGS}
                        -std=c++20 -fmodules-ts
                        -x c++-system-header ${mod}
                        DEPENDS ${mod_path}
                        COMMENT "Build system module ${mod} for target ${name}"
                )
            endif ()
            add_dependencies(${name} ${mod}.gcm)
        endforeach ()
    endfunction()

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(HAS_CXX_MODULES_SUPPORT TRUE)

    set(CMAKE_CXX_EXTENSIONS OFF)
    set(PREBUILT_MODULE_PATH ${CMAKE_BINARY_DIR}/modules)

    function(add_module_executable name)
        add_executable(${name}.exe ${ARGN})
        target_compile_options(${name}.exe PRIVATE -fmodules)
        target_compile_options(${name}.exe PRIVATE -fbuiltin-module-map)
        target_compile_options(${name}.exe PRIVATE -fimplicit-module-maps)
        target_compile_options(${name}.exe PRIVATE -fprebuilt-module-path=${PREBUILT_MODULE_PATH})
        target_compile_options(${name}.exe PRIVATE -x c++) # to help .ixx file to be allowed
        target_include_directories(${name}.exe PRIVATE ${CMAKE_SOURCE_DIR}/include)
        message(STATUS "\tAdd module example ${TEST_MODULE_PREFIX}/${name}")
        add_test(NAME ${TEST_MODULE_PREFIX}/${name} COMMAND ${name}.exe)
        set_property(TEST ${TEST_MODULE_PREFIX}/${name} PROPERTY LABELS "Modules")
    endfunction()

    function(add_modules name)
        file(MAKE_DIRECTORY ${PREBUILT_MODULE_PATH})
        separate_arguments(CXX_FLAGS UNIX_COMMAND "${CMAKE_CXX_FLAGS}")
        foreach (mod ${ARGN})
            string(REGEX REPLACE "[^A-Za-z0-9_.]" "_" mod_target ${mod})
            get_filename_component(modfile ${mod} NAME_WLE)
            add_custom_target(${mod_target} DEPENDS "${PREBUILT_MODULE_PATH}/${modfile}.pcm")
            add_custom_command(
                    OUTPUT
                    ${PREBUILT_MODULE_PATH}/${modfile}.pcm
                    COMMAND
                    ${CMAKE_CXX_COMPILER} ${CXX_FLAGS}
                    -I ${CMAKE_SOURCE_DIR}/include
                    -std=c++20 -stdlib=libc++ -fmodules
                    -pthread # due to enabled pthread
                    -c -x c++ ${CMAKE_CURRENT_SOURCE_DIR}/${mod}
                    -Xclang -emit-module-interface
                    -fprebuilt-module-path=${PREBUILT_MODULE_PATH}
                    -o ${PREBUILT_MODULE_PATH}/${modfile}.pcm
                    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${mod}
                    COMMENT "Build module ${mod} for target ${name}"
            )
            add_dependencies(${name}.exe ${mod_target})
            set_source_files_properties(${mod} PROPERTIES LANGUAGE CXX)
        endforeach ()
    endfunction()

    function(add_system_modules name)
        # Do nothing for Clang
    endfunction()

endif ()

if (HAS_CXX_MODULES_SUPPORT)

    # Use add_module_executable with all module interface (.ixx) before .cpp's

    add_module_executable(helloworld helloworld/helloworld.ixx helloworld/helloworld.cpp)
    add_modules(helloworld helloworld/helloworld.ixx)
    add_system_modules(helloworld iostream)

    if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # GCC => error: sorry, unimplemented: private module fragment
        add_module_executable(helloworld2 helloworld2/helloworld2.ixx helloworld2/helloworld2.cpp)
        add_modules(helloworld2 helloworld2/helloworld2.ixx)
        add_system_modules(helloworld2 iostream)
    endif ()

    if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        add_module_executable(anatomy anatomy/reexported.ixx anatomy/useful.module.ixx anatomy/anatomy.module.ixx anatomy/anatomy.module.mxx anatomy/anatomy.cpp)
        add_modules(anatomy anatomy/reexported.ixx anatomy/useful.module.ixx anatomy/anatomy.module.ixx)
    endif ()

    add_module_executable(std_import std_import/std_import.cpp)
    add_system_modules(std_import iostream)

    add_module_executable(no_macro_effect no_macro_effect/preproc_citadel.ixx no_macro_effect/no_macro_effect.cpp)
    add_modules(no_macro_effect no_macro_effect/preproc_citadel.ixx)

    if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        # Clang => fatal error: module 'std_config' is defined in both
        add_module_executable(std_dual_import std_dual_import/module_std_import.ixx std_dual_import/main_std_import.cpp)
        add_modules(std_dual_import std_dual_import/module_std_import.ixx)
        add_system_modules(std_dual_import iostream)
    endif ()

    if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # GCC => error: sorry, unimplemented: private module fragment
        add_module_executable(pimpl pimpl/pimpled.ixx pimpl/use_pimpled.cpp)
        add_modules(pimpl pimpl/pimpled.ixx)
    endif ()

    if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        # Clang => error: 'bar' has different definitions in different modules; definition in module 'module2' first difference is function body
        add_module_executable(ownership ownership/module1.ixx ownership/module2.ixx ownership/use_multiple_foo.cpp)
        add_modules(ownership ownership/module1.ixx ownership/module2.ixx)
    endif ()

    add_module_executable(template_import template_import/template_module.ixx template_import/other_module.ixx template_import/template_import.cpp)
    add_modules(template_import template_import/template_module.ixx template_import/other_module.ixx)
    add_system_modules(template_import iostream)

else ()
    # for MSVC, see https://devblogs.microsoft.com/cppblog/using-cpp-modules-in-msvc-from-the-command-line-part-1/
    # for GCC, clang: https://stackoverflow.com/questions/57300495/how-to-use-c20-modules-with-cmake
    message(STATUS "\tC++20 Modules build not supported")
    try_run(RUN_RESULT COMPILE_RESULT # use this to get exact date-time format as in unsupported.hpp 
            ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/get_cpp_time.cpp
            RUN_OUTPUT_VARIABLE RUN_OUTPUT)
    if (NOT COMPILE_RESULT OR RUN_RESULT)
        message(FATAL_ERROR "Cannot compile get_cpp_time.cpp")
    endif ()
    file(APPEND "${CMAKE_BINARY_DIR}/unsupported_examples.md" "* ${RUN_OUTPUT} : cannot build modules\n")
endif ()
