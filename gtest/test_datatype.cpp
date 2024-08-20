#include <gtest/gtest.h>

#include "nix.hpp"

using namespace nix;

class DataTypeTest : public testing::Test {

protected:
    // You can remove any or all of the following functions if their bodies would
    // be empty.
    std::vector<DataType> types;
    std::vector<std::string> names;
    std::vector<std::string> namesUp;
    std::vector<std::string> namesLow;
    std::map<DataType, int> fixed_sizes;

    DataTypeTest() {
        types = {DataType::Bool, DataType::Char,
                 DataType::Float, DataType::Double,
                 DataType::Int8, DataType::Int16,
                 DataType::Int32, DataType::Int64,
                 DataType::UInt8, DataType::UInt16,
                 DataType::UInt32, DataType::UInt64,
                 DataType::String, DataType::Nothing,
                 DataType::Opaque};
        names = {"Bool", "Char", "Float", "Double",
                 "Int8", "Int16", "Int32", "Int64",
                 "UInt8", "UInt16", "UInt32", "UInt64",
                 "String", "Nothing", "Opaque"};
        namesUp = {"BooL", "CHar", "fLoat", "Double",
                   "Int8", "InT16", "Int32", "inT64",
                   "UINT8", "UINt16", "UInt32", "uINt64",
                   "StrinG", "NOTHING",  "oPAQUE"};
        namesLow = {"bool", "char", "float", "double",
                    "int8", "int16", "int32", "int64",
                    "uint8", "uint16", "uint32", "uint64",
                    "string", "nothing",  "opaque"};
        fixed_sizes = decltype(fixed_sizes) {{DataType::Int8, 1},
                                             {DataType::UInt8, 1},
                                             {DataType::Int16, 2},
                                             {DataType::UInt16, 2},
                                             {DataType::Int32, 4},
                                             {DataType::UInt32, 4},
                                             {DataType::Float, 4},
                                             {DataType::Int64, 8},
                                             {DataType::UInt64, 8},
                                             {DataType::Double, 8}};
     // You can do set-up work for each test here.
    }

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:
    
  // Class members declared here can be used by all tests in the test suite
  // for Foo.
};

TEST_F(DataTypeTest, TestToString) {
    for (size_t i = 0; i < types.size(); ++i){
        EXPECT_EQ(data_type_to_string(types[i]), names[i]);
    }
}

TEST_F(DataTypeTest, TestFromString) {
    for (size_t i = 0; i < namesUp.size(); ++i){
        EXPECT_EQ(string_to_data_type(namesUp[i]), types[i]);
        EXPECT_EQ(string_to_data_type(namesLow[i]), types[i]);
    }
    EXPECT_THROW(string_to_data_type("unknown"), std::invalid_argument);
}

TEST_F(DataTypeTest, TestSize) {
    for (auto t : types) {
        if (fixed_sizes.find(t) != fixed_sizes.end()){
            EXPECT_EQ(data_type_to_size(t), static_cast<size_t>(fixed_sizes[t]));
        }
    }
}
