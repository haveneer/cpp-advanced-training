#!/usr/bin/env bash
set -eo pipefail

if [[ "$DEBUG_CI" == true ]]; then
  set -x
fi

if [[ "$DEBUG_CI" == true ]]; then
    choco install vswhere
    vswhere
    vswhere -all -prerelease -property installationPath
    echo $PATH
    cmake --version
fi