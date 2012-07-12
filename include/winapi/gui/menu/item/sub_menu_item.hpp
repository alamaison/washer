/**
    @file

    Item that pops open another menu.

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

#ifndef WINAPI_GUI_MENU_ITEM_SUB_MENU_ITEM_HPP
#define WINAPI_GUI_MENU_ITEM_SUB_MENU_ITEM_HPP
#pragma once

#include <winapi/gui/menu/detail/item_common.hpp> // selectable_item_core
#include <winapi/gui/menu/detail/item_position.hpp>
#include <winapi/gui/menu/menu.hpp>
#include <winapi/gui/menu/menu_handle.hpp>
#include <winapi/gui/menu/item/selectable_item.hpp>

#include <boost/noncopyable.hpp>

namespace winapi {
namespace gui {
namespace menu {

/**
 * Menu nested in or below main menu bar.
 *
 * Purpose: to extract an item with a submenu from an HMENU.
 */
class sub_menu_item : public selectable_item, private boost::noncopyable
{
    friend class item;

public:

    /**
     * The ID of the item.
     *
     * Used to find an item in a menu even if its position changes.
     */
    UINT id() const
    {
        return m_core.id();
    }

    virtual sub_menu_item& selectability(
        BOOST_SCOPED_ENUM(selectability) state)
    {
        m_core.button_state(state);
        return *this;
    }

    virtual sub_menu_item& check_mark_visibility(
        BOOST_SCOPED_ENUM(check_mark) state)
    {
        m_core.check_mark(state);
        return *this;
    }

    virtual ::winapi::gui::menu::button button() const
    {
        return m_core.button();
    }

    virtual bool is_selectable() const
    {
        return m_core.is_selectable();
    }

    virtual bool check_mark_is_visible() const
    {
        return m_core.check_mark_is_visible();
    }

    virtual bool is_highlighted() const
    {
        return m_core.is_highlighted();
    }

    ::winapi::gui::menu::menu menu() const
    {
        HMENU submenu = m_core.get_menuiteminfo(MIIM_SUBMENU).hSubMenu;
        assert(submenu);
        return ::winapi::gui::menu::menu(menu_handle::foster_handle(submenu));
    }

private:

    explicit sub_menu_item(const detail::item_position& item) : m_core(item) {}

    detail::selectable_item_core m_core;
};

}}} // namespace winapi::gui::menu

#endif
