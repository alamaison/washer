/**
    @file

    Shell utility functions.

    @if license

    Copyright (C) 2010, 2011, 2012  Alexander Lamaison <awl03@doc.ic.ac.uk>

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

#ifndef WINAPI_SHELL_SHELL_HPP
#define WINAPI_SHELL_SHELL_HPP
#pragma once

#include <winapi/detail/path_traits.hpp> // choose_path
#include <winapi/shell/pidl_iterator.hpp> // pidl_iterator
#include <winapi/shell/folder_error_adapters.hpp> // comtype<IShellFolder>
#include <winapi/shell/pidl.hpp> // cpidl_t, apidl_t

#include <comet/ptr.h> // com_ptr
#include <comet/error.h> // com_error

#include <boost/exception/errinfo_api_function.hpp> // errinfo_api_function
#include <boost/exception/info.hpp> // errinfo
#include <boost/shared_ptr.hpp> // shared_ptr
#include <boost/throw_exception.hpp> // BOOST_THROW_EXCEPTION
#include <boost/utility.hpp> // next

#include <cassert> // assert
#include <stdexcept> // runtime_error, logic_error
#include <vector>

#include <shlobj.h> // SHGetSpecialFolderPath, SHGetDesktopFolder
#include <Shlwapi.h> // StrRetToStr, SHStrDup

namespace winapi {
namespace shell {

namespace detail {
    namespace native {

        inline HRESULT special_folder_path(
            HWND hwnd, char* path_out, int folder, BOOL create)
        { return ::SHGetSpecialFolderPathA(hwnd, path_out, folder, create); }

        inline HRESULT special_folder_path(
            HWND hwnd, wchar_t* path_out, int folder, BOOL create)
        { return ::SHGetSpecialFolderPathW(hwnd, path_out, folder, create); }


        inline HRESULT str_ret_to_str(
            STRRET* strret, const ITEMID_CHILD* pidl, char** string_out)
        { return ::StrRetToStrA(strret, pidl, string_out); }

        inline HRESULT str_ret_to_str(
            STRRET* strret, const ITEMID_CHILD* pidl, wchar_t** string_out)
        { return ::StrRetToStrW(strret, pidl, string_out); }


        inline HRESULT sh_str_dup(const char* narrow_in, wchar_t** wide_out)
        { return ::SHStrDupA(narrow_in, wide_out); }

        inline HRESULT sh_str_dup(const wchar_t* wide_in, wchar_t** wide_out)
        { return ::SHStrDupW(wide_in, wide_out); }
    }

    /**
    * Create a STRRET from a string.
    *
    * @note  Stores as wide string regardless of input type to avoid MAX_PATH
    *        size limit of cStr.
    */
    template<typename T>
    inline STRRET string_to_strret(const std::basic_string<T>& str)
    {
        STRRET strret;
        strret.uType = STRRET_WSTR;
        strret.pOleStr = NULL;

        HRESULT hr = detail::native::sh_str_dup(str.c_str(), &strret.pOleStr);
        if (FAILED(hr))
            BOOST_THROW_EXCEPTION(
            boost::enable_error_info(
            comet::com_error(
            "Failed to copy string", hr, "SHStrDup")) <<
            boost::errinfo_api_function("SHStrDup"));

        return strret;
    }
}

/**
 * Common system folder path by CSIDL.
 *
 * @example
 *   @code known_folder_path(CSIDL_PROFILE) @endcode returns something like
 *   @code C:\\Users\\Username @endcode
 */
template<typename T>
inline typename winapi::detail::choose_path<T>::type special_folder_path(
    int folder, bool create_if_missing=false)
{
    std::vector<T> buffer(MAX_PATH);
    BOOL found = detail::native::special_folder_path(
        NULL, &buffer[0], folder, create_if_missing);

    if (!found)
        BOOST_THROW_EXCEPTION(
            boost::enable_error_info(
                std::runtime_error("Couldn't find special folder")) <<
                boost::errinfo_api_function("SHGetSpecialFolderPath"));

    buffer[buffer.size() - 1] = T(); // null-terminate

    return typename winapi::detail::choose_path<T>::type(&buffer[0]);
}

/**
 * Common system folder pidl by CSIDL.
 */
inline winapi::shell::pidl::apidl_t special_folder_pidl(int folder)
{
    winapi::shell::pidl::apidl_t pidl;
    HRESULT hr = ::SHGetSpecialFolderLocation(NULL, folder, pidl.out());
    if (FAILED(hr))
        BOOST_THROW_EXCEPTION(
            boost::enable_error_info(comet::com_error(hr)) <<
            boost::errinfo_api_function("SHGetSpecialFolderLocation"));

    return pidl;
}

/**
 * Return the desktop folder IShellFolder handler.
 *
 * There is no guarantee that the same object is returned each time.  It appears
 * that if you call CoInitialize before calling this function, the same object
 * is returned on every call.  Otherwise, it isn't.
 */
inline comet::com_ptr<IShellFolder> desktop_folder()
{
    comet::com_ptr<IShellFolder> folder;
    HRESULT hr = ::SHGetDesktopFolder(folder.out());
    if (FAILED(hr))
        BOOST_THROW_EXCEPTION(
            boost::enable_error_info(comet::com_error(hr)) <<
            boost::errinfo_api_function("SHGetDesktopFolder"));
    return folder;
}

/**
 * Convert a STRRET structure to a string.
 *
 * If the STRRET is using its pOleStr member to store the data (rather
 * than holding it directly or extracting it from the PIDL offset)
 * the data will be freed.  In other words, this function destroys
 * the STRRET passed to it.
 *
 * @param strret  STRRET to convert into string.  Its contents are destroyed
 *                by this function if using STRRET_WSTR type.
 * @param pidl    PIDL in which the string data may be embedded (optional).
 */
template<typename T>
inline std::basic_string<T> strret_to_string(
    STRRET& strret, const pidl::cpidl_t& pidl=pidl::cpidl_t())
{
    T* str = NULL;
    HRESULT hr = detail::native::str_ret_to_str(&strret, pidl.get(), &str);

    // RAII for CoTaskMemAlloced string
    boost::shared_ptr<T> str_lifetime(str, ::CoTaskMemFree);

    if (FAILED(hr))
        BOOST_THROW_EXCEPTION(
            boost::enable_error_info(
                comet::com_error(
                    "Failed to convert STRRET to string", hr,
                    "StrRetToStr")) <<
            boost::errinfo_api_function("StrRetToStr"));


    return (str) ? std::basic_string<T>(str) : std::basic_string<T>();
}

/**
 * Create a STRRET from an ANSI string.
 *
 * @note  Stores the string in the STRRET's unicode string field.
 */
inline STRRET string_to_strret(const std::string& str)
{ return detail::string_to_strret(str); }

/**
 * Create a STRRET from a Unicode string.
 */
inline STRRET string_to_strret(const std::wstring& str)
{ return detail::string_to_strret(str); }

/**
 * Return the FORPARSING name of the given PIDL.
 *
 * For filesystem items this will be the absolute path.
 */
inline std::wstring parsing_name_from_pidl(
    const winapi::shell::pidl::apidl_t& pidl)
{
    comet::com_ptr<IShellFolder> folder;
    PCUITEMID_CHILD child_pidl;
    HRESULT hr = ::SHBindToParent(
        pidl.get(), folder.iid(), reinterpret_cast<void**>(folder.out()),
        &child_pidl);
    if (FAILED(hr))
        BOOST_THROW_EXCEPTION(
            boost::enable_error_info(comet::com_error(hr)) <<
            boost::errinfo_api_function("SHBindToParent"));

    STRRET str;
    hr = folder->GetDisplayNameOf(child_pidl, SHGDN_FORPARSING, &str);
    if (FAILED(hr))
        BOOST_THROW_EXCEPTION(comet::com_error_from_interface(folder, hr));

    return strret_to_string<wchar_t>(str, child_pidl);
}

/**
 * Fetch a PIDL from a display name.
 */
inline winapi::shell::pidl::apidl_t pidl_from_parsing_name(
    const std::wstring& parsing_name)
{
    // We make a copy of the name string because ParseDisplayName
    // might modify the string it's passed.
    std::vector<wchar_t> name(
        parsing_name.c_str(), parsing_name.c_str() + parsing_name.length() + 1);
    assert(!name.empty());
    if (name.empty())
        BOOST_THROW_EXCEPTION(std::logic_error("Empty buffer"));
    name[name.size() - 1] = wchar_t(); // null-terminate

    winapi::shell::pidl::apidl_t pidl;
    comet::com_ptr<IShellFolder> desktop = desktop_folder();

    // Casting the PIDL from relative to absolute here is OK because it's
    // relative to the desktop, i.e., absolute.
    HRESULT hr = desktop->ParseDisplayName(
        NULL, NULL, &name[0], NULL,
        reinterpret_cast<PIDLIST_RELATIVE*>(pidl.out()), NULL);
    if (FAILED(hr))
        BOOST_THROW_EXCEPTION(comet::com_error_from_interface(desktop, hr));

    return pidl;
}

/**
 * Bind to the handler object of an item.
 *
 * This handler object is usually an IShellFolder implementation but may be
 * an IStream as well as other handler types.  The type of handler is
 * determined by the template parameter.
 *
 * Analogous to BindToObject().
 *
 * @tparam T  Type of handler to return.
 *
 * @param pidl  The item for which the handler is being requested.  Usually a
 *              PIDL for a folder when T is IShellFolder.
 *              If pidl is empty, the item is the Desktop (namespace root).
 */
template<typename T>
inline comet::com_ptr<T> bind_to_handler_object(
    const winapi::shell::pidl::apidl_t& pidl)
{
    comet::com_ptr<IShellFolder> desktop = winapi::shell::desktop_folder();
    comet::com_ptr<T> handler;

    if (pidl.empty()) // get handler via QI
        return try_cast(desktop);

    HRESULT hr = desktop->BindToObject(
        (pidl.empty()) ? NULL : pidl.get(), NULL, comet::uuidof<T>(), 
        reinterpret_cast<void**>(handler.out()));

    if (FAILED(hr))
        BOOST_THROW_EXCEPTION(comet::com_error_from_interface(desktop, hr));
    if (!handler)
        BOOST_THROW_EXCEPTION(comet::com_error(E_FAIL));

    return handler;
}

/**
 * Bind to the parent object of an absolute PIDL.
 *
 * Analogous to SHBindToParent.
 *
 * The implementation doesn't call SHBindToParent and Windows 9x doesn't have
 * that API function.  This is different to SHBindToParent (or at least the
 * Wine version: http://source.winehq.org/source/dlls/shell32/pidl.c#L1282) in
 * that the desktop (empty) PIDL is not a legal argument.  It makes no sense to
 * ask for the parent of the top level and makes even less sense when we try and
 * return its item (child PIDL).
 *
 * @tparam T  Type of interface on the parent object to return.
 *
 * @returns  The requested interface of the parent object along
 *           with a copy of the last item in the given PIDL.
 */
template<typename T>
inline std::pair<comet::com_ptr<T>, pidl::cpidl_t> bind_to_parent(
    const pidl::apidl_t& pidl)
{
    if (pidl.empty())
        BOOST_THROW_EXCEPTION(std::logic_error("Already at top level"));

    // Create parent of PIDL given
    pidl::apidl_t parent;
    pidl::pidl_iterator it(pidl);
    while (next(it) != pidl::pidl_iterator())
    {
        parent += *it++;
    }

    pidl::cpidl_t item = *it;

    comet::com_ptr<T> requested_interface;
    if (parent.empty())
    {
        /*
        The given PIDL is a child of the desktop so the requested
        interface is one supported by the desktop folder
        */
        requested_interface = comet::try_cast(desktop_folder());
    }
    else
    {
        requested_interface = bind_to_handler_object<T>(parent);
    }

    return std::make_pair(requested_interface, item);
}

/**
 * Given a PIDL, return an IStream to it.
 *
 * @note  This fails with E_NOTIMPL on Windows 2000 and below.
 */
inline comet::com_ptr<IStream> stream_from_pidl(const pidl::apidl_t& pidl)
{
    std::pair<comet::com_ptr<IShellFolder>, pidl::cpidl_t> parent =
        bind_to_parent<IShellFolder>(pidl);

    comet::com_ptr<IStream> stream;
    
    HRESULT hr = parent.first->BindToObject(
        parent.second.get(), NULL, stream.iid(),
        reinterpret_cast<void**>(stream.out()));
    if (FAILED(hr))
    {
        hr = parent.first->BindToStorage(
            parent.second.get(), NULL, stream.iid(),
            reinterpret_cast<void**>(stream.out()));
        if (FAILED(hr))
            BOOST_THROW_EXCEPTION(
                comet::com_error(
                    L"Couldn't get stream for source file: " +
                    parsing_name_from_pidl(pidl), hr));
    }

    return stream;
}

}} // namespace winapi::shell

#endif
