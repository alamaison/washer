/**
    @file

    OLE Window.

    @if license

    Copyright (C) 2013  Alexander Lamaison <awl03@doc.ic.ac.uk>

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

#ifndef WINAPI_COM_OLE_WINDOW_HPP
#define WINAPI_COM_OLE_WINDOW_HPP

#include <winapi/window/window.hpp>

#include <boost/optional.hpp>

#include <comet/ptr.h> // com_ptr

#include <oleidl.h> // IOleWindow, IOleInPlaceFrame
#include <shobjidl.h> // IShellView

template<> struct comet::comtype<IOleWindow>
{
    static const IID& uuid() throw() { return IID_IOleWindow; }
    typedef IUnknown base;
};

template<> struct comet::comtype<IOleInPlaceFrame>
{
    static const IID& uuid() throw() { return IID_IOleInPlaceFrame; }
    typedef IOleInPlaceUIWindow base;
};

template<> struct comet::comtype<IShellView>
{
    static const IID& uuid() throw() { return IID_IShellView; }
    typedef IOleWindow base;
};


namespace winapi {
namespace com {

/**
 * Ask windowed OLE container for its window handle.
 *
 * There are different types of OLE object with which could support this
 * operation.  Try them in turn until one works.
 *
 * @todo  Add more supported OLE object types.
 */
inline boost::optional< winapi::window::window<wchar_t> > window_from_ole_window(
    comet::com_ptr<IUnknown> ole_window)
{
    HWND hwnd = NULL;

    if (comet::com_ptr<IOleWindow> window = com_cast(ole_window))
    {
        window->GetWindow(&hwnd);
    }
    else if (comet::com_ptr<IShellView> view = com_cast(ole_window))
    {
        view->GetWindow(&hwnd);
    }

    if (hwnd)
        return winapi::window::window<wchar_t>(
            winapi::window::window_handle::foster_handle(hwnd));
    else
        return boost::optional< winapi::window::window<wchar_t> >();
}

}} // namespace winapi::com

#endif
