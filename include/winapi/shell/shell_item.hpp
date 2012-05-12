/**
    @file

    Shell item classes.

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

#ifndef WINAPI_SHELL_SHELL_ITEM_HPP
#define WINAPI_SHELL_SHELL_ITEM_HPP
#pragma once

#include <winapi/shell/pidl.hpp> // apidl_t, cpidl_t
#include <winapi/shell/folder_error_adapters.hpp> // comtype<IShellFolder>

#include <comet/ptr.h> // com_ptr
#include <comet/error.h> // com_error

#include <boost/detail/scoped_enum_emulation.hpp> // BOOST_SCOPED_ENUM
#include <boost/throw_exception.hpp> // BOOST_THROW_EXCEPTION

#include <cassert> // assert
#include <stdexcept> // invalid_argument

#include <ShObjIdl.h> // SHGDNF

namespace winapi {
namespace shell {

template<typename T>
inline comet::com_ptr<T> bind_to_parent(const pidl::apidl_t& pidl);

template<typename T>
inline std::basic_string<T> strret_to_string(
    STRRET& strret, const pidl::cpidl_t& pidl);

/**
 * Interface to items in the shell namespace.
 *
 * **Purpose:** to encapsulate those operations common to all shell namespace
 * inhabitants.
 *
 * @note There is no assumption that the shell item is linked to a PIDL though,
 * of course, that is the case internally.
 */
class shell_item
{
public:

    /**
     * Names suitable for use in UI.
     */
    BOOST_SCOPED_ENUM_START(friendly_name_type)
    {
        /**
         * Name of the item suitable for use in UI anywhere in the system.
         *
         * This is the name that should generally be used when displaying a
         * name for the item anywhere.  It is designed to be useful even outside
         * the context of its containing folder.  In other words, the name may
         * include some disambiguation information (such as the name of
         * a computer) in order to make the name make sense when displayed
         * apart from its folder (folder such as in a shortcut on the desktop).
         *
         * This name may not uniquely identify the item, even within its parent
         * folder.  Files, for example, might not include the file extension so
         * two files with the same stem but different extensions give the same
         * name.
         *
         * - Equivalent to: `SIGDN_NORMALDISPLAY`
         * - Equivalent to: `SHGDN_NORMAL`
         *
         * *Examples:*
         *
         * Item                      | Name
         * ------------------------- | ---------------
         * C:\\directory\\file.txt   | file
         * C:\\                      | Local Disk (C:)
         * Control Panel\\Printers   | Printers
         */
        in_global_context,

        /**
         * Name of the item suitable for use in UI only in the item's folder
         *
         * This name should not be used anywhere else as it may
         * not include enough disambiguation information (such as the name of
         * a computer) in order to make sense elsewhere.
         *
         * This name may not uniquely identify the item, even within its parent
         * folder.  Files, for example, might not include the file extension so
         * two files with the same stem but different extensions give the same
         * name.
         *
         * - Equivalent to: `SIGDN_PARENTRELATIVE`
         * - Equivalent to: `SHGDN_INFOLDER`
         *
         * *Examples:*
         *
         * Item                      | Name
         * ------------------------- | ---------------
         * C:\\directory\\file.txt   | file
         * C:\\                      | Local Disk (C:)
         * Control Panel\\Printers   | Printers
         */
         in_folder_context,

        /**
         * Name of the item suitable for renaming it.
         *
         * This name is suitable for use either in or outside the context of its
         * containing folder as it can never include disambiguation information;
         * renaming an item must involve the same string wherever it occurs.
         *
         * The name does not have to be the *full* name of the item, however.
         * Files, for example, might not include the file extension.
         *
         * - Equivalent to: `SIGDN_PARENTRELATIVEEDITING`
         * - Equivalent to: `SHGDN_INFOLDER | SHGDN_FOREDITING`
         *
         * *Examples:*
         *
         * Item                      | Name
         * ------------------------- | ---------------
         * C:\\directory\\file.txt   | file
         * C:\\                      | Local Disk
         * Control Panel\\Printers   | Printers
         */
         editable,

        /**
         * Absolute unique name of the item suitable for use in UI.
         *
         * This is very similar to the absolute parsing name but will never give
         * an unfriendly name so is always safe to show the user.  This means
         * it is not necessarily possible to parse the name to obtain a PIDL
         * but the name should nevertheless be unique throughout the system.
         *
         * For filesystem items this is the absolute path but for virtual items
         * that don't have a user-readable path this may just be its name.
         *
         * This name is suitable for use in the address bar among other places.
         * It will not always be a filesystem path but will never be an 
         * unfriendly GUID-style name.  Sometime it will be the same as the 
         * normal display name such as "Computer".
         *
         * - Equivalent to: `SIGDN_DESKTOPABSOLUTEEDITING`
         * - Equivalent to: `SHGDN_FORPARSING | SHGDN_FORADDRESSBAR`
         *
         * *Examples:*
         *
         * Item                      | Name
         * ------------------------- | ---------------
         * C:\\directory\\file.txt   | C:\\directory\\file.txt
         * C:\\                      | C:\\
         * Control Panel\\Printers   | Control Panel\\Printers
         */
        absolute,

        /**
         * Relative unique name of the item suitable for use in UI.
         *
         * This is very similar to the relative parsing name but will never give
         * an unfriendly name so is always safe to show the user.  This means
         * it is not necessarily possible to parse the name to obtain a PIDL
         * but the name should nevertheless be unique with respect to the
         * parent folder.
         *
         * For some reason, drives don't get a trailing slash here unlike the
         * absolute name.
         *
         * - Equivalent to: `SIGDN_PARENTRELATIVEFORADDRESSBAR`
         * - Equivalent to: `SHGDN_INFOLDER | SHGDN_FORPARSING |
         *                   SHGDN_FORADDRESSBAR`
         *
         * *Examples:*
         *
         * Item                      | Name
         * ------------------------- | ---------------
         * C:\\directory\\file.txt   | file.txt
         * C:\\                      | C:
         * Control Panel\\Printers   | Printers
         */
        relative,

        default = in_global_context
    };
    BOOST_SCOPED_ENUM_END;

    virtual std::wstring friendly_name(
        BOOST_SCOPED_ENUM(friendly_name_type) type=friendly_name_type::default)
    const = 0;

    /**
     * Names only suitable for internal program use.
     */
    BOOST_SCOPED_ENUM_START(parsing_name_type)
    {
        /**
         * Name that uniquely identifies the item in the system.
         *
         * The name is not suitable for use in UI but is suitable for passing
         * to SHParseDisplayName to return the item's PIDL.
         *
         * -  Equivalent to: `SIGDN_DESKTOPABSOLUTEPARSING`
         * -  Equivalent to: `SHGDN_FORPARSING`
         *
         * *Examples:*
         *
         * Item                     | Name
         * ------------------------ | ---------------
         * C:\\directory\\file.txt  | C:\\directory\\file.txt
         * C:\\                     | C:\\
         * Control Panel\\Printers  | ::{21EC2020-3AEA-1069-A2DD-08002B30309D}\\
         *                            ::{2227A280-3AEA-1069-A2DE-08002B30309D}
         */
        absolute,

        /**
         * Name that uniquely identifies the item within its parent folder.
         *
         * The name is not suitable for use in UI but is suitable for passing
         * to the parent's ParseDisplayName method to return the item's PIDL.
         *
         * For some reason, drives don't get a trailing slash here unlike the
         * absolute name.
         *
         * - Equivalent to: `SIGDN_PARENTRELATIVE`
         * - Equivalent to: `SHGDN_FORPARSING | SHGDN_INFOLDER`
         *
         * *Examples:*
         *
         * Item                     | Name
         * ------------------------ | ---------------
         * C:\\directory\\file.txt  | file.txt
         * C:\\                     | C:
         * Control Panel\\Printers  | ::{2227A280-3AEA-1069-A2DE-08002B30309D}
         */
        relative,

        default = absolute
    };
    BOOST_SCOPED_ENUM_END;

    virtual std::wstring parsing_name(
        BOOST_SCOPED_ENUM(parsing_name_type) type=parsing_name_type::default)
    const = 0;

    virtual ~shell_item() {}

};

namespace detail {

    inline SHGDNF friendly_name_type_to_shgdnf(
        BOOST_SCOPED_ENUM(shell_item::friendly_name_type) type)
    {
        switch(type)
        {
        case shell_item::friendly_name_type::in_global_context:
            return SHGDN_NORMAL;
        case shell_item::friendly_name_type::in_folder_context:
            return SHGDN_INFOLDER;

        case shell_item::friendly_name_type::editable:
            // We think that FOREDITING always produces the same string 
            // regardless of whether INFOLDER is included.  It doesn't make 
            // sense for them to be different as you must always rename the 
            // same string.

            // The comments in shobjidl.idl imply that this editing name should
            // include INFOLDER but maybe they do that only to be defensive
            // against flaky folder implementations.

            return SHGDN_FOREDITING | SHGDN_INFOLDER;

        case shell_item::friendly_name_type::absolute:

            // Despite the presence of FORPARSING here, this does give us a 
            // 'friendly' name.  Without FORPARSING, it only gives the single
            // name of the item.

            // The comments in shobjidl.idl say this is equivalent to
            // SIGDN_DESKTOPABSOLUTEEDITING.

            return SHGDN_FORPARSING | SHGDN_FORADDRESSBAR;

        case shell_item::friendly_name_type::relative:
            
            // This is the folder-relative version of 'absolute' above that
            // that shobjidl.idl calls SIGDN_PARENTRELATIVEFORADDRESSBAR.
            //
            // Despite the presence of FORPARSING here, this does give us a 
            // 'friendly' name.  FORPARSING is necessary to include the file
            // extension.  Without it it gives the same result as SHGDN_NORMAL.
            // INFOLDER is needed to get a relative path rather than the
            // absolute path above.

            return SHGDN_INFOLDER | SHGDN_FORPARSING | SHGDN_FORADDRESSBAR;

        default:
            BOOST_THROW_EXCEPTION(
                std::invalid_argument("Unknown friendly name type"));
        }
    }

    inline SHGDNF parsing_name_type_to_shgdnf(
        BOOST_SCOPED_ENUM(shell_item::parsing_name_type) type)
    {
        switch(type)
        {
        case shell_item::parsing_name_type::relative:
            return SHGDN_FORPARSING | SHGDN_INFOLDER;

        case shell_item::parsing_name_type::absolute:
            return SHGDN_FORPARSING;

        default:
            BOOST_THROW_EXCEPTION(
                std::invalid_argument("Unknown parsing name type"));
        }
    }

    /**
     * Return the name of a PIDL as given by its parent folder.
     */
    inline std::wstring display_name_from_pidl(
        const pidl::apidl_t& pidl, SHGDNF type_flags)
    {
        comet::com_ptr<IShellFolder> parent = bind_to_parent<IShellFolder>(pidl);

        STRRET str;
        HRESULT hr = parent->GetDisplayNameOf(
            pidl.last_item().get(), type_flags, &str);
        if (FAILED(hr))
            BOOST_THROW_EXCEPTION(comet::com_error_from_interface(parent, hr));

        return strret_to_string<wchar_t>(str, pidl.last_item());
    }
}

class pidl_shell_item /* final */ : public shell_item
{
public:

    explicit pidl_shell_item(const pidl::apidl_t& pidl) : m_pidl(pidl) {}

    virtual std::wstring friendly_name(
        BOOST_SCOPED_ENUM(friendly_name_type) type=friendly_name_type::default)
    const
    {
        std::wstring name = detail::display_name_from_pidl(
            m_pidl, detail::friendly_name_type_to_shgdnf(type));

        // Editing result should always be the same whether INFOLDER is
        // specified or not
        assert(
            type != friendly_name_type::editable ||
            name == detail::display_name_from_pidl(m_pidl, SHGDN_FOREDITING));

        return name;
    }

    virtual std::wstring parsing_name(
        BOOST_SCOPED_ENUM(parsing_name_type) type=parsing_name_type::default)
    const
    {
        return detail::display_name_from_pidl(
            m_pidl, detail::parsing_name_type_to_shgdnf(type));
    }

private:
    pidl::apidl_t m_pidl;
};

}} // namespace winapi::shell

#endif
