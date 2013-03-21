/**
    @file

    Icon HWND wrapper class.

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

#ifndef WINAPI_WINDOW_ICON_HPP
#define WINAPI_WINDOW_ICON_HPP
#pragma once

#include <winapi/error.hpp> // last_error
#include <winapi/window/window.hpp> // window base class
#include <winapi/message.hpp> // send_message

#include <boost/exception/errinfo_api_function.hpp> // errinfo_api_function
#include <boost/exception/info.hpp> // errinfo
#include <boost/throw_exception.hpp> // BOOST_THROW_EXCEPTION

#include <cassert> // assert

namespace winapi {
namespace window {

/**
 * Wrapper around an icon (a STATIC window with SS_ICON style).
 *
 * @todo  Consider if it is worth checking class and style and throwing
 *        an exception if it fails.
 */
template<typename T>
class icon_window : public window<T>
{
public:
    explicit icon_window(const window_handle& handle) : window(handle) {}

    HICON change_icon(HICON new_icon)
    {
        return winapi::send_message_return<T, HICON>(
            hwnd(), STM_SETIMAGE, IMAGE_ICON, new_icon);
    }
};

}} // namespace winapi::window

#endif
