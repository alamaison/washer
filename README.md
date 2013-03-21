winapi: a lightweight C++ wrapper for the Windows API
========================================================

> **IMPORTANT**: You do **not** have to build this library before
> using it.  It is a header-only library.  Many modern C++ libraries
> are developed this way and it makes them much easier to reuse -
> something we _really_ want to encourage.

What is winapi?
---------------

The Microsoft Windows API, aka Win32, is a large beast written in a
way that suits C programmers but doesn't really fit with the modern
C++ way of working.  winapi aims to wrap parts of it to be easier to
use for any programmer familiar with the C++ standard library (aka the
STL).

### The Win32 API is huge.  Surely you can't have wrapped it all?

Not even close!  So far we have only wrapped a tiny part of the
Windows API, mostly the parts we needed for our own projects, but we
encourage you to use this library where you can and submit patches
back to use when you have had to wrap part of it yourself.  We hope
that, this way, we can begin to build up a substantial library to make
Windows development in C++ much more fun than it sometimes is.

Dependencies
------------

Inevitably, in a world where we are trying to encourage code reuse,
winapi depends on some third-party libraries.

  * [Boost] is fast becoming essential to any modern C++ development
    and winapi uses it heavily.  Any recent version should do although
    building the test suite requires version 1.40 or above as previous
    versions lack Boost.Test.

  * [Comet] is a very clever, portable C++ library for Microsoft COM
    development.  COM is used for parts of the Windows API so we use
    Comet to wrap those bits. We have been using our own
    [private fork] for development so it may be best to use that.

[Boost]:        http://www.boost.org
[Comet]:        http://bitbucket.org/sofusmortensen/comet
[private fork]: http://bitbucket.org/alamaison/swish_comet

Usage
-----

Just download the dependencies, add `winapi/include` to your
compiler's include path and you are ready to go.  The library is
documented inline and you can use [Doxygen] to generate documentation
for it if you want.

[Doxygen]: http://www.doxygen.org/

Licensing
---------

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

If you modify this Program, or any covered work, by linking or
combining it with the OpenSSL project's OpenSSL library (or a modified
version of that library), containing parts covered by the terms of the
OpenSSL or SSLeay licenses, the licensors of this Program grant you
additional permission to convey the resulting work.

### Why have an exception for OpenSSL?

The [OpenSSL] library is incompatible with the GPL license because it
contains an advertising clause.  However lots of useful, open source
software (including our own projects) need to use it and currently the
alternatives aren't quite up to scratch.  As we want these projects to
be able to reuse winapi, we have added this exception to the GPL - a
common technique used by other projects such as [wget].

If [GnuTLS] improves to the point where OpenSSL is no longer
necessary, we may remove this exception.

[OpenSSL]: http://www.openssl.org/
[wget]:    http://www.gnu.org/software/wget/
[GnuTLS]:  http://www.gnu.org/software/gnutls/
