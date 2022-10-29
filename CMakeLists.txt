cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

if(NOT DEFINED PROJECT_NAME)
    set(BUILD_AS_STANDALONE ON)
else()
    set(BUILD_AS_STANDALONE OFF)
endif()

project(meta.hpp)

add_library(${PROJECT_NAME} INTERFACE)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)
target_include_directories(${PROJECT_NAME} INTERFACE headers)

find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} INTERFACE Threads::Threads)

#
# BUILD_AS_STANDALONE
#

if(NOT ${BUILD_AS_STANDALONE})
    return()
endif()

option(BUILD_WITH_COVERAGE "Build with coverage" OFF)
option(BUILD_WITH_SANITIZERS "Build with sanitizers" OFF)

enable_testing()
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(EnableASan)
include(EnableGCov)
include(EnableUBSan)

add_subdirectory(manuals)
add_subdirectory(singles)
add_subdirectory(vendors)
add_subdirectory(untests)
