// Copyright (c) 2014, German Neuroinformatics Node (G-Node)
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted under the terms of the BSD License. See
// LICENSE file in the root of the Project.

#include <nix/valid/helper.hpp>

#include <nix.hpp>
#include <nix/util/dataAccess.hpp>


namespace nix {
namespace valid {

std::vector<std::string> getDimensionsUnits(DataArray darray) {
    std::vector<std::string> units;
    
    for (auto &dim : darray.dimensions()) {
        units.push_back(util::getDimensionUnit(dim));
    }

    return units;
}

} // namespace valid
} // namespace nix
