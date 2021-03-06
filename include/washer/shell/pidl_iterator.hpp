/**
    @file

    Iterator for walking ID lists.

    @if license

    Copyright (C) 2010, 2011, 2012  Alexander Lamaison <awl03@doc.ic.ac.uk>

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

#ifndef WASHER_SHELL_PIDL_ITERATOR_HPP
#define WASHER_SHELL_PIDL_ITERATOR_HPP
#pragma once

#include <washer/shell/pidl.hpp> // next, empty, pidl_t

#include <boost/iterator/iterator_adaptor.hpp> // iterator_adaptor

#include <cassert> // assert
#include <stdexcept> // range_error

#ifndef STRICT_TYPED_ITEMIDS
#error Currently, washer requires strict PIDL types: define STRICT_TYPED_ITEMIDS
#endif
#include <Shlobj.h> // ILCloneFirst
#include <ShTypes.h> // Raw PIDL types

namespace washer {
namespace shell {
namespace pidl {

namespace detail {

inline PCUIDLIST_RELATIVE canonicalise_pidl(PCUIDLIST_RELATIVE pidl)
{
    if (pidl != NULL && pidl->mkid.cb != 0U)
        return pidl;
    else
        return NULL;
}

}

/**
 * Iterator that points to each item of an ITEMIDLIST in turn.
 *
 * The iteration is complete when the iterator is equal to a default-constructed
 * one.  I.e when @code iterator == raw_pidl_iterator() @endcode
 *
 * The iterator becomes invalid if the IDLIST that it is created from is
 * deallocated.  When dereferenced, the iterator returns a pointer into the
 * original IDLIST, i.e. a PIDL, rather than a copy of the item at that point
 * in the list.

 * In other words, each returned PIDL still references all the other items in
 * the list after it.  For example, the second iteration on the PIDL for
 * @c C:\\Windows\\System32 will return a pointer to the IDLIST
 * @c Windows\\System32 rather than than @c Windows.
 */
class raw_pidl_iterator :
    public boost::iterator_adaptor<
        raw_pidl_iterator, PCUIDLIST_RELATIVE, const PCUIDLIST_RELATIVE,
        boost::forward_traversal_tag>
{
public:
    explicit raw_pidl_iterator(PCUIDLIST_RELATIVE pidl)
        :
    raw_pidl_iterator::iterator_adaptor_(detail::canonicalise_pidl(pidl)) {}

    raw_pidl_iterator() : raw_pidl_iterator::iterator_adaptor_(NULL) {}

private:
    friend boost::iterator_core_access;

    reference dereference() const
    {
        if (this->base_reference() == NULL)
            // If you get an error here, you are probably trying to detect the
            // end of the iteration by checking for NULL.  Don't.  While this
            // will work, it is an internal detail of the iterator
            // implementation and could change.
            BOOST_THROW_EXCEPTION(
                std::logic_error(
                    "Dereferencing past the end of the ITEMIDLIST"));

        return this->base_reference();
    }

    void increment()
    {
        // We have to have a consistent definition of when we are at the end of
        // a PIDL.  However, a PIDL that just points to zeroed-terminator
        // and a PIDL that is NULL (points to nothing) represent the same thing.
        // Therefore, any assignment to the iterator's internal PIDL is
        // canonicalised to NULL when we're at the end.
        // This also causes iteration to only occur over non-terminator items
        // of the PIDL.  In other words, an iterator over a PIDL containing
        // just the terminator will have no length.
        assert(!raw_pidl::empty(this->base()) || this->base() == NULL);

        if (this->base_reference() == NULL)
            BOOST_THROW_EXCEPTION(
                std::range_error("Cannot increment past end of ITEMIDLIST"));

        this->base_reference() =
            detail::canonicalise_pidl(raw_pidl::next(this->base()));
    }
};

/**
* Iterates over the items in an Item ID List (aka a PIDL).
*
* The iteration is complete when the iterator is equal to a default-constructed
* one.  I.e when @code iterator == pidl_iterator() @endcode
*
* The iterator becomes invalid if the IDLIST that it is created from is
* deallocated.
 *
 * When dereferenced, returns the single item at that point in the original
 * PIDL.
 *
 * Differences between this class as raw_pidl_iterator:
 * - Returns managed (pidl_t-based) PIDLs.
 * - Returns a child PIDL to just the current item rather than a relative PIDL
 *   to the remained of the list.
 */
class pidl_iterator :
    public boost::iterator_adaptor<pidl_iterator, raw_pidl_iterator, cpidl_t>
{
public:
    template<typename T, typename Alloc>
    explicit pidl_iterator(const basic_pidl<T, Alloc>& pidl)
        : pidl_iterator::iterator_adaptor_(raw_pidl_iterator(pidl.get())) {}

    explicit pidl_iterator(PCUIDLIST_RELATIVE pidl)
        : pidl_iterator::iterator_adaptor_(raw_pidl_iterator(pidl)) {}

    pidl_iterator() : pidl_iterator::iterator_adaptor_() {}

private:
    friend boost::iterator_core_access;

    reference dereference() const
    {
        m_item.attach(::ILCloneFirst(*this->base_reference()));
        return m_item;
    }

    mutable cpidl_t m_item;
};

inline bool operator==(const raw_pidl_iterator& lhs, const pidl_iterator& rhs)
{
    return lhs == rhs.base();
}

inline bool operator==(const pidl_iterator& lhs, const raw_pidl_iterator& rhs)
{
    return rhs == lhs;
}

inline bool operator!=(const raw_pidl_iterator& lhs, const pidl_iterator& rhs)
{
    return !(lhs == rhs);
}

inline bool operator!=(const pidl_iterator& lhs, const raw_pidl_iterator& rhs)
{
    return !(rhs == lhs);
}

}}} // namespace washer::shell::pidl

#endif
