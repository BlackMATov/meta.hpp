project(meta.hpp.vendors)

set(DOCTEST_NO_INSTALL ON CACHE INTERNAL "")
add_subdirectory(doctest)
set_target_properties(doctest_with_main PROPERTIES FOLDER meta.hpp.vendors)

set(FMT_INSTALL OFF CACHE INTERNAL "")
add_subdirectory(fmtlib)
set_target_properties(fmt PROPERTIES FOLDER meta.hpp.vendors)
