/**
    @file

    Thin layer around Win32 window functions.

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

#ifndef WINAPI_WINDOW_DETAIL_WINDOW_WIN32_HPP
#define WINAPI_WINDOW_DETAIL_WINDOW_WIN32_HPP
#pragma once

#include <winapi/error.hpp> // last_error

#include <boost/exception/errinfo_api_function.hpp>
#include <boost/exception/info.hpp> // errinfo
#include <boost/throw_exception.hpp> // BOOST_THROW_EXCEPTION

#include <Winuser.h> // SetWindowLongPtr, GetWindowLongPtr, GetWindowTextLength
                     // GetWindowText, SetWindowText, DestroyWindow, SetMenu,
                     // GetWidowRect

namespace winapi {
namespace window {
namespace detail {
namespace win32 {

inline void destroy_window(HWND hwnd)
{
    if (!::DestroyWindow(hwnd))
        BOOST_THROW_EXCEPTION(
            boost::enable_error_info(winapi::last_error()) <<
            boost::errinfo_api_function("DestroyMenu"));
}

inline void set_menu(HWND hwnd, HMENU hmenu)
{
    if (!::SetMenu(hwnd, hmenu))
        BOOST_THROW_EXCEPTION(
            boost::enable_error_info(winapi::last_error()) <<
            boost::errinfo_api_function("SetMenu"));
}

inline void get_window_rect(HWND hwnd, RECT* rect)
{
    if (::GetWindowRect(hwnd, rect) == 0)
        BOOST_THROW_EXCEPTION(
            boost::enable_error_info(winapi::last_error()) <<
            boost::errinfo_api_function("GetWindowRect"));
}


/// @name GetWindowTextLength
// @{
template<typename T>
inline int get_window_text_length(HWND hwnd);

template<> inline int get_window_text_length<char>(HWND hwnd)
{ return ::GetWindowTextLengthA(hwnd); }

template<> inline int get_window_text_length<wchar_t>(HWND hwnd)
{ return ::GetWindowTextLengthW(hwnd); }
// @}

/// @name GetWindowText
// @{
inline int get_window_text(
    HWND hwnd, char* out_buffer, int out_buffer_size)
{ return ::GetWindowTextA(hwnd, out_buffer, out_buffer_size); }

inline int get_window_text(
    HWND hwnd, wchar_t* out_buffer, int out_buffer_size)
{ return ::GetWindowTextW(hwnd, out_buffer, out_buffer_size); }
// @}
// @}

/// @name SetWindowText
// @{
inline BOOL set_window_text(HWND hwnd, const char* text)
{ return ::SetWindowTextA(hwnd, text); }

inline BOOL set_window_text(HWND hwnd, const wchar_t* text)
{ return ::SetWindowTextW(hwnd, text); }
// @}

}}}} // namespace winapi::window::detail::win32

#endif
