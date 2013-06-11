cmake_minimum_required(VERSION 2.6)

project(winapi)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(INCLUDE_DIRECTORY include)
set(LIBRARY_DIRECTORY ${INCLUDE_DIRECTORY}/winapi)

include_directories(${INCLUDE_DIRECTORY})
link_libraries(Shlwapi)

set(Boost_USE_STATIC_LIBS TRUE)
set(Boost_DETAILED_FAILURE_MSG TRUE)
set(Boost_ADDITIONAL_VERSIONS 1.46.1 1.50 1.50.0 1.51 1.51.0)
find_package(
  Boost 1.40 REQUIRED
  COMPONENTS filesystem system)
link_directories(${Boost_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../comet/include)

# Find comet library - not required for much of the Windows API
find_package(Comet)
if(COMET_FOUND)
  include_directories(${COMET_INCLUDE_DIRS})
endif()

add_definitions(
  -DWIN32_LEAN_AND_MEAN -DSTRICT_TYPED_ITEMIDS)

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
  ${LIBRARY_DIRECTORY}/com/object.hpp
  ${LIBRARY_DIRECTORY}/com/ole_window.hpp)
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

set(LIBRARY_SOURCES_gui_menu
  ${LIBRARY_DIRECTORY}/gui/menu/basic_menu.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/menu.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/menu_handle.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/visitor.hpp)
source_group(winapi\\gui\\menu FILES ${LIBRARY_SOURCES_gui_menu})

set(LIBRARY_SOURCES_gui_menu_button
  ${LIBRARY_DIRECTORY}/gui/menu/button/bitmap_button.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/button/bitmap_button_description.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/button/button.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/button/button_description.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/button/owner_drawn_button.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/button/owner_drawn_button_description.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/button/string_button.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/button/string_button_description.hpp)
source_group(winapi\\gui\\menu\\button FILES ${LIBRARY_SOURCES_gui_menu_button})

set(LIBRARY_SOURCES_gui_menu_detail
  ${LIBRARY_DIRECTORY}/gui/menu/detail/item_common.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/detail/item_position.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/detail/menu.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/detail/item_iterator.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/detail/menu_win32.hpp)
source_group(winapi\\gui\\menu\\detail FILES ${LIBRARY_SOURCES_gui_menu_detail})

set(LIBRARY_SOURCES_gui_menu_item
  ${LIBRARY_DIRECTORY}/gui/menu/item/command_item.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/item/command_item_description.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/item/item.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/item/item_description.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/item/item_state.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/item/selectable_item.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/item/selectable_item_description.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/item/separator_item.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/item/separator_item_description.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/item/sub_menu_item.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/item/sub_menu_item_description.hpp)
source_group(winapi\\gui\\menu\\item FILES ${LIBRARY_SOURCES_gui_menu_item})

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

set(LIBRARY_SOURCES_window
  ${LIBRARY_DIRECTORY}/window/dialog.hpp
  ${LIBRARY_DIRECTORY}/window/icon.hpp
  ${LIBRARY_DIRECTORY}/window/window.hpp
  ${LIBRARY_DIRECTORY}/window/window_handle.hpp)
source_group(winapi\\window FILES ${LIBRARY_SOURCES_window})

set(LIBRARY_SOURCES_window_detail
  ${LIBRARY_DIRECTORY}/window/detail/window.hpp
  ${LIBRARY_DIRECTORY}/window/detail/window_win32.hpp)
source_group(
  winapi\\window\\detail FILES ${LIBRARY_SOURCES_window_detail})

add_custom_target(winapi SOURCES
  ${LIBRARY_SOURCES}
  ${LIBRARY_SOURCES_com}
  ${LIBRARY_SOURCES_detail}
  ${LIBRARY_SOURCES_gui}
  ${LIBRARY_SOURCES_gui_menu}
  ${LIBRARY_SOURCES_gui_menu_button}
  ${LIBRARY_SOURCES_gui_menu_detail}
  ${LIBRARY_SOURCES_gui_menu_item}
  ${LIBRARY_SOURCES_window}
  ${LIBRARY_SOURCES_window_detail}
  ${LIBRARY_SOURCES_shell})

add_subdirectory(test)

# Target to generate documentation with Doxygen
option(BUILD_DOCS "Build documentation if Doxygen is available" ON)
if(BUILD_DOCS)

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

endif()
