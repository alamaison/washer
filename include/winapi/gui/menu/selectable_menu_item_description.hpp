/**
    @file

    Interface to objects describing selectable menu items.

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

#ifndef WINAPI_GUI_MENU_SELECTABLE_MENU_ITEM_DESCRIPTION_HPP
#define WINAPI_GUI_MENU_SELECTABLE_MENU_ITEM_DESCRIPTION_HPP
#pragma once

#include <winapi/gui/menu/menu_item_description.hpp>

#include <Windows.h> // MENUITEMINFO

namespace winapi {
namespace gui {
namespace menu {

/**
 * Interface to objects that describe (but aren't) menu items that, when 
 * clicked, might result in an action.
 *
 * Such items have an clickable appearance which we are calling a ''button''.
 * It is commonly a text caption but can also be a bitmap or a custom drawn
 * item.  Separators are the only non-selectable menu item.
 *
 * Selectable menu items are the only items that can appear in menu bars.  In
 * other words, menu bars don't allow separators.
 *
 * @note  It is possible to put a separator into a menu item using the windows
 *        API but it's forbidden by the documentation.  Similarly, you can
 *        add string captions, add submenus and force-enable the separator.
 *        However, as these are invalid uses of a separator, we don't support
 *        them.
 */
class selectable_menu_item_description : public menu_item_description
{
    // To allow basic_menu to be the only class that can see our
    // underlying Win32 representation
    template<typename,typename,typename> friend class basic_menu;

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
