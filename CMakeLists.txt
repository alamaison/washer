cmake_minimum_required(VERSION 3.0.0) # Interface libraries require v3

project(washer VERSION 0.1.0)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

option(BUILD_TESTING "Build test suite" ON)
option(BUILD_DOCS "Build documentation if Doxygen is available" ON)

# Package management ###########################################################

include(HunterGate)

HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.7.3.tar.gz"
    SHA1 "ff6aaa393d32db13e353e4452fb679fc26e15efb")

hunter_add_package(Comet)
hunter_add_package(Boost COMPONENTS filesystem system thread date_time)
if(BUILD_TESTING)
  hunter_add_package(Boost COMPONENTS test)
endif()

################################################################################

set(LIBRARY_DIRECTORY include/washer)

set(LIBRARY_SOURCES
  ${LIBRARY_DIRECTORY}/clipboard.hpp
  ${LIBRARY_DIRECTORY}/dynamic_link.hpp
  ${LIBRARY_DIRECTORY}/error.hpp
  ${LIBRARY_DIRECTORY}/filesystem.hpp
  ${LIBRARY_DIRECTORY}/global_lock.hpp
  ${LIBRARY_DIRECTORY}/hook.hpp
  ${LIBRARY_DIRECTORY}/message.hpp
  ${LIBRARY_DIRECTORY}/object_with_site.hpp
  ${LIBRARY_DIRECTORY}/trace.hpp
  ${LIBRARY_DIRECTORY}/com/catch.hpp
  ${LIBRARY_DIRECTORY}/com/object.hpp
  ${LIBRARY_DIRECTORY}/com/ole_window.hpp
  ${LIBRARY_DIRECTORY}/detail/path_traits.hpp
  ${LIBRARY_DIRECTORY}/detail/remove_calling_convention.hpp
  ${LIBRARY_DIRECTORY}/gui/commands.hpp
  ${LIBRARY_DIRECTORY}/gui/hwnd.hpp
  ${LIBRARY_DIRECTORY}/gui/message_box.hpp
  ${LIBRARY_DIRECTORY}/gui/messages.hpp
  ${LIBRARY_DIRECTORY}/gui/progress.hpp
  ${LIBRARY_DIRECTORY}/gui/task_dialog.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/basic_menu.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/menu.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/menu_handle.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/visitor.hpp
  ${LIBRARY_DIRECTORY}/gui/icon/icon.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/button/bitmap_button.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/button/bitmap_button_description.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/button/button.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/button/button_description.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/button/owner_drawn_button.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/button/owner_drawn_button_description.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/button/string_button.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/button/string_button_description.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/detail/item_common.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/detail/item_position.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/detail/menu.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/detail/item_iterator.hpp
  ${LIBRARY_DIRECTORY}/gui/menu/detail/menu_win32.hpp
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
  ${LIBRARY_DIRECTORY}/gui/menu/item/sub_menu_item_description.hpp
  ${LIBRARY_DIRECTORY}/shell/folder_error_adapters.hpp
  ${LIBRARY_DIRECTORY}/shell/folder_interfaces.hpp
  ${LIBRARY_DIRECTORY}/shell/format.hpp
  ${LIBRARY_DIRECTORY}/shell/pidl.hpp
  ${LIBRARY_DIRECTORY}/shell/pidl_array.hpp
  ${LIBRARY_DIRECTORY}/shell/pidl_iterator.hpp
  ${LIBRARY_DIRECTORY}/shell/property_key.hpp
  ${LIBRARY_DIRECTORY}/shell/services.hpp
  ${LIBRARY_DIRECTORY}/shell/shell.hpp
  ${LIBRARY_DIRECTORY}/shell/shell_item.hpp
  ${LIBRARY_DIRECTORY}/window/dialog.hpp
  ${LIBRARY_DIRECTORY}/window/icon.hpp
  ${LIBRARY_DIRECTORY}/window/window.hpp
  ${LIBRARY_DIRECTORY}/window/window_handle.hpp
  ${LIBRARY_DIRECTORY}/window/detail/window.hpp
  ${LIBRARY_DIRECTORY}/window/detail/window_win32.hpp)

add_custom_target(washer-src SOURCES ${LIBRARY_SOURCES})


# Interface library for usage requirements

add_library(washer INTERFACE)
target_include_directories(washer
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

install(
  DIRECTORY ${LIBRARY_DIRECTORY}
  DESTINATION include
  FILES_MATCHING PATTERN "*.hpp")
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY ON)

target_link_libraries(washer INTERFACE Shlwapi)
target_compile_definitions(washer
  INTERFACE
    WIN32_LEAN_AND_MEAN
    STRICT_TYPED_ITEMIDS)

find_package(Comet 4.0.0 REQUIRED CONFIG)
target_link_libraries(washer INTERFACE Comet::comet)

set(Boost_USE_STATIC_LIBS TRUE)
find_package(
  Boost 1.40 REQUIRED
  COMPONENTS filesystem system thread date_time)
target_include_directories(washer INTERFACE ${Boost_INCLUDE_DIRS})
target_link_libraries(washer INTERFACE ${Boost_LIBRARIES})

# Package config

## During package installation, install WasherConfig.cmake
install(TARGETS washer EXPORT WasherConfig)
install(EXPORT WasherConfig NAMESPACE Washer:: DESTINATION lib/cmake/Washer)

## Directly from build tree
# create WasherConfig.cmake
export(TARGETS washer NAMESPACE Washer:: FILE WasherConfig.cmake)
export(PACKAGE Washer) # register it

## Versioning

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/WasherConfigVersion.cmake
  VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}"
  COMPATIBILITY SameMajorVersion)
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/WasherConfigVersion.cmake
  DESTINATION lib/cmake/Washer)

# Tests

if(BUILD_TESTING)
  enable_testing()
  add_subdirectory(test)
endif()

# Docs

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
