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

#ifndef WINAPI_GUI_MENU_SELECTABLE_MENU_ITEM_HPP
#define WINAPI_GUI_MENU_SELECTABLE_MENU_ITEM_HPP
#pragma once

#include <boost/shared_ptr.hpp>

namespace winapi {
namespace gui {
namespace menu {

class menu_button_nature;

/**
 * Interface to items that, when clicked, might result in an action.
 *
 * These items have an clickable appearance which we are calling a ''button''.
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
class selectable_menu_item
{
public:

    virtual ~selectable_menu_item() {}

    /**
     * Enable or disable the menu item's button.
     *
     * Will grey the button and prevent it reacting when hovered over or
     * clicked.
     */
    virtual selectable_menu_item& selectability(
        BOOST_SCOPED_ENUM(selectability) state) = 0;

    /**
     * Add or remove a check mark from beside the menu item.
     *
     * @note  Even popup sub-menu items can have a check mark, odd as that
     *        may seem.
     * @note  This has no visible effect on items in menu bars.
     */
    virtual selectable_menu_item& check_mark_visibility(
        BOOST_SCOPED_ENUM(check_mark) state) = 0;

    /**
     * The menu item's clickable appearance.
     */
    virtual boost::shared_ptr<menu_button_nature> button() const = 0;

    virtual bool is_default() const = 0;
    virtual bool is_enabled() const = 0;
    virtual bool is_checked() const = 0;
    virtual bool is_highlighted() const = 0;
};

}}} // namespace winapi::gui::menu

#endif
