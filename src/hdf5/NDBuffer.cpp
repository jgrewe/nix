// Copyright (c) 2013, German Neuroinformatics Node (G-Node)
// Copyright (c) 2013, Christian Kellner <kellner@bio.lmu.de>
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted under the terms of the BSD License. See
// LICENSE file in the root of the Project.

#include <nix/hdf5/NDBuffer.hpp>

namespace nix {
namespace hdf5 {


NDBuffer::NDBuffer(DataType dtype, PSize dims) : dataType(dtype), extends(dims) {
    allocate_space();
}


void NDBuffer::allocate_space() {
    size_t type_size = data_type_to_size(dataType);
    dstore.resize(extends.nelms() * type_size);

    calc_strides();
}


void NDBuffer::resize(const PSize &new_size) {
    extends = new_size;
    allocate_space();
}


void NDBuffer::calc_strides() {
    size_t _rank = rank();

    strides = PSize(_rank, 1);

    for (size_t i = 1; i < _rank; i++) {
        size_t lst = _rank - i;
        size_t cur = lst - 1;
        strides[cur] = strides[lst] * extends[lst];
    }
}


size_t NDBuffer::sub2index(const PSize &sub) const {
    size_t index = strides.dot(sub);
    return index;
}


} // namespace hdf5
} // namespace nix