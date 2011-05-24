/**
    @file

    Windows message system.

    @if license

    Copyright (C) 2010, 2011  Alexander Lamaison <awl03@doc.ic.ac.uk>

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

#ifndef WINAPI_MESSAGE_HPP
#define WINAPI_MESSAGE_HPP
#pragma once

#include <Winuser.h> // SendMessage

namespace winapi {

namespace detail {
    namespace native {

        template<typename T>
        LRESULT send_message(
            HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

        template<>
        LRESULT send_message<char>(
            HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
        { return ::SendMessageA(hwnd, message, wparam, lparam); }

        template<>
        LRESULT send_message<wchar_t>(
            HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
        { return ::SendMessageW(hwnd, message, wparam, lparam); }
    }
}

template<typename T, typename R, typename W, typename L>
inline R send_message(HWND hwnd, UINT message, W wparam, L lparam)
{
    return (R)(
        detail::native::send_message<T>(
            hwnd, message, (WPARAM)wparam, (LPARAM)lparam));
}

template<typename T, typename W, typename L>
inline void send_message(HWND hwnd, UINT message, W wparam, L lparam)
{
    send_message<T, LRESULT, W, L>(hwnd, message, wparam, lparam);
}

} // namespace winapi

#endif
