#ifndef BINVIEW_READER_HPP
#define BINVIEW_READER_HPP

#include <cstddef>
#include <cstdint>

#include "endian.hpp"
#include "view.hpp"

namespace binview
{
    // @note - wizard: this gives us a cursor over a view
    class Reader
    {
      public:
        explicit Reader( View data ) noexcept : data_( data ), offset_( 0 ) { }
        std::size_t position( ) const noexcept { return offset_; }
        std::size_t remaining( ) const noexcept { return data_.size( ) - offset_; }

        bool skip( std::size_t size ) noexcept
        {
            if( size > remaining( ) )
            {
                return false;
            }

            offset_ += size;
            return true;
        }

        template< typename T >
        bool read_le( T& out ) noexcept
        {
            if( !data_.contains( offset_, sizeof( T ) ) )
            {
                return false;
            }

            out = binview::read_le< T >( data_.data( ) + offset_ );
            offset_ += sizeof( T );

            return true;
        }

        template< typename T >
        bool read_be( T& out ) noexcept
        {
            if( !data_.contains( offset_, sizeof( T ) ) )
            {
                return false;
            }

            out = binview::read_be< T >( data_.data( ) + offset_ );
            offset_ += sizeof( T );

            return true;
        }

        View remaining_view( ) const noexcept { return data_.subview( offset_, remaining( ) ); }

      private:
        View data_;
        std::size_t offset_;
    };
}

#endif