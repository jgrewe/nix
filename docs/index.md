<!-- ![Nix_logo](./nix_logo.png "NIX") -->

C++ library for storing scientific data in the NIX data model.

<!-- The *NIX* project started as an initiative within the -->
<!-- Electrophysiology Task Force a part of the -->
<!-- [INCF](http://www.incf.org/) Data sharing Program.   -->

The *NIX* data model allows to store fully annotated scientific
datasets, i.e. the data together with its metadata within the same
container. Our aim is to achieve standardization by providing a
common/generic data structure for a multitude of data
types.
[See the wiki for more information](https://github.com/G-Node/nix/wiki)

NIX originates from neurosciences but the generic nature of the
underlying data model makes it applicable to a much wider range. In
fact we would be happy to know of datasets that cannot be represented
in NIX.

The current implementations store the actual data using
the [HDF5](http://www.hdfgroup.org/) file format as a storage backend.


## The NIX ecosystem

### APIs

- [*nix* c++ library](https://github.com/g-node/nix "C++ api for nix files"). C++ library for reading and writing NIX files.

- [*nixpy*](https://github.com/g-node/nixpy "Python library either as bindings or using h5py")

**Language bindings**

We provide bindings for:

- [Matlab: *nix-mx*](https://github.com/g-node/nix-mx "Matlab language bindings, requires the C++ library")
- [Java: *nix-java*](https://github.com/g-node/nix-java "Java language bindings, requires the C++ library")

### Tools

- Viewer for *NIX* data files [nixView](https://github.com/bendalab/nixview "NixView - viewer for nix files")

- [relacs](https://relacs.net "Relacs - enjoy your recordings") is a software platform for closed-loop data acquisition, online analysis, and stimulus generation specifically designed for (but not limited to) electrophysiological recordings. Writes out NIX files if the nix c++-library is installed.

- [IO class](https://github.com/python-neo-nixio) for the [NEO](http://neuralensemble.org/neo/) data model for spike train data.

# Tutorial and examples

- We have assembled a set of
 [tutorials](http://g-node.github.io/nixpy/tutorial.html "Python Tutorial") and
 [demos](https://github.com/g-node/nix-demo "Jupiter notebooks demonstrating nix for various use-cases") using the python language
 bindings.


# Installation

Platform specific installation instructions can be found:

- [Linux](./install_linux.md)
- [macOS](./install_mac.md)
- [Windows](./install_win.md)


<!-- ## NIX API Documentation -->

<!-- The API documentation for the C++ library can be found [here](http://g-node.github.io/nix/) -->

# Citation

If you use NIX, it would be much appreciated if you would cite it in publications:

*Stoewer A., Kellner C. J., Sobolev A., Sonntag M., Benda J., Wachtler T., and Grewe J. (2015); Integrating data storage and annotation in the data workflow using the NIX format and libraries; Frontiers in Neuroscience 9, DOI10.3389/conf.fnins.2015.91.00050*

# Contributing

Any kind of contribution is welcome! This includes reporting bugs and issues. If you want to contribute to code or documentation please refer to the [contributing guide](https://github.com/G-Node/nix/blob/master/CONTRIBUTING.md).

# Support

If experience problems using NIX feel free to join our IRC channel
#gnode on FreeNode, write an email to <dev@gnode.org>. If you find a
bug, please report it using
the [github issue tracker](https://github.com/.g-node/nix/issues).


# License
This project is open source published under the BSD-3 for [details](https://github.com/G-Node/nix/blob/master/LICENSE) for details.


[![Build Status](https://travis-ci.org/G-Node/nix.svg?branch=master)](https://travis-ci.org/G-Node/nix)
[![Build status](https://ci.appveyor.com/api/projects/status/cdupf2np8ffg5hjt/branch/master?svg=true)](https://ci.appveyor.com/project/stoewer/nix/branch/master)
[![Coverage Status](https://coveralls.io/repos/G-Node/nix/badge.svg?branch=master)](https://coveralls.io/r/G-Node/nix?branch=master)