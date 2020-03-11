find_package(Catch2 REQUIRED)
include(Catch)

add_executable(catch-tests catch-main.cc tests.cc)
target_link_libraries(catch-tests condition-cpp Catch2::Catch2)
catch_discover_tests(catch-tests)
