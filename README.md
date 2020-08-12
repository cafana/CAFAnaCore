### CAFAnaCore -- Base libraries of the CAFAna analysis framework

----------------------------------------------------------------------

  *Package author*:               C. Backhouse <c.backhouse@ucl.ac.uk>

  *Build system*:                 J. Wolcott <jwolcott@fnal.gov>

----------------------------------------------------------------------

This document briefly describes the build process of the CAFAna framework contained in this package.

## Prerequisites
* A compiler that understands C++14 (tested with gcc)
* ROOT v6.00.00 or above

## Build

The package should be built using CMake.

CMake builds are 'out-of-source' --- that is, they use a dedicated build directory that you can put anywhere.  In this example, we'll use a build directory that is a subdirectory of the source root (that is, the directory containing this file).

By default, the generated libraries and test executables are installed back into the source tree at `lib/` and `bin/`, respecively.  If you'd like to put them somewhere else, pass `-DCMAKE_INSTALL_PREFIX=/path/to/dir` (where `/path/to/dir` is where you'd like them to be installed) in the `cmake` command below.

```shell script
$ mkdir build
$ cd build
$ cmake ..
$ make install
```

## Usage
Coming soon!
