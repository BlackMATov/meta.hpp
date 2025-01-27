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

option(META_HPP_DEVELOP "Generate develop targets" OFF)
option(META_HPP_INSTALL "Generate install targets" ${PROJECT_IS_TOP_LEVEL})

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

if(META_HPP_DEVELOP)
    enable_testing()
    add_subdirectory(develop)
endif()

#
# install
#

if(META_HPP_INSTALL)
    include(CMakePackageConfigHelpers)
    include(GNUInstallDirs)

    set(META_HPP_INSTALL_CONFIG_DIR
        "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

    set(META_HPP_INSTALL_CONFIG_INPUT
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.cmake.in")

    set(META_HPP_INSTALL_CONFIG_CMAKE
        "${CMAKE_CURRENT_BINARY_DIR}/configs/${PROJECT_NAME}-config.cmake")

    set(META_HPP_INSTALL_CONFIG_VERSION_CMAKE
        "${CMAKE_CURRENT_BINARY_DIR}/configs/${PROJECT_NAME}-config-version.cmake")

    configure_package_config_file(
        "${META_HPP_INSTALL_CONFIG_INPUT}"
        "${META_HPP_INSTALL_CONFIG_CMAKE}"
        INSTALL_DESTINATION "${META_HPP_INSTALL_CONFIG_DIR}"
        NO_SET_AND_CHECK_MACRO)

    write_basic_package_version_file(
        "${META_HPP_INSTALL_CONFIG_VERSION_CMAKE}"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
        ARCH_INDEPENDENT)

    install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}-targets)

    install(
        DIRECTORY headers/${PROJECT_NAME}
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

    install(
        EXPORT ${PROJECT_NAME}-targets
        FILE ${PROJECT_NAME}-targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION "${META_HPP_INSTALL_CONFIG_DIR}")

    install(
        FILES "${META_HPP_INSTALL_CONFIG_CMAKE}"
              "${META_HPP_INSTALL_CONFIG_VERSION_CMAKE}"
        DESTINATION "${META_HPP_INSTALL_CONFIG_DIR}")
endif()
