/*-----------------------------------------------------------------------------------------------
The MIT License (MIT)

Copyright (c) 2015-2020 OSRE ( Open Source Render Engine ) by Kim Kulling

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
-----------------------------------------------------------------------------------------------*/
#include <osre/IO/Stream.h>

namespace OSRE {
namespace IO {

Stream::Stream() noexcept
: m_Uri()
, m_AccessMode() {
	// empty
}

Stream::Stream( const Uri &uri, AccessMode mode ) 
: m_Uri( uri )
, m_AccessMode( mode ) {
	// empty
}

Stream::~Stream() {
	// empty
}

void Stream::setUri( const Uri &rURI ) {
	m_Uri = rURI;
}

const Uri &Stream::getUri() const {
	return m_Uri;
}

bool Stream::canRead() const {
	return false;
}

bool Stream::canWrite() const {
	return false;
}

bool Stream::canSeek() const {
	return false;
}

bool Stream::canBeMapped() const {
	return false;
}

void Stream::setAccessMode( AccessMode accessMode ) {
	m_AccessMode = accessMode;
}

Stream::AccessMode Stream::getAccessMode() const {
	return m_AccessMode;
}

ui32 Stream::getSize() const {
	return 0;
}

ui32 Stream::read( void *, ui32 ) {
	return 0;
}

ui32 Stream::write( const void *, ui32 ) {
	return 0;
}

ui32 Stream::readI32( i64 & ) {
    return 0;
}

ui32 Stream::writeI32( i64 ) {
    return 0;
}

ui32 Stream::readUI32( ui32 & ) {
    return 0;
}

ui32 Stream::writeUI32( ui32  ) {
    return 0;
}

ui32 Stream::readF32( f32 & ) {
    return 0;
}

ui32 Stream::writeF32( f32  ) {
    return 0;
}

ui32 Stream::readD32( d32 & ) {
    return 0;
}

ui32 Stream::writeD32( d32  ) {
    return 0;
}

Stream::Position Stream::seek( Offset , Origin  ) {
	return 0;
}

Stream::Position Stream::tell() {
	return 0;
}

bool Stream::isOpen() const {
	return false;
}

bool Stream::open() {
	return false;
}

bool Stream::close() {
	return false;
}

} // Namespace IO
} // Namespace OSRE
