/**
    @file

    Visitor convenience interface for menu items and menu item buttons.

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

#ifndef WINAPI_GUI_MENU_VISITOR_HPP
#define WINAPI_GUI_MENU_VISITOR_HPP
#pragma once

namespace winapi {
namespace gui {
namespace menu {

/**
 * Convenience interface giving subclasses the static visitor concept necessary
 * to visit menu items and menu item buttons.
 *
 * Purpose: This class only exists to make it easier to create visitors suitable
 * to pass to `item::accept` by defining their result type.  In particular,
 * classes are not required to inherit from this interface in order to be used
 * as menu item visitors.
 */
template<typename ResultType=void>
class menu_visitor
{
public:

    typedef ResultType result_type;

    ~menu_visitor() {}
};

}}} // namespace winapi::gui::menu

#endif
