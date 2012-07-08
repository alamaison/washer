/**
    @file

    Friendship indirectors.

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

#ifndef WINAPI_GUI_MENU_DETAIL_BEFRIENDERS_HPP
#define WINAPI_GUI_MENU_DETAIL_BEFRIENDERS_HPP
#pragma once

#include <winapi/gui/menu/menu_handle.hpp> // menu_handle

namespace winapi {
namespace gui {

template<typename>
class window;

namespace menu {

class sub_menu_item_description;

namespace detail {

/**
 * Purpose: to indirect friendship between a menu and a sub_menu_item_description
 *          so we can parameterise the basic_menu template with the friendship.
 */
class sub_menu_item_description_befriender
{
    friend class sub_menu_item_description;

private:

    sub_menu_item_description_befriender(const menu& menu) : m_menu(menu) {}

    const menu_handle handle() const
    {
        return m_menu.handle();
    }

    const menu& m_menu;
};

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

}}}} // namespace winapi::gui::menu::detail

#endif
