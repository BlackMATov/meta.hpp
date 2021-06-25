#!/bin/bash
set -e
DIR="$( cd "$( dirname "${BASH_SOURCE}" )" && pwd )"

ROOT_DIR="${DIR}/.."
BUILD_DIR="${ROOT_DIR}/build/darwin_release"

mkdir -p "${BUILD_DIR}"
(cd "${BUILD_DIR}" && cmake "${ROOT_DIR}" -DCMAKE_BUILD_TYPE=Release)
(cd "${BUILD_DIR}" && cmake --build .)
(cd "${BUILD_DIR}" && ctest --verbose)
