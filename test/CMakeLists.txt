set(TEST_SOURCES
  fixture_permutator.hpp
  button_test_visitors.hpp
  item_test_visitors.hpp
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
  menu_button_visitor_test.cpp
  menu_item_test.cpp
  menu_item_extraction_test.cpp
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

include(max_warnings)


# DLL used for DLL-function load testing

add_library(
  load_test_dll SHARED
  load_test_dll/load_test_dll.h
  load_test_dll/load_test_dll.cpp
  load_test_dll/load_test_dll.def)

# End DLL

set(Boost_USE_STATIC_LIBS TRUE)
find_package(
  Boost 1.40 REQUIRED
  COMPONENTS filesystem system unit_test_framework)

add_executable(tests ${TEST_SOURCES})
target_include_directories(tests PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(tests
  PRIVATE washer load_test_dll ${Boost_LIBRARIES})
target_compile_definitions(tests PRIVATE BOOST_ALL_NO_LIB=1)

add_executable(tests_unicode ${TEST_SOURCES})
target_include_directories(tests_unicode PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(tests_unicode
  PRIVATE washer load_test_dll ${Boost_LIBRARIES})
target_compile_definitions(tests_unicode PRIVATE BOOST_ALL_NO_LIB=1 _UNICODE)

add_executable(tests_win9x ${TEST_SOURCES})
target_include_directories(tests_win9x PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(tests_win9x
  PRIVATE washer load_test_dll ${Boost_LIBRARIES})
target_compile_definitions(tests_win9x PRIVATE
  BOOST_ALL_NO_LIB=1 WINVER=0x0400 _WIN32_WINNT=0x0400)

set(TEST_RUNNER_ARGUMENTS
  --catch_system_errors --detect_memory_leaks
  --result_code=no --log_level=test_suite)

add_test(tests tests ${TEST_RUNNER_ARGUMENTS})
add_test(tests_unicode tests_unicode ${TEST_RUNNER_ARGUMENTS})
add_test(tests_win9x tests_win9x ${TEST_RUNNER_ARGUMENTS})
