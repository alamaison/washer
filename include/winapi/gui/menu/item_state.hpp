/**
    @file

    Menu item appearance and behaviour properties.

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

#ifndef WINAPI_GUI_MENU_ITEM_STATE_HPP
#define WINAPI_GUI_MENU_ITEM_STATE_HPP
#pragma once

#include <boost/detail/scoped_enum_emulation.hpp> // BOOST_SCOPED_ENUM
#include <boost/throw_exception.hpp> // BOOST_THROW_EXCEPTION

#include <stdexcept> // logic_error

#include <Windows.h> // MENUITEMINFO

namespace winapi {
namespace gui {
namespace menu {

BOOST_SCOPED_ENUM_START(selectability)
{
    enabled,
    disabled,
    default
};
BOOST_SCOPED_ENUM_END;

BOOST_SCOPED_ENUM_START(checkedness)
{
    checked,
    unchecked,
    default
};
BOOST_SCOPED_ENUM_END;

namespace detail {

    // If C++ had Java-style enums, these would be methods of the enums above

    inline void adjust_selectability(
        BOOST_SCOPED_ENUM(selectability) state, MENUITEMINFOW& info)
    {
        switch (state)
        {
        case selectability::disabled:
            info.fState |= MFS_DISABLED;
            break;

        case selectability::default:
        case selectability::enabled:
            // MFS_ENABLED is zero so we have to negate disabled instead
            info.fState &= (~MFS_DISABLED);
            break;

        default:
            assert(!"Passed a selectability state that doesn't exist in enum");
            BOOST_THROW_EXCEPTION(std::logic_error("Unknown selectability"));
        }
    }

    inline void adjust_checkedness(
        BOOST_SCOPED_ENUM(checkedness) state, MENUITEMINFOW& info)
    {
        switch (state)
        {
        case checkedness::checked:
            info.fState |= MFS_CHECKED;
            break;

        case checkedness::default:
        case checkedness::unchecked:
            // MFS_UNCHECKED is zero so we have to negate checked instead
            info.fState &= (~MFS_CHECKED);
            break;

        default:
            assert(!"Passed a check mark state that doesn't exist in enum");
            BOOST_THROW_EXCEPTION(std::logic_error("Unknown checkedness"));
        }
    }

}

}}} // namespace winapi::gui::menu

#endif
