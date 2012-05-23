/**
    @file

    Interface to items which can only be added to regular menus, not menu bars.

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

#ifndef WINAPI_GUI_MENU_MENU_ITEM_HPP
#define WINAPI_GUI_MENU_MENU_ITEM_HPP
#pragma once

#include <cassert> // assert

#include <Windows.h> // MENUITEMINFO

namespace winapi {
namespace gui {
namespace menu {

/**
 * Interface to items which can only be added to regular menus, not menu bars.
 */
class menu_item
{
    friend class menu;

public:

    virtual ~menu_item() {}

    menu_item* clone() const
    {
        menu_item* item = do_clone();
        assert(typeid(*this) == typeid(*item) && "do_clone() sliced object!");
        return item;
    }

private:

    /**
     * Win32 representation of the menu item as a completed MENUITEMINFO.
     *
     * Classes that implement menu items expose themselves to the menu class
     * through this method.
     */
    virtual MENUITEMINFOW as_menuiteminfo() const = 0;

    virtual menu_item* do_clone() const = 0;
};

inline menu_item* new_clone(const menu_item& item)
{
    return item.clone();
}

}}} // namespace winapi::gui::menu

#endif
