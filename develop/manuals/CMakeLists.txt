project(meta.hpp.manuals)

file(GLOB_RECURSE MANUALS_SOURCES CONFIGURE_DEPENDS "*.cpp" "*.hpp")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${MANUALS_SOURCES})

add_executable(${PROJECT_NAME} ${MANUALS_SOURCES})
add_executable(${PROJECT_NAME}.singles ${MANUALS_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
    meta.hpp::meta.hpp
    meta.hpp::setup_targets
    meta.hpp.vendors::doctest
    meta.hpp.vendors::fmt)

target_link_libraries(${PROJECT_NAME}.singles PRIVATE
    meta.hpp::singles
    meta.hpp::setup_targets
    meta.hpp.vendors::doctest
    meta.hpp.vendors::fmt)

add_test(${PROJECT_NAME} ${PROJECT_NAME})
add_test(${PROJECT_NAME} ${PROJECT_NAME}.singles)
