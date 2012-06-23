/**
    @file

    Code to convert MENUITEMINFO into button instances.

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

#ifndef WINAPI_GUI_MENU_DETAIL_CONVERT_BUTTON_HPP
#define WINAPI_GUI_MENU_DETAIL_CONVERT_BUTTON_HPP
#pragma once

#include <winapi/gui/menu/detail/item_position.hpp>

#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>

#include <Windows.h> // MENUITEMINFO, HBITMAP

namespace winapi {
namespace gui {
namespace menu {
namespace detail {

template<UINT Mft>
struct item_type_converter
{
    static boost::shared_ptr<menu_button_nature> convert(
        const item_position& /*item*/);
};

template<>
struct item_type_converter<MFT_BITMAP> 
{
    static boost::shared_ptr<menu_button_nature> convert(const item_position& item)
    {
        // Old-style whole-button bitmap specified through dwTypeData and
        // MFT_BITMAP (MF_BITMAP) requires MIIM_TYPE rather than MIIM_FTYPE
        MENUITEMINFOW info = item.get_menuiteminfo(MIIM_TYPE);

        HBITMAP bitmap = reinterpret_cast<HBITMAP>(info.dwTypeData);
        return boost::make_shared<bitmap_menu_button>(bitmap);
    }
};

template<>
struct item_type_converter<MFT_OWNERDRAW>
{
    /**
     * @todo  Include custom data in dwTypeData and dwItemData.
     */
    static boost::shared_ptr<menu_button_nature> convert(const item_position& item)
    {
        return boost::make_shared<owner_drawn_menu_button>();
    }
};

template<>
struct item_type_converter<MFT_STRING>
{
    static boost::shared_ptr<menu_button_nature> convert(const item_position& item)
    {
        return boost::make_shared<string_menu_button>(item.get_caption());
    }
};

inline boost::shared_ptr<menu_button_nature> convert_menu_button(
    const item_position& item)
{
    UINT button_type_flag = item.get_menuiteminfo(MIIM_FTYPE).fType;

    if (button_type_flag & MFT_BITMAP)
    {
        return item_type_converter<MFT_BITMAP>::convert(item);
    }
    else if (button_type_flag & MFT_OWNERDRAW)
    {
        // MSDN doesn't say that MFT_OWNERDRAWN is part of the
        // mutually exclusive set of flags but I don't see how it couldn't
        // be.  These flags specify the meaning of dwTypeData and cch which
        // MFT_OWNERDRAWN gives a user-defined meaning.
        return item_type_converter<MFT_OWNERDRAW>::convert(item);
    }
    else
    {
        // MFT_STRING which is zero so can only be detected by elimination
        return item_type_converter<MFT_STRING>::convert(item);
    }
}

}}}} // namespace winapi::gui::menu::detail

#endif