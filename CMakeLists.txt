cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

#
# VERSION
#

file(READ headers/meta.hpp/meta_base/base.hpp META_HPP_BASE_FILE)

string(REGEX MATCH "#define[ ]+META_HPP_VERSION_MAJOR[ ]+([0-9]+)" _ ${META_HPP_BASE_FILE})
set(META_HPP_VERSION_MAJOR "${CMAKE_MATCH_1}")

string(REGEX MATCH "#define[ ]+META_HPP_VERSION_MINOR[ ]+([0-9]+)" _ ${META_HPP_BASE_FILE})
set(META_HPP_VERSION_MINOR "${CMAKE_MATCH_1}")

string(REGEX MATCH "#define[ ]+META_HPP_VERSION_PATCH[ ]+([0-9]+)" _ ${META_HPP_BASE_FILE})
set(META_HPP_VERSION_PATCH "${CMAKE_MATCH_1}")

set(META_HPP_VERSION ${META_HPP_VERSION_MAJOR}.${META_HPP_VERSION_MINOR}.${META_HPP_VERSION_PATCH})

#
# PROJECT
#

project(meta.hpp
    VERSION ${META_HPP_VERSION}
    DESCRIPTION "C++20 tiny dynamic reflection library"
    HOMEPAGE_URL "https://github.com/blackmatov/meta.hpp")

add_library(${PROJECT_NAME} INTERFACE)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)
target_include_directories(${PROJECT_NAME} INTERFACE headers)

find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} INTERFACE Threads::Threads)

#
# DEVELOPER
#

if(NOT PROJECT_IS_TOP_LEVEL)
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
