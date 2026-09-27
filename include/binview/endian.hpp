#ifndef BINVIEW_ENDIAN_HPP
#define BINVIEW_ENDIAN_HPP

#include <cstdint>
#include <type_traits>

#include "types.hpp"

namespace binview
{
    template< typename T >
    T read_le( const byte* data ) noexcept
    {
        static_assert( std::is_integral< T >::value, "T must be an integral type" );

        typedef typename std::make_unsigned< T >::type unsigned_type;
        unsigned_type value = 0;

        for( std::size_t i = 0; i < sizeof( T ); ++i )
        {
            value |= static_cast< unsigned_type >( data[ i ] ) << ( i * 8 );
        }

        return static_cast< T >( value );
    }

    template< typename T >
    T read_be( const byte* data ) noexcept
    {
        static_assert( std::is_integral< T >::value, "T must be an integral type" );

        typedef typename std::make_unsigned< T >::type unsigned_type;
        unsigned_type value = 0;

        for( std::size_t i = 0; i < sizeof( T ); ++i )
        {
            value <<= 8;
            value |= static_cast< unsigned_type >( data[ i ] );
        }

        return static_cast< T >( value );
    }
}

#endif