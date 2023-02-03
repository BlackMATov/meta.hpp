cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(meta.hpp
    VERSION "0.0.1"
    DESCRIPTION "C++20 tiny dynamic reflection library"
    HOMEPAGE_URL "https://github.com/blackmatov/meta.hpp"
    LANGUAGES CXX)

option(META_HPP_NO_EXCEPTIONS "Don't use exceptions" OFF)
option(META_HPP_NO_RTTI "Don't use RTTI" OFF)

#
# library
#

file(GLOB_RECURSE META_HPP_HEADERS CONFIGURE_DEPENDS "headers/*.hpp")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${META_HPP_HEADERS})

add_library(${PROJECT_NAME} INTERFACE ${META_HPP_HEADERS})
add_library(meta.hpp::meta.hpp ALIAS ${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} INTERFACE
    cxx_std_20)

target_include_directories(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/headers>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_definitions(${PROJECT_NAME} INTERFACE
    $<$<BOOL:${META_HPP_NO_EXCEPTIONS}>:META_HPP_NO_EXCEPTIONS>
    $<$<BOOL:${META_HPP_NO_RTTI}>:META_HPP_NO_RTTI>)

find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} INTERFACE Threads::Threads)

#
# develop
#

if(PROJECT_IS_TOP_LEVEL)
    enable_testing()
    add_subdirectory(develop)
endif()
