
cmake_minimum_required(VERSION 3.23)

project(iterable LANGUAGES CXX VERSION 0.2.0)

add_library(iterable INTERFACE)
add_library(iterable::iterable ALIAS iterable)

target_sources(iterable PUBLIC 
  FILE_SET  HEADERS 
  BASE_DIRS include
  FILES
  include/iterable/define.h
  include/iterable/iterator.h
  include/iterable/iterator.h
)
install(
  TARGETS iterable 
  EXPORT  iterable-targets FILE_SET HEADERS
)
install(
  EXPORT    iterable-targets 
  NAMESPACE iterable:: DESTINATION lib/cmake/iterable
)
include(CMakePackageConfigHelpers)

set(ITERABLE_INCLUDE_DIR include)
set(ITERABLE_LIBRARY_DIR lib)

configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/iterable-config.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/iterable-config.cmake
  INSTALL_DESTINATION lib/cmake/iterable 
  PATH_VARS 
    ITERABLE_INCLUDE_DIR 
    ITERABLE_LIBRARY_DIR
)
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/iterable-config-version.cmake COMPATIBILITY SameMajorVersion
)
install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/iterable-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/iterable-config-version.cmake
  DESTINATION lib/cmake/iterable
)
