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
#include <winapi/gui/menu/selectable_menu_item.hpp>
#include <winapi/gui/menu/menu_item.hpp>
#include <winapi/gui/menu/buttons.hpp> // menu_button_nature

#include <boost/make_shared.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <boost/utility/enable_if.hpp>

#include <cassert> // assert
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
 */
class command_menu_item : public selectable_menu_item
{
public:

    command_menu_item(const menu_button_nature& button_nature, UINT command_id)
        : m_button(button_nature.clone()), m_command_id(command_id) {}

    /**
     * The button description for this command item.
     */
    virtual const menu_button_nature& button() const
    {
        return *m_button;
    }

    /**
     * The ID sent to the owner windows when this command on the menu is
     * selected.
     */
    UINT command_id() const
    {
        return m_command_id;
    }

private:

    virtual MENUITEMINFOW as_menuiteminfo() const
    {
        MENUITEMINFOW info = m_button->as_menuiteminfo();

        info.fMask |= MIIM_ID;
        info.wID = m_command_id;

        return info;
    }

    virtual command_menu_item* do_clone() const
    {
        return new command_menu_item(*this);
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
class sub_menu : public selectable_menu_item
{
public:

    sub_menu(const menu_button_nature& button_nature, const menu& menu)
        : m_button(button_nature.clone()), m_menu(menu) {}

    virtual const menu_button_nature& button() const
    {
        return *m_button;
    }

    menu menu() const
    {
        return m_menu;
    }

private:

    MENUITEMINFOW as_menuiteminfo() const
    {
        MENUITEMINFOW info = m_button->as_menuiteminfo();

        info.fMask |= MIIM_SUBMENU;
        info.hSubMenu = m_menu.handle().get();

        return info;
    }

    virtual menu_item* do_clone() const
    {
        return new sub_menu(*this);
    }

    const boost::shared_ptr<menu_button_nature> m_button;
    const ::winapi::gui::menu::menu m_menu;
};

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

    /**
     * Combines a menu and a menu position.
     *
     * Purpose: to abstract the notion of an item (whose type we do not know
     *          yet) at a particular point in a menu.
     *
     * Encapsulates the operations that only make sense when a menu and
     * a menu item position exist in unison.
     */
    class menu_item_proxy
    {
    public:

        menu_item_proxy(const menu_handle& menu, UINT position)
            : m_handle(menu), m_position(position) {}

        MENUITEMINFOW get_menuiteminfo(UINT fMask) const
        {
            MENUITEMINFOW info = MENUITEMINFOW();
            info.cbSize = sizeof(MENUITEMINFOW);

            info.fMask = fMask;
            detail::win32::get_menu_item_info(
                m_handle.get(), m_position, TRUE, &info);

            return info;
        }

        /**
         * Getting the string from a MENUITEMINFO gets its own special method
         * because of the double-call semantics and buffer ownership.
         */
        std::wstring get_caption() const
        {
            MENUITEMINFOW info = get_menuiteminfo(MIIM_FTYPE | MIIM_STRING);

            assert(
                (info.fType & (MFT_BITMAP | MFT_OWNERDRAW | MFT_SEPARATOR))
                == 0);

            std::vector<wchar_t> buffer(info.cch + 1);
            info.cch = buffer.size();
            info.dwTypeData = (!buffer.empty()) ? &buffer[0] : NULL;
            detail::win32::get_menu_item_info(
                m_handle.get(), m_position, TRUE, &info);

            if (info.cch > 0)
            {
                assert(info.dwTypeData);
                return std::wstring(info.dwTypeData, info.cch);
            }
            else
            {
                return std::wstring();
            }
        }

    private:

        const menu_handle m_handle;
        const UINT m_position;
    };

    template<UINT Mft>
    struct item_type_converter
    {
        static boost::shared_ptr<menu_button_nature> convert(
            const menu_item_proxy& /*item*/);
    };

    template<>
    struct item_type_converter<MFT_BITMAP> 
    {
        static boost::shared_ptr<menu_button_nature> convert(
            const menu_item_proxy& item)
        {
            // Old-style whole-button bitmap specified through dwTypeData and
            // MFT_BITMAP (MF_BITMAP) requires MIIM_TYPE rather than MIIM_FTYPE
            MENUITEMINFOW info = item.get_menuiteminfo(MIIM_TYPE);

            HBITMAP bitmap = reinterpret_cast<HBITMAP>(info.dwTypeData);
            return boost::make_shared<bitmap_menu_button>(bitmap);
        }
    };

    template<>
    struct item_type_converter<MFT_OWNERDRAW>
    {
        /**
         * @todo  Include custom data in dwTypeData and dwItemData.
         */
        static boost::shared_ptr<menu_button_nature> convert(
            const menu_item_proxy& item)
        {
            return boost::make_shared<owner_drawn_menu_button>();
        }
    };

    template<>
    struct item_type_converter<MFT_STRING>
    {
        static boost::shared_ptr<menu_button_nature> convert(
            const menu_item_proxy& item)
        {
            return boost::make_shared<string_menu_button>(item.get_caption());
        }
    };

    inline boost::shared_ptr<menu_button_nature> convert_menu_button(
        const MENUITEMINFOW& info, const menu_item_proxy& item)
    {
        if (info.fType & MFT_BITMAP)
        {
            return item_type_converter<MFT_BITMAP>::convert(item);
        }
        else if (info.fType & MFT_OWNERDRAW)
        {
            // MSDN doesn't say that MFT_OWNERDRAWN is part of the
            // mutually exclusive set of flags but I don't see how it couldn't
            // be.  These flags specify the meaning of dwTypeData and cch which
            // MFT_OWNERDRAWN gives a user-defined meaning.
            return item_type_converter<MFT_OWNERDRAW>::convert(item);
        }
        else
        {
            // MFT_STRING which is zero so can only be detected by elimination
            return item_type_converter<MFT_STRING>::convert(item);
        }
    }

    template<typename ItemType>
    inline boost::shared_ptr<ItemType> menu_item_from_position(
        const menu_item_proxy& item)
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
            return boost::make_shared<separator_menu_item>();
        }
        else
        {
            const boost::shared_ptr<menu_button_nature> button =
                convert_menu_button(info, item);

            if (info.hSubMenu)
            {
                return boost::make_shared<sub_menu>(
                    *button, menu_handle::foster_handle(info.hSubMenu));
            }
            else
            {
                return boost::make_shared<command_menu_item>(
                    *button, item.get_menuiteminfo(MIIM_ID).wID);
            }
        }
    }

}

}}} // namespace winapi::gui::menu

#endif
