/**
    @file

    Helper class to use wrapped PIDLs as an array.

    @if license

    Copyright (C) 2009, 2011  Alexander Lamaison <awl03@doc.ic.ac.uk>

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

#ifndef WINAPI_SHELL_PIDL_ARRAY_HPP
#define WINAPI_SHELL_PIDL_ARRAY_HPP
#pragma once

#include <algorithm>  // swap, transform
#include <vector>

namespace winapi {
namespace shell {
namespace pidl {

namespace detail {

    template<typename T>
    typename T::const_pointer raw_pidl_from_wrapper(const T& wrapped_pidl)
    {
        return wrapped_pidl.get();
    }

    template<typename It>
    std::vector<typename It::value_type::const_pointer>
    raw_pidls_from_wrappers(It begin, It end)
    {
        std::vector<typename It::value_type::const_pointer> array;
        transform(
            begin, end, back_inserter(array),
            raw_pidl_from_wrapper<It::value_type>);
        return array;
    }
}

/**
 * Helper class to let a collection of wrapped PIDLs be used where an array
 * of raw pidls is expected.
 *
 * The values are only valid during the lifetime of the original collection
 * of wrapped PIDLs with which the array was constructed.  Once the original
 * collection is destroyed the values in the array point to deallocated
 * memory.
 */
template<typename T>
class pidl_array
{
public:

    typedef typename T::const_pointer value_type;

    template<typename It>
    pidl_array(It begin, It end) :
        m_array(detail::raw_pidls_from_wrappers(begin, end)) {}

    ~pidl_array() throw() {}

    pidl_array(const pidl_array& a) :
        m_array(a.m_array) {}

    pidl_array& operator=(const pidl_array& a)
    {
        pidl_array copy(a);
        swap(copy);
        return *this;
    }

    /**
     * Return a pointer to the array.
     */
    const value_type* as_array() const
    {
        return (size()) ? &m_array[0] : NULL;
    }

    /**
     * Number of PIDLs in the array.
     */
    size_t size() const
    {
        return m_array.size();
    }

    /**
     * No-fail swap.
     */
    void swap(pidl_array& a) throw()
    {
        std::swap(a.m_array, m_array);
    }

private:
    std::vector<value_type> m_array;
};

}}} // namespace winapi::shell::pidl

#endif
