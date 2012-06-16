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

set(LIBRARY_SOURCES_gui_detail
  ${LIBRARY_DIRECTORY}/gui/detail/window_win32.hpp)
source_group(winapi\\gui\\detail FILES ${LIBRARY_SOURCES_gui_detail})

set(LIBRARY_SOURCES_gui_menu
  ${LIBRARY_DIRECTORY}/gui/menu/buttons.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/item_descriptions.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/items.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/menu.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/menu_bar.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/menu_handle.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/menu_item.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/menu_item_description.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/selectable_menu_item.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/selectable_menu_item_description.hpp)
source_group(winapi\\gui\\menu FILES ${LIBRARY_SOURCES_gui_menu})

set(LIBRARY_SOURCES_gui_menu_detail
  ${LIBRARY_DIRECTORY}/gui/menu/detail/menu.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/detail/menu_common_core.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/detail/menu_item_iterator.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/detail/menu_win32.hpp)
source_group(winapi\\gui\\menu\\detail FILES ${LIBRARY_SOURCES_gui_menu_detail})

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
  ${LIBRARY_SOURCES_gui_detail}
  ${LIBRARY_SOURCES_gui_menu}
  ${LIBRARY_SOURCES_gui_menu_detail}
  ${LIBRARY_SOURCES_gui_windows}
  ${LIBRARY_SOURCES_shell})

add_subdirectory(test)

# Target to generate documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)

  get_property(
    INCLUDE_LIST
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
    PROPERTY INCLUDE_DIRECTORIES)

  foreach(arg ${INCLUDE_LIST})
    set(DOXY_INCLUDE_DIRS "${DOXY_INCLUDE_DIRS} \"${arg}\"")
  endforeach()

  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

  add_custom_target(
    doc ALL
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating documentation with Doxygen" VERBATIM)
endif()
