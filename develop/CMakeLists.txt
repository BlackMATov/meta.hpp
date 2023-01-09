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
