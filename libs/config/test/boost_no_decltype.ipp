
//  (C) Copyright Beman Dawes 2008

//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/config for more information.

//  MACRO:         BOOST_NO_DECLTYPE
//  TITLE:         C++0x decltype unavailable
//  DESCRIPTION:   The compiler does not support C++0x decltype

namespace boost_no_decltype {

struct test_class
{
   test_class() {}
};

test_class get_test_class()
{
   return test_class();
}

template<typename F>
void baz(F f)
{
    //
    // Strangely VC-10 deduces the return type of F
    // to be "test_class&".  Remove the constructor
    // from test_class and then decltype does work OK!!
    //
    typedef decltype(f()) res;
    res r;
}

int test()
{
  int i;
  decltype(i) j;
  decltype(get_test_class()) k;
  baz(get_test_class);
  return 0;
}

}
