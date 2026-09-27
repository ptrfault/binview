// SPDX-License-Identifier: MIT
// Copyright (c) 2026 ptrfault

#ifndef BINVIEW_VIEW_HPP
#define BINVIEW_VIEW_HPP

#include <cstddef>
#include <cstring>
#include <type_traits>

#include "types.hpp"

namespace binview
{
    // @note - ptrfault: BasicView is shared by View and MutableView to avoid duplicating the common view logic
    template< typename Byte >
    class BasicView
    {
        static_assert( std::is_same< Byte, byte >::value || std::is_same< Byte, const byte >::value,
            "BasicView must use byte or const byte" );

      public:
        BasicView( ) noexcept : m_data_( nullptr ), m_size_( 0 ) { }

        BasicView( Byte* data, std::size_t size ) noexcept : m_data_( data ), m_size_( size ) { }

        // @note - ptrfault: prevents MutableView from binding to const arrays and accidentally discarding constness
        template< typename T, std::size_t N,
            typename std::enable_if< std::is_const< Byte >::value || !std::is_const< T >::value, int >::type = 0 >
        BasicView( T ( &data )[ N ] ) noexcept : m_data_( reinterpret_cast< Byte* >( data ) ), m_size_( sizeof( T ) * N )
        {
        }

        Byte* data( ) const noexcept { return m_data_; }

        std::size_t size( ) const noexcept { return m_size_; }

        bool empty( ) const noexcept { return m_size_ == 0; }

        bool contains( std::size_t offset, std::size_t length ) const noexcept
        {
            return offset <= m_size_ && length <= m_size_ - offset;
        }

        // @note - ptrfault: returning an empty view on invalid ranges keeps bounds failures non throwing and alloc free
        BasicView subview( std::size_t offset, std::size_t length ) const noexcept
        {
            if( !contains( offset, length ) )
            {
                return BasicView( );
            }

            return BasicView( length == 0 ? m_data_ : m_data_ + offset, length );
        }

        Byte& operator[]( std::size_t offset ) const noexcept { return m_data_[ offset ]; }

        template< typename T >
        bool read( std::size_t offset, T& out ) const noexcept
        {
            static_assert( std::is_trivially_copyable< T >::value, "T must be trivially copyable" );

            if( !contains( offset, sizeof( T ) ) )
            {
                return false;
            }

            std::memcpy( &out, m_data_ + offset, sizeof( T ) );

            return true;
        }

      private:
        Byte* m_data_;
        std::size_t m_size_;
    };

    typedef BasicView< const byte > View;
    typedef BasicView< byte > MutableView;

    template< typename Container >
    View make_view( const Container& container ) noexcept
    {
        return View( container.data( ), container.size( ) * sizeof( typename Container::value_type ) );
    }

    // @note - ptrfault: Mutable views intentionally do not accept const containers
    template< typename Container >
    MutableView make_mutable_view( Container& container ) noexcept
    {
        static_assert( !std::is_const< Container >::value, "make_mutable_view cannot be used with a const container" );

        return MutableView( container.data( ), container.size( ) * sizeof( typename Container::value_type ) );
    }

    template< typename T, std::size_t N >
    View make_view( const T ( &data )[ N ] ) noexcept
    {
        return View( data );
    }

    template< typename T, std::size_t N >
    MutableView make_mutable_view( T ( &data )[ N ] ) noexcept
    {
        return MutableView( data );
    }
}

#endif