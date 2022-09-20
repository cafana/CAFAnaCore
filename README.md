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

## How to build and and test your changes locally

- `export QUALIFIER=e19:prof` or `e17:debug`, etc
- `jenkins/jenkins_build.sh` (or paste parts of it into your terminal)
- set `CAFANACORE_LIB` and `CAFANACORE_INC` manually to point to what you just built
- rebuild your test release
- `export CAFANA_DISABLE_VERSION_CHECK=1`
- Profit!

## How to build with jenkins

```
git push
git tag $NEW_TAG_NUMBER
git push --tags
```

- Navigate to https://buildmaster.fnal.gov/buildmaster/view/Nova/job/external/job/cafanacore_build/ and click "Build Now".
- Wait

## How to deploy

```
wget https://buildmaster.fnal.gov/buildmaster/view/Nova/job/external/job/cafanacore_collect/lastSuccessfulBuild/artifact/*zip*/archive.zip
unzip archive.zip
mv archive/* .
rm archive.zip

ssh cvmfs${EXPERIMENT}@oasiscfs.fnal.gov
cvmfs_server transaction ${EXPERIMENT}.opensciencegrid.org
# fetch the files you extracted previously to the correct /cvmfs directory
cd /cvmfs/ ... /cafanacore/$NEW_TAG_NUMBER/include
ln -s . CAFAnaCore # can't be done earlier because neither jenkins or scp like symlinks
ln -s CAFAna/Core/ cafanacore
cd -
cvmfs_server publish ${EXPERIMENT}.opensciencegrid.org
```

### For NOvA

Update `CAFAna/Core/VersionCheck.cxx`, `setup/nova-offline-ups-externals-development`, and `nova-offline-ups-externals-development-prof`. Notify `#cmake`

### For DUNE

Update `cmake/ups_env_setup.sh`

## Usage
Coming soon!
