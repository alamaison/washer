cmake_minimum_required(VERSION 2.6)

project(winapi)

set(INCLUDE_DIRECTORY include)
set(LIBRARY_DIRECTORY ${INCLUDE_DIRECTORY}/winapi)

include_directories(${INCLUDE_DIRECTORY})
link_libraries(Shlwapi)

set(Boost_USE_STATIC_LIBS TRUE)
set(Boost_DETAILED_FAILURE_MSG TRUE)
set(Boost_ADDITIONAL_VERSIONS 1.46.1)
find_package(
  Boost 1.40 REQUIRED
  COMPONENTS filesystem system)
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
  ${LIBRARY_DIRECTORY}/shell/shell.hpp
  ${LIBRARY_DIRECTORY}/shell/shell_item.hpp)
source_group(winapi\\shell FILES ${LIBRARY_SOURCES_shell})

add_custom_target(winapi SOURCES
  ${LIBRARY_SOURCES}
  ${LIBRARY_SOURCES_com}
  ${LIBRARY_SOURCES_detail}
  ${LIBRARY_SOURCES_gui}
  ${LIBRARY_SOURCES_gui_windows}
  ${LIBRARY_SOURCES_shell})

add_subdirectory(test)