/**
    @file

    Menu separator builder.

    @if license

    Copyright (C) 2012, 2013  Alexander Lamaison <awl03@doc.ic.ac.uk>

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

#ifndef WINAPI_GUI_MENU_ITEM_SEPARATOR_ITEM_DESCRIPTION_HPP
#define WINAPI_GUI_MENU_ITEM_SEPARATOR_ITEM_DESCRIPTION_HPP
#pragma once

#include <winapi/gui/menu/item/item_description.hpp>

#include <Windows.h> // MENUITEMINFO

namespace winapi {
namespace gui {
namespace menu {

/**
 * Line dividing menu into logical sections.
 */
class separator_item_description : public item_description
{
public:
    separator_item_description() : m_id(0) {}

    virtual separator_item_description& id(UINT new_id)
    {
        m_id = new_id;
        return *this;
    }

private:

    virtual void do_insertion_and_relinquish_resources(
        const boost::function<void(const MENUITEMINFOW&)>& inserter) const
    {
        inserter(as_menuiteminfo());
        // No resources to release
    }

    virtual separator_item_description* do_clone() const
    {
        return new separator_item_description(*this);
    }

    MENUITEMINFOW as_menuiteminfo() const
    {
        MENUITEMINFOW info = MENUITEMINFOW();
        info.cbSize = sizeof(MENUITEMINFOW);

        // Using MIIM_TYPE instead of MIIM_FTYPE for backward compatibility
        // with Win 98.
        //info.fMask = MIIM_FTYPE | MIIM_ID;
        info.fMask = MIIM_TYPE | MIIM_ID;
        info.fType = MFT_SEPARATOR;
        info.wID = m_id;

        return info;
    }

    UINT m_id;
};

}}} // namespace winapi::gui::menu

#endif
