#ifndef BOOST_ENDIAN_DETAIL_ENDIAN_LOAD_HPP_INCLUDED
#define BOOST_ENDIAN_DETAIL_ENDIAN_LOAD_HPP_INCLUDED

// Copyright 2019 Peter Dimov
//
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt

#include <boost/endian/detail/constexpr.hpp>
#include <boost/endian/detail/endian_reverse.hpp>
#include <boost/endian/detail/order.hpp>
#include <boost/endian/detail/integral_by_size.hpp>
#include <boost/endian/detail/is_trivially_copyable.hpp>
#include <boost/endian/detail/static_assert.hpp>
#include <type_traits>
#include <cstddef>
#include <cstring>

#if BOOST_ENDIAN_HAS_CXX20_CONSTEXPR
#include <algorithm>
#include <bit>
#endif

namespace boost
{
namespace endian
{

namespace detail
{

template<class T, std::size_t N1, order O1, std::size_t N2, order O2> struct endian_load_impl
{
};

} // namespace detail

// Requires:
//
//    sizeof(T) must be 1, 2, 4, or 8
//    1 <= N <= sizeof(T)
//    T is TriviallyCopyable
//    if N < sizeof(T), T is integral or enum

template<class T, std::size_t N, order Order>
BOOST_ENDIAN_CONSTEXPR T endian_load( unsigned char const * p ) BOOST_NOEXCEPT
{
    BOOST_ENDIAN_STATIC_ASSERT( sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8 );
    BOOST_ENDIAN_STATIC_ASSERT( N >= 1 && N <= sizeof(T) );

    return detail::endian_load_impl<T, sizeof(T), order::native, N, Order>()( p );
}

namespace detail
{

// same endianness, same size

template<class T, std::size_t N, order O> struct endian_load_impl<T, N, O, N, O>
{
    BOOST_ENDIAN_CXX20_CONSTEXPR T operator()( unsigned char const * p ) const BOOST_NOEXCEPT
    {
        BOOST_ENDIAN_STATIC_ASSERT( is_trivially_copyable<T>::value );
#if BOOST_ENDIAN_HAS_CXX20_CONSTEXPR
        if (std::is_constant_evaluated()) {
            unsigned char v[N];
            std::copy(p, p + N, v);
            return std::bit_cast<T>(v);
        } else
#endif
        {
            T t;
            std::memcpy(&t, p, N);
            return t;
        }
    }
};

// same size, reverse endianness

template<class T, std::size_t N, order O1, order O2> struct endian_load_impl<T, N, O1, N, O2>
{
    BOOST_ENDIAN_CXX20_CONSTEXPR T operator()( unsigned char const * p ) const BOOST_NOEXCEPT
    {
        BOOST_ENDIAN_STATIC_ASSERT( is_trivially_copyable<T>::value );
#if BOOST_ENDIAN_HAS_CXX20_CONSTEXPR
        if (std::is_constant_evaluated()) {
            unsigned char v[N];
            std::reverse_copy(p, p + N, v);
            return std::bit_cast<T>(v);
        } else
#endif
        {
            typename integral_by_size<N>::type tmp;
            std::memcpy( &tmp, p, N );

            endian_reverse_inplace(tmp);

            T t;
            std::memcpy(&t, &tmp, N);
            return t;
        }
    }
};

// expanding load 1 -> 2

template<class T, order Order> struct endian_load_impl<T, 2, Order, 1, order::little>
{
    BOOST_ENDIAN_CXX20_CONSTEXPR T operator()( unsigned char const * p ) const BOOST_NOEXCEPT
    {
        BOOST_ENDIAN_STATIC_ASSERT( std::is_integral<T>::value || std::is_enum<T>::value );

        unsigned char tmp[ 2 ];

        tmp[0] = p[0];
        tmp[1] = std::is_signed<T>::value && ( p[0] & 0x80 )? 0xFF: 0x00;

        return boost::endian::endian_load<T, 2, order::little>( tmp );
    }
};

template<class T, order Order> struct endian_load_impl<T, 2, Order, 1, order::big>
{
    BOOST_ENDIAN_CXX20_CONSTEXPR T operator()( unsigned char const * p ) const BOOST_NOEXCEPT
    {
        BOOST_ENDIAN_STATIC_ASSERT( std::is_integral<T>::value || std::is_enum<T>::value );

        unsigned char tmp[ 2 ];

        tmp[0] = std::is_signed<T>::value && ( p[0] & 0x80 )? 0xFF: 0x00;
        tmp[1] = p[0];

        return boost::endian::endian_load<T, 2, order::big>( tmp );
    }
};

// expanding load 1 -> 4

template<class T, order Order> struct endian_load_impl<T, 4, Order, 1, order::little>
{
    BOOST_ENDIAN_CXX20_CONSTEXPR T operator()( unsigned char const * p ) const BOOST_NOEXCEPT
    {
        BOOST_ENDIAN_STATIC_ASSERT( std::is_integral<T>::value || std::is_enum<T>::value );

        unsigned char tmp[ 4 ];

        unsigned char fill = std::is_signed<T>::value && ( p[0] & 0x80 )? 0xFF: 0x00;

        tmp[0] = p[0];
        tmp[1] = fill;
        tmp[2] = fill;
        tmp[3] = fill;

        return boost::endian::endian_load<T, 4, order::little>( tmp );
    }
};

template<class T, order Order> struct endian_load_impl<T, 4, Order, 1, order::big>
{
    BOOST_ENDIAN_CXX20_CONSTEXPR T operator()( unsigned char const * p ) const BOOST_NOEXCEPT
    {
        BOOST_ENDIAN_STATIC_ASSERT( std::is_integral<T>::value || std::is_enum<T>::value );

        unsigned char tmp[ 4 ];

        unsigned char fill = std::is_signed<T>::value && ( p[0] & 0x80 )? 0xFF: 0x00;

        tmp[0] = fill;
        tmp[1] = fill;
        tmp[2] = fill;
        tmp[3] = p[0];

        return boost::endian::endian_load<T, 4, order::big>( tmp );
    }
};

// expanding load 2 -> 4

template<class T, order Order> struct endian_load_impl<T, 4, Order, 2, order::little>
{
    BOOST_ENDIAN_CXX20_CONSTEXPR T operator()( unsigned char const * p ) const BOOST_NOEXCEPT
    {
        BOOST_ENDIAN_STATIC_ASSERT( std::is_integral<T>::value || std::is_enum<T>::value );

        unsigned char tmp[ 4 ];

        unsigned char fill = std::is_signed<T>::value && ( p[1] & 0x80 )? 0xFF: 0x00;

        tmp[0] = p[0];
        tmp[1] = p[1];
        tmp[2] = fill;
        tmp[3] = fill;

        return boost::endian::endian_load<T, 4, order::little>( tmp );
    }
};

template<class T, order Order> struct endian_load_impl<T, 4, Order, 2, order::big>
{
    BOOST_ENDIAN_CXX20_CONSTEXPR T operator()( unsigned char const * p ) const BOOST_NOEXCEPT
    {
        BOOST_ENDIAN_STATIC_ASSERT( std::is_integral<T>::value || std::is_enum<T>::value );

        unsigned char tmp[ 4 ];

        unsigned char fill = std::is_signed<T>::value && ( p[0] & 0x80 )? 0xFF: 0x00;

        tmp[0] = fill;
        tmp[1] = fill;
        tmp[2] = p[0];
        tmp[3] = p[1];

        return boost::endian::endian_load<T, 4, order::big>( tmp );
    }
};

// expanding load 3 -> 4

template<class T, order Order> struct endian_load_impl<T, 4, Order, 3, order::little>
{
    BOOST_ENDIAN_CXX20_CONSTEXPR T operator()( unsigned char const * p ) const BOOST_NOEXCEPT
    {
        BOOST_ENDIAN_STATIC_ASSERT( std::is_integral<T>::value || std::is_enum<T>::value );

        unsigned char tmp[ 4 ];

        tmp[0] = p[0];
        tmp[1] = p[1];
        tmp[2] = p[2];
        tmp[3] = std::is_signed<T>::value && ( p[2] & 0x80 )? 0xFF: 0x00;

        return boost::endian::endian_load<T, 4, order::little>( tmp );
    }
};

template<class T, order Order> struct endian_load_impl<T, 4, Order, 3, order::big>
{
    BOOST_ENDIAN_CXX20_CONSTEXPR T operator()( unsigned char const * p ) const BOOST_NOEXCEPT
    {
        BOOST_ENDIAN_STATIC_ASSERT( std::is_integral<T>::value || std::is_enum<T>::value );

        unsigned char tmp[ 4 ];

        tmp[0] = std::is_signed<T>::value && ( p[0] & 0x80 )? 0xFF: 0x00;
        tmp[1] = p[0];
        tmp[2] = p[1];
        tmp[3] = p[2];

        return boost::endian::endian_load<T, 4, order::big>( tmp );
    }
};

// expanding load 1 -> 8

template<class T, order Order> struct endian_load_impl<T, 8, Order, 1, order::little>
{
    BOOST_ENDIAN_CXX20_CONSTEXPR T operator()( unsigned char const * p ) const BOOST_NOEXCEPT
    {
        BOOST_ENDIAN_STATIC_ASSERT( std::is_integral<T>::value || std::is_enum<T>::value );

        unsigned char tmp[ 8 ];

        unsigned char fill = std::is_signed<T>::value && ( p[0] & 0x80 )? 0xFF: 0x00;

        tmp[0] = p[0];

        tmp[1] = fill;
        tmp[2] = fill;
        tmp[3] = fill;
        tmp[4] = fill;
        tmp[5] = fill;
        tmp[6] = fill;
        tmp[7] = fill;

        return boost::endian::endian_load<T, 8, order::little>( tmp );
    }
};

template<class T, order Order> struct endian_load_impl<T, 8, Order, 1, order::big>
{
    BOOST_ENDIAN_CXX20_CONSTEXPR T operator()( unsigned char const * p ) const BOOST_NOEXCEPT
    {
        BOOST_ENDIAN_STATIC_ASSERT( std::is_integral<T>::value || std::is_enum<T>::value );

        unsigned char tmp[ 8 ];

        unsigned char fill = std::is_signed<T>::value && ( p[0] & 0x80 )? 0xFF: 0x00;

        tmp[0] = fill;
        tmp[1] = fill;
        tmp[2] = fill;
        tmp[3] = fill;
        tmp[4] = fill;
        tmp[5] = fill;
        tmp[6] = fill;

        tmp[7] = p[0];

        return boost::endian::endian_load<T, 8, order::big>( tmp );
    }
};

// expanding load 2 -> 8

template<class T, order Order> struct endian_load_impl<T, 8, Order, 2, order::little>
{
    BOOST_ENDIAN_CXX20_CONSTEXPR T operator()( unsigned char const * p ) const BOOST_NOEXCEPT
    {
        BOOST_ENDIAN_STATIC_ASSERT( std::is_integral<T>::value || std::is_enum<T>::value );

        unsigned char tmp[ 8 ];

        unsigned char fill = std::is_signed<T>::value && ( p[1] & 0x80 )? 0xFF: 0x00;

        tmp[0] = p[0];
        tmp[1] = p[1];

        tmp[2] = fill;
        tmp[3] = fill;
        tmp[4] = fill;
        tmp[5] = fill;
        tmp[6] = fill;
        tmp[7] = fill;

        return boost::endian::endian_load<T, 8, order::little>( tmp );
    }
};

template<class T, order Order> struct endian_load_impl<T, 8, Order, 2, order::big>
{
    BOOST_ENDIAN_CXX20_CONSTEXPR T operator()( unsigned char const * p ) const BOOST_NOEXCEPT
    {
        BOOST_ENDIAN_STATIC_ASSERT( std::is_integral<T>::value || std::is_enum<T>::value );

        unsigned char tmp[ 8 ];

        unsigned char fill = std::is_signed<T>::value && ( p[0] & 0x80 )? 0xFF: 0x00;

        tmp[0] = fill;
        tmp[1] = fill;
        tmp[2] = fill;
        tmp[3] = fill;
        tmp[4] = fill;
        tmp[5] = fill;

        tmp[6] = p[0];
        tmp[7] = p[1];

        return boost::endian::endian_load<T, 8, order::big>( tmp );
    }
};

// expanding load 3 -> 8

template<class T, order Order> struct endian_load_impl<T, 8, Order, 3, order::little>
{
    BOOST_ENDIAN_CXX20_CONSTEXPR T operator()( unsigned char const * p ) const BOOST_NOEXCEPT
    {
        BOOST_ENDIAN_STATIC_ASSERT( std::is_integral<T>::value || std::is_enum<T>::value );

        unsigned char tmp[ 8 ];

        unsigned char fill = std::is_signed<T>::value && ( p[2] & 0x80 )? 0xFF: 0x00;

        tmp[0] = p[0];
        tmp[1] = p[1];
        tmp[2] = p[2];

        tmp[3] = fill;
        tmp[4] = fill;
        tmp[5] = fill;
        tmp[6] = fill;
        tmp[7] = fill;

        return boost::endian::endian_load<T, 8, order::little>( tmp );
    }
};

template<class T, order Order> struct endian_load_impl<T, 8, Order, 3, order::big>
{
    BOOST_ENDIAN_CXX20_CONSTEXPR T operator()( unsigned char const * p ) const BOOST_NOEXCEPT
    {
        BOOST_ENDIAN_STATIC_ASSERT( std::is_integral<T>::value || std::is_enum<T>::value );

        unsigned char tmp[ 8 ];

        unsigned char fill = std::is_signed<T>::value && ( p[0] & 0x80 )? 0xFF: 0x00;

        tmp[0] = fill;
        tmp[1] = fill;
        tmp[2] = fill;
        tmp[3] = fill;
        tmp[4] = fill;

        tmp[5] = p[0];
        tmp[6] = p[1];
        tmp[7] = p[2];

        return boost::endian::endian_load<T, 8, order::big>( tmp );
    }
};

// expanding load 4 -> 8

template<class T, order Order> struct endian_load_impl<T, 8, Order, 4, order::little>
{
    BOOST_ENDIAN_CXX20_CONSTEXPR T operator()( unsigned char const * p ) const BOOST_NOEXCEPT
    {
        BOOST_ENDIAN_STATIC_ASSERT( std::is_integral<T>::value || std::is_enum<T>::value );

        unsigned char tmp[ 8 ];

        unsigned char fill = std::is_signed<T>::value && ( p[3] & 0x80 )? 0xFF: 0x00;

        tmp[0] = p[0];
        tmp[1] = p[1];
        tmp[2] = p[2];
        tmp[3] = p[3];

        tmp[4] = fill;
        tmp[5] = fill;
        tmp[6] = fill;
        tmp[7] = fill;

        return boost::endian::endian_load<T, 8, order::little>( tmp );
    }
};

template<class T, order Order> struct endian_load_impl<T, 8, Order, 4, order::big>
{
    BOOST_ENDIAN_CXX20_CONSTEXPR T operator()( unsigned char const * p ) const BOOST_NOEXCEPT
    {
        BOOST_ENDIAN_STATIC_ASSERT( std::is_integral<T>::value || std::is_enum<T>::value );

        unsigned char tmp[ 8 ];

        unsigned char fill = std::is_signed<T>::value && ( p[0] & 0x80 )? 0xFF: 0x00;

        tmp[0] = fill;
        tmp[1] = fill;
        tmp[2] = fill;
        tmp[3] = fill;

        tmp[4] = p[0];
        tmp[5] = p[1];
        tmp[6] = p[2];
        tmp[7] = p[3];

        return boost::endian::endian_load<T, 8, order::big>( tmp );
    }
};

// expanding load 5 -> 8

template<class T, order Order> struct endian_load_impl<T, 8, Order, 5, order::little>
{
    BOOST_ENDIAN_CXX20_CONSTEXPR T operator()( unsigned char const * p ) const BOOST_NOEXCEPT
    {
        BOOST_ENDIAN_STATIC_ASSERT( std::is_integral<T>::value || std::is_enum<T>::value );

        unsigned char tmp[ 8 ];

        unsigned char fill = std::is_signed<T>::value && ( p[4] & 0x80 )? 0xFF: 0x00;

        tmp[0] = p[0];
        tmp[1] = p[1];
        tmp[2] = p[2];
        tmp[3] = p[3];
        tmp[4] = p[4];

        tmp[5] = fill;
        tmp[6] = fill;
        tmp[7] = fill;

        return boost::endian::endian_load<T, 8, order::little>( tmp );
    }
};

template<class T, order Order> struct endian_load_impl<T, 8, Order, 5, order::big>
{
    BOOST_ENDIAN_CXX20_CONSTEXPR T operator()( unsigned char const * p ) const BOOST_NOEXCEPT
    {
        BOOST_ENDIAN_STATIC_ASSERT( std::is_integral<T>::value || std::is_enum<T>::value );

        unsigned char tmp[ 8 ];

        unsigned char fill = std::is_signed<T>::value && ( p[0] & 0x80 )? 0xFF: 0x00;

        tmp[0] = fill;
        tmp[1] = fill;
        tmp[2] = fill;

        tmp[3] = p[0];
        tmp[4] = p[1];
        tmp[5] = p[2];
        tmp[6] = p[3];
        tmp[7] = p[4];

        return boost::endian::endian_load<T, 8, order::big>( tmp );
    }
};

// expanding load 6 -> 8

template<class T, order Order> struct endian_load_impl<T, 8, Order, 6, order::little>
{
    BOOST_ENDIAN_CXX20_CONSTEXPR T operator()( unsigned char const * p ) const BOOST_NOEXCEPT
    {
        BOOST_ENDIAN_STATIC_ASSERT( std::is_integral<T>::value || std::is_enum<T>::value );

        unsigned char tmp[ 8 ];

        unsigned char fill = std::is_signed<T>::value && ( p[5] & 0x80 )? 0xFF: 0x00;

        tmp[0] = p[0];
        tmp[1] = p[1];
        tmp[2] = p[2];
        tmp[3] = p[3];
        tmp[4] = p[4];
        tmp[5] = p[5];

        tmp[6] = fill;
        tmp[7] = fill;

        return boost::endian::endian_load<T, 8, order::little>( tmp );
    }
};

template<class T, order Order> struct endian_load_impl<T, 8, Order, 6, order::big>
{
    BOOST_ENDIAN_CXX20_CONSTEXPR T operator()( unsigned char const * p ) const BOOST_NOEXCEPT
    {
        BOOST_ENDIAN_STATIC_ASSERT( std::is_integral<T>::value || std::is_enum<T>::value );

        unsigned char tmp[ 8 ];

        unsigned char fill = std::is_signed<T>::value && ( p[0] & 0x80 )? 0xFF: 0x00;

        tmp[0] = fill;
        tmp[1] = fill;

        tmp[2] = p[0];
        tmp[3] = p[1];
        tmp[4] = p[2];
        tmp[5] = p[3];
        tmp[6] = p[4];
        tmp[7] = p[5];

        return boost::endian::endian_load<T, 8, order::big>( tmp );
    }
};

// expanding load 7 -> 8

template<class T, order Order> struct endian_load_impl<T, 8, Order, 7, order::little>
{
    BOOST_ENDIAN_CXX20_CONSTEXPR T operator()( unsigned char const * p ) const BOOST_NOEXCEPT
    {
        BOOST_ENDIAN_STATIC_ASSERT( std::is_integral<T>::value || std::is_enum<T>::value );

        unsigned char tmp[ 8 ];

        unsigned char fill = std::is_signed<T>::value && ( p[6] & 0x80 )? 0xFF: 0x00;

        tmp[0] = p[0];
        tmp[1] = p[1];
        tmp[2] = p[2];
        tmp[3] = p[3];
        tmp[4] = p[4];
        tmp[5] = p[5];
        tmp[6] = p[6];

        tmp[7] = fill;

        return boost::endian::endian_load<T, 8, order::little>( tmp );
    }
};

template<class T, order Order> struct endian_load_impl<T, 8, Order, 7, order::big>
{
    BOOST_ENDIAN_CXX20_CONSTEXPR T operator()( unsigned char const * p ) const BOOST_NOEXCEPT
    {
        BOOST_ENDIAN_STATIC_ASSERT( std::is_integral<T>::value || std::is_enum<T>::value );

        unsigned char tmp[ 8 ];

        unsigned char fill = std::is_signed<T>::value && ( p[0] & 0x80 )? 0xFF: 0x00;

        tmp[0] = fill;

        tmp[1] = p[0];
        tmp[2] = p[1];
        tmp[3] = p[2];
        tmp[4] = p[3];
        tmp[5] = p[4];
        tmp[6] = p[5];
        tmp[7] = p[6];

        return boost::endian::endian_load<T, 8, order::big>( tmp );
    }
};

} // namespace detail

} // namespace endian
} // namespace boost

#endif  // BOOST_ENDIAN_DETAIL_ENDIAN_LOAD_HPP_INCLUDED
