/**
    @file

    Clipboard functions.

    @if license

    Copyright (C) 2011, 2013  Alexander Lamaison <awl03@doc.ic.ac.uk>

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

#ifndef WINAPI_CLIPBOARD_HPP
#define WINAPI_CLIPBOARD_HPP
#pragma once

#include "error.hpp" // last_error

#include <boost/exception/info.hpp> // errinfo
#include <boost/exception/errinfo_api_function.hpp> // errinfo_api_function
#include <boost/throw_exception.hpp> // BOOST_THROW_EXCEPTION

#include <windows.h> // RegisterClipboardFormat

namespace winapi {
namespace clipboard {

namespace detail {

    template<typename T>
    inline UINT register_format_(const T* format_name);

    template<>
    inline UINT register_format_(const char* format_name)
    {
        return ::RegisterClipboardFormatA(format_name);
    }

    template<>
    inline UINT register_format_(const wchar_t* format_name)
    {
        return ::RegisterClipboardFormatW(format_name);
    }

    template<typename T>
    inline CLIPFORMAT register_format(const T* format_name)
    {
        UINT format_id = detail::register_format_(format_name);
        if (format_id == 0)
            BOOST_THROW_EXCEPTION(
                boost::enable_error_info(winapi::last_error()) <<
                boost::errinfo_api_function("RegisterClipboardFormat"));

        return static_cast<CLIPFORMAT>(format_id);
    }
}

/**
 * Register new clipboard format.
 */
inline CLIPFORMAT register_format(const wchar_t* format_name)
{
    return detail::register_format(format_name);
}

/**
 * Register new clipboard format (ANSI version).
 */
inline CLIPFORMAT register_format(const char* format_name)
{
    return detail::register_format(format_name);
}

}} // namespace winapi::clipboard

#endif
