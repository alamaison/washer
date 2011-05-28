/**
    @file

    Window hooking.

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

#ifndef WINAPI_HOOK_HPP
#define WINAPI_HOOK_HPP
#pragma once

#include <winapi/error.hpp> // last_error

#include <boost/exception/errinfo_api_function.hpp> // errinfo_api_function
#include <boost/exception/info.hpp> // errinfo
#include <boost/throw_exception.hpp> // BOOST_THROW_EXCEPTION
#include <boost/type_traits/remove_pointer.hpp> // remove_pointer

#include <Windows.h> // GetCurrentThreadId, SetWindowsHookEx

namespace winapi {

typedef boost::shared_ptr<boost::remove_pointer<HHOOK>::type> hhook;

namespace detail {

inline hhook windows_hook(int type, HOOKPROC hook_function, DWORD thread_id)
{
    HHOOK hook = ::SetWindowsHookExW(type, hook_function, NULL, thread_id);
    if (hook == NULL)
        BOOST_THROW_EXCEPTION(
            boost::enable_error_info(last_error()) <<
            boost::errinfo_api_function("SetWindowsHookExW"));

    return hhook(hook, ::UnhookWindowsHookEx);
}

}

/**
 * Install a windows hook function for the current thread.
 *
 * The hook is uninstalled when the returned hhook goes out of scope.
 */
inline hhook windows_hook(int type, HOOKPROC hook_function)
{
    return detail::windows_hook(type, hook_function, ::GetCurrentThreadId());
}

/**
 * Install a windows hook function globally.
 *
 * The hook is uninstalled when the returned hhook goes out of scope.
 */
inline hhook windows_global_hook(int type, HOOKPROC hook_function)
{
    return detail::windows_hook(type, hook_function, 0);
}

}

#endif
