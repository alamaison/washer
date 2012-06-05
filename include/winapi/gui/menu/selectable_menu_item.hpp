/**
    @file

    Interface to items that can appear in menu bars as well as menus.

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

#ifndef WINAPI_GUI_MENU_MENU_BAR_ITEM_HPP
#define WINAPI_GUI_MENU_MENU_BAR_ITEM_HPP
#pragma once

#include <winapi/gui/menu/menu_item.hpp>

#include <Windows.h> // MENUITEMINFO

namespace winapi {
namespace gui {
namespace menu {

namespace detail {

template<typename>
class menu_common_core;

}

/**
 * Interface to items that can appear in menu bars as well as menus.
 *
 * Anything that can be added to a menu bar can also be added to a regular menu.
 */
class selectable_menu_item : public menu_item
{
    friend class detail::menu_common_core<selectable_menu_item>;

private:

    /**
     * Win32 representation of the menu-bar item as a completed MENUITEMINFO.
     *
     * Classes that implement menu items expose themselves to the menu and
     * menu-bar classes through this method.
     */
    virtual MENUITEMINFOW as_menuiteminfo() const = 0;
};

}}} // namespace winapi::gui::menu

#endif
