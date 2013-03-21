/**
    @file

    Window functions working on HWNDs.

    @if license

    Copyright (C) 2010, 2011, 2013  Alexander Lamaison <awl03@doc.ic.ac.uk>

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

#ifndef WINAPI_GUI_HWND_HPP
#define WINAPI_GUI_HWND_HPP
#pragma once

#include <winapi/error.hpp> // last_error

#include <boost/exception/errinfo_api_function.hpp> // errinfo_api_function
#include <boost/exception/info.hpp> // errinfo
#include <boost/throw_exception.hpp> // BOOST_THROW_EXCEPTION

#include <cassert> // assert

#include <Winuser.h> // SetWindowLongPtr, GetWindowLongPtr, GetWindowTextLength
                     // GetWindowText, SetWindowText
namespace winapi {
namespace gui {

namespace detail {

    namespace native {

        /// @name SetWindowLongPtr
        // @{
        template<typename T>
        inline LONG_PTR set_window_long_ptr(
            HWND hwnd, int index, LONG_PTR new_long);

        template<> inline LONG_PTR set_window_long_ptr<char>(
            HWND hwnd, int index, LONG_PTR new_long)
        { return ::SetWindowLongPtrA(hwnd, index, new_long); }

        template<> inline LONG_PTR set_window_long_ptr<wchar_t>(
            HWND hwnd, int index, LONG_PTR new_long)
        { return ::SetWindowLongPtrW(hwnd, index, new_long); }
        // @}

        /// @name GetWindowLongPtr
        // @{
        template<typename T>
        inline LONG_PTR get_window_long_ptr(HWND hwnd, int index);

        template<> inline LONG_PTR get_window_long_ptr<char>(
            HWND hwnd, int index)
        { return ::GetWindowLongPtrA(hwnd, index); }

        template<> inline LONG_PTR get_window_long_ptr<wchar_t>(
            HWND hwnd, int index)
        { return ::GetWindowLongPtrW(hwnd, index); }
        // @}


    }

}

/**
 * Store a value in the given field of the window descriptor.
 *
 * The value type must be no bigger than a LONG_PTR.
 *
 * @param hwnd   Window handle whose data we are setting.
 * @param field  Which data field are we storing this data in.
 * @param value   Data to store.
 *
 * @return  The previous value.
 */
template<typename T, typename U>
inline U set_window_field(HWND hwnd, int field, const U& value)
{
    BOOST_STATIC_ASSERT(sizeof(U) <= sizeof(LONG_PTR));

    ::SetLastError(0);

    LONG_PTR previous = detail::native::set_window_long_ptr<T>(
        hwnd, field, reinterpret_cast<LONG_PTR>(value));

    if (previous == 0 && ::GetLastError() != 0)
        BOOST_THROW_EXCEPTION(
            boost::enable_error_info(winapi::last_error()) <<
            boost::errinfo_api_function("SetWindowLongPtr"));

    return reinterpret_cast<U>(previous);
}

/**
 * Get a value previously stored in the window descriptor.
 *
 * The value type must be no bigger than a LONG_PTR.
 *
 * @param hwnd      Window handle whose data we are getting.
 * @param field     Which data field are we getting the value of.
 * @param no_throw  Controls whether this function can throw.
 *
 * @throws  A system_error if `no_throw` is `false` and there is an
 *          error or if no value has yet been stored.  If no_throw is true
 *          it returns 0 instead.
 *
 * @note  Storing 0 will count as not having a previous value so will
 *        throw an exception unless `no_throw` is `true`.
 */
template<typename T, typename U>
inline U window_field(HWND hwnd, int field, bool no_throw=false)
{
    BOOST_STATIC_ASSERT(sizeof(U) <= sizeof(LONG_PTR));

    LONG_PTR value = detail::native::get_window_long_ptr<T>(hwnd, field);
    if (value == 0 && !no_throw)
        BOOST_THROW_EXCEPTION(
            boost::enable_error_info(winapi::last_error()) <<
            boost::errinfo_api_function("GetWindowLongPtr"));

    return reinterpret_cast<U>(value);
}

}} // namespace winapi::gui

#endif
