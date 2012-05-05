cmake_minimum_required(VERSION 2.6)

project(winapi-test)

set(INCLUDE_DIRECTORY ../include)
set(LIBRARY_DIRECTORY ${INCLUDE_DIRECTORY}/winapi)

include_directories(${INCLUDE_DIRECTORY})
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
  
set(LIBRARY_SOURCES
  ${LIBRARY_DIRECTORY}/clipboard.hpp
  ${LIBRARY_DIRECTORY}/dynamic_link.hpp
  ${LIBRARY_DIRECTORY}/error.hpp
  ${LIBRARY_DIRECTORY}/filesystem.hpp
  ${LIBRARY_DIRECTORY}/global_lock.hpp
  ${LIBRARY_DIRECTORY}/hook.hpp
  ${LIBRARY_DIRECTORY}/message.hpp
  ${LIBRARY_DIRECTORY}/object_with_site.hpp
  ${LIBRARY_DIRECTORY}/trace.hpp)
source_group(winapi FILES ${LIBRARY_SOURCES})

set(LIBRARY_SOURCES_com
  ${LIBRARY_DIRECTORY}/com/catch.hpp
  ${LIBRARY_DIRECTORY}/com/object.hpp)
source_group(winapi\\com FILES ${LIBRARY_SOURCES_com})

set(LIBRARY_SOURCES_detail
  ${LIBRARY_DIRECTORY}/detail/path_traits.hpp)
source_group(winapi\\detail FILES ${LIBRARY_SOURCES_detail})

set(LIBRARY_SOURCES_gui
  ${LIBRARY_DIRECTORY}/gui/commands.hpp
  ${LIBRARY_DIRECTORY}/gui/hwnd.hpp
  ${LIBRARY_DIRECTORY}/gui/icon.hpp
  ${LIBRARY_DIRECTORY}/gui/message_box.hpp
  ${LIBRARY_DIRECTORY}/gui/messages.hpp
  ${LIBRARY_DIRECTORY}/gui/progress.hpp
  ${LIBRARY_DIRECTORY}/gui/task_dialog.hpp)
source_group(winapi\\gui FILES ${LIBRARY_SOURCES_gui})

set(LIBRARY_SOURCES_gui_windows
  ${LIBRARY_DIRECTORY}/gui/windows/dialog.hpp
  ${LIBRARY_DIRECTORY}/gui/windows/icon.hpp
  ${LIBRARY_DIRECTORY}/gui/windows/window.hpp)
source_group(winapi\\gui\\windows FILES ${LIBRARY_SOURCES_gui_windows})

set(LIBRARY_SOURCES_shell
  ${LIBRARY_DIRECTORY}/shell/folder_error_adapters.hpp
  ${LIBRARY_DIRECTORY}/shell/folder_interfaces.hpp
  ${LIBRARY_DIRECTORY}/shell/format.hpp
  ${LIBRARY_DIRECTORY}/shell/pidl.hpp
  ${LIBRARY_DIRECTORY}/shell/pidl_array.hpp
  ${LIBRARY_DIRECTORY}/shell/pidl_iterator.hpp
  ${LIBRARY_DIRECTORY}/shell/property_key.hpp
  ${LIBRARY_DIRECTORY}/shell/services.hpp
  ${LIBRARY_DIRECTORY}/shell/shell.hpp)
source_group(winapi\\shell FILES ${LIBRARY_SOURCES_shell})

add_custom_target(winapi SOURCES
  ${LIBRARY_SOURCES}
  ${LIBRARY_SOURCES_com}
  ${LIBRARY_SOURCES_detail}
  ${LIBRARY_SOURCES_gui}
  ${LIBRARY_SOURCES_gui_windows}
  ${LIBRARY_SOURCES_shell})
  
set(TEST_SOURCES
  wchar_output.hpp
  dynamic_link_test.cpp
  filesystem_test.cpp
  folder_error_adapter_test.cpp
  format_test.cpp
  global_lock_test.cpp
  hook_test.cpp
  icon_test.cpp
  module.cpp
  pidl_iterator_test.cpp
  pidl_test.cpp
  progress_test.cpp
  shell_test.cpp
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
