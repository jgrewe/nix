// Copyright (c) 2013-2024, German Neuroinformatics Node (G-Node)
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted under the terms of the BSD License. See
// LICENSE file in the root of the Project.
#include <gtest/gtest.h>

#include <nix/hydra/multiArray.hpp>
#include <nix/util/util.hpp>
#include <nix.hpp>

#include <iostream>
#include <sstream>
#include <iterator>
#include <stdexcept>
#include <limits>
#include <ctime>
#include <typeinfo>
#include <boost/any.hpp>
#include <boost/math/constants/constants.hpp>

using namespace std;
using namespace nix;

namespace test {
    template<typename T>
    bool TtoBool(T var) {
        return !!var;
    }
    template<>
    bool TtoBool<std::vector<std::string>>(std::vector<std::string> var) {
        return !(var.empty());
    }
    template<>
    bool TtoBool<std::vector<double>>(std::vector<double> var) {
        return !(var.empty());
    }
    template<>
    bool TtoBool<nix::Section>(nix::Section var) {
        return !(var.isNone());
    }
    template<>
    bool TtoBool<nix::DataArray>(nix::DataArray var) {
        return !(var.isNone());
    }
    template<>
    bool TtoBool<nix::LinkType>(nix::LinkType var) {
        return true; // LinkType as enum is always set
    }
    template<>
    bool TtoBool<std::vector<nix::Variant>>(std::vector<nix::Variant> var) {
        return !(var.empty());
    }
    template<>
    bool TtoBool<std::vector<nix::DataArray>>(std::vector<nix::DataArray> var) {
        return !(var.empty());
    }
    template<>
    bool TtoBool<std::vector<nix::Source>>(std::vector<nix::Source> var) {
        return !(var.empty());
    }
    template<>
    bool TtoBool<std::string>(std::string var) {
        return (var.length() > 0);
    }

    bool isValidOptional(bool is_opt, bool is_set, bool accepts_none) {
        return (is_opt && accepts_none);
    }

    bool isValidObligatory(bool is_opt, bool is_set, bool accepts_none) {
        return (!is_opt && !accepts_none && is_set);
    }

    enum METHOD_NAME { FIRST };
    template<typename T, METHOD_NAME S>
    class accepts_noneT
    {
        template <typename U, void (U::*)(boost::none_t)> struct Check;
        template <typename U> static char func(Check<U, &U::id> *);
        template <typename U> static int func(...);
    public:
        enum { value = sizeof(func<T>(0)) == sizeof(char) };
    };
    #define ACCEPTS(M) \
        static const METHOD_NAME M = static_cast<METHOD_NAME>(__COUNTER__ + 1); \
        template<typename T> \
        class accepts_noneT<T, M> \
        { \
            template <typename U, void (U::*)(boost::none_t)> struct Check; \
            template <typename U> static char func(Check<U, &U::M> *); \
            template <typename U> static int func(...); \
        public: \
            enum { value = sizeof(func<T>(0)) == sizeof(char) }; \
        };
    ACCEPTS(id) ACCEPTS(index) ACCEPTS(type) ACCEPTS(name) ACCEPTS(definition) ACCEPTS(label)
    ACCEPTS(labels) ACCEPTS(unit) ACCEPTS(uncertainty) ACCEPTS(metadata) ACCEPTS(ticks) ACCEPTS(offset)
    ACCEPTS(extent) ACCEPTS(extents) ACCEPTS(position) ACCEPTS(positions)
    ACCEPTS(references) ACCEPTS(expansionOrigin) ACCEPTS(samplingInterval)
    ACCEPTS(values) ACCEPTS(data) ACCEPTS(linkType) ACCEPTS(link)
    ACCEPTS(repository) ACCEPTS(units) ACCEPTS(sources)
} // namespace test


class TestOptionalObligatory : public testing::Test {

protected:
    nix::File file;
    nix::Section section;
    nix::Block block;
    nix::Property property;
    nix::DataArray dataArray, positions;
    nix::Tag tag;
    nix::MultiTag multiTag;
    nix::Source source;
    nix::SetDimension set_dim;
    nix::SampledDimension sampled_dim;
    nix::RangeDimension range_dim;
    nix::Feature feature;
    time_t startup_time;
    nix::Variant dummy;
    bool is_opt;   // whether getter return value is boost::optional
    bool is_set;   // whether getter return value is set
    // bool accepts_none; // whether setter accepts boost::none (declared in place)
    std::string summary;

    TestOptionalObligatory() {
        startup_time = time(NULL);

        // File-------------------------------------------------------------
        file = File::open("test_block.h5", FileMode::Overwrite);

        // Section---------------------------------------------------------
        section = file.createSection("foo_section", "metadata");

        // Block------------------------------------------------------------
        block = file.createBlock("block_one", "dataset");

        // property---------------------------------------------------------
        dummy = Variant(10);
        property = section.createProperty("prop", dummy);

        // dataAray---------------------------------------------------------
        dataArray = block.createDataArray("dimensionTest", "test", DataType::Double, nix::NDSize({ 0 }));

        // tag--------------------------------------------------------
        vector<string> array_names = { "data_array_a", "data_array_b", "data_array_c",
                                    "data_array_d", "data_array_e" };

        tag = block.createTag("featureTest", "Test", {0.0, 2.0, 3.4});
        vector<DataArray> refs;
        for (const auto &name : array_names) {
            refs.push_back(block.createDataArray(name, "reference", DataType::Double, nix::NDSize({ 0 })));
        }
        tag.references(refs);

        // multiTag----------------------------------------------------------
        positions = block.createDataArray("positions_DataArray", "dataArray",
                                        DataType::Double, nix::NDSize({ 0 }));
        typedef boost::multi_array<double, 1> array_type;
        typedef array_type::index index;
        array_type A(boost::extents[5]);
        for(index i = 0; i < 5; ++i){
            A[i] = 100.0*i;
        }
        positions.setData(A);
        multiTag = block.createMultiTag("tag_one", "test_tag", positions);

        // source-----------------------------------------------------------
        source = block.createSource("source_one", "channel");

        // set_dimension-----------------------------------------------------
        set_dim = dataArray.appendSetDimension();

        // sampled_dimension-------------------------------------------------
        double samplingInterval = boost::math::constants::pi<double>();
        sampled_dim = dataArray.appendSampledDimension(samplingInterval);

        // range_dimension---------------------------------------------------
        std::vector<double> ticks;
        for (size_t i = 0; i < 5; i++) {
            ticks.push_back(i * boost::math::constants::pi<double>());
        }
        range_dim = dataArray.appendRangeDimension(ticks);

        // feature
        feature = tag.createFeature(dataArray, nix::LinkType::Tagged);
    }

    void summarize(std::string name, bool is_opt, bool is_set, bool accepts_none) {
        summary += name; summary += ": ";
        if(test::isValidOptional(is_opt, is_set, accepts_none)) {
            summary += "OPTIONAL";
        }
        else if(test::isValidObligatory(is_opt, is_set, accepts_none)) {
            summary += "OBLIGATORY";
        }
        else {
            summary += "FAIL"; summary += "\n    is_optional:"; summary += util::numToStr(is_opt);
            summary += "\n    is_set:"; summary += util::numToStr(is_set); summary += "\n    accepts_none:";
            summary += util::numToStr(accepts_none);
        }
        summary += "\n";
    }

    ~TestOptionalObligatory() {
        file.close();
        std::cout << summary;
    }
};


TEST_F(TestOptionalObligatory, testNamedEntityId) {
    static const bool accepts_none = test::accepts_noneT<nix::base::NamedEntity<base::INamedEntity>, test::id>::value;
    // using SFINAE if-else block instead of normal one to avoid "unreachable code" warnings
    // equivalent: std::is_class<decltype(block.id())>::value ? accepts_none : util::is_optional<decltype(block.id());
    is_opt   = std::conditional<std::is_class<decltype(block.id())>::value,
                                std::integral_constant<bool, accepts_none>,
                                std::integral_constant<bool, util::is_optional<decltype(block.id())>::value>
                                    >::type::value;
    is_set   = test::TtoBool(util::deRef(block.id()));
    summarize("NamedEntity::id", is_opt, is_set, accepts_none);
    EXPECT_TRUE(test::isValidOptional(is_opt, is_set, accepts_none) ||
                test::isValidObligatory(is_opt, is_set, accepts_none));
}

TEST_F(TestOptionalObligatory, testNamedEntityType) {
    static const bool accepts_none = test::accepts_noneT<nix::base::NamedEntity<base::INamedEntity>, test::type>::value;
    is_opt   = std::conditional<std::is_class<decltype(block.type())>::value,
                                std::integral_constant<bool, accepts_none>,
                                std::integral_constant<bool, util::is_optional<decltype(block.type())>::value>
                                    >::type::value;
    is_set   = test::TtoBool(util::deRef(block.type()));
    summarize("NamedEntity::type", is_opt, is_set, accepts_none);
    EXPECT_TRUE(test::isValidOptional(is_opt, is_set, accepts_none) ||
                test::isValidObligatory(is_opt, is_set, accepts_none));
}

TEST_F(TestOptionalObligatory, testNamedEntityName) {
    static const bool accepts_none = test::accepts_noneT<nix::base::NamedEntity<base::INamedEntity>, test::name>::value;
    is_opt   = std::conditional<std::is_class<decltype(block.name())>::value,
                                std::integral_constant<bool, accepts_none>,
                                std::integral_constant<bool, util::is_optional<decltype(block.name())>::value>
                                    >::type::value;
    is_set   = test::TtoBool(util::deRef(block.name()));
    summarize("NamedEntity::name", is_opt, is_set, accepts_none);
    EXPECT_TRUE(test::isValidOptional(is_opt, is_set, accepts_none) ||
                test::isValidObligatory(is_opt, is_set, accepts_none));
}

TEST_F(TestOptionalObligatory, testNamedEntityDefinition) {
    static const bool accepts_none = test::accepts_noneT<nix::base::NamedEntity<base::INamedEntity>, test::definition>::value;
    is_opt   = std::conditional<std::is_class<decltype(block.definition())>::value,
                                std::integral_constant<bool, accepts_none>,
                                std::integral_constant<bool, util::is_optional<decltype(block.definition())>::value>
                                    >::type::value;
    is_set   = test::TtoBool(util::deRef(block.definition()));
    summarize("NamedEntity::definition", is_opt, is_set, accepts_none);
    EXPECT_TRUE(test::isValidOptional(is_opt, is_set, accepts_none) ||
                test::isValidObligatory(is_opt, is_set, accepts_none));
}

TEST_F(TestOptionalObligatory, testEntityWithMetadataMetadata) {
    static const bool accepts_none = test::accepts_noneT<nix::base::EntityWithMetadata<base::IEntityWithMetadata>, test::metadata>::value;
    is_opt   = std::conditional<std::is_class<decltype(block.metadata())>::value,
                                    std::integral_constant<bool, accepts_none>,
                                    std::integral_constant<bool, util::is_optional<decltype(block.metadata())>::value>
                                    >::type::value;
    is_set   = test::TtoBool(util::deRef(block.metadata()));
    summarize("EntityWithMetadata::metadata", is_opt, is_set, accepts_none);
    EXPECT_TRUE(test::isValidOptional(is_opt, is_set, accepts_none) ||
                test::isValidObligatory(is_opt, is_set, accepts_none));
}

TEST_F(TestOptionalObligatory, testEntityWithSourcesSources) {
    static const bool accepts_none = test::accepts_noneT<nix::base::EntityWithSources<base::IEntityWithSources>, test::sources>::value;
    is_opt   = std::conditional<std::is_class<decltype(block.sources())>::value,
                                    std::integral_constant<bool, accepts_none>,
                                    std::integral_constant<bool, util::is_optional<decltype(block.sources())>::value>
                                    >::type::value;
    is_set   = test::TtoBool(util::deRef(block.sources()));
    summarize("EntityWithSources::sources", is_opt, is_set, accepts_none);
    EXPECT_TRUE(test::isValidOptional(is_opt, is_set, accepts_none) ||
                test::isValidObligatory(is_opt, is_set, accepts_none));
}

TEST_F(TestOptionalObligatory, testDataArrayLabel) {
    static const bool accepts_none = test::accepts_noneT<nix::DataArray, test::label>::value;
    is_opt   = std::conditional<std::is_class<decltype(dataArray.label())>::value,
                                    std::integral_constant<bool, accepts_none>,
                                    std::integral_constant<bool, util::is_optional<decltype(dataArray.label())>::value>
                                    >::type::value;
    is_set   = test::TtoBool(util::deRef(dataArray.label()));
    summarize("DataArray::label", is_opt, is_set, accepts_none);
    EXPECT_TRUE(test::isValidOptional(is_opt, is_set, accepts_none) ||
                test::isValidObligatory(is_opt, is_set, accepts_none));
}

TEST_F(TestOptionalObligatory, testDataArrayUnit) {
    static const bool accepts_none = test::accepts_noneT<nix::DataArray, test::unit>::value;
    is_opt   = std::conditional<std::is_class<decltype(dataArray.unit())>::value,
                                    std::integral_constant<bool, accepts_none>,
                                    std::integral_constant<bool, util::is_optional<decltype(dataArray.unit())>::value>
                                    >::type::value;
    is_set   = test::TtoBool(util::deRef(dataArray.unit()));
    summarize("DataArray::unit", is_opt, is_set, accepts_none);
    EXPECT_TRUE(test::isValidOptional(is_opt, is_set, accepts_none) ||
                test::isValidObligatory(is_opt, is_set, accepts_none));
}

TEST_F(TestOptionalObligatory, testDataArrayExpansionOrigin) {
    static const bool accepts_none = test::accepts_noneT<nix::DataArray, test::expansionOrigin>::value;
    is_opt   = std::conditional<std::is_class<decltype(dataArray.expansionOrigin())>::value,
                                    std::integral_constant<bool, accepts_none>,
                                    std::integral_constant<bool, util::is_optional<decltype(dataArray.expansionOrigin())>::value>
                                    >::type::value;
    is_set   = test::TtoBool(util::deRef(dataArray.expansionOrigin()));
    summarize("DataArray::expansionOrigin", is_opt, is_set, accepts_none);
    EXPECT_TRUE(test::isValidOptional(is_opt, is_set, accepts_none) ||
                test::isValidObligatory(is_opt, is_set, accepts_none));
}

TEST_F(TestOptionalObligatory, testSetDimensionIndex) {
    static const bool accepts_none = test::accepts_noneT<nix::SetDimension, test::index>::value;
    is_opt   = std::conditional<std::is_class<decltype(set_dim.index())>::value,
                                    std::integral_constant<bool, accepts_none>,
                                    std::integral_constant<bool, util::is_optional<decltype(set_dim.index())>::value>
                                    >::type::value;
    is_set   = test::TtoBool(util::deRef(set_dim.index()));
    summarize("SetDimension::index", is_opt, is_set, accepts_none);
    EXPECT_TRUE(test::isValidOptional(is_opt, is_set, accepts_none) ||
                test::isValidObligatory(is_opt, is_set, accepts_none));
}

TEST_F(TestOptionalObligatory, testSetDimensionLabels) {
    static const bool accepts_none = test::accepts_noneT<nix::SetDimension, test::labels>::value;
    is_opt   = std::conditional<std::is_class<decltype(set_dim.labels())>::value,
                                    std::integral_constant<bool, accepts_none>,
                                    std::integral_constant<bool, util::is_optional<decltype(set_dim.labels())>::value>
                                    >::type::value;
    is_set   = test::TtoBool(util::deRef(set_dim.labels()));
    summarize("SetDimension::labels", is_opt, is_set, accepts_none);
    EXPECT_TRUE(test::isValidOptional(is_opt, is_set, accepts_none) ||
                test::isValidObligatory(is_opt, is_set, accepts_none));
}

TEST_F(TestOptionalObligatory, testSampledDimensionIndex) {
    static const bool accepts_none = test::accepts_noneT<nix::SampledDimension, test::index>::value;
    is_opt   = std::conditional<std::is_class<decltype(sampled_dim.index())>::value,
                                    std::integral_constant<bool, accepts_none>,
                                    std::integral_constant<bool, util::is_optional<decltype(sampled_dim.index())>::value>
                                    >::type::value;
    is_set   = test::TtoBool(util::deRef(sampled_dim.index()));
    summarize("SampledDimension::index", is_opt, is_set, accepts_none);
    EXPECT_TRUE(test::isValidOptional(is_opt, is_set, accepts_none) ||
                test::isValidObligatory(is_opt, is_set, accepts_none));
}

TEST_F(TestOptionalObligatory, testSampledDimensionLabel) {
    static const bool accepts_none = test::accepts_noneT<nix::SampledDimension, test::label>::value;
    is_opt   = std::conditional<std::is_class<decltype(sampled_dim.label())>::value,
                                    std::integral_constant<bool, accepts_none>,
                                    std::integral_constant<bool, util::is_optional<decltype(sampled_dim.label())>::value>
                                    >::type::value;
    is_set   = test::TtoBool(util::deRef(sampled_dim.label()));
    summarize("SampledDimension::label", is_opt, is_set, accepts_none);
    EXPECT_TRUE(test::isValidOptional(is_opt, is_set, accepts_none) ||
                test::isValidObligatory(is_opt, is_set, accepts_none));
}

TEST_F(TestOptionalObligatory, testSampledDimensionOffset) {
    static const bool accepts_none = test::accepts_noneT<nix::SampledDimension, test::offset>::value;
    is_opt   = std::conditional<std::is_class<decltype(sampled_dim.offset())>::value,
                                    std::integral_constant<bool, accepts_none>,
                                    std::integral_constant<bool, util::is_optional<decltype(sampled_dim.offset())>::value>
                                    >::type::value;
    is_set   = test::TtoBool(util::deRef(sampled_dim.offset()));
    summarize("SampledDimension::offset", is_opt, is_set, accepts_none);
    EXPECT_TRUE(test::isValidOptional(is_opt, is_set, accepts_none) ||
                test::isValidObligatory(is_opt, is_set, accepts_none));
}

TEST_F(TestOptionalObligatory, testSampledDimensionSamplingInterval) {
    static const bool accepts_none = test::accepts_noneT<nix::SampledDimension, test::samplingInterval>::value;
    is_opt   = std::conditional<std::is_class<decltype(sampled_dim.samplingInterval())>::value,
                                    std::integral_constant<bool, accepts_none>,
                                    std::integral_constant<bool, util::is_optional<decltype(sampled_dim.samplingInterval())>::value>
                                    >::type::value;
    is_set   = test::TtoBool(util::deRef(sampled_dim.samplingInterval()));
    summarize("SampledDimension::samplingInterval", is_opt, is_set, accepts_none);
    EXPECT_TRUE(test::isValidOptional(is_opt, is_set, accepts_none) ||
                test::isValidObligatory(is_opt, is_set, accepts_none));
}

TEST_F(TestOptionalObligatory, testSampledDimensionUnit) {
    static const bool accepts_none = test::accepts_noneT<nix::SampledDimension, test::unit>::value;
    is_opt   = std::conditional<std::is_class<decltype(sampled_dim.unit())>::value,
                                    std::integral_constant<bool, accepts_none>,
                                    std::integral_constant<bool, util::is_optional<decltype(sampled_dim.unit())>::value>
                                    >::type::value;
    is_set   = test::TtoBool(util::deRef(sampled_dim.unit()));
    summarize("SampledDimension::unit", is_opt, is_set, accepts_none);
    EXPECT_TRUE(test::isValidOptional(is_opt, is_set, accepts_none) ||
                test::isValidObligatory(is_opt, is_set, accepts_none));
}

TEST_F(TestOptionalObligatory, testRangeDimensionIndex) {
    static const bool accepts_none = test::accepts_noneT<nix::RangeDimension, test::index>::value;
    is_opt   = std::conditional<std::is_class<decltype(range_dim.index())>::value,
                                    std::integral_constant<bool, accepts_none>,
                                    std::integral_constant<bool, util::is_optional<decltype(range_dim.index())>::value>
                                    >::type::value;
    is_set   = test::TtoBool(util::deRef(range_dim.index()));
    summarize("RangeDimension::index", is_opt, is_set, accepts_none);
    EXPECT_TRUE(test::isValidOptional(is_opt, is_set, accepts_none) ||
                test::isValidObligatory(is_opt, is_set, accepts_none));
}

TEST_F(TestOptionalObligatory, testRangeDimensionLabel) {
    static const bool accepts_none = test::accepts_noneT<nix::RangeDimension, test::label>::value;
    is_opt   = std::conditional<std::is_class<decltype(range_dim.label())>::value,
                                std::integral_constant<bool, accepts_none>,
                                std::integral_constant<bool, util::is_optional<decltype(range_dim.label())>::value>
                                    >::type::value;
    is_set   = test::TtoBool(util::deRef(range_dim.label()));
    summarize("RangeDimension::label", is_opt, is_set, accepts_none);
    EXPECT_TRUE(test::isValidOptional(is_opt, is_set, accepts_none) ||
                test::isValidObligatory(is_opt, is_set, accepts_none));
}

TEST_F(TestOptionalObligatory, testRangeDimensionTicks) {
    static const bool accepts_none = test::accepts_noneT<nix::RangeDimension, test::ticks>::value;
    is_opt   = std::conditional<std::is_class<decltype(range_dim.ticks())>::value,
                                std::integral_constant<bool, accepts_none>,
                                std::integral_constant<bool, util::is_optional<decltype(range_dim.ticks())>::value>
                                    >::type::value;
    is_set   = test::TtoBool(util::deRef(range_dim.ticks()));
    summarize("RangeDimension::ticks", is_opt, is_set, accepts_none);
    EXPECT_TRUE(test::isValidOptional(is_opt, is_set, accepts_none) ||
                test::isValidObligatory(is_opt, is_set, accepts_none));
}

TEST_F(TestOptionalObligatory, testRangeDimensionUnit) {
    static const bool accepts_none = test::accepts_noneT<nix::RangeDimension, test::unit>::value;
    is_opt   = std::conditional<std::is_class<decltype(range_dim.unit())>::value,
                                std::integral_constant<bool, accepts_none>,
                                std::integral_constant<bool, util::is_optional<decltype(range_dim.unit())>::value>
                                    >::type::value;
    is_set   = test::TtoBool(util::deRef(range_dim.unit()));
    summarize("RangeDimension::unit", is_opt, is_set, accepts_none);
    EXPECT_TRUE(test::isValidOptional(is_opt, is_set, accepts_none) ||
                test::isValidObligatory(is_opt, is_set, accepts_none));
}

TEST_F(TestOptionalObligatory, testPropertyUnit) {
    static const bool accepts_none = test::accepts_noneT<nix::Property, test::unit>::value;
    is_opt   = std::conditional<std::is_class<decltype(property.unit())>::value,
                                std::integral_constant<bool, accepts_none>,
                                std::integral_constant<bool, util::is_optional<decltype(property.unit())>::value>
                                    >::type::value;
    is_set   = test::TtoBool(util::deRef(property.unit()));
    summarize("Property::unit", is_opt, is_set, accepts_none);
    EXPECT_TRUE(test::isValidOptional(is_opt, is_set, accepts_none) ||
                test::isValidObligatory(is_opt, is_set, accepts_none));
}

TEST_F(TestOptionalObligatory, testPropertyUncertainty) {
    static const bool accepts_none = test::accepts_noneT<nix::Property, test::uncertainty>::value;
    is_opt   = std::conditional<std::is_class<decltype(property.uncertainty())>::value,
                                std::integral_constant<bool, accepts_none>,
                                std::integral_constant<bool, util::is_optional<decltype(property.uncertainty())>::value>
                                    >::type::value;
    is_set   = test::TtoBool(util::deRef(property.uncertainty()));
    summarize("Property::uncertainty", is_opt, is_set, accepts_none);
    EXPECT_TRUE(test::isValidOptional(is_opt, is_set, accepts_none) ||
                test::isValidObligatory(is_opt, is_set, accepts_none));
}

TEST_F(TestOptionalObligatory, testPropertyValues) {
    static const bool accepts_none = test::accepts_noneT<nix::Property, test::values>::value;
    is_opt   = std::conditional<std::is_class<decltype(property.values())>::value,
                                std::integral_constant<bool, accepts_none>,
                                std::integral_constant<bool, util::is_optional<decltype(property.values())>::value>
                                    >::type::value;
    is_set   = test::TtoBool(property.values());
    summarize("Property::values", is_opt, is_set, accepts_none);
    EXPECT_TRUE(test::isValidOptional(is_opt, is_set, accepts_none) ||
                test::isValidObligatory(is_opt, is_set, accepts_none));
}

TEST_F(TestOptionalObligatory, testFeatureData) {
    static const bool accepts_none = test::accepts_noneT<nix::Feature, test::data>::value;
    is_opt   = std::conditional<std::is_class<decltype(feature.data())>::value,
                                std::integral_constant<bool, accepts_none>,
                                std::integral_constant<bool, util::is_optional<decltype(feature.data())>::value>
                                >::type::value;
    is_set   = test::TtoBool(util::deRef(feature.data()));
    summarize("Feature::data", is_opt, is_set, accepts_none);
    EXPECT_TRUE(test::isValidOptional(is_opt, is_set, accepts_none) ||
                test::isValidObligatory(is_opt, is_set, accepts_none));
}

TEST_F(TestOptionalObligatory, testFeatureLinkType) {
    static const bool accepts_none = test::accepts_noneT<nix::Feature, test::linkType>::value;
    is_opt   = std::conditional<std::is_class<decltype(feature.linkType())>::value,
                                std::integral_constant<bool, accepts_none>,
                                std::integral_constant<bool, util::is_optional<decltype(feature.linkType())>::value>
                                >::type::value;
    is_set   = test::TtoBool(util::deRef(feature.linkType()));
    summarize("Feature::linkType", is_opt, is_set, accepts_none);
    EXPECT_TRUE(test::isValidOptional(is_opt, is_set, accepts_none) ||
                test::isValidObligatory(is_opt, is_set, accepts_none));
}

TEST_F(TestOptionalObligatory, testSectionLink) {
    static const bool accepts_none = test::accepts_noneT<nix::Section, test::link>::value;
    is_opt   = std::conditional<std::is_class<decltype(section.link())>::value,
                                std::integral_constant<bool, accepts_none>,
                                std::integral_constant<bool, util::is_optional<decltype(section.link())>::value>
                                >::type::value;
    is_set   = test::TtoBool(util::deRef(section.link()));
    summarize("Section::link", is_opt, is_set, accepts_none);
    EXPECT_TRUE(test::isValidOptional(is_opt, is_set, accepts_none) ||
                test::isValidObligatory(is_opt, is_set, accepts_none));
}

TEST_F(TestOptionalObligatory, testSectionRepository) {
    static const bool accepts_none = test::accepts_noneT<nix::Section, test::repository>::value;
    is_opt   = std::conditional<std::is_class<decltype(section.repository())>::value,
                                std::integral_constant<bool, accepts_none>,
                                std::integral_constant<bool, util::is_optional<decltype(section.repository())>::value>
                                >::type::value;
    is_set   = test::TtoBool(util::deRef(section.repository()));
    summarize("Repository::repository", is_opt, is_set, accepts_none);
    EXPECT_TRUE(test::isValidOptional(is_opt, is_set, accepts_none) ||
                test::isValidObligatory(is_opt, is_set, accepts_none));
}

TEST_F(TestOptionalObligatory, testTagExtent) {
    static const bool accepts_none = test::accepts_noneT<nix::Tag, test::extent>::value;
    is_opt   = std::conditional<std::is_class<decltype(tag.extent())>::value,
                                std::integral_constant<bool, accepts_none>,
                                std::integral_constant<bool, util::is_optional<decltype(tag.extent())>::value>
                                >::type::value;
    is_set   = test::TtoBool(util::deRef(tag.extent()));
    summarize("Tag::extent", is_opt, is_set, accepts_none);
    EXPECT_TRUE(test::isValidOptional(is_opt, is_set, accepts_none) ||
                test::isValidObligatory(is_opt, is_set, accepts_none));
}

TEST_F(TestOptionalObligatory, testTagPosition) {
    static const bool accepts_none = test::accepts_noneT<nix::Tag, test::position>::value;
    is_opt   = std::conditional<std::is_class<decltype(tag.position())>::value,
                                std::integral_constant<bool, accepts_none>,
                                std::integral_constant<bool, util::is_optional<decltype(tag.position())>::value>
                                >::type::value;
    is_set   = test::TtoBool(util::deRef(tag.position()));
    summarize("Tag::position", is_opt, is_set, accepts_none);
    EXPECT_TRUE(test::isValidOptional(is_opt, is_set, accepts_none) ||
                test::isValidObligatory(is_opt, is_set, accepts_none));
}

TEST_F(TestOptionalObligatory, testTagUnits) {
    static const bool accepts_none = test::accepts_noneT<nix::Tag, test::units>::value;
    is_opt   = std::conditional<std::is_class<decltype(tag.units())>::value,
                                std::integral_constant<bool, accepts_none>,
                                std::integral_constant<bool, util::is_optional<decltype(tag.units())>::value>
                                >::type::value;
    is_set   = test::TtoBool(util::deRef(tag.units()));
    summarize("Tag::units", is_opt, is_set, accepts_none);
    EXPECT_TRUE(test::isValidOptional(is_opt, is_set, accepts_none) ||
                test::isValidObligatory(is_opt, is_set, accepts_none));
}

TEST_F(TestOptionalObligatory, testMultiTagExtents) {
    static const bool accepts_none = test::accepts_noneT<nix::MultiTag, test::extents>::value;
    is_opt   = std::conditional<std::is_class<decltype(multiTag.extents())>::value,
                                std::integral_constant<bool, accepts_none>,
                                std::integral_constant<bool, util::is_optional<decltype(multiTag.extents())>::value>
                                >::type::value;
    is_set   = test::TtoBool(util::deRef(multiTag.extents()));
    summarize("MultiTag::extents", is_opt, is_set, accepts_none);
    EXPECT_TRUE(test::isValidOptional(is_opt, is_set, accepts_none) ||
                test::isValidObligatory(is_opt, is_set, accepts_none));
}

TEST_F(TestOptionalObligatory, testMultiTagPositions) {
    static const bool accepts_none = test::accepts_noneT<nix::MultiTag, test::positions>::value;
    is_opt   = std::conditional<std::is_class<decltype(multiTag.positions())>::value,
                                std::integral_constant<bool, accepts_none>,
                                std::integral_constant<bool, util::is_optional<decltype(multiTag.positions())>::value>
                                >::type::value;
    is_set   = test::TtoBool(util::deRef(multiTag.positions()));
    summarize("MultiTag::positions", is_opt, is_set, accepts_none);
    EXPECT_TRUE(test::isValidOptional(is_opt, is_set, accepts_none) ||
                test::isValidObligatory(is_opt, is_set, accepts_none));
}
