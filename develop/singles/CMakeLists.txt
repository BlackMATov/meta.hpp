project(meta.hpp.singles)

#
# generate
#

find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(META_HPP_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..")
set(META_HPP_SINGLES_INPUT "${META_HPP_ROOT_DIR}/headers/meta.hpp/meta_all.hpp")
set(META_HPP_SINGLES_OUTPUT "${META_HPP_ROOT_DIR}/develop/singles/headers/meta.hpp/meta_all.hpp")
set(META_HPP_SINGLES_SCRIPT "${META_HPP_ROOT_DIR}/develop/singles/scripts/build_singles.py")
file(GLOB_RECURSE META_SINGLES_DEPENDS CONFIGURE_DEPENDS "${META_HPP_ROOT_DIR}/headers/*.hpp")

add_custom_command(OUTPUT "${META_HPP_SINGLES_OUTPUT}"
    COMMAND "${Python3_EXECUTABLE}" "${META_HPP_SINGLES_SCRIPT}"
            "${META_HPP_SINGLES_INPUT}" "${META_HPP_SINGLES_OUTPUT}"
    DEPENDS ${META_SINGLES_DEPENDS}
    WORKING_DIRECTORY "${META_HPP_ROOT_DIR}")

add_custom_target(${PROJECT_NAME}.generate
    DEPENDS "${META_HPP_SINGLES_OUTPUT}")

#
# library
#

add_library(${PROJECT_NAME} INTERFACE)
add_library(meta.hpp::singles ALIAS ${PROJECT_NAME})

add_dependencies(${PROJECT_NAME}
    ${PROJECT_NAME}.generate)

target_compile_features(${PROJECT_NAME} INTERFACE
    cxx_std_20)

target_include_directories(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/headers>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} INTERFACE Threads::Threads)
