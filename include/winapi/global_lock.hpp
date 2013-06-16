/**
    @file

    Manage Windows global memory locks.

    @if license

    Copyright (C) 2009, 2013  Alexander Lamaison <awl03@doc.ic.ac.uk>

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

#ifndef WINAPI_GLOBAL_LOCK_HPP
#define WINAPI_GLOBAL_LOCK_HPP
#pragma once

#include <winapi/error.hpp> // last_error

#include <boost/exception/errinfo_api_function.hpp> // errinfo_api_function
#include <boost/exception/info.hpp> // errinfo
#include <boost/throw_exception.hpp> // BOOST_THROW_EXCEPTION

#include <windows.h> // GlobalLock/Unlock, HGLOBAL, GetLastError

namespace winapi {

/**
 * Resource-management (RAII) container handling locking on an HGLOBAL.
 *
 * @tparam  Type of object the HGLOBAL points to.  The get() method
 *          returns a pointer to an object of this type.
 */
template<typename T>
class global_lock
{
public:

    /**
     * Lock the given HGLOBAL.
     *
     * The HGLOBAL remains locked for the lifetime of the object.
     *
     * @throws system_error if locking fails.
     */
    explicit global_lock(HGLOBAL hglobal) :
        m_hglobal(hglobal), m_mem(::GlobalLock(hglobal))
    {
        if (!m_mem)
            BOOST_THROW_EXCEPTION(
                boost::enable_error_info(last_error()) <<
                boost::errinfo_api_function("GlobalLock"));
    }

    /**
     * Unlock the HGLOBAL.
     *
     * As the global lock functions maintain a lock-count for each
     * HGLOBAL, our HGLOBAL may remain locked after this object is
     * destroyed if it has been locked elsewhere.  For example, if the
     * global_lock is copied, that will increment the lock-count.
     */
    ~global_lock() throw()
    {
        m_mem = NULL;
        if (m_hglobal)
        {
            BOOL result = ::GlobalUnlock(m_hglobal);
            (void) result;
            assert(result || ::GetLastError() == NO_ERROR); // Too many unlocks
        }
        m_hglobal = NULL;
    }

    /**
     * Copy the lock.
     *
     * Global locking is maintains a lock-count per HGLOBAL that holds the
     * number of outstanding locks.  It increases every time the HGLOBAL
     * is locked and decreases on each call the GlobalUnlock().  When it
     * reaches zero, the global memory is actually unlocked and free to be
     * moved.
     *
     * Instances of the global_lock object can be copied safely as the
     * operation increments the lock count and so destruction of one
     * global_lock instance can't accidentally unlock the memory held by
     * another.
     *
     * @throws system_error if locking fails.
     */
    global_lock(const global_lock& lock) :
        m_hglobal(lock.m_hglobal), m_mem(::GlobalLock(m_hglobal))
    {
        if (!m_mem)
            BOOST_THROW_EXCEPTION(
                boost::enable_error_info(last_error()) <<
                boost::errinfo_api_function("GlobalLock"));
    }

    /**
     * Copy-assign one lock to another.
     *
     * @see the copy constructor for more info on copying locks.
     */
    global_lock& operator=(global_lock lock) throw()
    {
        swap(*this, lock);
        return *this;
    }

    /**
     * Return a pointer to the item held in the HGLOBAL.
     */
    T* get() const
    {
        return static_cast<T*>(m_mem);
    }

private:
    HGLOBAL m_hglobal;
    void* m_mem;

    /**
     * Swap two global_lock instances.
     *
     * This operation cannot fail and offers the strong guarantee.
     */
    template<typename U>
    inline friend void swap(global_lock<U>& lhs, global_lock<U>& rhs) throw()
    {
        std::swap(lhs.m_hglobal, rhs.m_hglobal);
        std::swap(lhs.m_mem, rhs.m_mem);
    }
};

}

#endif
