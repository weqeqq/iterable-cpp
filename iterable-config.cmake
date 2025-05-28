@PACKAGE_INIT@

set_and_check(ITERABLE_INCLUDE_DIR @PACKAGE_ITERABLE_INCLUDE_DIR@)
set_and_check(ITERABLE_LIBRARY_DIR @PACKAGE_ITERABLE_LIBRARY_DIR@)

include(${CMAKE_CURRENT_LIST_DIR}/iterable-targets.cmake)
check_required_components(iterable)
