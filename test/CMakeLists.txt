cmake_minimum_required(VERSION 2.6)

project(winapi-test)
include_directories(../include)
include_directories(comet/include)
link_libraries(Shlwapi)

set(Boost_USE_STATIC_LIBS TRUE)
set(Boost_DETAILED_FAILURE_MSG TRUE)
set(Boost_ADDITIONAL_VERSIONS 1.46.1)
find_package(
  Boost 1.40 REQUIRED
  COMPONENTS filesystem system unit_test_framework)
link_directories(${Boost_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIRS})

add_definitions(
  -DWIN32_LEAN_AND_MEAN -DBOOST_FILESYSTEM_VERSION=2 -DSTRICT_TYPED_ITEMIDS)

file(GLOB TEST_SOURCES *.cpp *.hpp)

add_executable(tests ${TEST_SOURCES})
get_target_property(test_EXE tests LOCATION)

add_executable(tests_unicode ${TEST_SOURCES})
get_target_property(test_unicode_EXE tests LOCATION)
set_target_properties(
  tests_unicode PROPERTIES
  COMPILE_DEFINITIONS _UNICODE COMPILE_DEFINITIONS _UNICODE)

set(TEST_RUNNER_ARGUMENTS
  --catch_system_errors --detect_memory_leaks
  --result_code=no --log_level=test_suite)

ADD_CUSTOM_TARGET(RUN_TESTS
  ${test_EXE} ${TEST_RUNNER_ARGUMENTS} DEPENDS tests)
ADD_CUSTOM_TARGET(RUN_UNICODE_TESTS
  ${test_unicode_EXE} ${TEST_RUNNER_ARGUMENTS} DEPENDS tests_unicode)
