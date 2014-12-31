/**
    @file

    Menu iterator.

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

#ifndef WASHER_GUI_DETAIL_ITEM_ITERATOR_HPP
#define WASHER_GUI_DETAIL_ITEM_ITERATOR_HPP
#pragma once

#include <boost/iterator/iterator_categories.hpp> // random_access_traversal_tag
#include <boost/iterator/iterator_facade.hpp>
#include <boost/throw_exception.hpp> // BOOST_THROW_EXCEPTION

#include <cassert> // assert

namespace washer {
namespace gui {
namespace menu {

namespace detail {

template<typename Menu, typename Item>
class item_iterator :
    public boost::iterator_facade<
        item_iterator<Menu, Item>, Item,
        boost::random_access_traversal_tag, Item> // reference = value_type
        // using boost tag to allow non-& reference
{
    friend class boost::iterator_core_access;
    // Enables conversion constructor to work:
    template<typename,typename> friend class item_iterator;

    // For some reason GCC doesn't see the iterator_facade_ typedef that
    // we're supposed to inherit
    typedef boost::iterator_facade<
        item_iterator<Menu, Item>, Item, boost::random_access_traversal_tag, Item>
    iterator_facade_;

public:

    explicit item_iterator(const Menu& menu) : m_menu(menu), m_pos(0) {}

    /**
     * Copy conversion constructor.
     *
     * Purpose: to allow mutable iterators to be converted to const iterators.
     */
    template<typename OtherValue>
    item_iterator(const item_iterator<Menu, OtherValue>& other)
        : m_menu(other.m_menu), m_pos(other.m_pos) {}

    static item_iterator end(const Menu& menu)
    {
        return item_iterator(menu, end_tag());
    }

private:

    class end_tag {};

    item_iterator(const Menu& menu, const end_tag&)
        : m_menu(menu), m_pos(m_menu.size()) {}

    bool equal(item_iterator const& other) const
    {
        return m_menu == other.m_menu && this->m_pos == other.m_pos;
    }

    typename iterator_facade_::reference dereference() const
    {
        if (m_pos >= m_menu.size())
            BOOST_THROW_EXCEPTION(
                std::logic_error("Dereferencing past the end of the menu"));

        return m_menu[m_pos];
    }

    void increment()
    {
        advance(1);
    }

    void decrement()
    {
        advance(-1);
    }

    void advance(typename iterator_facade_::difference_type step)
    {
        if (step + m_pos < 0) // leaves iterator unchanged
        {
            BOOST_THROW_EXCEPTION(
                std::range_error("Cannot decrement past beginning of the menu"));
        }

        if (step + m_pos > m_menu.size()) // leaves iterator unchanged
        {
            BOOST_THROW_EXCEPTION(
                std::range_error("Cannot increment past end of the menu"));
        }

        m_pos += step; // iterator changed from here
    }

    typename iterator_facade_::difference_type distance_to(
        const item_iterator& other) const
    {
        return other.m_pos - m_pos;
    }

    Menu m_menu;
    size_t m_pos;
};

}}}} // namespace washer::gui::menu::detail

#endif
