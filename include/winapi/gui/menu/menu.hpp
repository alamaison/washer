/**
    @file

    Windows HMENU menu wrapper for menus and menu bars.

    @if license

    Copyright (C) 2012  Alexander Lamaison <awl03@doc.ic.ac.uk>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    If you modify this Program, or any covered work, by linking or
    combining it with the OpenSSL project's OpenSSL library (or a
    modified version of that library), containing parts covered by the
    terms of the OpenSSL or SSLeay licenses, the licensors of this
    Program grant you additional permission to convey the resulting work.

    @endif
*/

#ifndef WINAPI_GUI_MENU_MENU_HPP
#define WINAPI_GUI_MENU_MENU_HPP
#pragma once

#include <winapi/gui/menu/basic_menu.hpp>
#include <winapi/gui/menu/detail/menu.hpp> 
                                 // menu_handle_creator, menu_bar_handle_creator
#include <winapi/gui/menu/menu_item_description.hpp>
#include <winapi/gui/menu/selectable_menu_item_description.hpp>

namespace winapi {
namespace gui {
namespace menu {

namespace detail {

    class sub_menu_description_befriender;
    class window_class_befriender;

}

/**
 * Menu that can be used as a context menu, as a sub-menu of a menu bar or as
 * a sub-menu of another menu.
 */
typedef basic_menu<
    menu_item_description, detail::menu_handle_creator,
    detail::sub_menu_description_befriender> menu;

/**
 * Main application window menu bar.
 */
typedef basic_menu<
    selectable_menu_item_description, detail::menu_bar_handle_creator,
    detail::window_class_befriender> menu_bar;

}}} // namespace winapi::gui::menu

#include <winapi/gui/menu/detail/befrienders.ipp>

#endif
