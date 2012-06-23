/**
    @file

    Windows HMENU menu wrapper for menus and menu bars.

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

#ifndef WINAPI_GUI_MENU_MENU_HPP
#define WINAPI_GUI_MENU_MENU_HPP
#pragma once

#include <winapi/gui/menu/basic_menu.hpp>
#include <winapi/gui/menu/detail/menu_win32.hpp>
                                               // create_popup_menu, create_menu
#include <winapi/gui/menu/menu_handle.hpp> // menu_handle
#include <winapi/gui/menu/menu_item_description.hpp>
#include <winapi/gui/menu/selectable_menu_item_description.hpp>

#include <Windows.h> // MENUITEMINFO

namespace winapi {
namespace gui {

template<typename>
class window;

namespace menu {

namespace detail {

    struct menu_handle_creator
    {
        HMENU operator()()
        {
            return detail::win32::create_popup_menu();
        }
    };

    struct menu_bar_handle_creator
    {
        HMENU operator()()
        {
            return detail::win32::create_menu();
        }
    };

}

class sub_menu_description_befriender;

/**
 * Menu that can be used as a context menu, as a sub-menu of a menu bar or as
 * a sub-menu of another menu.
 */
typedef basic_menu<
    menu_item_description, detail::menu_handle_creator,
    sub_menu_description_befriender> menu;

/**
 * Purpose: to indirect friendship with the basic_menu so we can pass
 *          different friends in the typedef.
 */
class sub_menu_description_befriender
{
    friend class sub_menu_description;

private:

    sub_menu_description_befriender(const menu& menu) : m_menu(menu) {}

    const menu_handle handle() const
    {
        return m_menu.handle();
    }

    const menu& m_menu;
};

class window_class_befriender;

/**
 * Main application window menu bar.
 */
typedef basic_menu<
    selectable_menu_item_description, detail::menu_bar_handle_creator,
    window_class_befriender> menu_bar;

/**
 * Purpose: to indirect friendship between a menu_bar and a window so we can
 *          parameterise the basic_menu template with the friendship.
 */
class window_class_befriender
{
#if defined (_MSC_VER) && (_MSC_VER > 1400)
    template<typename>
    friend class window;
#else
    // HACK to workaround VC8 (2005) and presumably earlier that can't
    // befriend a template class in a parent namespace.
    // See: http://stackoverflow.com/q/10694416/67013
    friend class window<char>;
    friend class window<wchar_t>;
#endif

private:

    window_class_befriender(const menu_bar& menu) : m_menu(menu) {}

    const menu_handle handle() const
    {
        return m_menu.handle();
    }

    const menu_bar& m_menu;
};

}}} // namespace winapi::gui::menu

#endif
