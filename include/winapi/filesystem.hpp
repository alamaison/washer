/**
    @file

    File and directory functions.

    @if license

    Copyright (C) 2012  Alexander Lamaison <awl03@doc.ic.ac.uk>

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

#ifndef WINAPI_FILESYSTEM_HPP
#define WINAPI_FILESYSTEM_HPP
#pragma once

#include "winapi/detail/path_traits.hpp" // choose_path

#include "winapi/error.hpp" // last_error

#include <boost/exception/info.hpp> // errinfo
#include <boost/exception/errinfo_file_name.hpp> // errinfo_file_name
#include <boost/exception/errinfo_api_function.hpp> // errinfo_api_function
#include <boost/numeric/conversion/cast.hpp>  // numeric_cast
#include <boost/filesystem/path.hpp> // basic_path
#include <boost/system/system_error.hpp> // system_error, get_system_category
#include <boost/throw_exception.hpp> // BOOST_THROW_EXCEPTION
#include <boost/uuid/uuid_generators.hpp> // uuid, random_generator
#include <boost/uuid/uuid_io.hpp> // uuid <<

#include <sstream> // basic_stringstream
#include <vector>

#include <Windows.h>

namespace winapi {
namespace filesystem {

namespace detail {

    inline DWORD get_temp_path(DWORD buffer_length, char* buffer)
    {
        return ::GetTempPathA(buffer_length, buffer);
    }

    inline DWORD get_temp_path(DWORD buffer_length, wchar_t* buffer)
    {
        return ::GetTempPathW(buffer_length, buffer);
    }

}

/**
 * Returns the absolute path to the system temporary directory.
 */
template<typename T>
inline typename ::winapi::detail::choose_path<T>::type
temporary_directory_path()
{
    T* null = 0;
    DWORD required_len = detail::get_temp_path(0, null);
    if (required_len == 0)
        BOOST_THROW_EXCEPTION(
            boost::enable_error_info(winapi::last_error()) <<
            boost::errinfo_api_function("GetTempPath"));

    std::vector<T> buffer(required_len);
    DWORD len = detail::get_temp_path(
        boost::numeric_cast<DWORD>(buffer.size()), &buffer[0]);
    if (len == 0)
        BOOST_THROW_EXCEPTION(
            boost::enable_error_info(winapi::last_error()) <<
            boost::errinfo_api_function("GetTempPath"));

    return typename ::winapi::detail::choose_path<T>::type(
        buffer.begin(), buffer.begin() + len);
}

/**
 * Return a name to use to create a new file or directory that is
 * sufficiently random never to collide.
 *
 * This is not an absolute path so may often need to be combined with the
 * result of temporary_directory_path().
 */
template<typename T>
inline typename ::winapi::detail::choose_path<T>::type unique_path()
{
    boost::uuids::uuid name = boost::uuids::random_generator()();

    std::basic_stringstream< T, std::char_traits<T>, std::allocator<T> >  out;
    out << name;
    return out.str();
}

}}

#endif
