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
    cd "${TRAVIS_BUILD_DIR}"
fi

mkdir -p build
cd build
cmake \
  -G "Unix Makefiles" \
  -DCMAKE_BUILD_TYPE="${MODE}" \
  ..

cmake --build . --config "${MODE}" \
  && ctest -C "${MODE}" --output-on-failure # --verbose
