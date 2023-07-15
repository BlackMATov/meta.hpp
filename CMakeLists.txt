cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

#
# version
#

file(READ "headers/meta.hpp/meta_base/base.hpp" META_HPP_BASE_HEADER_CONTENT)

set(META_HPP_VERSION_MAJOR_MATCHER "#[\t\r\n ]*define[\t\r\n ]*META_HPP_VERSION_MAJOR[\t\r\n ]*([0-9]+)")
set(META_HPP_VERSION_MINOR_MATCHER "#[\t\r\n ]*define[\t\r\n ]*META_HPP_VERSION_MINOR[\t\r\n ]*([0-9]+)")
set(META_HPP_VERSION_PATCH_MATCHER "#[\t\r\n ]*define[\t\r\n ]*META_HPP_VERSION_PATCH[\t\r\n ]*([0-9]+)")

if(META_HPP_BASE_HEADER_CONTENT MATCHES ${META_HPP_VERSION_MAJOR_MATCHER})
    set(META_HPP_VERSION_MAJOR ${CMAKE_MATCH_1})
else()
    message(FATAL_ERROR "meta.hpp: failed to resolve a major version number")
endif()

if(META_HPP_BASE_HEADER_CONTENT MATCHES ${META_HPP_VERSION_MINOR_MATCHER})
    set(META_HPP_VERSION_MINOR ${CMAKE_MATCH_1})
else()
    message(FATAL_ERROR "meta.hpp: failed to resolve a minor version number")
endif()

if(META_HPP_BASE_HEADER_CONTENT MATCHES ${META_HPP_VERSION_PATCH_MATCHER})
    set(META_HPP_VERSION_PATCH ${CMAKE_MATCH_1})
else()
    message(FATAL_ERROR "meta.hpp: failed to resolve a patch version number")
endif()

#
# project
#

project(meta.hpp
    VERSION "${META_HPP_VERSION_MAJOR}.${META_HPP_VERSION_MINOR}.${META_HPP_VERSION_PATCH}"
    DESCRIPTION "C++20 Dynamic Reflection Library"
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
