/**
    @file

    Calling convention meta-programming.

    @if license

    Copyright (C) 2014  Alexander Lamaison <awl03@doc.ic.ac.uk>

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

#ifndef WASHER_DETAIL_REMOVE_CALLING_CONVENTION_HPP
#define WASHER_DETAIL_REMOVE_CALLING_CONVENTION_HPP

#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_trailing_params.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>

#define WASHER_CALLING_CONVENTION_MAX_ARGS 10

namespace washer {
namespace detail {

template<typename R>
struct remove_calling_convention;

// Turns function _value_ type with specific calling convention into function
// value type with the default calling convention

// TODO: Do this for function pointers/references as well
// TODO: Test with GCC and add __attribute__((stdcall)) if necessary

#define WASHER_REMOVE_CALLING_CONVENTION_TEMPLATE(z, n, convention) \
template<typename R BOOST_PP_ENUM_TRAILING_PARAMS(n, typename P)> \
struct remove_calling_convention<R convention (BOOST_PP_ENUM_PARAMS(n, P))> \
{ \
    typedef R (type)(BOOST_PP_ENUM_PARAMS(n, P)); \
};

#if defined(__i386) || defined(_M_IX86)

BOOST_PP_REPEAT(
    WASHER_CALLING_CONVENTION_MAX_ARGS,
    WASHER_REMOVE_CALLING_CONVENTION_TEMPLATE, __cdecl);

BOOST_PP_REPEAT(
    WASHER_CALLING_CONVENTION_MAX_ARGS,
    WASHER_REMOVE_CALLING_CONVENTION_TEMPLATE, __fastcall);

BOOST_PP_REPEAT(
    WASHER_CALLING_CONVENTION_MAX_ARGS,
    WASHER_REMOVE_CALLING_CONVENTION_TEMPLATE, __stdcall);

#elif defined(__x86_64__) || defined(_M_X64)

// Only one calling convention on x64

BOOST_PP_REPEAT(
    WASHER_CALLING_CONVENTION_MAX_ARGS,
    WASHER_REMOVE_CALLING_CONVENTION_TEMPLATE, );

#endif


}} // namespace washer::detail

#endif
