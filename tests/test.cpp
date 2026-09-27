// SPDX-License-Identifier: MIT
// Copyright (c) 2026 ptrfault

#include <array>
#include <binview/binview.hpp>
#include <cassert>
#include <cstdint>
#include <vector>

static void test_view( )
{
    const std::uint8_t data[] = { 0x10, 0x20, 0x30, 0x40 };

    binview::View v( data );

    assert( v.size( ) == 4 );
    assert( !v.empty( ) );
    assert( v[ 0 ] == 0x10 );
    assert( v[ 3 ] == 0x40 );

    assert( v.contains( 0, 4 ) );
    assert( v.contains( 1, 3 ) );
    assert( !v.contains( 2, 3 ) );

    binview::View sub = v.subview( 1, 2 );

    assert( sub.size( ) == 2 );
    assert( sub[ 0 ] == 0x20 );
    assert( sub[ 1 ] == 0x30 );

    binview::View invalid = v.subview( 3, 2 );

    assert( invalid.empty( ) );
}

static void test_make_view( )
{
    std::vector< std::uint8_t > vector_data = { 1, 2, 3, 4 };

    std::array< std::uint8_t, 4 > array_data = { { 5, 6, 7, 8 } };

    binview::View vector_view = binview::make_view( vector_data );

    binview::View array_view = binview::make_view( array_data );

    assert( vector_view.size( ) == 4 );
    assert( array_view.size( ) == 4 );

    assert( vector_view[ 2 ] == 3 );
    assert( array_view[ 2 ] == 7 );
}

static void test_read( )
{
    const std::uint8_t data[] = { 0x78, 0x56, 0x34, 0x12 };

    binview::View v( data );

    std::uint32_t value = 0;

    assert( v.read( 0, value ) );
    assert( value == 0x12345678 );
}

static void test_endian( )
{
    const std::uint8_t data[] = { 0x12, 0x34, 0x56, 0x78 };

    assert( binview::read_be< std::uint32_t >( data ) == 0x12345678 );

    assert( binview::read_le< std::uint32_t >( data ) == 0x78563412 );
}

static void test_reader( )
{
    const std::uint8_t data[] = { 0x34, 0x12, 0x78, 0x56, 0x34, 0x12, 0xAA };

    binview::Reader reader{ binview::View( data ) };

    std::uint16_t a = 0;
    std::uint32_t b = 0;
    std::uint8_t c = 0;

    assert( reader.position( ) == 0 );

    assert( reader.read_le( a ) );
    assert( a == 0x1234 );
    assert( reader.position( ) == 2 );

    assert( reader.read_le( b ) );
    assert( b == 0x12345678 );
    assert( reader.position( ) == 6 );

    assert( reader.read_le( c ) );
    assert( c == 0xAA );
    assert( reader.position( ) == 7 );

    assert( reader.remaining( ) == 0 );

    assert( !reader.skip( 1 ) );
}

int main( )
{
    test_view( );
    test_make_view( );
    test_read( );
    test_endian( );
    test_reader( );

    return 0;
}