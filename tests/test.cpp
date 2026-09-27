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

static void test_mutable_view( )
{
    std::uint8_t data[] = { 1, 2, 3, 4 };

    binview::MutableView v( data );

    assert( v.size( ) == 4 );
    assert( v[ 0 ] == 1 );

    v[ 0 ] = 10;
    v[ 3 ] = 40;

    assert( data[ 0 ] == 10 );
    assert( data[ 3 ] == 40 );

    binview::MutableView sub = v.subview( 1, 2 );

    sub[ 0 ] = 20;
    sub[ 1 ] = 30;

    assert( data[ 1 ] == 20 );
    assert( data[ 2 ] == 30 );
}

static void test_make_view( )
{
    std::vector< std::uint8_t > vector_data = { 1, 2, 3, 4 };

    std::array< std::uint8_t, 4 > array_data = { { 5, 6, 7, 8 } };

    const std::uint8_t raw_data[] = { 9, 10, 11, 12 };

    binview::View vector_view = binview::make_view( vector_data );

    binview::View array_view = binview::make_view( array_data );

    binview::View raw_view = binview::make_view( raw_data );

    assert( vector_view.size( ) == 4 );
    assert( array_view.size( ) == 4 );
    assert( raw_view.size( ) == 4 );

    assert( vector_view[ 2 ] == 3 );
    assert( array_view[ 2 ] == 7 );
    assert( raw_view[ 2 ] == 11 );
}

static void test_make_mutable_view( )
{
    std::vector< std::uint8_t > vector_data = { 1, 2, 3, 4 };

    std::array< std::uint8_t, 4 > array_data = { { 5, 6, 7, 8 } };

    std::uint8_t raw_data[] = { 9, 10, 11, 12 };

    binview::MutableView vector_view = binview::make_mutable_view( vector_data );

    binview::MutableView array_view = binview::make_mutable_view( array_data );

    binview::MutableView raw_view = binview::make_mutable_view( raw_data );

    vector_view[ 0 ] = 10;
    array_view[ 0 ] = 50;
    raw_view[ 0 ] = 90;

    assert( vector_data[ 0 ] == 10 );
    assert( array_data[ 0 ] == 50 );
    assert( raw_data[ 0 ] == 90 );
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

static void test_writer( )
{
    std::uint8_t data[ 7 ] = { };

    binview::MutableView view( data );

    binview::Writer writer{ view };

    assert( writer.position( ) == 0 );

    assert( writer.write( std::uint16_t( 0x1234 ) ) );
    assert( writer.position( ) == 2 );

    assert( data[ 0 ] == 0x34 );
    assert( data[ 1 ] == 0x12 );

    assert( writer.write( std::uint32_t( 0x12345678 ) ) );
    assert( writer.position( ) == 6 );

    assert( data[ 2 ] == 0x78 );
    assert( data[ 3 ] == 0x56 );
    assert( data[ 4 ] == 0x34 );
    assert( data[ 5 ] == 0x12 );

    assert( writer.write_be( std::uint8_t( 0xAA ) ) );
    assert( writer.position( ) == 7 );

    assert( data[ 6 ] == 0xAA );

    assert( writer.remaining( ) == 0 );
    assert( !writer.write( std::uint8_t( 0xFF ) ) );
}

static void test_writer_big_endian( )
{
    std::uint8_t data[ 6 ] = { };

    binview::Writer writer{ binview::MutableView( data ) };

    assert( writer.write_be( std::uint16_t( 0x1234 ) ) );

    assert( writer.write_be( std::uint32_t( 0x12345678 ) ) );

    assert( data[ 0 ] == 0x12 );
    assert( data[ 1 ] == 0x34 );

    assert( data[ 2 ] == 0x12 );
    assert( data[ 3 ] == 0x34 );
    assert( data[ 4 ] == 0x56 );
    assert( data[ 5 ] == 0x78 );
}

static void test_writer_raw( )
{
    std::uint8_t data[ 4 ] = { };

    binview::Writer writer{ binview::MutableView( data ) };

    const std::uint32_t value = 0x12345678;

    assert( writer.write_raw( value ) );

    std::uint32_t result = 0;

    std::memcpy( &result, data, sizeof( result ) );

    assert( result == value );
}

static void test_writer_bytes( )
{
    std::uint8_t data[ 4 ] = { };

    const std::uint8_t source[] = { 0x10, 0x20, 0x30, 0x40 };

    binview::Writer writer{ binview::MutableView( data ) };

    assert( writer.write_bytes( source, sizeof( source ) ) );

    assert( data[ 0 ] == 0x10 );
    assert( data[ 1 ] == 0x20 );
    assert( data[ 2 ] == 0x30 );
    assert( data[ 3 ] == 0x40 );
}

int main( )
{
    test_view( );
    test_mutable_view( );
    test_make_view( );
    test_make_mutable_view( );
    test_read( );
    test_endian( );
    test_reader( );
    test_writer( );
    test_writer_big_endian( );
    test_writer_raw( );
    test_writer_bytes( );

    return 0;
}