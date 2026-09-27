#ifndef BINVIEW_VIEW_HPP
#define BINVIEW_VIEW_HPP

#include <cstddef>
#include <type_traits>

#include "types.hpp"

namespace binview
{
    class View
    {
      public:
        View( ) noexcept : m_data_( 0 ), m_size_( 0 ) { }
        View( const void* data, std::size_t size ) noexcept : m_data_( static_cast< const byte* >( data ) ), m_size_( size ) { }

        // @note - wizard: support for arrays
        template< typename T, std::size_t N >
        View( const T ( &data )[ N ] ) noexcept : m_data_( reinterpret_cast< const byte* >( data ) ), m_size_( sizeof( T ) * N )
        {
        }

        const byte* data( ) const noexcept { return m_data_; }
        std::size_t size( ) const noexcept { return m_size_; }
        bool empty( ) const noexcept { return m_size_ == 0; }

        bool contains( std::size_t offset, std::size_t length ) const noexcept
        {
            return offset <= m_size_ && length <= m_size_ - offset;
        }

        View subview( std::size_t offset, std::size_t length ) const noexcept
        {
            if( !contains( offset, length ) )
            {
                return View( );
            }

            return View( m_data_ + offset, length );
        }

        byte operator[]( std::size_t offset ) const noexcept { return m_data_[ offset ]; }

        template< typename T >
        bool read( std::size_t offset, T& out ) const noexcept
        {
            static_assert( std::is_trivially_copyable< T >::value, "T must be trivially copyable" );

            if( !contains( offset, sizeof( T ) ) )
            {
                return false;
            }

            const byte* src = m_data_ + offset;
            for( std::size_t i = 0; i < sizeof( T ); ++i )
            {
                reinterpret_cast< byte* >( &out )[ i ] = src[ i ];
            }

            return true;
        }

      private:
        const byte* m_data_;
        std::size_t m_size_;
    };

    template< typename Container >
    View make_view( const Container& container ) noexcept
    {
        return View( container.data( ), container.size( ) * sizeof( typename Container::value_type ) );
    }
}

#endif