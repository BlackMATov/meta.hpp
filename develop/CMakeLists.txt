option(BUILD_WITH_COVERAGE "Build with coverage" OFF)
option(BUILD_WITH_SANITIZERS "Build with sanitizers" OFF)

enable_testing()
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")
list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(EnableASan)
include(EnableGCov)
include(EnableUBSan)

add_subdirectory(manuals)
add_subdirectory(singles)
add_subdirectory(vendors)
add_subdirectory(untests)

#
# install
#

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

set(META_HPP_INSTALL_CONFIG_DIR
    "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

set(META_HPP_INSTALL_CONFIG_INPUT
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in")

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
    DIRECTORY ../headers/${PROJECT_NAME}
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
