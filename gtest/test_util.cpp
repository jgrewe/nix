// Copyright (c) 2024 German Neuroinformatics Node (G-Node)
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted under the terms of the BSD License. See
// LICENSE file in the root of the Project.

#include <gtest/gtest.h>

#include <ctime>
#include <cmath>

#include "nix.hpp"

using namespace std;
using namespace nix;


TEST(TestUtil, testStrUtils) {
    string a = " a ";
    string b = "\tb\t";
    string c = " c c\tc ";

    const string ac = a;
    const string bc = b;
    const string cc = c;

    string ad = util::deblankString(ac);
    string bd = util::deblankString(bc);
    string cd = util::deblankString(cc);

    util::deblankString(a);
    util::deblankString(b);
    util::deblankString(c);

    EXPECT_EQ(string("a"), a);
    EXPECT_EQ(string("b"), b);
    EXPECT_EQ(string("ccc"), c);

    EXPECT_EQ(ad, a);
    EXPECT_EQ(bd, b);
    EXPECT_EQ(cd, c);
}

TEST(TestUtil, testUnitScaling) {
    EXPECT_THROW(util::getSIScaling("mOhm","ms"), nix::InvalidUnit);
    EXPECT_TRUE(util::getSIScaling("mV","kV") == 1e-6);
    EXPECT_TRUE(util::getSIScaling("V","kV") == 1e-03);
    EXPECT_TRUE(util::getSIScaling("kV","V") == 1e+03);

    EXPECT_THROW(util::getSIScaling("mV^2","V"), nix::InvalidUnit);
    EXPECT_TRUE(util::getSIScaling("V^2","V^2") == 1.0);
    EXPECT_TRUE(util::getSIScaling("V","mV") == 1e+03);
    EXPECT_TRUE(util::getSIScaling("V^2","mV^2") == 1e+06);
    EXPECT_TRUE(util::getSIScaling("mV^2","kV^2") == 1e-12);
}

TEST(TestUtil, testIsSIUnit) {
    EXPECT_TRUE(util::isSIUnit("V"));
    EXPECT_TRUE(util::isSIUnit("mV"));
    EXPECT_TRUE(util::isSIUnit("mV^-2"));
    EXPECT_TRUE(util::isSIUnit("mV/cm"));
    EXPECT_TRUE(util::isSIUnit("dB"));
    EXPECT_TRUE(util::isSIUnit("rad"));
}

TEST(TestUtil, testSIUnitSplit) {
    string unit_1 = "V";
    string unit_2 = "mV";
    string unit_3 = "mV^2";
    string unit_4 = "mV^-2";
    string unit_5 = "m^2";

    string unit, prefix, power;
    util::splitUnit(unit_1, prefix, unit, power);
    EXPECT_TRUE(prefix == "" && unit == "V" && power == "");
    util::splitUnit(unit_2, prefix, unit, power);
    EXPECT_TRUE(prefix == "m" && unit == "V" && power == "");
    util::splitUnit(unit_3, prefix, unit, power);
    EXPECT_TRUE(prefix == "m" && unit == "V" && power == "2");
    util::splitUnit(unit_4, prefix, unit, power);
    EXPECT_TRUE(prefix == "m" && unit == "V" && power == "-2");
    util::splitUnit(unit_5, prefix, unit, power);
    EXPECT_TRUE(prefix == "" && unit == "m" && power == "2");
}

TEST(TestUtil, testIsAtomicSIUnit) {
    EXPECT_TRUE(util::isAtomicSIUnit("V"));
    EXPECT_TRUE(util::isAtomicSIUnit("mV"));
    EXPECT_TRUE(util::isAtomicSIUnit("mV^-2"));
    EXPECT_TRUE(!util::isAtomicSIUnit("mV/cm"));
    EXPECT_TRUE(util::isAtomicSIUnit("dB"));
    EXPECT_TRUE(util::isAtomicSIUnit("rad"));
}

TEST(TestUtil, testIsCompoundSIUnit) {
    string unit_1 = "mV*cm^-2";
    string unit_2 = "mV/cm^2";
    string unit_3 = "mV/cm^2*kg";
    string unit_4 = "mV";

    EXPECT_TRUE(util::isCompoundSIUnit(unit_1));
    EXPECT_TRUE(util::isCompoundSIUnit(unit_2));
    EXPECT_TRUE(util::isCompoundSIUnit(unit_3));
    EXPECT_TRUE(!util::isCompoundSIUnit(unit_4));
}

TEST(TestUtil, testSplitCompoundUnit) {
    string unit = "mV/cm^2*kg*V";
    string unit_2 = "mOhm/m";
    string unit_3 = "mV";

    vector<string> atomic_units, atomic_units_2, atomic_units_3;
    util::splitCompoundUnit(unit, atomic_units);
    EXPECT_TRUE(atomic_units.size() == 4);
    EXPECT_TRUE(atomic_units[0] == "mV" && atomic_units[1] == "cm^-2" &&
                atomic_units[2] == "kg" && atomic_units[3] == "V");

    util::splitCompoundUnit(unit_2, atomic_units_2);
    EXPECT_TRUE(atomic_units_2.size() == 2);
    EXPECT_TRUE(atomic_units_2[0] == "mOhm" && atomic_units_2[1] == "m^-1");

    util::splitCompoundUnit(unit_3, atomic_units_3);
    EXPECT_TRUE(atomic_units_3.size() == 1);
    EXPECT_TRUE(atomic_units_3[0] == unit_3);
}

TEST(TestUtil, testConvertToSeconds) {
    string unit_min = "min";
    string unit_h = "h";
    string unit_s = "s";
    string unit_ms = "ms";
    string unit_Ms = "Ms";
    double min_value = 25.5;
    double h_value = 12.25;
    double s_value = 100;
    int64_t m_value = 25;
    EXPECT_NEAR(1530.0, util::convertToSeconds(unit_min, min_value), std::numeric_limits<double>::round_error());
    EXPECT_NEAR(44100.0, util::convertToSeconds(unit_h, h_value), std::numeric_limits<double>::round_error());
    EXPECT_TRUE(1500 == util::convertToSeconds(unit_min, m_value));
    EXPECT_NEAR(s_value, util::convertToSeconds(unit_s, s_value), std::numeric_limits<double>::round_error());
    EXPECT_NEAR(s_value/1000., util::convertToSeconds(unit_ms, s_value), std::numeric_limits<double>::round_error());
    EXPECT_NEAR(s_value*1000000., util::convertToSeconds(unit_Ms, s_value), std::numeric_limits<double>::round_error());
    EXPECT_NEAR(s_value, util::convertToSeconds("std", s_value), std::numeric_limits<double>::round_error());
}

TEST(TestUtil, testConvertToKelvin) {
    string unit_f = "°F";
    string unit_f2 = "F";
    string unit_c = "°C";
    string unit_c2 = "C";
    string unit_k = "K";
    string unit_mk = "mK";
    string unit_Mk = "MK";
    string unit_k2 ="°K" ;
    double temperature = 100.0;
    EXPECT_NEAR(373.15, util::convertToKelvin(unit_c, temperature), std::numeric_limits<double>::round_error());
    EXPECT_NEAR(373.15, util::convertToKelvin(unit_c2, temperature), std::numeric_limits<double>::round_error());
    EXPECT_NEAR(311.0, round(util::convertToKelvin(unit_f, temperature)), std::numeric_limits<double>::round_error());
    EXPECT_NEAR(311.0, round(util::convertToKelvin(unit_f2, temperature)), std::numeric_limits<double>::round_error());
    EXPECT_NEAR(temperature, util::convertToKelvin(unit_k, temperature), std::numeric_limits<double>::round_error());
    EXPECT_NEAR(temperature, util::convertToKelvin(unit_k2, temperature), std::numeric_limits<double>::round_error());
    EXPECT_NEAR(temperature/1000., util::convertToKelvin(unit_mk, temperature), std::numeric_limits<double>::round_error());
    EXPECT_NEAR(temperature*1000000., util::convertToKelvin(unit_Mk, temperature), std::numeric_limits<double>::round_error());
    EXPECT_NEAR(temperature, util::convertToKelvin("kelvin", temperature), std::numeric_limits<double>::round_error());
    int temp_fi = 100;
    EXPECT_TRUE(util::convertToKelvin(unit_f, temp_fi) == 311);
}

TEST(TestUtil, testUnitSanitizer) {
    std::string unit = " mul/µs ";
    EXPECT_TRUE(util::unitSanitizer(unit) == "ul/us");
}

TEST(TestUtil, testDimTypeToStr) {
    std::string r("Range");
    std::string set("Set");
    std::string sam("Sample");
    EXPECT_TRUE(r.compare(util::dimTypeToStr(nix::DimensionType::Range)) == 0);
    EXPECT_TRUE(set.compare(util::dimTypeToStr(nix::DimensionType::Set)) == 0);
    EXPECT_TRUE(sam.compare(util::dimTypeToStr(nix::DimensionType::Sample)) == 0);
}

TEST(TestUtil, testChecks) {
    EXPECT_THROW(util::checkEntityName("invalid/entity/name"), InvalidName);
    EXPECT_THROW(util::checkEntityName(""), EmptyString);
    EXPECT_NO_THROW(util::checkEntityName("valid name"));

    EXPECT_THROW(util::checkEntityType(""), EmptyString);
    EXPECT_NO_THROW(util::checkEntityType("valid type"));
}

TEST(TestUtil, testStringVectors) {
    std::vector<std::string> vec_a{"", "A", "b", "", "C"};
    std::vector<std::string> vec_b{"", "D", "E", "F", "G"};
    std::vector<std::string> vec_c{"", "H", "E", "", "J"};
    std::vector<std::string> vec_d{"", "", "C"};

    EXPECT_TRUE(!util::isSetAtSamePos(vec_a, vec_b));
    EXPECT_TRUE(util::isSetAtSamePos(vec_a, vec_c));
    EXPECT_TRUE(!util::isSetAtSamePos(vec_a, vec_d));
}

