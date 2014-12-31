/**
    @file

    Builder for item with a command ID.

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

#ifndef WASHER_GUI_MENU_ITEM_COMMAND_ITEM_DESCRIPTION_HPP
#define WASHER_GUI_MENU_ITEM_COMMAND_ITEM_DESCRIPTION_HPP
#pragma once

#include <washer/gui/menu/button/button.hpp>
#include <washer/gui/menu/menu.hpp>
#include <washer/gui/menu/item/item_description.hpp>
#include <washer/gui/menu/item/item_state.hpp> // adjust_selectability,
                                               // adjust_checkedness
#include <washer/gui/menu/item/selectable_item_description.hpp>

#include <Windows.h> // MENUITEMINFO

namespace washer {
namespace gui {
namespace menu {

/**
 * Simple button menu entry with an associated command ID.
 *
 * The ID is used when informing the program that the user clicked a menu item.
 * The type of button is passed as an argument to the constructor.
 */
class command_item_description : public selectable_item_description
{
public:

    command_item_description(const button_description& button, UINT command_id)
        :
        m_button(button.clone()),
        m_command_id(command_id),
        m_selectability(selectability::default_state),
        m_checkedness(check_mark::default_state) {}

    virtual command_item_description& id(UINT new_id)
    {
        m_command_id = new_id;
        return *this;
    }

    virtual command_item_description& selectability(
        BOOST_SCOPED_ENUM(washer::gui::menu::selectability) state)
    {
        m_selectability = state;
        return *this;
    }

    virtual command_item_description& check_mark_visibility(
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
        // No resources to release
    }

    virtual command_item_description* do_clone() const
    {
        return new command_item_description(*this);
    }

    MENUITEMINFOW as_menuiteminfo() const
    {
        MENUITEMINFOW info = m_button->as_menuiteminfo();

        info.fMask |= MIIM_ID;
        info.wID = m_command_id;

        if (m_selectability != selectability::default_state ||
            m_checkedness != check_mark::default_state)
        {
            info.fMask |= MIIM_STATE;
        }

        detail::adjust_selectability(m_selectability, info);
        detail::adjust_checkedness(m_checkedness, info);

        return info;
    }

    boost::shared_ptr<button_description> m_button;
    UINT m_command_id;
    BOOST_SCOPED_ENUM(washer::gui::menu::selectability) m_selectability;
    BOOST_SCOPED_ENUM(check_mark) m_checkedness;
};

}}} // namespace washer::gui::menu

#endif
