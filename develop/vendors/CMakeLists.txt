project(meta.hpp.vendors)

set(DOCTEST_NO_INSTALL ON CACHE INTERNAL "")
set(DOCTEST_USE_STD_HEADERS ON CACHE INTERNAL "")

add_subdirectory(doctest)
set_target_properties(doctest doctest_with_main PROPERTIES FOLDER meta.hpp.vendors)
