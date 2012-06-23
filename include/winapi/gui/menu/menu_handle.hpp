/**
    @file

    HMENU lifetime management.

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

#ifndef WINAPI_GUI_MENU_MENU_HANDLE_HPP
#define WINAPI_GUI_MENU_MENU_HANDLE_HPP
#pragma once

#include <winapi/gui/menu/detail/menu.hpp> // safe_destroy_menu

#include <boost/shared_ptr.hpp>
#include <boost/type_traits/remove_pointer.hpp>

namespace winapi {
namespace gui {
namespace menu {

/**
 * Lifetime characteristics for wrapped menu handle.
 *
 * Purpose: to offer two different ways of managing a wrapped HMENU; one that
 * takes ownership of its lifetime and one that does not take responsibility for
 * managing its lifetime.
 */
class menu_handle
{
    typedef boost::shared_ptr<boost::remove_pointer<HMENU>::type>
        shared_menu_handle;

public:

    static menu_handle foster_handle(HMENU handle)
    {
        return menu_handle(
            shared_menu_handle(handle, detail::no_destroy_menu));
    }

    static menu_handle adopt_handle(HMENU handle)
    {
        return menu_handle(
            shared_menu_handle(handle, detail::safe_destroy_menu));
    }

    HMENU get() const
    {
        return m_handle.get();
    }

    friend void swap(menu_handle& a, menu_handle& b)
    {
        swap(a.m_handle, b.m_handle);
    }

private:

    menu_handle(shared_menu_handle handle) : m_handle(handle) {}

    shared_menu_handle m_handle;
};

}}} // namespace winapi::gui::menu

#endif
