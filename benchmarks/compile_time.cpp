// SPDX-License-Identifier: MIT
// Copyright (c) 2026 ptrfault

#include <array>
#include <binview/binview.hpp>
#include <cstdint>
#include <type_traits>
#include <utility>

namespace
{
    template< std::size_t N >
    struct CompileTimeWorkload
    {
        typedef typename CompileTimeWorkload< N - 1 >::type type;
    };

    template<>
    struct CompileTimeWorkload< 0 >
    {
        typedef std::uint8_t type;
    };

    template< typename T, std::size_t N >
    struct CompileTimeChecks
    {
        static_assert( std::is_same< typename CompileTimeWorkload< N >::type, std::uint8_t >::value, "Unexpected workload type" );
        static_assert( std::is_trivially_copyable< T >::value, "Expected a trivially copyable type" );
        CompileTimeChecks< T, N - 1 > next;
    };

    template< typename T >
    struct CompileTimeChecks< T, 0 >
    {
        static_assert( sizeof( T ) > 0, "Expected a complete type" );
    };

    typedef CompileTimeChecks< std::uint32_t, 512 > Workload;
    typedef std::array< std::uint8_t, 256 > Data;
    typedef decltype( binview::make_view( std::declval< const Data& >( ) ) ) ViewType;

    static_assert( std::is_same< ViewType, binview::View >::value, "Expected make_view to return View" );
    static_assert( sizeof( Workload ) > 0, "Expected compile-time checks to be instantiated" );
}
