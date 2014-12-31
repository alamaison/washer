/**
    @file

    Standard IObjectWithSite implementation.

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

#ifndef WASHER_OBJECT_WITH_SITE_HPP
#define WASHER_OBJECT_WITH_SITE_HPP
#pragma once

#include <comet/error.h> // com_error
#include <comet/interface.h> // comtype

#include <washer/com/catch.hpp> // WASHER_COM_CATCH_AUTO_INTERFACE

#include <boost/throw_exception.hpp> // BOOST_THROW_EXCEPTION

#include <OCIdl.h> // IObjectWithSite

template<> struct ::comet::comtype<::IObjectWithSite>
{
    static const ::IID& uuid() throw() { return ::IID_IObjectWithSite; }
    typedef ::IUnknown base;
};

namespace washer {

/**
 * Mixin providing a standard implementation of IObjectWithSite.
 */
class object_with_site : public IObjectWithSite
{
public:

    typedef IObjectWithSite interface_is;

    virtual IFACEMETHODIMP SetSite(IUnknown* pUnkSite)
    {
        try
        {
            m_ole_site = pUnkSite;
            on_set_site(m_ole_site);
        }
        WASHER_COM_CATCH_AUTO_INTERFACE();

        return S_OK;
    }

    virtual IFACEMETHODIMP GetSite(REFIID riid, void** ppvSite)
    {
        try
        {
            if (!ppvSite)
                BOOST_THROW_EXCEPTION(comet::com_error(E_POINTER));
            *ppvSite = NULL;

            HRESULT hr = m_ole_site.get()->QueryInterface(riid, ppvSite);
            if (FAILED(hr))
                BOOST_THROW_EXCEPTION(
                    comet::com_error_from_interface(m_ole_site, hr));
        }
        WASHER_COM_CATCH_AUTO_INTERFACE();

        return S_OK;
    }

protected:

    comet::com_ptr<IUnknown> ole_site() { return m_ole_site; }

private:

    /**
     * Custom site action.
     *
     * Override this method to perform a custom action when the site is set.
     */
    virtual void on_set_site(comet::com_ptr<IUnknown> ole_site) {}

    comet::com_ptr<IUnknown> m_ole_site;
};

} // namespace washer

#endif
