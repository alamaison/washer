/**
    @file

    PROPERTYKEY wrapper.

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

#ifndef WASHER_SHELL_PROPERTY_KEY_HPP
#define WASHER_SHELL_PROPERTY_KEY_HPP
#pragma once

#include <boost/operators.hpp> // totally_ordered

#include <comet/uuid.h> // uuid_t

#include <WTypes.h> // PROPERTYKEY

namespace washer {
namespace shell {

/**
 * C++ version of the PROPERTYKEY (aka SHCOLUMNID) struct.
 *
 * Provides total ordering for use as keys in associative containers.
 */
class property_key : boost::totally_ordered<property_key>
{
public:
    property_key(const PROPERTYKEY& pkey)
        : m_pid(pkey.pid), m_fmtid(pkey.fmtid) {}

    bool operator==(const property_key& other) const
    {
        return m_pid == other.m_pid && m_fmtid == other.m_fmtid;
    }

    bool operator<(const property_key& other) const
    {
        return m_pid < other.m_pid ||
            ((m_pid == other.m_pid) && (m_fmtid < other.m_fmtid));
    }

    /**
     * Convert to raw PROPERTYKEY struct.
     */
    PROPERTYKEY get() const
    {
        PROPERTYKEY pkey;
        pkey.fmtid = m_fmtid;
        pkey.pid = m_pid;
        return pkey;
    }

private:
    DWORD m_pid;
    comet::uuid_t m_fmtid;
};

}} // namespace washer::shell

#endif
