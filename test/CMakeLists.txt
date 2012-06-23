cmake_minimum_required(VERSION 2.6)

project(test)

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

set(TEST_SOURCES
  fixture_permutator.hpp
  menu_fixtures.hpp
  sandbox_fixture.hpp
  wchar_output.hpp
  dynamic_link_test.cpp
  filesystem_test.cpp
  folder_error_adapter_test.cpp
  format_test.cpp
  global_lock_test.cpp
  hook_test.cpp
  icon_test.cpp
  menu_item_description_test.cpp
  menu_item_test.cpp
  menu_item_visitor_test.cpp
  menu_test.cpp
  module.cpp
  pidl_iterator_test.cpp
  pidl_test.cpp
  progress_test.cpp
  shell_test.cpp
  shell_item_test.cpp
  task_dialog_test.cpp
  window_test.cpp)

add_executable(tests ${TEST_SOURCES})
get_target_property(test_EXE tests LOCATION)

add_executable(tests_unicode ${TEST_SOURCES})
get_target_property(test_unicode_EXE tests_unicode LOCATION)
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
