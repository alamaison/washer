/**
    @file

    Dynamic linking and loading.

    @if license

    Copyright (C) 2010, 2011, 2013, 2014
    Alexander Lamaison <awl03@doc.ic.ac.uk>

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

#ifndef WINAPI_DYNAMIC_LINK_HPP
#define WINAPI_DYNAMIC_LINK_HPP
#pragma once

#include "winapi/detail/path_traits.hpp" // choose_path
#include "winapi/detail/remove_calling_convention.hpp"
#include "error.hpp" // last_error

#include <boost/exception/info.hpp> // errinfo
#include <boost/exception/errinfo_file_name.hpp> // errinfo_file_name
#include <boost/exception/errinfo_api_function.hpp> // errinfo_api_function
#include <boost/filesystem.hpp> // basic_path, path
#include <boost/function.hpp>
#include <boost/numeric/conversion/cast.hpp>  // numeric_cast
#include <boost/shared_ptr.hpp> // shared_ptr
#include <boost/throw_exception.hpp> // BOOST_THROW_EXCEPTION
#include <boost/type_traits/remove_pointer.hpp> // remove_pointer

#include <Windows.h> // LoadLibrary, FreeLibrary, GetProcAddress,
                     // GetModuleHandle, GetModuleFileName

namespace winapi {

typedef boost::shared_ptr<boost::remove_pointer<HMODULE>::type> hmodule;

namespace detail {

    inline HMODULE get_handle(HMODULE hmod) { return hmod; }
    inline HMODULE get_handle(hmodule hmod) { return hmod.get(); }

    namespace native {

        inline HMODULE load_library(const char* file)
        { return ::LoadLibraryA(file); }

        inline HMODULE load_library(const wchar_t* file)
        { return ::LoadLibraryW(file); }

        inline HMODULE get_module_handle(const char* file)
        { return ::GetModuleHandleA(file); }

        inline HMODULE get_module_handle(const wchar_t* file)
        { return ::GetModuleHandleW(file); }

        inline DWORD module_filename(HMODULE hmod, char* file_out, DWORD size)
        { return ::GetModuleFileNameA(hmod, file_out, size); }

        inline DWORD module_filename(
            HMODULE hmod, wchar_t* file_out, DWORD size)
        { return ::GetModuleFileNameW(hmod, file_out, size); }

    }

    /**
     * Load a DLL by file name.
     *
     * This implementation works for wide or narrow paths.
     *
     * @todo  Use boost::errinfo_file_name(library_path) once we use
     *        filesystem v3 which allows all paths to be converted
     *        to std::string.
     */
#if defined(BOOST_FILESYSTEM_VERSION) && BOOST_FILESYSTEM_VERSION > 2
    inline hmodule load_library(const boost::filesystem::path& library_path)
    {
        HMODULE hinst = native::load_library(library_path.native().c_str());
        if (hinst == NULL)
            BOOST_THROW_EXCEPTION(
                boost::enable_error_info(winapi::last_error()) <<
                boost::errinfo_file_name(library_path.string()) <<
                boost::errinfo_api_function("LoadLibrary"));

        return hmodule(hinst, ::FreeLibrary);
    }
#else
    template<typename T, typename Traits>
    inline hmodule load_library(
        const boost::filesystem::basic_path<T, Traits>& library_path)
    {
        HMODULE hinst = native::load_library(
            library_path.external_file_string().c_str());
        if (hinst == NULL)
            BOOST_THROW_EXCEPTION(
                boost::enable_error_info(winapi::last_error()) <<
                boost::errinfo_api_function("LoadLibrary"));

        return hmodule(hinst, ::FreeLibrary);
    }
#endif

    /**
     * Get handle of an already-loaded DLL by file name.
     *
     * This implementation works for wide or narrow paths.
     */
#if defined(BOOST_FILESYSTEM_VERSION) && BOOST_FILESYSTEM_VERSION > 2
    inline HMODULE module_handle(const boost::filesystem::path& module_path)
    {
        HMODULE hinst = native::get_module_handle(
            (module_path.empty()) ? NULL : module_path.native().c_str());
        if (hinst == NULL)
            BOOST_THROW_EXCEPTION(
                boost::enable_error_info(winapi::last_error()) <<
                boost::errinfo_file_name(module_path.string()) <<
                boost::errinfo_api_function("GetModuleHandle"));

        return hinst;
    }
#else
    template<typename T, typename Traits>
    inline HMODULE module_handle(
        const boost::filesystem::basic_path<T, Traits>& module_path)
    {
        HMODULE hinst = native::get_module_handle(
            (module_path.empty()) ?
                NULL : module_path.external_file_string().c_str());
        if (hinst == NULL)
            BOOST_THROW_EXCEPTION(
                boost::enable_error_info(winapi::last_error()) <<
                boost::errinfo_api_function("GetModuleHandle"));

        return hinst;
    }
#endif

    /**
     * Get handle of currently-loaded executable.
     */
    inline HMODULE module_handle()
    {
        return module_handle(boost::filesystem::wpath());
    }
}

/**
 * Load a DLL by file name.
 */
inline hmodule load_library(const boost::filesystem::path& library_path)
{ return detail::load_library(library_path); }

/**
 * Load a DLL by file name.
 */
#if defined(BOOST_FILESYSTEM_VERSION) && BOOST_FILESYSTEM_VERSION < 3
inline hmodule load_library(const boost::filesystem::wpath& library_path)
{ return detail::load_library(library_path); }
#endif

/**
 * Get handle of an already-loaded module by file name.
 */
inline HMODULE module_handle(const boost::filesystem::path& module_path)
{ return detail::module_handle(module_path); }

/**
 * Get handle of an already-loaded module by file name.
 */
#if defined(BOOST_FILESYSTEM_VERSION) && BOOST_FILESYSTEM_VERSION < 3
inline HMODULE module_handle(const boost::filesystem::wpath& module_path)
{ return detail::module_handle(module_path); }
#endif

/**
 * Get handle of current executable.
 */
inline HMODULE module_handle() { return detail::module_handle(); }

/**
 * Path to the module whose handle is @p module which has been loaded by the
 * current process.
 */
template<typename T, typename H>
inline typename detail::choose_path<T>::type module_path(H module)
{
    std::vector<T> buffer(MAX_PATH);
    DWORD size = detail::native::module_filename(
        detail::get_handle(module), &buffer[0],
        boost::numeric_cast<DWORD>(buffer.size()));

    if (size >= buffer.size())
        BOOST_THROW_EXCEPTION(
            boost::enable_error_info(
                std::logic_error("Insufficient buffer space")) <<
            boost::errinfo_api_function("GetModuleFileName"));
    if (size == 0)
        BOOST_THROW_EXCEPTION(
            boost::enable_error_info(winapi::last_error()) <<
            boost::errinfo_api_function("GetModuleFileName"));

    return typename detail::choose_path<T>::type(
        buffer.begin(), buffer.begin() + size);
}

/**
 * Path to the current executable.
 */
template<typename T>
inline typename detail::choose_path<T>::type module_path()
{
    return module_path<T, HMODULE>(NULL);
}

/**
 * Dynamically bind to function given by name.
 *
 * @param hmod  Module defining the requested function.
 * @param name  Name of the function.
 * @returns  Pointer to the function with signature T.
 */
template<typename T>
inline T proc_address(HMODULE hmod, const std::string& name)
{
    FARPROC f = ::GetProcAddress(detail::get_handle(hmod), name.c_str());
    if (f == NULL)
        BOOST_THROW_EXCEPTION(
            boost::enable_error_info(winapi::last_error()) <<
            boost::errinfo_api_function("GetProcAddress"));

    return reinterpret_cast<T>(f);
}

/**
 * Dynamically bind to function given by name.
 *
 * @warning  It is the caller's responsibility to ensure that module `hmod`
 *           remains alive for the duration of any calls to the returned
 *           function.
 *
 * @param hmod  Module defining the requested function.
 * @param name  Name of the function.
 * @returns  Pointer to the function with signature T.
 */
template<typename T>
inline T proc_address(hmodule hmod, const std::string& name)
{
    return proc_address<T>(hmod.get(), name);
}

namespace detail {

    // The purpose of this class is to link the library lifetime to the
    // lifetime of the callable.  This prevents the loaded function being called
    // after the library it was loaded having been unloaded
    template<typename Signature>
    class library_function
    {
    public:
        library_function(hmodule library, const std::string& function_name)
            :
        m_library(library),
        m_function(
            ::winapi::proc_address<Signature*>(m_library, function_name))
        {}

        operator Signature*() const
        {
            return m_function;
        }

    private:
        hmodule m_library;
        Signature* m_function;
    };

    /**
     * Dynamically bind to function given by name.
     */
#if defined(BOOST_FILESYSTEM_VERSION) && BOOST_FILESYSTEM_VERSION > 2
    template<typename Signature>
    inline boost::function<
        typename detail::remove_calling_convention<Signature>::type>
    load_function(
        const boost::filesystem::path& library_path,
        const std::string& function_name)
#else
    template<typename Signature, typename String, typename Traits>
    inline boost::function<
        typename detail::remove_calling_convention<Signature>::type>
    load_function(
        const boost::filesystem::basic_path<String, Traits>& library_path,
        const std::string& function_name)
#endif
    {
        hmodule library = ::winapi::load_library(library_path);

        // Embed the module handle in the callable we return, so that the
        // module remains loaded for the lifetime of the call.
        boost::function<
            typename detail::remove_calling_convention<Signature>::type> f =
            library_function<Signature>(library, function_name);
        return f;
    }

}

/**
 * Dynamically bind to function given by name.
 *
 * If the function uses a non-default calling convention, specify it in the
 * given `Signature`.  The returned function will have the same signature but
 * with the default calling convention:
 *
 *     function<int(char*)> f = load_function<int __stdcall (char*)>(
 *         "my_lib.dll", "my_func");
 *
 * @param library_path  Path or filename of the DLL exporting the
                        requested function.
 * @param name          Name of the function.
 * @returns  Callable that invokes loaded function with given `Signature`.
 */
template<typename Signature>
inline boost::function<
    typename detail::remove_calling_convention<Signature>::type>
load_function(
    const boost::filesystem::path& library_path, const std::string& name)
{ return detail::load_function<Signature>(library_path, name); }

/**
 * Dynamically bind to function given by name.
 *
 * If the function uses a non-default calling convention, specify it in the
 * given `Signature`.  The returned function will have the same signature but
 * with the default calling convention:
 *
 *     function<int(char*)> f = load_function<int __stdcall (char*)>(
 *         "my_lib.dll", "my_func");
 *
 * @param library_path  Path or filename of the DLL exporting the
                        requested function.
 * @param name          Name of the function.
 * @returns  Callable that invokes loaded function with given `Signature`.
 */
#if defined(BOOST_FILESYSTEM_VERSION) && BOOST_FILESYSTEM_VERSION < 3
template<typename Signature>
inline boost::function<
    typename detail::remove_calling_convention<Signature>::type>
load_function(
    const boost::filesystem::wpath& library_path, const std::string& name)
{ return detail::load_function<Signature>(library_path, name); }
#endif

} // namespace winapi

#endif
