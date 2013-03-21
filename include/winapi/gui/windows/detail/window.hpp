/**
    @file

    Window wrapper implementation details.

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

#ifndef WINAPI_GUI_WINDOW_DETAIL_WINDOW_HPP
#define WINAPI_GUI_WINDOW_DETAIL_WINDOW_HPP
#pragma once

#include <winapi/gui/windows/detail/window_win32.hpp> // destroy_window
#include <winapi/trace.hpp>

#include <exception>

namespace winapi {
namespace gui {
namespace windows {
namespace detail {

inline void safe_destroy_window(HWND hwnd)
{
    try
    {
        win32::destroy_window(hwnd);
    }
    catch (const std::exception& e)
    {
        winapi::trace("Exception while destroying window: %s") % e.what();
    }
}

inline void no_destroy_window(HWND)
{
}

}}}} // namespace winapi::gui::windows::detail

#endif
