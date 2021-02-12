#!/usr/bin/env bash
set -eo pipefail

if [[ "$DEBUG_CI" == true ]]; then
  set -x
fi

# Default configuration when used out of travis-ci
MODE=${MODE:-Debug}
EXTRA_CMAKE_OPTIONS=${EXTRA_CMAKE_OPTIONS:-}
BUILD_TEST=${BUILD_TEST:-true}

if [[ -n ${TRAVIS_BUILD_DIR:+x} ]]; then
echo
    cd ${TRAVIS_BUILD_DIR}
fi

# find "/c" -iname "Common7" || true
# Required by CMake to find VS2019
export VS160COMNTOOLS="/c/Program Files (x86)/Microsoft Visual Studio/2019/Community/Common7/Tools"
echo "VS160COMNTOOLS = $VS160COMNTOOLS"

mkdir -p build
cd build
cmake \
  -DCMAKE_GENERATOR_PLATFORM=x64 \
  -DDONT_VS_EXCLUDE=true \
  -G "Visual Studio 16 2019" \
  ..

cmake --build . --target ALL_BUILD --config ${MODE} \
  && ctest -C ${MODE} # --verbose
