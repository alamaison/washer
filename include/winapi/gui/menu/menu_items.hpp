/**
    @file

    Range of items that can appear in menus and menu bars.

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

#ifndef WINAPI_GUI_MENU_MENU_ITEMS_HPP
#define WINAPI_GUI_MENU_MENU_ITEMS_HPP
#pragma once

#include <winapi/gui/menu/detail/menu_win32.hpp> // get_menu_item_info
#include <winapi/gui/menu/detail/menu.hpp> // menu_handle
#include <winapi/gui/menu/menu.hpp>
#include <winapi/gui/menu/menu_bar_item.hpp>
#include <winapi/gui/menu/menu_item.hpp>
#include <winapi/gui/menu/menu_buttons.hpp> // menu_button_nature

#include <cassert> // assert
#include <memory> // auto_ptr
#include <stdexcept> // logic_error
#include <string>
#include <vector>

#include <Windows.h> // MENUITEMINFO

namespace winapi {
namespace gui {
namespace menu {

/**
 * Simple button menu entry with an associated command ID.
 *
 * The ID is used when informing the program that the user clicked a menu item.
 * The type of button is passed as an argument to the constructor.
 *
 * @tparam  Button nature concrete type.
 */
template<typename T>
class command_menu_item : public menu_bar_item
{
public:

    command_menu_item(const T& button_nature, UINT command_id)
        : m_button(button_nature), m_command_id(command_id) {}

private:

    virtual MENUITEMINFOW as_menuiteminfo() const
    {
        const menu_button_nature& button = m_button; // friends with parent
        MENUITEMINFOW info = button.as_menuiteminfo();

        info.fMask |= MIIM_ID;
        info.wID = m_command_id;

        return info;
    }

    virtual command_menu_item* do_clone() const
    {
        return new command_menu_item(*this);
    }

    T m_button;
    UINT m_command_id;
};

/**
 * Construction helper for command menu entry templated on button type.
 *
 * Purpose: to avoid having to specify template argument twice when creating
 * an instance of command_menu_item.
 *
 * @tparam  Button nature concrete type.
 */
template<typename T>
inline command_menu_item<T> make_command_menu_item(
    const T& button_nature, UINT command_id)
{
    return command_menu_item<T>(button_nature, command_id);
}

/**
 * Menu nested in or below main menu bar.
 *
 * Purpose: to associate a caption text and command ID with a menu to ready it
 * for insertion into a menu bar or into another menu.
 *
 * Menus can be used independently so they do not intrinsically have
 * a caption or command ID; these only make sense for menus that are going to
 * be inserted into another menu or menu bar.
 *
 * @tparam  Button nature concrete type.
 */
template<typename T>
class sub_menu : public menu_bar_item
{
public:

    sub_menu(const T& button_nature, const menu& menu)
        : m_button(button_nature), m_menu(menu) {}

private:

    MENUITEMINFOW as_menuiteminfo() const
    {
        const menu_button_nature& button = m_button; // friends with parent
        MENUITEMINFOW info = button.as_menuiteminfo();

        info.fMask |= MIIM_SUBMENU;
        info.hSubMenu = m_menu.handle().get();

        return info;
    }

    virtual menu_item* do_clone() const
    {
        return new sub_menu(*this);
    }

    const T m_button;
    const menu m_menu;
};

/**
 * Construction helper for sub-menu entry templated on button type.
 *
 * Purpose: to avoid having to specify template argument twice when creating
 * an instance of sub_menu.
 *
 * @tparam  Button nature concrete type.
 */
template<typename T>
inline sub_menu<T> make_sub_menu(const T& button_nature, const menu& menu)
{
    return sub_menu<T>(button_nature, menu);
}

/**
 * Line dividing menu into logical sections.
 */
class separator_menu_item : public menu_item
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

    virtual separator_menu_item* do_clone() const
    {
        return new separator_menu_item(*this);
    }
};

namespace detail {

    template<UINT Mft, typename ItemType>
    inline std::auto_ptr<ItemType> item_type_converter(
        const MENUITEMINFOW& info, const menu_handle& menu, UINT pos)
    {
        BOOST_THROW_EXCEPTION(
            std::logic_error("No menu item conversion for combination"));
    }

    template<>
    inline std::auto_ptr<menu_bar_item>
    item_type_converter<MFT_BITMAP, menu_bar_item>(
        const MENUITEMINFOW& info, const menu_handle& menu, UINT pos)
    {
        HBITMAP bitmap = reinterpret_cast<HBITMAP>(info.dwTypeData);
        return make_command_menu_item(
            bitmap_menu_button(bitmap), info.wID).clone();
    }

    template<>
    inline std::auto_ptr<menu_bar_item>
    item_type_converter<MFT_OWNERDRAW, menu_bar_item>(
        const MENUITEMINFOW& info, const menu_handle& menu, UINT pos)
    {
        return make_command_menu_item(
            owner_drawn_menu_button(), info.wID).clone();
    }

    // Note: this specialisation is for menu_item rather than menu_bar_item
    template<>
    inline std::auto_ptr<menu_item>
    item_type_converter<MFT_SEPARATOR, menu_item>(
        const MENUITEMINFOW& info, const menu_handle& menu, UINT pos)
    {
        assert(info.wID == 0);
        return new separator_menu_item();
    }

    template<>
    inline std::auto_ptr<menu_bar_item>
    item_type_converter<MFT_STRING, menu_bar_item>(
        const MENUITEMINFOW& info, const menu_handle& menu, UINT pos)
    {
        std::vector<wchar_t> buffer(info.cch);
        MENUITEMINFOW new_info = info;
        new_info.cch = buffer.size();
        detail::win32::get_menu_item_info(menu.get(), pos, TRUE, &new_info);

        return make_command_menu_item(
            string_menu_button(std::wstring(new_info.dwTypeData, new_info.cch)),
            new_info.wID).clone();
    }


    template<typename ItemType>
    inline std::auto_ptr<ItemType> menu_item_from_position(
        const menu_handle& menu, UINT pos)
    {
        MENUITEMINFOW info = MENUITEMINFOW();
        info.cbSize = sizeof(MENUITEMINFOW);
        detail::win32::get_menu_item_info(menu.get(), pos, TRUE, &info);

        // MSDN doesn't say that MFT_OWNERDRAWN is part of the
        // mutually exclusive set of flags but I don't see how it couldn't
        // be.  These flags specify the meaning of dwTypeData and cch which
        // MFT_OWNERDRAWN gives a user-defined meaning.
        if (info.fType & MFT_BITMAP)
        {
            return item_type_converter<MFT_BITMAP, ItemType>(info);
        }
        else if (info.fType & MFT_OWNERDRAW)
        {
            return item_type_converter<MFT_OWNERDRAW, ItemType>(info);
        }
        else if (info.fType & MFT_SEPARATOR)
        {
            return item_type_converter<MFT_SEPARATOR, ItemType>(info);
        }
        else
        {
            // MFT_STRING which is zero so can only be detected by elimination
            return item_type_converter<MFT_STRING, ItemType>(info);
        }
    }

}

}}} // namespace winapi::gui::menu

#endif
