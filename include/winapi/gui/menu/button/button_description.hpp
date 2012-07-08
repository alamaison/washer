/**
    @file

    Interface of menu button builders.

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

#ifndef WINAPI_GUI_MENU_BUTTON_BUTTON_DESCRIPTION_HPP
#define WINAPI_GUI_MENU_BUTTON_BUTTON_DESCRIPTION_HPP
#pragma once

#include <cassert> // assert

#include <Windows.h> // MENUITEMINFO

namespace winapi {
namespace gui {
namespace menu {

/**
 * Interface to objects that describe (but aren't) menu item buttons.
 *
 * Purpose: to separate the specification of a menu item we are about to
 *          create, from its identity once it exists in the menu.
 *
 * Command items and submenu items have buttons and separator items do not.
 * The button type is independent of the other appearance properties such as
 * whether an item is enabled or checked.
 */
class button_description
{
    friend class command_item_description;
    friend class sub_menu_item_description;

public:

    virtual ~button_description() {}

    button_description* clone() const
    {
        button_description* item = do_clone();
        assert(typeid(*this) == typeid(*item) && "do_clone() sliced object!");
        return item;
    }

private:

    /**
     * Win32 representation of the button as a partially-completed MENUITEMINFO.
     *
     * Classes that implement menu buttons expose themselves to the menu
     * classes that use them through this method.
     */
    virtual MENUITEMINFOW as_menuiteminfo() const = 0;

    virtual button_description* do_clone() const = 0;
};

}}} // namespace winapi::gui::menu

#endif
