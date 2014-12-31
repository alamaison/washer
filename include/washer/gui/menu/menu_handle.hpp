/**
    @file

    HMENU lifetime management.

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

#ifndef WASHER_GUI_MENU_MENU_HANDLE_HPP
#define WASHER_GUI_MENU_MENU_HANDLE_HPP
#pragma once

#include <washer/gui/menu/detail/menu.hpp> // safe_destroy_menu
#include <washer/gui/menu/detail/menu_win32.hpp> // is_menu

#include <boost/shared_ptr.hpp>
#include <boost/type_traits/remove_pointer.hpp>

#include <cassert> // assert

namespace washer {
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

    // Because shared_ptr doesn't let us release, but we really have to be
    // able to to hand ownership to objects we don't manage, (don't worry,
    // the valid() method of basic_menu catches the stragglers by throwing
    // if we try to use them after they have gone away) we use this class
    // to change the behaviour of the deleter when we need to release.
    class settable_deleter
    {
    public:
        settable_deleter(bool do_delete) : m_do_delete(do_delete) {}

        void operator()(HMENU raw_handle)
        {
            if (m_do_delete)
            {
                detail::safe_destroy_menu(raw_handle);
            }
        }

        void set_deletion(bool do_delete)
        {
            m_do_delete = do_delete;
        }

    private:
        bool m_do_delete;
    };

public:

    static menu_handle foster_handle(HMENU handle)
    {
        assert(detail::win32::is_menu(handle) || !"Must be valid handle");
        return menu_handle(
            shared_menu_handle(handle, settable_deleter(false)));
    }

    static menu_handle adopt_handle(HMENU handle)
    {
        assert(detail::win32::is_menu(handle) || !"Must be valid handle");
        return menu_handle(
            shared_menu_handle(handle, settable_deleter(true)));
    }

    HMENU get() const
    {
        return m_handle.get();
    }

    /**
     * Gives up this object's ownership, if any, of the menu.
     *
     * If the menu handle is adopted, this converts it to a fostered handle.
     * If the menu handle is already fostered, this simply returns it.
     */
    HMENU release()
    {
        boost::get_deleter<settable_deleter>(m_handle)->set_deletion(false);
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

}}} // namespace washer::gui::menu

#endif
