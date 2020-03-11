find_package(Catch2 REQUIRED)
include(Catch)

foreach(standard 11 14 17)
  add_library(catch-main-${standard} STATIC catch-main.cc)
  target_link_libraries(catch-main-${standard} Catch2::Catch2)
  add_executable(catch-tests-${standard} tests.cc)
  set_target_properties(catch-main-${standard} catch-tests-${standard}
    PROPERTIES
    CXX_STANDARD ${standard}
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)
  target_link_libraries(catch-tests-${standard}
    condition-cpp Catch2::Catch2 catch-main-${standard})
  catch_discover_tests(catch-tests-${standard} TEST_PREFIX "c++${standard}: ")
endforeach()
