// Copyright (c) 2013, German Neuroinformatics Node (G-Node)
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted under the terms of the BSD License. See
// LICENSE file in the root of the Project.

#ifndef NIX_GROUP_H
#define NIX_GROUP_H

#include <string>

#include <nix/hdf5/hdf5include.hpp>
#include <nix/hdf5/DataSet.hpp>

#include <nix/Hydra.hpp>
#include <nix/hdf5/DataSpace.hpp>

namespace nix {
namespace hdf5 {


/**
 * TODO documentation
 */
class Group {

private:

    H5::Group h5group;

public:

    Group();

    Group(H5::Group h5group);

    Group(const Group &group);

    Group& operator=(const Group &group);

    bool hasAttr(const std::string &name) const;
    void removeAttr(const std::string &name) const;

    template <typename T>
    void setAttr(const std::string &name, const T &value) const;

    template <typename T>
    bool getAttr(const std::string &name, T &value) const;

    bool hasObject(const std::string &path) const;
    size_t objectCount() const;
    std::string objectName(size_t index) const;

    bool hasData(const std::string &name) const;
    DataSet openData(const std::string &name) const;
    void removeData(const std::string &name);

    template<typename T>
    void setData(const std::string &name, const T &value);
    template<typename T>
    bool getData(const std::string &name, T &value) const;

    bool hasGroup(const std::string &name) const;
    Group openGroup(const std::string &name, bool create = true) const;
    void removeGroup(const std::string &name);
    void renameGroup(const std::string &old_name, const std::string &new_name);

    bool operator==(const Group &group) const;
    bool operator!=(const Group &group) const;

    H5::Group h5Group() const;
    virtual ~Group();

private:
    static void readAttr(const H5::Attribute &attr, H5::DataType memType, const NDSize &size, void *data);
    static void readAttr(const H5::Attribute &attr, H5::DataType memType, const NDSize &size, std::string *data);

    static void writeAttr(const H5::Attribute &attr, H5::DataType memType, const NDSize &size, const void *data);
    static void writeAttr(const H5::Attribute &attr, H5::DataType memType, const NDSize &size, const std::string *data);
}; // group Group

//template functions

template<typename T> void Group::setAttr(const std::string &name, const T &value) const
{
    typedef Hydra<const T> hydra_t;
    typedef typename hydra_t::reader_t reader_t;

    const hydra_t hydra(value);
    DataType dtype = hydra.element_data_type();
    NDSize shape = hydra.shape();

    H5::Attribute attr;

    if (hasAttr(name)) {
        attr = h5group.openAttribute(name);
    } else {
        H5::DataType fileType = data_type_to_h5_filetype(dtype);
        H5::DataSpace fileSpace = DataSpace::create(shape, false);
        attr = h5group.createAttribute(name, fileType, fileSpace);
    }

    reader_t reader = hydra.reader();
    //attr.write(data_type_to_h5_memtype(dtype), reader.begin());
    writeAttr(attr, data_type_to_h5_memtype(dtype), shape, reader.begin());
    reader.finish();
}



template<typename T> bool Group::getAttr(const std::string &name, T &value) const
{
    typedef Hydra<T> hydra_t;
    typedef typename hydra_t::writer_t writer_t;

    if (!hasAttr(name)) {
        return false;
    }

    hydra_t hydra(value);

    //determine attr's size and resize value accordingly
    H5::Attribute attr = h5group.openAttribute(name);
    H5::DataSpace space = attr.getSpace();
    int rank = space.getSimpleExtentNdims();
    NDSize dims(static_cast<size_t>(rank));
    space.getSimpleExtentDims (dims.data(), nullptr);
    hydra.resize(dims);

    writer_t writer = hydra.writer();
    DataType dtype = hydra.element_data_type();
    H5::DataType memType = data_type_to_h5_memtype(dtype);
    //attr.read(data_type_to_h5_memtype(dtype), writer.begin());
    readAttr(attr, memType, dims, writer.begin());
    writer.finish();

    return true;
}

template<typename T>
void Group::setData(const std::string &name, const T &value)
{
    typedef Hydra<const T> hydra_t;
    typedef typename hydra_t::reader_t reader_t;

    DataSet ds;

    const hydra_t hydra(value);
    DataType dtype = hydra.element_data_type();
    NDSize shape = hydra.shape();

    if (!hasData(name)) {
        NDSize maxsize(shape.size(), H5S_UNLIMITED);
        NDSize chunks(shape.size(), 1);
        ds = DataSet::create(h5group, name, dtype, shape, &maxsize, &chunks);
    } else {
        ds = openData(name);
        ds.extend(shape); //FIXME: this should be ds.set_extend, for i.e. shrinking
    }

    reader_t reader = hydra.reader();

    ds.set(dtype, shape, reader.begin());
    reader.finish();
}

template<typename T>
bool Group::getData(const std::string &name, T &value) const
{
    typedef Hydra<T> hydra_t;
    typedef typename hydra_t::writer_t writer_t;

    if (!hasData(name)) {
        return false;
    }

    DataSet ds = openData(name);

    hydra_t hydra(value);

    writer_t writer = hydra.writer();
    DataType dtype = hydra.element_data_type();
    NDSize shape = ds.size();

    hydra.resize(shape);
    ds.get(dtype, shape, writer.begin());
    writer.finish();

    return true;
}

} // namespace hdf5
} // namespace nix


#endif /* NIX_GROUP_H */
