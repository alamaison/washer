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

#ifndef WASHER_GUI_WINDOW_DETAIL_WINDOW_HPP
#define WASHER_GUI_WINDOW_DETAIL_WINDOW_HPP
#pragma once

#include <washer/error.hpp> // last_error
#include <washer/window/detail/window_win32.hpp>
                          // destroy_window, get_window_text_length
#include <washer/trace.hpp>

#include <boost/exception/errinfo_api_function.hpp> // errinfo_api_function
#include <boost/exception/info.hpp> // errinfo
#include <boost/numeric/conversion/cast.hpp> // numeric_cast
#include <boost/throw_exception.hpp> // BOOST_THROW_EXCEPTION

#include <cassert> // assert
#include <stdexcept> // logic_error
#include <string>

namespace washer {
namespace window {
namespace detail {

inline void safe_destroy_window(HWND hwnd)
{
    try
    {
        win32::destroy_window(hwnd);
    }
    catch (const std::exception& e)
    {
        washer::trace("Exception while destroying window: %s") % e.what();
    }
}

inline void no_destroy_window(HWND)
{
}

/**
 * The *lower bound* on the length of a window's text.
 *
 * In other words, the text may actually be shorter but never longer.
 */
template<typename T>
inline size_t window_text_length(HWND hwnd)
{
    ::SetLastError(0);

    int cch = win32::get_window_text_length<T>(hwnd);

    if (cch < 0)
        BOOST_THROW_EXCEPTION(
            std::logic_error("impossible (negative) text length"));
    if (cch == 0 && ::GetLastError() != 0)
        BOOST_THROW_EXCEPTION(
            boost::enable_error_info(washer::last_error()) <<
            boost::errinfo_api_function("GetWindowTextLength"));

    return boost::numeric_cast<size_t>(cch);
}

/**
 * Set a window's text.
 */
template<typename T>
inline void window_text(HWND hwnd, const std::basic_string<T>& text)
{
    if (!win32::set_window_text(hwnd, text.c_str()))
        BOOST_THROW_EXCEPTION(
            boost::enable_error_info(washer::last_error()) <<
            boost::errinfo_api_function("SetWindowText"));
}


template<typename U>
inline std::basic_string<U> window_text(HWND hwnd)
{
    std::vector<U> buffer(detail::window_text_length<U>(hwnd) + 1);
                                             // +space for NULL

    ::SetLastError(0);

    int cch = detail::win32::get_window_text(
        hwnd, (buffer.empty()) ? NULL : &buffer[0],
        boost::numeric_cast<int>(buffer.size()));

    if (cch < 0)
        BOOST_THROW_EXCEPTION(
            std::logic_error("impossible (negative) text length"));
    if (cch == 0 && ::GetLastError() != 0)
        BOOST_THROW_EXCEPTION(
            boost::enable_error_info(washer::last_error()) <<
            boost::errinfo_api_function("GetWindowText"));

    assert((unsigned long)cch <= buffer.size());

    if (buffer.empty())
        return std::basic_string<U>();
    else
        return std::basic_string<U>(
            &buffer[0],  boost::numeric_cast<size_t>(cch));
}


}}} // namespace washer::window::detail

#endif
