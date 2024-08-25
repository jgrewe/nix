// Copyright (c) 2013-2024, German Neuroinformatics Node (G-Node)
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted under the terms of the BSD License. See
// LICENSE file in the root of the Project.
//
// Author: Christian Kellner <kellner@bio.lmu.de>

#include <gtest/gtest.h>

#include <nix.hpp>
#include <cstring>

TEST(TestVariant, testObject) {

    nix::Variant boolVal(true);
    EXPECT_EQ(boolVal.get<bool>(), true);
    EXPECT_THROW(boolVal.get<int32_t>(), std::invalid_argument);

    const int32_t i32c = 42;
    nix::Variant i32Val(i32c);
    EXPECT_EQ(i32Val.get<int32_t>(), i32c);
    EXPECT_THROW(i32Val.get<bool>(), std::invalid_argument);

    const uint32_t ui32c = 42U;
    nix::Variant ui32Val(ui32c);
    EXPECT_EQ(ui32Val.get<uint32_t>(), ui32c);
    EXPECT_THROW(ui32Val.get<bool>(), std::invalid_argument);

    const int64_t i64c = 42;
    nix::Variant i64Val(i64c);
    EXPECT_EQ(i64Val.get<int64_t>(), i64c);
    EXPECT_THROW(i64Val.get<bool>(), std::invalid_argument);

    const uint64_t ui64c = 42U;
    nix::Variant ui64Val(ui64c);
    EXPECT_EQ(ui64Val.get<uint64_t>(), ui64c);
    EXPECT_THROW(ui64Val.get<bool>(), std::invalid_argument);

    const double dc = 2.71828;
    nix::Variant dVal(dc);
    EXPECT_EQ(dVal.get<double>(), dc);
    EXPECT_THROW(dVal.get<bool>(), std::invalid_argument);

    const std::string strc = "When shall we three meet again";
    nix::Variant sVal(strc);
    EXPECT_EQ(sVal.get<std::string>(), strc);
    EXPECT_THROW(sVal.get<bool>(), std::invalid_argument);

    const char long_string[] = "When shall we three meet again"
            "In thunder, lightning, or in rain?";

    sVal.set(long_string);

    nix::Variant v1 = sVal; //copy constructor
    EXPECT_EQ(sVal, v1); // check ==
    EXPECT_TRUE(v1 != dVal);

    EXPECT_EQ(v1.get<std::string>(), std::string(long_string));

    v1.set(long_string, strc.length()); // strc, long_string share this prefix
    EXPECT_EQ(v1.get<std::string>(), strc);

    v1 = dVal; //assignment op (copy)
    EXPECT_EQ(v1.get<double>(), dc);

    v1 = nix::Variant(31337); //assignment op (move)
    EXPECT_EQ(v1.get<int>(), 31337);

    v1.set(nix::none); //set v1 to none
    EXPECT_EQ(v1.type(), nix::DataType::Nothing);

    //the rest of the supports_type test are in ValTester::check_basic, below
    EXPECT_EQ(false, nix::Variant::supports_type(nix::DataType::Opaque));
}

struct ValTester {

    virtual void check_basic() const = 0;
    virtual void check_swap(const ValTester &other) const = 0;
    virtual void check_to_string() const = 0;
    virtual bool Variant_same(const nix::Variant &other) const = 0;

    virtual nix::Variant theVal() const = 0;
    virtual nix::DataType type() const = 0;

    virtual ~ValTester() { };
};

template<typename T>
struct VariantTester : ValTester {
    typedef T Variant_type;

    VariantTester(const T& v) : Variant(v), dtype(nix::to_data_type<T>::value), val(v) { }

    void check_basic() const override {
        EXPECT_TRUE(nix::Variant::supports_type(dtype));
        EXPECT_EQ(dtype, Variant.type());
        EXPECT_EQ(val, Variant.get<T>());
    }

    void check_swap(const ValTester &other) const override {
        nix::Variant a = Variant;
        nix::Variant b = other.theVal();

        a.swap(b);

        EXPECT_EQ(b.type(), dtype);
        EXPECT_EQ(a.type(), other.type());

        EXPECT_EQ(b.get<T>(), val);
        EXPECT_TRUE(other.Variant_same(a));

        EXPECT_EQ(other.type() == dtype, other.theVal() == Variant);
    }

    void check_to_string() const override {
        std::stringstream val_stream;

        val_stream << val;

        std::stringstream Variant_stream;

        Variant_stream << Variant;

        std::string::size_type pos = Variant_stream.str().find(val_stream.str());
        EXPECT_TRUE(pos != std::string::npos);
    }

    nix::Variant theVal() const override {
        return Variant;
    }

    nix::DataType type() const override {
        return dtype;
    }

    bool Variant_same(const nix::Variant &other) const override {
        return other.type() == dtype && other.get<T>() == val;
    }

    nix::Variant    Variant;
    nix::DataType dtype;
    T             val;
};

TEST(TestVariant, testSwap) {
    using std::swap;

    nix::Variant v1("Hallo");
    nix::Variant v2("Welt");

    v1.swap(v2);

    EXPECT_EQ(v1.get<std::string>(), std::string("Welt"));
    EXPECT_EQ(v2.get<std::string>(), std::string("Hallo"));

    //now swap them back with global swap
    swap(v1, v2);

    EXPECT_EQ(v1.get<std::string>(), std::string("Hallo"));
    EXPECT_EQ(v2.get<std::string>(), std::string("Welt"));

    //lets swap a int32_t with a Nothing
    nix::Variant v3(42);
    nix::Variant v4;

    v4.swap(v3);

    EXPECT_EQ(v4.get<int>(), 42);
    EXPECT_EQ(v3.type(), nix::DataType::Nothing);

    // and now with the string
    const std::string checkStr = v2.get<std::string>();
    v4.swap(v2);
    EXPECT_EQ(v2.get<int>(), 42);
    EXPECT_EQ(v4.get<std::string>(), checkStr);

    //now lets do that systematically

    std::vector<ValTester *> vals = { new VariantTester<std::string>("String"),
                                      new VariantTester<uint32_t>(42),
                                      new VariantTester< int32_t>(42),
                                      new VariantTester<uint64_t>(42),
                                      new VariantTester< int64_t>(42),
                                      new VariantTester< double>(42.0),
                                      new VariantTester< bool>(true)};

    for (const ValTester *a : vals) {
        a->check_basic();
        a->check_to_string();

        for (const ValTester *b : vals) {
            a->check_swap(*b);
        }
    }

    for (ValTester *vt : vals) {
        delete vt;
    }
}

TEST(TestVariant, testEquals) {
    nix::Variant v1("Hallo");
    nix::Variant v2("Welt");

    EXPECT_TRUE(v2 != v1);
    v2 = v1;
    EXPECT_TRUE(v2 == v1);
}

