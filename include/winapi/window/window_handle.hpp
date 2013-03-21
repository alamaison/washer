/**
    @file

    HWND lifetime management.

    @if license

    Copyright (C) 2013  Alexander Lamaison <awl03@doc.ic.ac.uk>

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

#ifndef WINAPI_WINDOW_WINDOW_HANDLE_HPP
#define WINAPI_WINDOW_WINDOW_HANDLE_HPP
#pragma once

#include <winapi/window/detail/window.hpp>
                                      // safe_destroy_window, no_destroy_window

#include <boost/shared_ptr.hpp>
#include <boost/type_traits/remove_pointer.hpp>

#include <Windows.h> // HWND

namespace winapi {
namespace window {

/**
 * Lifetime characteristics for wrapped window handle.
 *
 * Purpose: to abstract away the details of how an underlying Win32 window
 * handle's lifetime is managed.  One constructor takes ownership of its
 * lifetime and one does not.
 */
class window_handle
{
    typedef boost::shared_ptr<boost::remove_pointer<HWND>::type>
        shared_window_handle;

public:

    /**
     * Wrap a raw HWND without controlling its lifetime.
     *
     * The main purpose of this constructor is to use a window that we didn't
     * create and whose lifetime we don't own.  An example would be an HWND
     * passed to us by the windows API.
     *
     * The caller is responsible for ensuring the Win32 window will outlive
     * this wrapper.  A safer option is to use the `adopt_handle` constructor,
     * which will keep the window alive at least as long as the wrapper.
     */
    static window_handle foster_handle(HWND handle)
    {
        return window_handle(
            shared_window_handle(handle, detail::no_destroy_window));
    }

    /**
     * Wrap a window and take ownership of its underlying Win32 object.
     *
     * This is the safest of the two constructors as it keeps the window alive
     * as long as necessary and makes sure the window is destroyed after the 
     * wrapper is destroyed.  The Win32 window must not be destroyed by the 
     * caller after passing to this constructor.  Doing so will likely
     * result in a crash.
     */
    static window_handle adopt_handle(HWND handle)
    {
        return window_handle(
            shared_window_handle(handle, detail::safe_destroy_window));
    }

    HWND get() const
    {
        return m_handle.get();
    }

    friend void swap(window_handle& a, window_handle& b)
    {
        swap(a.m_handle, b.m_handle);
    }

private:

    window_handle(shared_window_handle handle) : m_handle(handle) {}

    shared_window_handle m_handle;
};

}} // namespace winapi::window

#endif
