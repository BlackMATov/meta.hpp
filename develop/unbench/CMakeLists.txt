project(meta.hpp.unbench)

file(GLOB_RECURSE UNBENCH_SOURCES CONFIGURE_DEPENDS "*.cpp" "*.hpp")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${UNBENCH_SOURCES})

add_executable(${PROJECT_NAME} EXCLUDE_FROM_ALL ${UNBENCH_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
    meta.hpp::meta.hpp
    meta.hpp::setup_targets
    meta.hpp.vendors::gbench
    meta.hpp.vendors::vmath.hpp)
