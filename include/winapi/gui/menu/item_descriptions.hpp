/**
    @file

    Classes describing range of items that can appear in menus and menu bars.

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

#ifndef WINAPI_GUI_MENU_ITEM_DESCRIPTIONS_HPP
#define WINAPI_GUI_MENU_ITEM_DESCRIPTIONS_HPP
#pragma once

#include <winapi/gui/menu/buttons.hpp> // menu_button_nature
#include <winapi/gui/menu/menu.hpp>
#include <winapi/gui/menu/menu_item_description.hpp>
#include <winapi/gui/menu/selectable_menu_item_description.hpp>

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
class command_item_description : public selectable_menu_item_description
{
public:

    command_item_description(
        const menu_button_nature& button_nature, UINT command_id)
        : m_button(button_nature.clone()), m_command_id(command_id) {}

private:

    virtual MENUITEMINFOW as_menuiteminfo() const
    {
        MENUITEMINFOW info = m_button->as_menuiteminfo();

        info.fMask |= MIIM_ID;
        info.wID = m_command_id;

        return info;
    }

    virtual command_item_description* do_clone() const
    {
        return new command_item_description(*this);
    }

    const boost::shared_ptr<menu_button_nature> m_button;
    const UINT m_command_id;
};

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
class sub_menu_description : public selectable_menu_item_description
{
public:

    sub_menu_description(
        const menu_button_nature& button_nature, const menu& menu)
        : m_button(button_nature.clone()), m_menu(menu) {}

private:

    MENUITEMINFOW as_menuiteminfo() const
    {
        MENUITEMINFOW info = m_button->as_menuiteminfo();

        info.fMask |= MIIM_SUBMENU;
        info.hSubMenu = sub_menu_description_befriender(m_menu).handle().get();

        return info;
    }

    virtual sub_menu_description* do_clone() const
    {
        return new sub_menu_description(*this);
    }

    const boost::shared_ptr<menu_button_nature> m_button;
    const ::winapi::gui::menu::menu m_menu;
};

/**
 * Line dividing menu into logical sections.
 */
class separator_description : public menu_item_description
{
private:

    virtual MENUITEMINFOW as_menuiteminfo() const
    {
        MENUITEMINFOW info = MENUITEMINFOW();
        info.cbSize = sizeof(MENUITEMINFOW);

        info.fMask = MIIM_FTYPE;
        info.fType = MFT_SEPARATOR;

        return info;
    }

    virtual separator_description* do_clone() const
    {
        return new separator_description(*this);
    }
};

}}} // namespace winapi::gui::menu

#endif
