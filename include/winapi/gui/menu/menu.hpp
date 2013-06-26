/**
    @file

    Windows HMENU menu wrapper for menus and menu bars.

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

#ifndef WINAPI_GUI_MENU_MENU_HPP
#define WINAPI_GUI_MENU_MENU_HPP
#pragma once

#include <winapi/gui/menu/basic_menu.hpp>
#include <winapi/gui/menu/detail/menu.hpp>
                                 // menu_handle_creator, menu_bar_handle_creator
#include <winapi/gui/menu/item/item_description.hpp>
#include <winapi/gui/menu/item/selectable_item_description.hpp>

#pragma warning(push)
#pragma warning(disable : 4100) // unreferenced formal parameter
#pragma warning(disable : 4510)
#pragma warning(disable : 4610)
#include <boost/concept/assert.hpp> // BOOST_CONCEPT_ASSERT
#include <boost/concept_check.hpp> // Collection
#include <boost/iterator/iterator_concepts.hpp> // ReadableIterator
#pragma warning(pop)

namespace winapi {
namespace gui {
namespace menu {

/**
 * Menu that can be used as a context menu, as a sub-menu of a menu bar or as
 * a sub-menu of another menu.
 */
typedef basic_menu<item_description, detail::menu_handle_creator> menu;

/**
 * Main application window menu bar.
 */
typedef basic_menu<
    selectable_item_description, detail::menu_bar_handle_creator> menu_bar;

}}} // namespace winapi::gui::menu

#if (BOOST_VERSION >= 104900)
// Not sure when Collection was added but 1.49 will do
BOOST_CONCEPT_ASSERT((boost::Collection<winapi::gui::menu::menu>));
BOOST_CONCEPT_ASSERT((boost::Collection<winapi::gui::menu::menu_bar>));
#endif

BOOST_CONCEPT_ASSERT((
    boost_concepts::ReadableIterator<winapi::gui::menu::menu::iterator>));
BOOST_CONCEPT_ASSERT((
    boost_concepts::ReadableIterator<winapi::gui::menu::menu_bar::iterator>));

#endif
