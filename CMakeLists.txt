cmake_minimum_required(VERSION 3.10)
message(STATUS "CMake version: ${CMAKE_VERSION}")
project(condition-cpp
  VERSION 0.1.0
  DESCRIPTION "A lightweight condition system for C++"
  HOMEPAGE_URL "https://github.com/musteresel/condition-cpp"
  LANGUAGES CXX)


# condition-cpp is a header only library.  This is best represented in
# CMake as a so called INTERFACE library.
#
# We set the PUBLIC_HEADER property so that the header file get's
# installed.
#
# We set INTERFACE include directories so that targets which "link"
# against condition-cpp use the correct include path (different for
# build and install time).
add_library(condition-cpp INTERFACE)
if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.15.0")
  set_target_properties(condition-cpp PROPERTIES
    PUBLIC_HEADER condition-cpp.hh)
endif()
target_include_directories(condition-cpp INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>)


# Installing a header only library requires only copying its public
# headers.  To ease use we also provide an export so that users can
# just use find_package(condition-cpp REQUIRED) in order to use this
# library.
install(TARGETS condition-cpp
  EXPORT condition-cpp-export
  PUBLIC_HEADER DESTINATION include)
if (${CMAKE_VERSION} VERSION_LESS "3.15.0")
  install(FILES condition-cpp.hh DESTINATION include)
endif()


# The "export" is what enables find_package(condition-cpp) to work:
# This auto generates a file which re-creates the INTERFACE library.
# It needs to have that special <package-name>Config.cmake name in
# order for find_package to find it.
install(EXPORT condition-cpp-export
  FILE condition-cppConfig.cmake
  DESTINATION lib/cmake/condition-cpp)


# This is a hack to make sure that the header file is listed as
# "source file" in some build systems (Visual Studio?)
add_custom_target(condition-cpp.header SOURCES condition-cpp.hh)


find_program(doxygen doxygen)
if (doxygen)
  message(STATUS "Found doxygen, building documentation")
  # Generate man pages and html documentation using doxygen.  The docs
  # target can be used to depend on the documentation being built.
  configure_file(Doxyfile.in Doxyfile @ONLY)
  add_custom_command(OUTPUT doc
    COMMAND
    ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/doc/html
    COMMAND
    ${doxygen} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile condition-cpp.hh
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  add_custom_target(docs ALL DEPENDS doc)


  # Install man pages and html documentation.
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc/man/man3
    TYPE MAN)
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc/html/condition-cpp
    TYPE DOC)
else()
  message(STATUS "Doxygen not found, no documentation will be built!")
endif()


# C++11 version of the "MVP" (minimal viable product, basically a show
# case example).
add_executable(mvp11 mvp.cc)
target_link_libraries(mvp11 condition-cpp)
set_target_properties(mvp11 PROPERTIES
  CXX_STANDARD 11
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS NO)


# C++14 version of the "MVP".
add_executable(mvp14 mvp.cc)
target_link_libraries(mvp14 condition-cpp)
set_target_properties(mvp14 PROPERTIES
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS NO)


# Last but not least enable as much warnings as possible for all
# compiling targets.
if(MSVC)
  target_compile_options(mvp11 PRIVATE /W4 /WX)
  target_compile_options(mvp14 PRIVATE /W4 /WX)
else()
  target_compile_options(mvp11 PRIVATE -Wall -Wextra -pedantic -Werror)
  target_compile_options(mvp14 PRIVATE -Wall -Wextra -pedantic -Werror)
endif()


# Simple package everything into a zip archive for now.
set(CPACK_GENERATOR "ZIP")
include(CPack)
