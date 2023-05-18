[![C++ CI](https://github.com/haveneer/cpp-advanced-training/workflows/C++%20CI/badge.svg)](https://github.com/haveneer/cpp-advanced-training/actions)

<p xmlns:dct="http://purl.org/dc/terms/" xmlns:cc="http://creativecommons.org/ns#" class="license-text">
    <a rel="license" href="https://creativecommons.org/licenses/by-nc-sa/4.0">
        <img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" />
    </a><br>
    <a rel="cc:attributionURL" property="dct:title" href="https://github.com/haveneer/cpp-advanced-training">
        Code snippets for Advanced C++ training
    </a>
    by 
    <a rel="cc:attributionURL dct:creator" property="cc:attributionName" href="mailto:hpwxf@haveneer.com">
        Pascal HAVÉ
    </a> is licensed under 
    <a rel="license" href="https://creativecommons.org/licenses/by-nc-sa/4.0">CC BY-NC-SA 4.0</a>
</p>

For more details about this training, contact [hpwxf@haveneer.com](mailto:hpwxf@haveneer.com).

# cpp-advanced-training

* Any file `filename.cpp` is automatically compiled using extra files `filename.h*` `filename--*.*`

* Any file `filename--unittest.cpp` is a google test and is automatically compiled using extra
  files `filename.h*` `filename--*.*`

* Any file `filaname.cxx` is not compiled and usually contains explicit bugs

* `type.h` contains tool `type(obj)` to get printable type name for given object argument

* Files in `src/modules` have a specific build rules

# Build with conan 2.0

```
conan install ..
mkdir build && cd build 
cmake -DCMAKE_TOOLCHAIN_FILE=../conan_toolchain.cmake ..
```

# CMake options

* `-DENABLE_STATIC_ANALYSIS=ON|OFF` : enable/disable static analysis while compiling

* `-DCMAKE_CXX_COMPILER_LAUNCHER=ccache` : enable `ccache` as compiler cache

# Third party libs

* [Google Benchmark](https://github.com/google/benchmark.git) as submodule in `third_party/benchmark`

* [Google Test](https://github.com/google/googletest.git) as submodule in `third_party/gtest`

* [lib {fmt}](https://github.com/fmtlib/fmt.git) as submodule in `third_party/fmt`

* [oneTBB (ex: TBB)](https://github.com/oneapi-src/oneTBB) as installed component