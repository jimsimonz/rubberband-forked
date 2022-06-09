/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*-  vi:set ts=8 sts=4 sw=4: */

/*
    Rubber Band Library
    An audio time-stretching and pitch-shifting library.
    Copyright 2007-2022 Particular Programs Ltd.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.

    Alternatively, if you have a valid commercial licence for the
    Rubber Band Library obtained by agreement with the copyright
    holders, you may redistribute and/or modify it under the terms
    described in that licence.

    If you wish to distribute code using the Rubber Band Library
    under terms other than those of the GNU General Public License,
    you must obtain a valid commercial licence before doing so.
*/

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "../common/MovingMedian.h"
#include "../finer/Peak.h"

using namespace RubberBand;
using namespace std;
namespace tt = boost::test_tools;

BOOST_AUTO_TEST_SUITE(TestSignalBits)

// NB our moving median has different lag behaviour from bsq - we
// begin padded with zeros, while bsq begins with an empty vector. The
// bsq behaviour is imho more correct, and this really shows up in the
// n_1 case below (where the correct answer is surely {1.0} rather
// than {0.0}) but ours is not wholly wrong, more efficient, "usually
// fine"

BOOST_AUTO_TEST_CASE(moving_median_simple_3)
{
    MovingMedian<double> mm(3);
    vector<double> arr { 1.0, 2.0, 3.0 };
    vector<double> expected { 1.0, 2.0, 2.0 };
    MovingMedian<double>::filter(mm, arr);
    BOOST_TEST(arr == expected, tt::per_element());
}

BOOST_AUTO_TEST_CASE(moving_median_simple_4)
{
    MovingMedian<double> mm(4);
    vector<double> arr { 1.0, 2.0, 3.0, 4.0 };
    vector<double> expected { 2.0, 3.0, 3.0, 3.0 };
    MovingMedian<double>::filter(mm, arr);
    BOOST_TEST(arr == expected, tt::per_element());
}

BOOST_AUTO_TEST_CASE(moving_median_simple_3_4)
{
    MovingMedian<double> mm(3);
    vector<double> arr { 1.2, 0.6, 1.0e-6, 1.0 };
    vector<double> expected { 0.6, 0.6, 0.6, 1.0e-6 };
    MovingMedian<double>::filter(mm, arr);
    BOOST_TEST(arr == expected, tt::per_element());
}

BOOST_AUTO_TEST_CASE(moving_median_simple_5_4)
{
    MovingMedian<double> mm(5);
    vector<double> arr { 1.2, 0.6, 1.0e-6, 1.0 };
    vector<double> expected { 1.0e-6, 0.6, 0.6, 1.0e-6 };
    MovingMedian<double>::filter(mm, arr);
    BOOST_TEST(arr == expected, tt::per_element());
}
  
BOOST_AUTO_TEST_CASE(moving_median_order_1)
{
    MovingMedian<double> mm(1);
    vector<double> arr { 1.2, 0.6, 1.0e-6, 1.0e-6 };
    vector<double> expected = arr;
    MovingMedian<double>::filter(mm, arr);
    BOOST_TEST(arr == expected, tt::per_element());
}
  
BOOST_AUTO_TEST_CASE(moving_median_n_1)
{
    MovingMedian<double> mm(6);
    vector<double> arr { 1.0 };
    vector<double> expected { 0.0 };
    MovingMedian<double>::filter(mm, arr);
    BOOST_TEST(arr == expected, tt::per_element());
}

BOOST_AUTO_TEST_CASE(peakpick_nearest_2_1)
{
    Peak<double> pp(1);
    vector<double> in { -0.1 };
    vector<int> out(1);
    vector<int> expected { 0 };
    pp.findNearestAndNextPeaks(in.data(), 2, out.data(), nullptr);
    BOOST_TEST(out == expected, tt::per_element());
}

BOOST_AUTO_TEST_CASE(peakpick_nearest_2_5)
{
    Peak<double> pp(5);
    vector<double> in { -0.3, -0.1, -0.2, 1.0, -0.3 };
    vector<int> out(5);
    vector<int> expected { 3, 3, 3, 3, 3 };
    pp.findNearestAndNextPeaks(in.data(), 2, out.data(), nullptr);
    BOOST_TEST(out == expected, tt::per_element());
}

BOOST_AUTO_TEST_CASE(peakpick_nearest_2_12)
{
    Peak<double> pp(12);
    vector<double> in { -0.3, -0.1, -0.2, 1.0, -0.3, -0.5,
                        -0.5, -0.4, -0.1, -0.1, -0.2, -0.3 };
    vector<int> out(12);
    vector<int> expected { 3, 3, 3, 3, 3, 3, 8, 8, 8, 8, 8, 8 };
    pp.findNearestAndNextPeaks(in.data(), 2, out.data(), nullptr);
    BOOST_TEST(out == expected, tt::per_element());
}

BOOST_AUTO_TEST_CASE(peakpick_next_2_1)
{
    Peak<double> pp(1);
    vector<double> in { -0.1 };
    vector<int> out(1);
    vector<int> expected { 0 };
    pp.findNearestAndNextPeaks(in.data(), 2, nullptr, out.data());
    BOOST_TEST(out == expected, tt::per_element());
}

BOOST_AUTO_TEST_CASE(peakpick_next_2_5)
{
    Peak<double> pp(5);
    vector<double> in { -0.3, -0.1, -0.2, 1.0, -0.3 };
    vector<int> out(5);
    vector<int> expected { 3, 3, 3, 3, 4 };
    pp.findNearestAndNextPeaks(in.data(), 2, nullptr, out.data());
    BOOST_TEST(out == expected, tt::per_element());
}

BOOST_AUTO_TEST_CASE(peakpick_next_2_12)
{
    Peak<double> pp(12);
    vector<double> in { -0.3, -0.1, 0.2, -1.0, -0.3, -0.5,
                        -0.5, -0.4, -0.1, -0.1, -0.2, -0.3 };
    vector<int> out(12);
    vector<int> expected { 2, 2, 2, 8, 8, 8, 8, 8, 8, 9, 10, 11 };
    pp.findNearestAndNextPeaks(in.data(), 2, nullptr, out.data());
    BOOST_TEST(out == expected, tt::per_element());
}

BOOST_AUTO_TEST_CASE(troughpick_nearest_2_1)
{
    Peak<double, std::less<double>> pp(1);
    vector<double> in { -0.1 };
    vector<int> out(1);
    vector<int> expected { 0 };
    pp.findNearestAndNextPeaks(in.data(), 2, out.data(), nullptr);
    BOOST_TEST(out == expected, tt::per_element());
}

BOOST_AUTO_TEST_CASE(troughpick_nearest_2_5)
{
    Peak<double, std::less<double>> pp(5);
    vector<double> in { -0.3, -0.1, -0.4, 0.1, -0.3 };
    vector<int> out(5);
    vector<int> expected { 2, 2, 2, 2, 2 };
    pp.findNearestAndNextPeaks(in.data(), 2, out.data(), nullptr);
    BOOST_TEST(out == expected, tt::per_element());
}

BOOST_AUTO_TEST_CASE(troughpick_nearest_2_12)
{
    Peak<double, std::less<double>> pp(12);
    vector<double> in { -0.3, -0.1, -0.2, 1.0, -0.3, -0.5,
                        -0.5, -0.4, -0.1, -0.1, -0.2, -0.3 };
    vector<int> out(12);
    vector<int> expected { 0, 0, 0, 5, 5, 5, 5, 5, 11, 11, 11, 11 };
    pp.findNearestAndNextPeaks(in.data(), 2, out.data(), nullptr);
    BOOST_TEST(out == expected, tt::per_element());
}

BOOST_AUTO_TEST_CASE(troughpick_next_2_1)
{
    Peak<double, std::less<double>> pp(1);
    vector<double> in { -0.1 };
    vector<int> out(1);
    vector<int> expected { 0 };
    pp.findNearestAndNextPeaks(in.data(), 2, nullptr, out.data());
    BOOST_TEST(out == expected, tt::per_element());
}

BOOST_AUTO_TEST_CASE(troughpick_next_2_5)
{
    Peak<double, std::less<double>> pp(5);
    vector<double> in { -0.3, -0.1, -0.4, 0.1, -0.3 };
    vector<int> out(5);
    vector<int> expected { 2, 2, 2, 3, 4 };
    pp.findNearestAndNextPeaks(in.data(), 2, nullptr, out.data());
    BOOST_TEST(out == expected, tt::per_element());
}

BOOST_AUTO_TEST_CASE(troughpick_next_2_12)
{
    Peak<double, std::less<double>> pp(12);
    vector<double> in { -0.3, -0.1, 0.2, 1.0, -0.3, -0.5,
                        -0.5, -0.4, -0.1, -0.1, -0.2, -0.3 };
    vector<int> out(12);
    vector<int> expected { 0, 5, 5, 5, 5, 5, 11, 11, 11, 11, 11, 11 };
    pp.findNearestAndNextPeaks(in.data(), 2, nullptr, out.data());
    BOOST_TEST(out == expected, tt::per_element());
}

BOOST_AUTO_TEST_SUITE_END()
