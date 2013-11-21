// Copyright (c) 2013, German Neuroinformatics Node (G-Node)
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted under the terms of the BSD License. See
// LICENSE file in the root of the Project.

#include <nix/util/util.hpp>
#include <nix/hdf5/DataTagHDF5.hpp>
#include <nix/hdf5/RepresentationHDF5.hpp>

using namespace std;

namespace nix {
namespace hdf5 {


DataTagHDF5::DataTagHDF5(const DataTagHDF5 &tag)
    : EntityWithSourcesHDF5(tag.file(), tag.block(), tag.group(), tag.id()),
      reference_list(tag.reference_list)
{
    representation_group = tag.representation_group;
}


DataTagHDF5::DataTagHDF5(const File &file, const Block &block, const Group &group, const string &id)
    : EntityWithSourcesHDF5(file, block, group, id), reference_list(group, "references")
{
    representation_group = this->group().openGroup("representations");
}


DataTagHDF5::DataTagHDF5(const File &file, const Block &block, const Group &group,
                         const std::string &id, time_t time)
    : EntityWithSourcesHDF5(file, block, group, id, time), reference_list(group, "references")
{
    representation_group = this->group().openGroup("representations");
}


DataArray DataTagHDF5::positions() const {
    string id;
    group().getAttr("positions", id);

    if(id.length() > 0 && block().hasDataArray(id)){
        return block().getDataArray(id);
    }

    throw runtime_error("Unable to find DataArray with id " + id + "!");
}


void DataTagHDF5::positions(const DataArray &pos) {
    positions(pos.id());
}


void DataTagHDF5::positions(const string &id) {
    if(!block().hasDataArray(id)){
        throw runtime_error("DataTagHDF5::extents: cannot set Extent because referenced DataArray does not exist!");
    }
    else{
        if(hasExtents()){
            DataArray pos = block().getDataArray(id);
            DataArray ext = extents();
            if(!checkDimensions(ext,pos))
                throw runtime_error("DataTagHDF5::positions: cannot set Positions because dimensionality of extent and position data do not match!");
        }
        group().setAttr("positions", id);
        forceUpdatedAt();
    }
}


bool DataTagHDF5::hasPositions() const{
    std::string posId;
    group().getAttr("positions", posId);
    return (posId.length() > 0);
}


DataArray DataTagHDF5::extents() const {
    std::string extId;
    group().getAttr("extents", extId);
    if(extId.length() > 0 && block().hasDataArray(extId)){
        return block().getDataArray(extId);
    }
    throw runtime_error("Unable to find DataArray with id " + extId + "!");
}


void DataTagHDF5::extents(const DataArray &extent) {
    extents(extent.id());
}


void DataTagHDF5::extents(const string &extentsId) {
    if(!block().hasDataArray(extentsId)){
        throw runtime_error("DataTagHDF5::extents: cannot set Extent because referenced DataArray does not exist!");
    }
    else{
        if(hasPositions()){
            DataArray ext = block().getDataArray(extentsId);
            DataArray pos = positions();
            if(!checkDimensions(ext,pos))
                throw runtime_error("DataTagHDF5::extents: cannot set Extent because dimensionality of extent and position data do not match!");
        }
        group().setAttr("extents", extentsId);
        forceUpdatedAt();
    }
}


bool DataTagHDF5::hasExtents() const{
    std::string extId;
    group().getAttr("extents", extId);
    return (extId.length() > 0);
}

// Methods concerning references.

bool DataTagHDF5::hasReference(const DataArray &reference) const {
    return hasReference(reference.id());
}


bool DataTagHDF5::hasReference(const std::string &id) const {
    return reference_list.has(id);
}


size_t DataTagHDF5::referenceCount() const {
    return reference_list.count();
}


DataArray DataTagHDF5::getReference(const std::string &id) const {
    if (hasReference(id)) {
        return block().getDataArray(id);
    } else {
        throw runtime_error("No reference with id: " + id);
    }
}


void DataTagHDF5::addReference(const DataArray &reference) {
    reference_list.add(reference.id());
}


bool DataTagHDF5::removeReference(const DataArray &reference) {
    return reference_list.remove(reference.id());
}


std::vector<DataArray> DataTagHDF5::references() const {
    vector<string> refs = reference_list.get();
    vector<DataArray> data_obj;

    for (size_t i = 0; i < refs.size(); i++) {
        if (block().hasDataArray(refs[i])) {
            data_obj.push_back(block().getDataArray(refs[i]));
        } else {
            // TODO ???
        }
    }
    return data_obj;
}


void DataTagHDF5::references(const std::vector<DataArray> &references) {
    vector<string> ids(references.size());

    for (size_t i = 0; i < references.size(); i++) {
        ids[i] = references[i].id();
    }

    reference_list.set(ids);
}

// Methods concerning representations.

bool DataTagHDF5::hasRepresentation(const string &id) const{
    return representation_group.hasGroup(id);
}


size_t DataTagHDF5::representationCount() const{
    return representation_group.objectCount();
}


Representation DataTagHDF5::getRepresentation(const std::string &id) const  {
    Group group = representation_group.openGroup(id, false);
    shared_ptr<RepresentationHDF5> tmp(new RepresentationHDF5(file(), block(), group, id));

    return Representation(tmp);
}


Representation DataTagHDF5::getRepresentation(size_t index) const{
    string id = representation_group.objectName(index);
    Group group = representation_group.openGroup(id, false);
    shared_ptr<RepresentationHDF5> tmp(new RepresentationHDF5(file(), block(), group, id));

    return Representation(tmp);
}


std::vector<Representation> DataTagHDF5::representations() const{
    vector<Representation>  representation_obj;
    size_t count = representation_group.objectCount();
    for (size_t i = 0; i < count; i++) {
        string id = representation_group.objectName(i);
        Group group = representation_group.openGroup(id, false);
        shared_ptr<RepresentationHDF5> tmp(new RepresentationHDF5(file(), block(), group, id));

        representation_obj.push_back(Representation(tmp));
    }
    return representation_obj;
}


Representation DataTagHDF5::createRepresentation(DataArray data, LinkType link_type) {
    string id = util::createId("representation");
    while(representation_group.hasObject(id))
        id = util::createId("representation");

    Group group = representation_group.openGroup(id, true);
    shared_ptr<RepresentationHDF5> tmp(new RepresentationHDF5(file(), block(), group, id));
    tmp->linkType(link_type);
    tmp->data(data);

    return Representation(tmp);
}


bool DataTagHDF5::removeRepresentation(const string &id){
    if (representation_group.hasGroup(id)) {
        representation_group.removeGroup(id);
        return true;
    } else {
        return false;
    }
}

// Other methods and functions


void DataTagHDF5::swap(DataTagHDF5 &other) {
    using std::swap;

    EntityWithSourcesHDF5::swap(other);
    swap(representation_group, other.representation_group);
    swap(reference_list, other.reference_list);
}


DataTagHDF5& DataTagHDF5::operator=(const DataTagHDF5 &other) {
    if (*this != other) {
        DataTagHDF5 tmp(other);
        swap(tmp);
    }
    return *this;
}


ostream& operator<<(ostream &out, const DataTagHDF5 &ent) {
    out << "DataTag: {name = " << ent.name();
    out << ", type = " << ent.type();
    out << ", id = " << ent.id() << "}";
    return out;
}


bool DataTagHDF5::checkDimensions(const DataArray &a, const DataArray &b)const{
    bool valid = true;
    boost::multi_array<double,1> aData, bData;
    a.getRawData(aData);
    b.getRawData(bData);
    valid = aData.num_dimensions() == bData.num_dimensions();
    if(!valid)
        return valid;

    boost::multi_array<double,1>::size_type dims = aData.num_dimensions();
    for(boost::multi_array<double,1>::size_type i = 0; i < *aData.shape(); i++){
        valid = (aData.shape()[i] != bData.shape()[i]);
        if(!valid)
            return valid;
    }
    return valid;
}


bool DataTagHDF5::checkPositionsAndExtents() const{
    bool valid = true;
    if(hasPositions() && hasExtents()){
        DataArray pos = positions();
        DataArray ext = extents();
        boost::multi_array<double,1> posData, extData;
        pos.getRawData(posData);
        ext.getRawData(extData);
        return checkDimensions(pos, ext);
    }
    return valid;
}


DataTagHDF5::~DataTagHDF5() {}

} // namespace hdf5
} // namespace nix