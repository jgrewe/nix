// Copyright (c) 2024 German Neuroinformatics Node (G-Node)
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted under the terms of the BSD License. See
// LICENSE file in the root of the Project.

#include <gtest/gtest.h>

#include "nix.hpp"
#include "nix/NDArray.hpp"


TEST(TestNDArray, basic) {
    nix::NDSize dims(2, 5);
    nix::NDArray A(nix::DataType::Double, dims);

    int values = 0;
    for(size_t i = 0; i != 5; ++i)
        for(size_t j = 0; j != 5; ++j)
            A.set<double>(i*5 + j, values++);

    values = 0;
    for(size_t i = 0; i != 5; ++i)
        for(size_t j = 0; j != 5; ++j)
            EXPECT_EQ(A.get<double>(i*5 + j), static_cast<double>(values++));

    EXPECT_EQ(static_cast<double>(5), A.get<double>(nix::NDSize({ 1, 0 })));
    EXPECT_EQ(static_cast<double>(18), A.get<double>(nix::NDSize({ 3, 3 })));
    EXPECT_EQ(static_cast<double>(24), A.get<double>(nix::NDSize({ 4, 4 })));

    dims = nix::NDSize({ 3, 4, 5 });
    nix::NDArray B(nix::DataType::Double, dims);
    values = 0;
    for(size_t i = 0; i != dims[0]; ++i)
        for(size_t j = 0; j != dims[1]; ++j)
            for(size_t k = 0; k != dims[2]; ++k)
                B.set<double>(nix::NDSize({ i, j, k }), values++);

    EXPECT_EQ(static_cast<double>(23), B.get<double>(nix::NDSize({ 1, 0, 3 })));
    EXPECT_EQ(static_cast<double>(42), B.get<double>(nix::NDSize({ 2, 0, 2 })));
    EXPECT_EQ(static_cast<double>(26), B.get<double>(nix::NDSize({ 1, 1, 1 })));

}
