/**
    @file

    Builder for pop-out items.

    @if license

    Copyright (C) 2012, 2013  Alexander Lamaison <awl03@doc.ic.ac.uk>

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

#ifndef WASHER_GUI_MENU_ITEM_SUB_MENU_ITEM_DESCRIPTION_HPP
#define WASHER_GUI_MENU_ITEM_SUB_MENU_ITEM_DESCRIPTION_HPP
#pragma once

#include <washer/gui/menu/button/button_description.hpp>
#include <washer/gui/menu/menu.hpp>
#include <washer/gui/menu/item/item_state.hpp> // adjust_selectability,
                                               // adjust_checkedness
#include <washer/gui/menu/item/selectable_item_description.hpp>

#include <Windows.h> // MENUITEMINFO

#include <cassert> // assert

namespace washer {
namespace gui {
namespace menu {

/**
 * Menu nested in or below main menu bar.
 *
 * Purpose: to associate a caption text and command ID with a menu to ready it
 * for insertion into a menu bar or into another menu.
 *
 * Menus can be used independently so they do not intrinsically have
 * a caption or command ID; these only make sense for menus that are going to
 * be inserted into another menu or menu bar.
 */
class sub_menu_item_description : public selectable_item_description
{
public:

    sub_menu_item_description(
        const button_description& button, const menu& menu)
        :
        m_id(0),
        m_button(button.clone()),
        m_menu(menu),
        m_selectability(selectability::default_state),
        m_checkedness(check_mark::default_state)
    {
        assert(m_menu.valid() || !"Constructed from invalid menu");
    }

    sub_menu_item_description() : m_id(0) {}

    virtual sub_menu_item_description& id(UINT new_id)
    {
        m_id = new_id;
        return *this;
    }

    virtual sub_menu_item_description& selectability(
        BOOST_SCOPED_ENUM(washer::gui::menu::selectability) state)
    {
        m_selectability = state;
        return *this;
    }

    virtual sub_menu_item_description& check_mark_visibility(
        BOOST_SCOPED_ENUM(check_mark) state)
    {
        m_checkedness = state;
        return *this;
    };

private:

    virtual void do_insertion_and_relinquish_resources(
        const boost::function<void(const MENUITEMINFOW&)>& inserter) const
    {
        inserter(as_menuiteminfo());
        m_menu.m_menu.release();
    }

    virtual sub_menu_item_description* do_clone() const
    {
        return new sub_menu_item_description(*this);
    }

    MENUITEMINFOW as_menuiteminfo() const
    {
        MENUITEMINFOW info = m_button->as_menuiteminfo();

        info.fMask |= MIIM_SUBMENU | MIIM_ID;
        info.wID = m_id;
        assert(m_menu.valid() || !"Invalid menu");
        info.hSubMenu = m_menu.handle().get();

        if (m_selectability != selectability::default_state ||
            m_checkedness != check_mark::default_state)
        {
            info.fMask |= MIIM_STATE;
        }

        detail::adjust_selectability(m_selectability, info);
        detail::adjust_checkedness(m_checkedness, info);

        return info;
    }

    UINT m_id;
    boost::shared_ptr<button_description> m_button;
    ::washer::gui::menu::menu m_menu;
    BOOST_SCOPED_ENUM(washer::gui::menu::selectability) m_selectability;
    BOOST_SCOPED_ENUM(check_mark) m_checkedness;
};

}}} // namespace washer::gui::menu

#endif
