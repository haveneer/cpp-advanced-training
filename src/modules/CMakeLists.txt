# For a portable module configuration, follow https://gitlab.kitware.com/cmake/cmake/-/issues/18355
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(HAS_CXX_MODULES_SUPPORT TRUE)

    set(CMAKE_CXX_EXTENSIONS OFF)
    # set_property(GLOBAL PROPERTY GLOBAL_DEPENDS_DEBUG_MODE 1)

    function(add_module_executable name)
        # GCC requires to compile all at once (AFAIK)
        separate_arguments(CXX_FLAGS UNIX_COMMAND "${CMAKE_CXX_FLAGS}")
        list(TRANSFORM ARGN PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
        add_custom_target(${name}.exe ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${name})
        add_custom_command(
                OUTPUT
                ${CMAKE_CURRENT_BINARY_DIR}/${name}
                COMMAND
                ${CMAKE_CXX_COMPILER} ${CXX_FLAGS}
                -std=c++20 -fmodules-ts
                ${ARGN} -o ${CMAKE_CURRENT_BINARY_DIR}/${name}
                DEPENDS ${ARGN}
                COMMENT "Build modular executable ${name}"
        )
        message(STATUS "\tAdd module example ${name}.exe")
        add_test(NAME ${name}.exe COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${name})
        set_property(TEST ${name}.exe PROPERTY LABELS "Modules")
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
            add_dependencies(${name}.exe ${mod}.gcm)
        endforeach ()
    endfunction()

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(HAS_CXX_MODULES_SUPPORT TRUE)

    set(CMAKE_CXX_EXTENSIONS OFF)
    set(PREBUILT_MODULE_PATH ${CMAKE_BINARY_DIR}/modules)

    function(add_module_executable name)
        add_executable(${name} ${ARGN})
        target_compile_options(${name} PRIVATE -fmodules)
        target_compile_options(${name} PRIVATE -fbuiltin-module-map)
        target_compile_options(${name} PRIVATE -fimplicit-module-maps)
        target_compile_options(${name} PRIVATE -fprebuilt-module-path=${PREBUILT_MODULE_PATH})
        message(STATUS "\tAdd module example ${name}")
        add_test(NAME ${name} COMMAND ${name})
        set_property(TEST ${name} PROPERTY LABELS "Modules")
    endfunction()

    function(add_modules name)
        file(MAKE_DIRECTORY ${PREBUILT_MODULE_PATH})
        separate_arguments(CXX_FLAGS UNIX_COMMAND "${CMAKE_CXX_FLAGS}")
        foreach (mod ${ARGN})
            string(REGEX REPLACE "[^A-Za-z0-9_.]" "_" mod_target ${mod})
            get_filename_component(modfile ${mod} NAME_WLE)
            add_custom_target(${mod_target}
                    COMMAND
                    ${CMAKE_CXX_COMPILER} ${CXX_FLAGS}
                    -std=c++20 -stdlib=libc++ -fmodules
                    -pthread # due to enabled pthread
                    -c ${CMAKE_CURRENT_SOURCE_DIR}/${mod}
                    -Xclang -emit-module-interface
                    -o ${PREBUILT_MODULE_PATH}/${modfile}.pcm
                    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${mod}
                    COMMENT "Build module ${mod}"
                    )
            add_dependencies(${name} ${mod_target})
        endforeach ()
    endfunction()

    function(add_system_modules name)
        # Do nothing for Clang
    endfunction()

endif ()

if (HAS_CXX_MODULES_SUPPORT)

    add_module_executable(modular_helloworld modular_helloworld/helloworld_module.cpp modular_helloworld/modular_helloworld.cpp)
    add_modules(modular_helloworld modular_helloworld/helloworld_module.cpp)
    add_system_modules(modular_helloworld iostream)

    if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # GCC => error: sorry, unimplemented: private module fragment
        add_module_executable(modular_helloworld2 modular_helloworld2/modular_helloworld2.cpp modular_helloworld2/helloworld_module2.cpp)
        add_modules(modular_helloworld2 modular_helloworld2/helloworld_module2.cpp)
        add_system_modules(modular_helloworld2 iostream)
    endif ()

    if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        # Clang => fatal error: module 'std_config' is defined in both
        add_module_executable(main_with_std_import dual_import/module_with_std_import.cpp dual_import/main_with_std_import.cpp)
        add_modules(main_with_std_import dual_import/module_with_std_import.cpp)
        add_system_modules(main_with_std_import iostream)
    endif ()

    if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # GCC => error: sorry, unimplemented: private module fragment
        add_module_executable(pimpl pimpl/use_pimpled.cpp pimpl/pimpled_module.cpp)
        add_modules(pimpl pimpl/pimpled_module.cpp)
    endif ()

    add_module_executable(ownership ownership/v1_module.cpp ownership/v2_module.cpp ownership/use_vx_foo.cpp)
    add_modules(ownership ownership/v1_module.cpp ownership/v2_module.cpp)

    add_module_executable(std_import std_import/std_import.cpp)
    add_system_modules(std_import iostream)

else ()
    message(STATUS "\tC++20 Modules build not supported")
    string(TIMESTAMP DATETIME "%b %d %Y at %H:%M") # use strftime date format
    file(APPEND "${CMAKE_BINARY_DIR}/unsupported_examples.md" "* ${DATETIME} : cannot build modules\n")
endif ()
