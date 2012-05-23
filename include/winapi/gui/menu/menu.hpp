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
class menu
{
    template<typename T> friend class sub_menu;

    typedef int iterator_type;

public:

    /**
     * Create empty menu.
     */
    menu()
        :
    m_menu(
        detail::menu_handle(
            detail::win32::create_popup_menu(), detail::safe_destroy_menu))
    {}

    /**
     * Returns the number of items in the menu.
     */
    size_t size() const
    {
        return detail::win32::get_menu_item_count(m_menu.get());
    }

    /**
     * Appends an item onto the end of the menu.
     */
    void append(const menu_item& menu)
    {
        insert_at_position(menu, -1);
    }

    iterator_type begin() { return 1; }
    iterator_type end() { return 1; }

private:

    void insert_at_position(const menu_item& menu, UINT position)
    {
        MENUITEMINFOW info = menu.as_menuiteminfo();
        // BUG: this function takes ownership of the submenu HMENU but the
        // passed-in menu will still try to destroy it
        detail::win32::insert_menu_item(m_menu.get(), position, TRUE, &info);
    }

    const detail::menu_handle m_menu;
};

}}} // namespace winapi::gui::menu

#endif
