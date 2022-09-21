project(meta.hpp.singles)

#
# generate
#

find_package(PythonInterp REQUIRED)

set(META_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")
set(META_SINGLES_INPUT "${META_ROOT_DIR}/headers/meta.hpp/meta_all.hpp")
set(META_SINGLES_OUTPUT "${META_ROOT_DIR}/singles/headers/meta.hpp/meta_all.hpp")
file(GLOB_RECURSE META_SINGLES_DEPENDS "${META_ROOT_DIR}/headers/*.hpp")

add_custom_command(OUTPUT "${META_SINGLES_OUTPUT}"
    COMMAND "${PYTHON_EXECUTABLE}" ".ci/build_singles.py" "${META_SINGLES_INPUT}" "${META_SINGLES_OUTPUT}"
    DEPENDS ${META_SINGLES_DEPENDS}
    WORKING_DIRECTORY "${META_ROOT_DIR}")

add_custom_target(${PROJECT_NAME}.generate
    DEPENDS "${META_SINGLES_OUTPUT}")

#
# library
#

add_library(${PROJECT_NAME} INTERFACE)
add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}.generate)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)
target_include_directories(${PROJECT_NAME} INTERFACE headers)
target_link_libraries(${PROJECT_NAME} INTERFACE Threads::Threads)
