cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(meta.hpp
    VERSION "0.0.1"
    DESCRIPTION "C++20 tiny dynamic reflection library"
    HOMEPAGE_URL "https://github.com/blackmatov/meta.hpp")

#
# library
#

file(GLOB_RECURSE META_HPP_HEADERS "headers/*.hpp")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${META_HPP_HEADERS})

add_library(${PROJECT_NAME} INTERFACE ${META_HPP_HEADERS})
add_library(meta.hpp::meta.hpp ALIAS ${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} INTERFACE
    cxx_std_20)

target_include_directories(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/headers>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} INTERFACE Threads::Threads)

#
# develop
#

if(PROJECT_IS_TOP_LEVEL)
    add_subdirectory(develop)
endif()
