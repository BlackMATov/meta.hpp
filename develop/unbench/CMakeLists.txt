project(meta.hpp.unbench)

file(GLOB_RECURSE UNBENCH_SOURCES CONFIGURE_DEPENDS "*.cpp" "*.hpp")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${UNBENCH_SOURCES})

add_executable(${PROJECT_NAME} ${UNBENCH_SOURCES})
add_executable(${PROJECT_NAME}.singles ${UNBENCH_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
    meta.hpp::meta.hpp
    meta.hpp::setup_targets
    meta.hpp.vendors::gbench
    meta.hpp.vendors::rttr
    meta.hpp.vendors::vmath.hpp)

target_link_libraries(${PROJECT_NAME}.singles PRIVATE
    meta.hpp::singles
    meta.hpp::setup_targets
    meta.hpp.vendors::gbench
    meta.hpp.vendors::rttr
    meta.hpp.vendors::vmath.hpp)

add_test(${PROJECT_NAME} ${PROJECT_NAME})
add_test(${PROJECT_NAME} ${PROJECT_NAME}.singles)
