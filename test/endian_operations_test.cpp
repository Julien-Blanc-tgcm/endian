//  endian_operations_test.cpp  --------------------------------------------------------//

//  Copyright Beman Dawes 2008

//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See library home page at http://www.boost.org/libs/endian

//--------------------------------------------------------------------------------------//

//  This test probes operator overloading, including interaction between
//  operand types.

//  See endian_test for tests of endianess correctness, size, and value.

//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//
//
//
//  TODO: transition to BOOST_TEST; more testing can only help
//
//
//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//

#define BOOST_ENDIAN_LOG

#include <boost/endian/detail/disable_warnings.hpp>

#ifdef _MSC_VER
# pragma warning( disable : 4242 )  // conversion ..., possible loss of data
# pragma warning( disable : 4244 )  // conversion ..., possible loss of data
# pragma warning( disable : 4018 )  // signed/unsigned mismatch
# pragma warning( disable : 4365 )  // signed/unsigned mismatch
# pragma warning( disable : 4389 )  // signed/unsigned mismatch
#endif

#define BOOST_ENDIAN_LOG

#include <boost/endian/arithmetic.hpp>
#include <boost/type_traits/is_signed.hpp>
#include <boost/core/lightweight_test.hpp>
#include <boost/detail/lightweight_main.hpp>
#include <cassert>
#include <iostream>
#include <sstream>

namespace be = boost::endian;

template <class T>
struct value_type
{
  typedef typename T::value_type type;
};

template<> struct value_type<char>  { typedef char type; };
template<> struct value_type<unsigned char>  { typedef unsigned char type; };
template<> struct value_type<signed char>  { typedef signed char type; };
template<> struct value_type<short>  { typedef short type; };
template<> struct value_type<unsigned short>  { typedef unsigned short type; };
template<> struct value_type<int>  { typedef int type; };
template<> struct value_type<unsigned int>  { typedef unsigned int type; };
template<> struct value_type<long>  { typedef long type; };
template<> struct value_type<unsigned long>  { typedef unsigned long type; };
template<> struct value_type<long long>  { typedef long long type; };
template<> struct value_type<unsigned long long>  { typedef unsigned long long type; };
template<> struct value_type<float>  { typedef float type; };
template<> struct value_type<double>  { typedef double type; };
template<> struct value_type<long double>  { typedef long double type; };

template <class T1,  class T2>
struct default_construct
{
  static void test()
  {
    T1 o1;
    o1 = 1;         // quiet warnings
    if (o1) return; // quiet warnings
  }
};

template <class T1,  class T2>
struct construct
{
  static void test()
  {
    T2 o2(1);
    T1 o1(o2);   // TODO: does this make sense for char types?
    ++o1;  // quiet gcc unused variable warning
  }
};

template <class T1,  class T2>
struct initialize
{
  static void test()
  {
    T1 o2(2);
    T1 o1 = o2;
    ++o1;  // quiet gcc unused variable warning
  }
};

template <class T1,  class T2>
struct assign
{
  static void test()
  {
    T2 o2;
    o2 = 1;
    T1 o1;
    o1 = o2;
    if (o1) return; // quiet warnings
  }
};

template <class T1,  class T2, bool SameSignedness>
struct do_relational
{
  static void test()
  {
    T1 o1(1);
    T2 o2(2);
    BOOST_TEST( !(o1 == o2) );
    BOOST_TEST( o1 != o2 );
    BOOST_TEST( o1 < o2 );
    BOOST_TEST( o1 <= o2 );
    BOOST_TEST( !(o1 > o2) );
    BOOST_TEST( !(o1 >= o2 ) );
  }
};

template <class T1,  class T2>
struct do_relational<T1, T2, false>
{
  static void test()
  {
  }
};

template <class T1,  class T2>
struct relational
{
  static void test()
  {
    do_relational<T1, T2,
      boost::is_signed<typename value_type<T1>::type>::value
        == boost::is_signed<typename value_type<T2>::type>::value
                 >::test();
 //   do_relational<T1, T2, true>::test();
  }
};

template <class T1,  class T2>
struct op_plus
{
  static void test()
  {
    T1 o1(1);
    T2 o2(2);
    T1 o3;

    o3 = +o1;

    o3 = o1 + o2;

    o1 += o2;

    if (o3) return; // quiet warnings
  }
};

template <class T1,  class T2>
struct op_star
{
  static void test()
  {
    T1 o1(1);
    T2 o2(2);
    T1 o3;

    o3 = o1 * o2;

    o1 *= o2;

    if (o3) return; // quiet warnings
  }
};

template <template<class,  class> class Test,  class T1>
void op_test_aux()
{
  Test<T1, char>::test();
  Test<T1, unsigned char>::test();
  Test<T1, signed char>::test();
  Test<T1, short>::test();
  Test<T1, unsigned short>::test();
  Test<T1, int>::test();
  Test<T1, unsigned int>::test();
  Test<T1, long>::test();
  Test<T1, unsigned long>::test();
  Test<T1, long long>::test();
  Test<T1, unsigned long long>::test();
  Test<T1, be::big_int16_t>::test();
  Test<T1, be::big_int32_t>::test();
  Test<T1, be::big_int64_t>::test();
  Test<T1, be::big_uint16_t>::test();
  Test<T1, be::big_uint32_t>::test();
  Test<T1, be::big_uint64_t>::test();
  Test<T1, be::little_int16_t>::test();
  Test<T1, be::little_int32_t>::test();
  Test<T1, be::little_int64_t>::test();
  Test<T1, be::little_uint16_t>::test();
  Test<T1, be::little_uint32_t>::test();
  Test<T1, be::little_uint64_t>::test();
  Test<T1, be::big_int8_ut>::test();
  Test<T1, be::big_int16_ut>::test();
  Test<T1, be::big_int24_ut>::test();
  Test<T1, be::big_int32_ut>::test();
  Test<T1, be::big_int40_ut>::test();
  Test<T1, be::big_int48_ut>::test();
  Test<T1, be::big_int56_ut>::test();
  Test<T1, be::big_int64_ut>::test();
  Test<T1, be::big_uint8_ut>::test();
  Test<T1, be::big_uint16_ut>::test();
  Test<T1, be::big_uint24_ut>::test();
  Test<T1, be::big_uint32_ut>::test();
  Test<T1, be::big_uint40_ut>::test();
  Test<T1, be::big_uint64_ut>::test();
  Test<T1, be::little_int16_ut>::test();
  Test<T1, be::little_int24_ut>::test();
  Test<T1, be::little_int32_ut>::test();
  Test<T1, be::little_int64_ut>::test();
  Test<T1, be::little_uint16_ut>::test();
  Test<T1, be::little_uint32_ut>::test();
  Test<T1, be::little_uint56_ut>::test();
  Test<T1, be::little_uint64_ut>::test();
  Test<T1, be::native_int16_ut>::test();
  Test<T1, be::native_int24_ut>::test();
  Test<T1, be::native_int32_ut>::test();
  Test<T1, be::native_int64_ut>::test();
#ifdef BOOST_LONG_ENDIAN_TEST
  Test<T1, be::native_uint16_ut>::test();
  Test<T1, be::native_uint24_ut>::test();
  Test<T1, be::native_uint32_ut>::test();
  Test<T1, be::native_uint48_ut>::test();
  Test<T1, be::native_uint64_ut>::test();
  Test<T1, be::big_uint48_ut>::test();
  Test<T1, be::big_uint56_ut>::test();
  Test<T1, be::little_int8_ut>::test();
  Test<T1, be::little_int56_ut>::test();
  Test<T1, be::little_int40_ut>::test();
  Test<T1, be::little_int48_ut>::test();
  Test<T1, be::little_uint8_ut>::test();
  Test<T1, be::little_uint24_ut>::test();
  Test<T1, be::little_uint40_ut>::test();
  Test<T1, be::little_uint48_ut>::test();
  Test<T1, be::native_int8_ut>::test();
  Test<T1, be::native_int40_ut>::test();
  Test<T1, be::native_int48_ut>::test();
  Test<T1, be::native_int56_ut>::test();
  Test<T1, be::native_uint8_ut>::test();
  Test<T1, be::native_uint40_ut>::test();
  Test<T1, be::native_uint56_ut>::test();
#endif
}

template <template<class,  class> class Test>
void op_test()
{
  op_test_aux<Test, char>();
  op_test_aux<Test, unsigned char>();
  op_test_aux<Test, signed char>();
  op_test_aux<Test, short>();
  op_test_aux<Test, unsigned short>();
  op_test_aux<Test, int>();
  op_test_aux<Test, unsigned int>();
  op_test_aux<Test, long>();
  op_test_aux<Test, unsigned long>();
  op_test_aux<Test, long long>();
  op_test_aux<Test, unsigned long long>();
  op_test_aux<Test, be::big_int16_t>();
  op_test_aux<Test, be::big_int32_t>();
  op_test_aux<Test, be::big_int64_t>();
  op_test_aux<Test, be::little_int16_t>();
  op_test_aux<Test, be::little_int32_t>();
  op_test_aux<Test, be::little_int64_t>();
#ifdef BOOST_LONG_ENDIAN_TEST
  op_test_aux<Test, be::big_int8_ut>();
  op_test_aux<Test, be::big_int16_ut>();
  op_test_aux<Test, be::big_int24_ut>();
  op_test_aux<Test, be::big_int32_ut>();
  op_test_aux<Test, be::big_int40_ut>();
  op_test_aux<Test, be::big_int48_ut>();
  op_test_aux<Test, be::big_int56_ut>();
  op_test_aux<Test, be::big_int64_ut>();
  op_test_aux<Test, be::big_uint8_ut>();
  op_test_aux<Test, be::big_uint16_ut>();
  op_test_aux<Test, be::big_uint24_ut>();
  op_test_aux<Test, be::big_uint32_ut>();
  op_test_aux<Test, be::big_uint40_ut>();
  op_test_aux<Test, be::big_uint48_ut>();
  op_test_aux<Test, be::big_uint56_ut>();
  op_test_aux<Test, be::big_uint64_ut>();
  op_test_aux<Test, be::little_int8_ut>();
  op_test_aux<Test, be::little_int16_ut>();
  op_test_aux<Test, be::little_int24_ut>();
  op_test_aux<Test, be::little_int32_ut>();
  op_test_aux<Test, be::little_int40_ut>();
  op_test_aux<Test, be::little_int48_ut>();
  op_test_aux<Test, be::little_int56_ut>();
  op_test_aux<Test, be::little_int64_ut>();
  op_test_aux<Test, be::little_uint8_ut>();
  op_test_aux<Test, be::little_uint16_ut>();
  op_test_aux<Test, be::little_uint24_ut>();
  op_test_aux<Test, be::little_uint32_ut>();
  op_test_aux<Test, be::little_uint40_ut>();
  op_test_aux<Test, be::little_uint48_ut>();
  op_test_aux<Test, be::little_uint56_ut>();
  op_test_aux<Test, be::little_uint64_ut>();
  op_test_aux<Test, be::native_int8_ut>();
  op_test_aux<Test, be::native_int16_ut>();
  op_test_aux<Test, be::native_int24_ut>();
  op_test_aux<Test, be::native_int32_ut>();
  op_test_aux<Test, be::native_int40_ut>();
  op_test_aux<Test, be::native_int48_ut>();
  op_test_aux<Test, be::native_int56_ut>();
  op_test_aux<Test, be::native_int64_ut>();
  op_test_aux<Test, be::native_uint8_ut>();
  op_test_aux<Test, be::native_uint16_ut>();
  op_test_aux<Test, be::native_uint24_ut>();
  op_test_aux<Test, be::native_uint32_ut>();
  op_test_aux<Test, be::native_uint40_ut>();
  op_test_aux<Test, be::native_uint48_ut>();
  op_test_aux<Test, be::native_uint56_ut>();
  op_test_aux<Test, be::native_uint64_ut>();
#endif
}

//  test_inserter_and_extractor  -----------------------------------------------------//

void test_inserter_and_extractor()
{
  std::cout << "test inserter and extractor..." << std::endl;

  be::big_uint64_ut bu64(0x010203040506070ULL);
  be::little_uint64_ut lu64(0x010203040506070ULL);

  uint64_t x;

  std::stringstream ss;

  ss << bu64;
  ss >> x;
  BOOST_TEST_EQ(x, 0x010203040506070ULL);

  ss.clear();
  ss << lu64;
  ss >> x;
  BOOST_TEST_EQ(x, 0x010203040506070ULL);

  ss.clear();
  ss << 0x010203040506070ULL;
  be::big_uint64_ut bu64z(0);
  ss >> bu64z;
  BOOST_TEST_EQ(bu64z, bu64);

  ss.clear();
  ss << 0x010203040506070ULL;
  be::little_uint64_ut lu64z(0);
  ss >> lu64z;
  BOOST_TEST_EQ(lu64z, lu64);

  std::cout << "test inserter and extractor complete" << std::endl;

}

void f_big_int32_ut(be::big_int32_ut) {}

//  main  ------------------------------------------------------------------------------//

int cpp_main(int, char * [])
{
  be::endian_log = false;

  //  make sure some simple things work

  be::big_int32_ut o1(1);
  be::big_int32_ut o2(2L);
  be::big_int32_ut o3(3LL);
  be::big_int64_ut o4(1);

  //  use cases; if BOOST_ENDIAN_LOG is defined, will output to clog info on
  //  what overloads and conversions are actually being performed.

  be::endian_log = true;

  std::clog << "set up test values\n";
  be::big_int32_ut      big(12345);
  be::little_uint16_ut  little_u(10);
  be::big_int64_ut      result;

  // this is the use case that is so irritating that it caused the endian
  // constructors to be made non-explicit
  std::clog << "\nf(1234) where f(big_int32_ut)\n";
  f_big_int32_ut(1234);

  std::clog << "\nresult = big\n";
  result = big;

  std::clog << "\nresult = +big\n";
  result = +big;

  std::clog << "\nresult = -big\n";
  result = -big;

  std::clog << "\n++big\n";
  ++big;

  std::clog << "\nresult = big++\n";
  result = big++;

  std::clog << "\n--big\n";
  --big;

  std::clog << "\nbig--\n";
  big--;

  std::clog << "\nresult = big * big\n";
  result = big * big;

  std::clog << "\nresult = big * big\n";
  result = big * big;

  std::clog << "\nresult = big * little_u\n";
  result = big * little_u;

  std::clog << "\nbig *= little_u\n";
  big *= little_u;

  std::clog << "\nresult = little_u * big\n";
  result = little_u * big;

  std::clog << "\nresult = big * 5\n";
  result = big * 5;

  std::clog << "\nbig *= 5\n";
  big *= 5;

  std::clog << "\nresult = 5 * big\n";
  result = 5 * big;

  std::clog << "\nresult = little_u * 5\n";
  result = little_u * 5;

  std::clog << "\nresult = 5 * little_u\n";
  result = 5 * little_u;

  std::clog << "\nresult = 5 * 10\n";
  result = 5 * 10;
  std::clog << "\n";

  //  test from Roland Schwarz that detected ambiguities; these ambiguities
  //  were eliminated by BOOST_ENDIAN_MINIMAL_COVER_OPERATORS
  unsigned u;
  be::little_uint32_ut u1;
  be::little_uint32_ut u2;

  u = 9;
  u1 = 1;
  std::clog << "\nu2 = u1 + u\n";
  u2 = u1 + u;
  std::clog << "\n";

  // variations to detect ambiguities

  be::little_uint32_ut u3 = u1 + 5;
  u3 = u1 + 5u;

  if (u1 == 5)
    {}
  if (u1 == 5u)
    {}

  u1 += 5;
  u1 += 5u;

  u2 = u1 + 5;
  u2 = u1 + 5u;

  //  one more wrinkle
  be::little_uint16_ut u4(3);
  u4 = 3;
  std::clog << "\nu2 = u1 + u4\n";
  u2 = u1 + u4;
  std::clog << "\n";

  be::endian_log = false;

  test_inserter_and_extractor();
    
  //  perform the indicated test on ~60*60 operand types

  op_test<default_construct>();
  op_test<construct>();  // includes copy construction
  op_test<initialize>();
  op_test<assign>();
  op_test<relational>();
  op_test<op_plus>();
  op_test<op_star>();

  return boost::report_errors();
}
