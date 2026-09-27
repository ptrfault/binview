// SPDX-License-Identifier: MIT
// Copyright (c) 2026 ptrfault

#include <array>
#include <binview/binview.hpp>
#include <cstdint>
#include <cstdlib>
#include <nanobench.h>

int main( )
{
    std::array< std::uint8_t, 256 > data = { };

    std::srand( 1 );
    for( std::size_t i = 0; i < data.size( ); ++i )
    {
        data[ i ] = static_cast< std::uint8_t >( std::rand( ) );
    }
    ankerl::nanobench::doNotOptimizeAway( data.data( ) );

    const std::size_t reads_per_batch = data.size( ) / sizeof( std::uint32_t );
    const binview::View view( data.data( ), data.size( ) );

    ankerl::nanobench::Bench bench;
    bench.title( "binview runtime benchmarks" ).unit( "read" ).batch( reads_per_batch ).warmup( 1000 ).epochs( 10 );

    bench.run( "read_le<uint32_t>",
        [ & ]( )
        {
            std::uint32_t sum = 0;

            for( std::size_t i = 0; i < reads_per_batch; ++i )
            {
                sum += binview::read_le< std::uint32_t >( data.data( ) + i * sizeof( std::uint32_t ) );
            }

            ankerl::nanobench::doNotOptimizeAway( sum );
        } );

    bench.run( "read_le<uint32_t> byte loop",
        [ & ]( )
        {
            std::uint32_t sum = 0;

            for( std::size_t i = 0; i < reads_per_batch; ++i )
            {
                const std::size_t offset = i * sizeof( std::uint32_t );
                std::uint32_t value = 0;

                for( std::size_t byte_index = 0; byte_index < sizeof( value ); ++byte_index )
                {
                    value |= static_cast< std::uint32_t >( data[ offset + byte_index ] ) << ( byte_index * 8 );
                }

                sum += value;
            }

            ankerl::nanobench::doNotOptimizeAway( sum );
        } );

    bench.run( "View::read<uint32_t>",
        [ & ]( )
        {
            std::uint32_t sum = 0;

            for( std::size_t i = 0; i < reads_per_batch; ++i )
            {
                std::uint32_t value = 0;
                view.read( i * sizeof( std::uint32_t ), value );
                sum += value;
            }

            ankerl::nanobench::doNotOptimizeAway( sum );
        } );

    ankerl::nanobench::Bench reader_bench;
    reader_bench.title( "binview sequential Reader benchmark" ).unit( "read" ).batch( reads_per_batch ).warmup( 1000 ).epochs( 10 );

    reader_bench.run( "Reader::read_le<uint32_t>",
        [ & ]( )
        {
            binview::Reader reader{ view };
            std::uint32_t sum = 0;

            for( std::size_t i = 0; i < reads_per_batch; ++i )
            {
                std::uint32_t value = 0;
                reader.read_le( value );
                sum += value;
            }

            ankerl::nanobench::doNotOptimizeAway( sum );
        } );
}
