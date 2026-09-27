// SPDX-License-Identifier: MIT
// Copyright (c) 2026 ptrfault

#ifndef BINVIEW_WRITER_HPP
#define BINVIEW_WRITER_HPP

#include <cstddef>
#include <cstring>
#include <type_traits>

#include "endian.hpp"
#include "view.hpp"


namespace binview
{
    // @note - ptrfault: writer provides a non-owning sequential view over mutable storage
    class Writer
    {
      public:
        explicit Writer( MutableView data ) noexcept : m_data_( data ), m_offset_( 0 ) { }

        std::size_t position( ) const noexcept { return m_offset_; }

        std::size_t remaining( ) const noexcept { return m_data_.size( ) - m_offset_; }

        bool skip( std::size_t size ) noexcept
        {
            if( size > remaining( ) )
            {
                return false;
            }

            m_offset_ += size;
            return true;
        }

        // @note - ptrfault: little-endian is the default encoding used
        template< typename T >
        bool write( const T& value ) noexcept
        {
            return write_le( value );
        }

        // @note - ptrfault: values are encoded manually to keep the byte order independent of the host endianness
        template< typename T >
        bool write_le( const T& value ) noexcept
        {
            static_assert( std::is_integral< T >::value, "write_le requires an integral type" );

            if( sizeof( T ) > remaining( ) )
            {
                return false;
            }

            typedef typename std::make_unsigned< T >::type unsigned_type;

            unsigned_type data = static_cast< unsigned_type >( value );

            byte* destination = m_data_.data( ) + m_offset_;

            for( std::size_t i = 0; i < sizeof( T ); ++i )
            {
                destination[ i ] = static_cast< byte >( data >> ( i * 8 ) );
            }

            m_offset_ += sizeof( T );

            return true;
        }

        template< typename T >
        bool write_be( const T& value ) noexcept
        {
            static_assert( std::is_integral< T >::value, "write_be requires an integral type" );

            if( sizeof( T ) > remaining( ) )
            {
                return false;
            }

            typedef typename std::make_unsigned< T >::type unsigned_type;

            unsigned_type data = static_cast< unsigned_type >( value );

            byte* destination = m_data_.data( ) + m_offset_;

            for( std::size_t i = 0; i < sizeof( T ); ++i )
            {
                destination[ sizeof( T ) - i - 1 ] = static_cast< byte >( data >> ( i * 8 ) );
            }

            m_offset_ += sizeof( T );

            return true;
        }

        // @note - ptrfault: writes the object's raw memory representation; this does not perform endian conversion
        template< typename T >
        bool write_raw( const T& value ) noexcept
        {
            static_assert( std::is_trivially_copyable< T >::value, "write_raw requires a trivially copyable type" );

            if( sizeof( T ) > remaining( ) )
            {
                return false;
            }

            std::memcpy( m_data_.data( ) + m_offset_, &value, sizeof( T ) );

            m_offset_ += sizeof( T );

            return true;
        }

        // @note - ptrfault: copies raw bytes without interpretation or endian conversion
        bool write_bytes( const byte* data, std::size_t size ) noexcept
        {
            if( size > remaining( ) )
            {
                return false;
            }

            std::memcpy( m_data_.data( ) + m_offset_, data, size );

            m_offset_ += size;

            return true;
        }

      private:
        MutableView m_data_;
        std::size_t m_offset_;
    };
}

#endif