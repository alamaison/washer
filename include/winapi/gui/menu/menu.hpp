/**
    @file

    Standard Windows HMENU menu wrapper.

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

#include <winapi/gui/menu/detail/menu.hpp> // menu_handle, safe_destroy_menu
#include <winapi/gui/menu/detail/menu_common_core.hpp>
#include <winapi/gui/menu/detail/menu_item_iterator.hpp>
#include <winapi/gui/menu/detail/menu_win32.hpp> // create_popup_menu
#include <winapi/gui/menu/menu_item.hpp>

#include <Windows.h> // MENUITEMINFO

namespace winapi {
namespace gui {
namespace menu {

/**
 * Menu that can be used as a context menu, as a sub-menu of a menu bar or as
 * a sub-menu of another menu.
 *
 * @todo  Add a method that can insert at an iterator position.
 */
class menu : private detail::menu_common_core<menu_item>
{
    template<typename T> friend class sub_menu;

    typedef detail::menu_common_core<menu_item> core;
    typedef int iterator_type;

public:

    /**
     * Create empty menu.
     */
    menu()
        :
    core(
        detail::menu_handle(
            detail::win32::create_popup_menu(), detail::safe_destroy_menu))
    {}

    /**
     * Returns the number of items in the menu.
     */
    using core::size;

    /**
     * Appends an item onto the end of the menu.
     */
    using core::append;

    /**
     * Insert an item into the menu at the given iterator position.
     *
     * Shuffles existing items along.
     */
    using core::insert;

    using core::begin;
    using core::end;

private:

    /** To allow sub_menu::as_menuiteminfo to access raw HWND. */
    using core::handle;
};

}}} // namespace winapi::gui::menu

#endif
