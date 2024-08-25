// Copyright (c) 2024, German Neuroinformatics Node (G-Node)
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted under the terms of the BSD License. See
// LICENSE file in the root of the Project.

#include <gtest/gtest.h>

#include <nix/NDSize.hpp>

using namespace nix;

TEST(TestNDSize, testAll) {
    NDSize invalidSize = {};
    NDSize a({23, 42, 1982});

    EXPECT_TRUE(!invalidSize); // testing operator bool()
    EXPECT_TRUE(a ? true : false);

    typedef NDSize::value_type value_type;

    EXPECT_EQ(static_cast<value_type>(23),   a[0]);
    EXPECT_EQ(static_cast<value_type>(42),   a[1]);
    EXPECT_EQ(static_cast<value_type>(1982), a[2]);

    EXPECT_THROW(a[3], std::out_of_range);

    a++;

    EXPECT_EQ(static_cast<value_type>(24),   a[0]);
    EXPECT_EQ(static_cast<value_type>(43),   a[1]);
    EXPECT_EQ(static_cast<value_type>(1983), a[2]);

    a--;

    EXPECT_EQ(static_cast<value_type>(23),   a[0]);
    EXPECT_EQ(static_cast<value_type>(42),   a[1]);
    EXPECT_EQ(static_cast<value_type>(1982), a[2]);

    a += 13;

    EXPECT_EQ(static_cast<value_type>(36),   a[0]);
    EXPECT_EQ(static_cast<value_type>(55),   a[1]);
    EXPECT_EQ(static_cast<value_type>(1995), a[2]);

    a -= 13;

    EXPECT_EQ(static_cast<value_type>(23),   a[0]);
    EXPECT_EQ(static_cast<value_type>(42),   a[1]);
    EXPECT_EQ(static_cast<value_type>(1982), a[2]);


    NDSize b({19, 1940, 18});

    NDSize c = a + b;

    EXPECT_EQ(static_cast<value_type>(42),   c[0]);
    EXPECT_EQ(static_cast<value_type>(1982), c[1]);
    EXPECT_EQ(static_cast<value_type>(2000), c[2]);

    NDSize d = c - b;

    EXPECT_EQ(static_cast<value_type>(23),   d[0]);
    EXPECT_EQ(static_cast<value_type>(42),   d[1]);
    EXPECT_EQ(static_cast<value_type>(1982), d[2]);

    NDSize f({1, 2, 3, 4});
    EXPECT_THROW(a + f, std::out_of_range);

    NDSize g(f.size(), 0);

    g += f;

    EXPECT_TRUE(g == f);
    EXPECT_TRUE(g != a);

    NDSize h = b / b;
    EXPECT_EQ(static_cast<value_type>(1), h[0]);
    EXPECT_EQ(static_cast<value_type>(1), h[1]);
    EXPECT_EQ(static_cast<value_type>(1), h[2]);

    NDSize j(h.size(), static_cast<value_type>(333));
    NDSize k = h * j;

    EXPECT_TRUE(j == k);

    NDSize::value_type dp = j.dot(h);
    EXPECT_EQ(static_cast<NDSize::value_type>(999), dp);

    NDSize s({3, 4});
    dp = s.dot(s);
    EXPECT_EQ(static_cast<NDSize::value_type>(25), dp);

    //comparison tests
    EXPECT_THROW(static_cast<void>(f < s), IncompatibleDimensions);

    NDSize t({4, 5});
    NDSize u({4, 4});

    // actual non-delegation implementations are < and <=
    EXPECT_TRUE(s < t);
    EXPECT_TRUE(u <= t);

    // everything else, i.e. >, >= is delegated
    EXPECT_TRUE(t > s);
    EXPECT_TRUE(t >= u);

    EXPECT_TRUE(!(t <= s));
    EXPECT_TRUE(!(t < u));
}
