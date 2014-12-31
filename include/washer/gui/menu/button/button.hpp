/**
    @file

    Clickable area of a menu item.

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

#ifndef WASHER_GUI_MENU_BUTTON_BUTTON_HPP
#define WASHER_GUI_MENU_BUTTON_BUTTON_HPP
#pragma once

#include <washer/gui/menu/button/bitmap_button.hpp>
#include <washer/gui/menu/button/string_button.hpp>
#include <washer/gui/menu/button/owner_drawn_button.hpp>
#include <washer/gui/menu/detail/item_position.hpp>

namespace washer {
namespace gui {
namespace menu {

class bitmap_button;
class string_button;
class owner_drawn_button;

/**
 * Menu item button.
 *
 * Purpose: to separate the button-appearance aspects of menus from other
 * orthogonal aspects which are muddled in with it in the Win32 API.
 *
 * Command items and submenu items have buttons and separator items do not.
 * The button type is independent of the other appearance properties such as
 * whether an item is enabled or checked.
 *
 * Buttons come in several different flavours and this can vary at runtime so
 * this class provides the `accept` method that takes a visitor so that code
 * operating on specific types of button can do so in a type-safe manner.
 *
 * Changes made using the specific button instance exposed by the visitor
 * are reflected in the appearance of the menu item's clickable area in the
 * menu.
 */
class button
{
public:

    /**
     * Accept a visitor to access type-safe, mutable view of the button.
     *
     * Visitors passed to this method must provide at least one
     * implementation of `operator()` that can accept arguments of types
     * `bitmap_button&`, `string_button&` and `owner_drawn_button&` as well
     * as a `result_type` type.
     */
    template<typename Visitor>
    typename Visitor::result_type accept(Visitor visitor)
    {
        // Should really be MIIM_FTYPE here but using MIIM_TYPE for
        // backward compatibility
        UINT button_type_flag = m_item.get_menuiteminfo(MIIM_TYPE).fType;

        if (button_type_flag & MFT_BITMAP)
        {
            bitmap_button button_view(m_item);
            return visitor(button_view);
        }
        else if (button_type_flag & MFT_OWNERDRAW)
        {
            // MSDN doesn't say that MFT_OWNERDRAWN is part of the
            // mutually exclusive set of flags but I don't see how it couldn't
            // be.  These flags specify the meaning of dwTypeData and cch which
            // MFT_OWNERDRAWN gives a user-defined meaning.
            owner_drawn_button button_view(m_item);
            return visitor(button_view);
        }
        else
        {
            // MFT_STRING which is zero so can only be detected by elimination
            string_button button_view(m_item);
            return visitor(button_view);
        }
    }

    explicit button(const detail::item_position& item) : m_item(item) {}

private:

    detail::item_position m_item;
};

}}} // namespace washer::gui::menu

#endif
