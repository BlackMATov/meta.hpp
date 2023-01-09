cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(meta.hpp
    VERSION "0.0.1"
    DESCRIPTION "C++20 tiny dynamic reflection library"
    HOMEPAGE_URL "https://github.com/blackmatov/meta.hpp")

#
# LIBRARY
#

add_library(${PROJECT_NAME} INTERFACE)
add_library(meta.hpp::meta.hpp ALIAS ${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} INTERFACE
    cxx_std_20)

target_include_directories(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/headers>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} INTERFACE Threads::Threads)

#
# DEVELOP
#

if(PROJECT_IS_TOP_LEVEL)
    add_subdirectory(develop)
endif()


#
# INSTALL
#

if(PROJECT_IS_TOP_LEVEL)
    include(CMakePackageConfigHelpers)
    include(GNUInstallDirs)

    set(META_HPP_INSTALL_CONFIG_DIR
        "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

    set(META_HPP_INSTALL_CONFIG_INPUT
        "${CMAKE_CURRENT_SOURCE_DIR}/develop/cmake/Config.cmake.in")

    set(META_HPP_INSTALL_GENERATED_CONFIG_CMAKE
        "${CMAKE_CURRENT_BINARY_DIR}/generated/${PROJECT_NAME}-config.cmake")

    set(META_HPP_INSTALL_GENERATED_CONFIG_VERSION_CMAKE
        "${CMAKE_CURRENT_BINARY_DIR}/generated/${PROJECT_NAME}-config-version.cmake")

    configure_package_config_file(
        "${META_HPP_INSTALL_CONFIG_INPUT}"
        "${META_HPP_INSTALL_GENERATED_CONFIG_CMAKE}"
        INSTALL_DESTINATION "${META_HPP_INSTALL_CONFIG_DIR}"
        NO_SET_AND_CHECK_MACRO)

    write_basic_package_version_file(
        "${META_HPP_INSTALL_GENERATED_CONFIG_VERSION_CMAKE}"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
        ARCH_INDEPENDENT)

    install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}-targets
        INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

    install(
        DIRECTORY headers/${PROJECT_NAME}
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

    install(
        EXPORT ${PROJECT_NAME}-targets
        FILE ${PROJECT_NAME}-targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION "${META_HPP_INSTALL_CONFIG_DIR}")

    install(
        FILES "${META_HPP_INSTALL_GENERATED_CONFIG_CMAKE}"
              "${META_HPP_INSTALL_GENERATED_CONFIG_VERSION_CMAKE}"
        DESTINATION "${META_HPP_INSTALL_CONFIG_DIR}")
endif()
