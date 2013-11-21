// Copyright (c) 2013, German Neuroinformatics Node (G-Node)
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted under the terms of the BSD License. See
// LICENSE file in the root of the Project.

#include <stdexcept>

#include <nix/hdf5/DataType.hpp>

namespace nix {
namespace hdf5 {


H5::DataType data_type_to_h5_filetype(DataType dtype) {
    switch (dtype) {

        case DataType::Int8:   return H5::PredType::STD_I8LE;
        case DataType::Int16:  return H5::PredType::STD_I16LE;
        case DataType::Int32:  return H5::PredType::STD_I32LE;
        case DataType::Int64:  return H5::PredType::STD_I64LE;
        case DataType::UInt8:  return H5::PredType::STD_U8LE;
        case DataType::UInt16: return H5::PredType::STD_U16LE;
        case DataType::UInt32: return H5::PredType::STD_U32LE;
        case DataType::UInt64: return H5::PredType::STD_U64LE;
        case DataType::Float:  return H5::PredType::IEEE_F32LE;
        case DataType::Double: return H5::PredType::IEEE_F64LE;

        default:
            throw std::invalid_argument("Unkown DataType"); //FIXME
    }

    return H5::DataType();
}


H5::DataType data_type_to_h5_memtype(DataType dtype) {
    switch(dtype) {

        case DataType::Int8:   return H5::PredType::NATIVE_INT8;
        case DataType::Int16:  return H5::PredType::NATIVE_INT16;
        case DataType::Int32:  return H5::PredType::NATIVE_INT32;
        case DataType::Int64:  return H5::PredType::NATIVE_INT64;
        case DataType::UInt8:  return H5::PredType::NATIVE_UINT8;
        case DataType::UInt16: return H5::PredType::NATIVE_UINT16;
        case DataType::UInt32: return H5::PredType::NATIVE_UINT32;
        case DataType::UInt64: return H5::PredType::NATIVE_UINT64;
        case DataType::Float:  return H5::PredType::NATIVE_FLOAT;
        case DataType::Double: return H5::PredType::NATIVE_DOUBLE;

        default:
            throw std::invalid_argument("Unkown DataType"); //FIXME
    }

    return H5::DataType();
}


size_t data_type_to_size(DataType dtype) {
    switch(dtype) {

        case DataType::Int8:
        case DataType::UInt8:
            return 1;

        case DataType::Int16:
        case DataType::UInt16:
            return 2;

        case DataType::Int32:
        case DataType::UInt32:
        case DataType::Float:
            return 4;

        case DataType::Int64:
        case DataType::UInt64:
        case DataType::Double:
            return 8;

        default:
            throw std::invalid_argument("Unkown DataType");
    }
}

} // namespace hdf4
} // namespace nix