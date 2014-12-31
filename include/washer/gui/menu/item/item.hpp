/**
    @file

    Type-agnostic representation of an item in a menu.

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

#ifndef WASHER_GUI_MENU_ITEM_ITEM_HPP
#define WASHER_GUI_MENU_ITEM_ITEM_HPP
#pragma once

#include <washer/gui/menu/detail/item_position.hpp>

#include <Windows.h> // MENUITEMINFO

namespace washer {
namespace gui {
namespace menu {

namespace detail {

    // The specific item visitation has been moved to the specific item's header
    // file to prevent a header dependency cycle.  It look strange but is
    // necessary.  These forward-declarations allow us to call the
    // specific visitation below without including its implementation.
    template<typename Visitor>
    typename Visitor::result_type do_command_item_accept(
        const item_position&, Visitor&);

    template<typename Visitor>
    typename Visitor::result_type do_separator_item_accept(
        const item_position&, Visitor&);

    template<typename Visitor>
    typename Visitor::result_type do_sub_menu_item_accept(
        const item_position&, Visitor&);

    template<typename Visitor>
    typename Visitor::result_type do_const_command_item_accept(
        const item_position&, Visitor&);

    template<typename Visitor>
    typename Visitor::result_type do_const_separator_item_accept(
        const item_position&, Visitor&);

    template<typename Visitor>
    typename Visitor::result_type do_const_sub_menu_item_accept(
        const item_position&, Visitor&);

}

/**
 * Type-agnostic representation of an item in a menu.
 *
 * Purpose: to allow code to reason in terms of an item in a menu at a
 *          particular position and to allow callers to convert that position
 *          to the specific typed representation safely.
 *
 * Menus entries can change their type either by an insertion/removal
 * shuffling items around or by explicitly setting an item.  Therefore, the
 * typed representations (`command_item`, `sub_menu_item` and
 * `separator_item`) must not be stored and used later as, by that time, their
 * underlying type in the Win32 menu may have become incompatible.
 *
 * Therefore, this class represents items blind to their type and only allows
 * access to the typed view of them via references to non-copyable objects
 * created when accepting a visitor.  These object cease to exist after the
 * visitor has returned which means the types cannot diverge from the
 * underlying menu: if the underlying menu type changes then a different typed
 * class is generated the next time a visitor is accepted.
 */
class item
{
    // to let basic_menu be our factory - the only class that can invoke
    // our private constructor
    template<typename,typename> friend class basic_menu;

public:

    /**
     * Accept a visitor to access type-safe, mutable view of the menu item.
     *
     * Visitors passed to this method must provide at least one
     * implementation of `operator()` that can accept arguments of types
     * `separator_item&`, `command_item&` and `sub_menu_item&` as well
     * as a `result_type` type.
     */
    template<typename Visitor>
    typename Visitor::result_type accept(Visitor visitor)
    {
        // MIIM_SUBMENU is part of the mask so isn't set by GetMenuItemInfo.
        // We have to set it and check hSubMenu to decide if this is a popup
        // item.
        // Using MIIM_TYPE instead of MIIM_FTYPE for backward compatibility
        // with Win 98.
        //const MENUITEMINFOW info =
        //    m_item.get_menuiteminfo(MIIM_FTYPE | MIIM_SUBMENU);
        const MENUITEMINFOW info =
            m_item.get_menuiteminfo(MIIM_TYPE | MIIM_SUBMENU);

        // Four mutually-exclusive menu possibilities.  The first, separator
        // determines the entire menu item behaviour while the others just
        // indicate the button type (separators have no button).
        if (info.fType & MFT_SEPARATOR)
        {
            // In reality, a separator can be forced to have a submenu but as
            // this is clearly not what is intended, we don't give a way to get
            // access to it.
            return do_separator_item_accept(m_item, visitor);
        }
        else
        {
            if (info.hSubMenu)
            {
                return do_sub_menu_item_accept(m_item, visitor);
            }
            else
            {
                return do_command_item_accept(m_item, visitor);
            }
        }
    }


    /**
     * Accept a visitor to access type-safe, immutable view of the menu item.
     *
     * Visitors passed to this method must provide at least one
     * implementation of `operator()` that can accept arguments of types
     * `const separator_item&`, `const command_item&` and
     * `const sub_menu_item&` as well as a `result_type` type.
     */
    template<typename Visitor>
    typename Visitor::result_type accept(Visitor visitor) const
    {
        // As above, but presents the concrete items to the visitor as
        // const items.

        const MENUITEMINFOW info =
            m_item.get_menuiteminfo(MIIM_TYPE | MIIM_SUBMENU);

        if (info.fType & MFT_SEPARATOR)
        {
            return do_const_separator_item_accept(m_item, visitor);
        }
        else
        {
            if (info.hSubMenu)
            {
                return do_const_sub_menu_item_accept(m_item, visitor);
            }
            else
            {
                return do_const_command_item_accept(m_item, visitor);
            }
        }
    }

private:

    explicit item(const detail::item_position& item) : m_item(item) {}

    detail::item_position m_item;
};

}}} // namespace washer::gui::menu

#endif
