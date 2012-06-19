/**
    @file

    Menu items extracted from underlying HMENU.

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

#ifndef WINAPI_GUI_MENU_ITEMS_HPP
#define WINAPI_GUI_MENU_ITEMS_HPP
#pragma once

#include <winapi/gui/menu/buttons.hpp> // menu_button_nature
#include <winapi/gui/menu/detail/item_common.hpp> // selectable_item_core
#include <winapi/gui/menu/detail/item_proxy.hpp>
#include <winapi/gui/menu/menu.hpp>
#include <winapi/gui/menu/menu_handle.hpp>
#include <winapi/gui/menu/menu_item.hpp>
#include <winapi/gui/menu/selectable_menu_item.hpp>

#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/static_assert.hpp> // BOOST_STATIC_ASSERT

#include <stdexcept> // runtime_error

#include <Windows.h> // MENUITEMINFO

namespace winapi {
namespace gui {
namespace menu {

/**
 * Simple button menu entry with an associated command ID.
 *
 * The ID is used when informing the program that the user clicked a menu item.
 * The type of button is passed as an argument to the constructor.
 */
class command_menu_item : public selectable_menu_item
{
public:

    command_menu_item(const detail::item_proxy& item) : m_core(item) {}

    /**
     * The button description for this command item.
     */
    virtual boost::shared_ptr<menu_button_nature> button() const
    {
        return m_core.button();
    }

    virtual bool is_default() const
    {
        return m_core.is_default();
    }

    virtual bool is_enabled() const
    {
        return m_core.is_enabled();
    }

    virtual bool is_checked() const
    {
        return m_core.is_checked();
    }

    virtual bool is_highlighted() const
    {
        return m_core.is_highlighted();
    }

    /**
     * The ID sent to the owner windows when this command on the menu is
     * selected.
     */
    UINT command_id() const
    {
        return m_core.get_menuiteminfo(MIIM_ID).wID;
    }

private:

    virtual command_menu_item* do_clone() const
    {
        return new command_menu_item(*this);
    }

    detail::selectable_item_core m_core;
};

/**
 * Menu nested in or below main menu bar.
 *
 * Purpose: to extract an item with a submenu from an HMENU.
 */
class sub_menu : public selectable_menu_item
{
public:

    sub_menu(const detail::item_proxy& item) : m_core(item) {}

    virtual boost::shared_ptr<menu_button_nature> button() const
    {
        return m_core.button();
    }

    virtual bool is_default() const
    {
        return m_core.is_default();
    }

    virtual bool is_enabled() const
    {
        return m_core.is_enabled();
    }

    virtual bool is_checked() const
    {
        return m_core.is_checked();
    }

    virtual bool is_highlighted() const
    {
        return m_core.is_highlighted();
    }

    menu menu() const
    {
        HMENU submenu = m_core.get_menuiteminfo(MIIM_SUBMENU).hSubMenu;
        assert(submenu);
        return ::winapi::gui::menu::menu(menu_handle::foster_handle(submenu));
    }

private:

    virtual menu_item* do_clone() const
    {
        return new sub_menu(*this);
    }

    detail::selectable_item_core m_core;
};

/**
 * Line dividing menu into logical sections.
 */
class separator_menu_item : public menu_item
{
private:

    virtual separator_menu_item* do_clone() const
    {
        return new separator_menu_item(*this);
    }
};

namespace detail {

    /**
     * Do correct thing when encountering a separator item.
     *
     * Purpose: to select (at compile time) the appropriate behaviour when
     *          encountering a separator item in a menu based on the type of
     *          menu item we are allowed to represent.
     */
    template<typename ItemType>
    inline boost::shared_ptr<ItemType> separator_behaviour_chooser()
    {
        // Triggers if this is instantiated
        BOOST_STATIC_ASSERT(sizeof(ItemType) == 0);
    }

    template<>
    inline boost::shared_ptr<selectable_menu_item>
    separator_behaviour_chooser<selectable_menu_item>()
    {
        // If this throws, a separator was encountered in a menu bar
        BOOST_THROW_EXCEPTION(
            std::runtime_error("Separator not a selectable item"));
    }

    template<>
    inline boost::shared_ptr<menu_item>
    separator_behaviour_chooser<menu_item>()
    {
        return boost::make_shared<separator_menu_item>();
    }

    template<typename ItemType>
    inline boost::shared_ptr<ItemType>
    menu_item_from_position(const item_proxy& item)
    {
        // MIIM_SUBMENU is part of the mask so isn't set by GetMenuItemInfo.
        // We have to set it and check hSubMenu to decide if this is a popup
        // item
        const MENUITEMINFOW info =
            item.get_menuiteminfo(MIIM_FTYPE | MIIM_SUBMENU);

        // Four mutually-exclusive menu possibilities.  The first, separator
        // determines the entire menu item behaviour while the others just
        // indicate the button type (separators have no button).
        if (info.fType & MFT_SEPARATOR)
        {
            // In reality, a separator can be forced to have a submenu but as
            // this is clearly not what is intended, we don't give a way to get
            // access to it.
            return separator_behaviour_chooser<ItemType>();
        }
        else
        {
            if (info.hSubMenu)
            {
                return boost::make_shared<sub_menu>(item);
            }
            else
            {
                return boost::make_shared<command_menu_item>(item);
            }
        }
    }

}

}}} // namespace winapi::gui::menu

#endif
