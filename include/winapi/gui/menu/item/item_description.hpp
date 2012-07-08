/**
    @file

    Interface to objects describing menu items.

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

#ifndef WINAPI_GUI_MENU_ITEM_ITEM_DESCRIPTION_HPP
#define WINAPI_GUI_MENU_ITEM_ITEM_DESCRIPTION_HPP
#pragma once

#include <cassert> // assert

#include <Windows.h> // MENUITEMINFO

namespace winapi {
namespace gui {
namespace menu {

/**
 * Interface to objects that describe (but aren't) menu items.
 *
 * Purpose: to separate the specification of a menu item we are about to
 *          create, from its identity once it exists in the menu.  The latter
 *          may have properties that only exist once it is created so cannot
 *          be represented by the former.
 *
 * Reason for having separate classes: menu items could have been created by
 * passing the properties directly to methods of the menu which would create the
 * menu item object as it inserts it.  But making the descriptions also objects
 * means they can be created by factories more easily or created preemptively
 * and not used.
 *
 * This is similar to a Builder design pattern except that the built object
 * is extracted as a Win32 MENUITEMINFO struct by the menu when passed to its
 * `insert` method.
 */
class item_description
{
    // To allow basic_menu to be the only class that can see our
    // underlying Win32 representation
    template<typename,typename,typename> friend class basic_menu;

public:

    virtual ~item_description() {}

    item_description* clone() const
    {
        item_description* item = do_clone();
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

    virtual item_description* do_clone() const = 0;
};

inline item_description* new_clone(const item_description& item)
{
    return item.clone();
}

}}} // namespace winapi::gui::menu

#endif
