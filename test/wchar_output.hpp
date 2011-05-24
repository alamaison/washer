/**
    @file

    Helper functions for Boost.Test allowing wide strings as test output.

    @if license

    Copyright (C) 2009, 2010, 2011  Alexander Lamaison <awl03@doc.ic.ac.uk>

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

#ifndef WINAPI_TEST_WCHAR_OUTPUT_HPP
#define WINAPI_TEST_WCHAR_OUTPUT_HPP
#pragma once

#include <winapi/error.hpp> // last_error

#include <boost/filesystem.hpp> // wpath
#include <boost/numeric/conversion/cast.hpp> // numeric_cast
#include <boost/test/test_tools.hpp>
#include <boost/throw_exception.hpp>  // BOOST_THROW_EXCEPTION

#include <cassert> // assert
#include <ostream>
#include <string>
#include <vector>

#include <Objbase.h> // GetRunningObjectTable
#include <Windows.h> // WideCharToMultiByte

namespace winapi {
namespace test {

namespace detail {

template<typename _FromElem, typename _ToElem>
struct converter
{
    typedef _FromElem FromElem;
    typedef _ToElem ToElem;
    typedef std::basic_string<FromElem> FromType;
    typedef std::basic_string<ToElem> ToType;
};

struct narrow : converter<wchar_t, char>
{
    int operator()(
        const FromElem* pszWide, int cchWide,
        ToElem* pszNarrow, int cbNarrow)
    {
        return ::WideCharToMultiByte(
            CP_UTF8, 0, pszWide, cchWide, pszNarrow, cbNarrow, NULL, NULL);
    }
};

}


/**
 * Convert a basic_string-style string from one element type to another.
 *
 * @tparam T  Converter functor to perform the actual conversion.
 */
template<typename T>
inline typename T::ToType convert_string(const typename T::FromType& from)
{
    const int size = boost::numeric_cast<int>(from.size());
    if (size == 0)
        return T::ToType();

    // Calculate necessary buffer size
    int len = T()(from.data(), size, NULL, 0);

    // Perform actual conversion
    if (len > 0)
    {
        std::vector<T::ToElem> buffer(len);
        len = T()(
            from.data(), size,
            &buffer[0], static_cast<int>(buffer.size()));
        if (len > 0)
        {
            assert(len == boost::numeric_cast<int>(buffer.size()));
            return T::ToType(&buffer[0], len);
        }
    }

    BOOST_THROW_EXCEPTION(winapi::last_error());
}

/**
 * Convert a Windows wide string to a UTF-8 (multi-byte) string.
 */
inline std::string wide_string_to_utf8(const std::wstring& wide)
{
    return convert_string<detail::narrow>(wide);
}

}}

namespace std {

    inline std::ostream& operator<<(
        std::ostream& out, const std::wstring& wide_in)
    {
        out << winapi::test::wide_string_to_utf8(wide_in);
        return out;
    }

    inline std::ostream& operator<<(
        std::ostream& out, const wchar_t* wide_in)
    {
        out << std::wstring(wide_in);
        return out;
    }

    inline std::ostream& operator<<(
        std::ostream& out, const boost::filesystem::wpath& path)
    {
        out << path.string();
        return out;
    }
}

#endif

