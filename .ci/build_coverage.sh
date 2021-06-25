#!/bin/bash
set -e
DIR="$( cd "$( dirname "${BASH_SOURCE}" )" && pwd )"

ROOT_DIR="${DIR}/.."
BUILD_DIR="${ROOT_DIR}/build/coverage"

mkdir -p "${BUILD_DIR}"
(cd "${BUILD_DIR}" && cmake "${ROOT_DIR}" -DCMAKE_BUILD_TYPE=Debug -DBUILD_WITH_COVERAGE=ON)
(cd "${BUILD_DIR}" && cmake --build .)

(cd "${BUILD_DIR}" && lcov -d . -z)
(cd "${BUILD_DIR}" && ctest --verbose)

(cd "${BUILD_DIR}" && lcov -d . -c -o "coverage.info")
(cd "${BUILD_DIR}" && lcov -r "coverage.info" "*/usr/*" "*/untests/*" -o "coverage.info")
(cd "${BUILD_DIR}" && lcov -l "coverage.info")

bash <(curl -s https://codecov.io/bash) -f "${BUILD_DIR}/coverage.info" || echo "Codecov did not collect coverage reports"
