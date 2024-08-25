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

using namespace nix;

TEST(TestValue, testObject) {
    nix::Value boolVal(true);
    EXPECT_EQ(boolVal.get<bool>(), true);
    EXPECT_THROW(boolVal.get<int32_t>(), std::invalid_argument);

    const int32_t i32c = 42;
    nix::Value i32Val(i32c);
    EXPECT_EQ(i32Val.get<int32_t>(), i32c);
    EXPECT_THROW(i32Val.get<bool>(), std::invalid_argument);

    const uint32_t ui32c = 42U;
    nix::Value ui32Val(ui32c);
    EXPECT_EQ(ui32Val.get<uint32_t>(), ui32c);
    EXPECT_THROW(ui32Val.get<bool>(), std::invalid_argument);

    const int64_t i64c = 42;
    nix::Value i64Val(i64c);
    EXPECT_EQ(i64Val.get<int64_t>(), i64c);
    EXPECT_THROW(i64Val.get<bool>(), std::invalid_argument);

    const uint64_t ui64c = 42U;
    nix::Value ui64Val(ui64c);
    EXPECT_EQ(ui64Val.get<uint64_t>(), ui64c);
    EXPECT_THROW(ui64Val.get<bool>(), std::invalid_argument);

    const double dc = 2.71828;
    nix::Value dVal(dc);
    EXPECT_EQ(dVal.get<double>(), dc);
    EXPECT_THROW(dVal.get<bool>(), std::invalid_argument);

    const std::string strc = "When shall we three meet again";
    nix::Value sVal(strc);
    EXPECT_EQ(sVal.get<std::string>(), strc);
    EXPECT_THROW(sVal.get<bool>(), std::invalid_argument);

    const char long_string[] = "When shall we three meet again"
                               "In thunder, lightning, or in rain?";

    sVal.set(long_string);

    nix::Value v1 = sVal; //copy constructor
    EXPECT_EQ(sVal, v1); // check ==
    EXPECT_TRUE(v1 != dVal);

    EXPECT_EQ(v1.get<std::string>(), std::string(long_string));

    v1.set(long_string, strc.length()); // strc, long_string share this prefix
    EXPECT_EQ(v1.get<std::string>(), strc);

    v1 = dVal; //assignment op (copy)
    EXPECT_EQ(v1.get<double>(), dc);

    v1 = nix::Value(31337); //assignment op (move)
    EXPECT_EQ(v1.get<int>(), 31337);

    v1.set(nix::none); //set v1 to none
    EXPECT_EQ(v1.type(), nix::DataType::Nothing);

    //the rest of the supports_type test are in ValTester::check_basic, below
    EXPECT_EQ(false, nix::Value::supports_type(nix::DataType::Opaque));
}

struct ValTester {

    virtual void check_basic() const = 0;
    virtual void check_swap(const ValTester &other) const = 0;
    virtual void check_to_string() const = 0;
    virtual bool value_same(const nix::Value &other) const = 0;

    virtual nix::Value theVal() const = 0;
    virtual nix::DataType type() const = 0;

    virtual ~ValTester() { };
};

template<typename T>
struct ValueTester : ValTester {
    typedef T value_type;

    ValueTester(const T& v) : value(v), dtype(nix::to_data_type<T>::value), val(v) { }

    void check_basic() const override {
        EXPECT_TRUE(nix::Value::supports_type(dtype));
        EXPECT_EQ(dtype, value.type());
        EXPECT_EQ(val, value.get<T>());
    }

    void check_swap(const ValTester &other) const override {
        nix::Value a = value;
        nix::Value b = other.theVal();

        a.swap(b);

        EXPECT_EQ(b.type(), dtype);
        EXPECT_EQ(a.type(), other.type());

        EXPECT_EQ(b.get<T>(), val);
        EXPECT_TRUE(other.value_same(a));

        EXPECT_EQ(other.type() == dtype, other.theVal() == value);
    }

    void check_to_string() const override {
        std::stringstream val_stream;

        val_stream << val;

        std::stringstream value_stream;

        value_stream << value;

        std::string::size_type pos = value_stream.str().find(val_stream.str());
        EXPECT_TRUE(pos != std::string::npos);
    }

    nix::Value theVal() const override {
        return value;
    }

    nix::DataType type() const override {
        return dtype;
    }

    bool value_same(const nix::Value &other) const override {
        return other.type() == dtype && other.get<T>() == val;
    }

    nix::Value    value;
    nix::DataType dtype;
    T             val;
};

TEST(TestValue, testSwap) {
    using std::swap;

    nix::Value v1("Hallo");
    nix::Value v2("Welt");

    v1.swap(v2);

    EXPECT_EQ(v1.get<std::string>(), std::string("Welt"));
    EXPECT_EQ(v2.get<std::string>(), std::string("Hallo"));

    //now swap them back with global swap
    swap(v1, v2);

    EXPECT_EQ(v1.get<std::string>(), std::string("Hallo"));
    EXPECT_EQ(v2.get<std::string>(), std::string("Welt"));

    //lets swap a int32_t with a Nothing
    nix::Value v3(42);
    nix::Value v4;

    v4.swap(v3);

    EXPECT_EQ(v4.get<int>(), 42);
    EXPECT_EQ(v3.type(), nix::DataType::Nothing);

    // and now with the string
    const std::string checkStr = v2.get<std::string>();
    v4.swap(v2);
    EXPECT_EQ(v2.get<int>(), 42);
    EXPECT_EQ(v4.get<std::string>(), checkStr);

    //now lets do that systematically

    std::vector<ValTester *> vals = { new ValueTester<std::string>("String"),
                                      new ValueTester<uint32_t>(42),
                                      new ValueTester< int32_t>(42),
                                      new ValueTester<uint64_t>(42),
                                      new ValueTester< int64_t>(42),
                                      new ValueTester< double>(42.0),
                                      new ValueTester< bool>(true)};

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

TEST(TestValue, testEquals) {
    nix::Value v1("Hallo");
    nix::Value v2("Welt");
    v1.reference = "reference";
    v1.encoder = "encoder";
    v1.filename = "filename";
    v1.checksum = "checksum";
    v1.uncertainty = 20;

    EXPECT_TRUE(v2 != v1);
    v2 = v1;
    EXPECT_TRUE(v2 == v1);
    v2.checksum = "checksum2";
    EXPECT_TRUE(!(v2 == v1));
    v2.checksum = "checksum";
    v2.uncertainty = 10;
    EXPECT_TRUE(v2 != v1);
    v2.uncertainty = v1.uncertainty;
    v2.filename = "another";
    EXPECT_TRUE(v2 != v1);
    v2.filename = v1.filename;
    v2.encoder = "another";
    EXPECT_TRUE(v2 != v1);
    v2.encoder = v1.encoder;
    v2.reference = "another";
    EXPECT_TRUE(v2 != v1);
}
