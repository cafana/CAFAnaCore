#!/bin/bash

set +ex
env

if [[ $QUALIFIER != *:n311* && $QUALIFIER != *:n313* ]]
then
    echo Unspecified nutools version in qualifier $QUALIFIER -- must be n311 or n313
    exit 1
fi

if [[ $QUALIFIER != *e20* && $QUALIFIER != *c7* ]]
then
    echo Unknown compiler in qualifier $QUALIFIER -- must be e20, or c7
    exit 1
fi

if [[ $QUALIFIER != *debug* && $QUALIFIER != *prof* ]]
then
    echo Unknown optimization level in qualifier $QUALIFIER -- must be debug or prof
    exit 1
fi

if [[ $EXPERIMENT == *n313* ]]
then
    source /cvmfs/nova.opensciencegrid.org/externals/setup || exit 1
else
    source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh || exit 1
fi

# Looping over lines is a total pain in bash. Easier to just send it to a file
TMPFILE=`mktemp`
# Expect to be run in the directory one above....
jenkins/dependencies.sh $QUALIFIER | sed 's/^/setup /' > $TMPFILE
cat $TMPFILE
source $TMPFILE

setup cmake v3_14_3 || exit 1
setup ninja v1_10_0 || exit 1

make clean

FLAGS=''

if [[ $QUALIFIER == *prof* ]]
then
    FLAGS=$FLAGS' CMAKE_BUILD_TYPE=Release' || exit 2
else
    FLAGS=$FLAGS' CMAKE_BUILD_TYPE=Debug' || exit 2
fi

if [[ $QUALIFIER == *c7* ]]
then
    FLAGS=$FLAGS' CMAKE_CXX_COMPILER=clang++' || exit 2
else
    FLAGS=$FLAGS' CMAKE_CXX_COMPILER=g++' || exit 2
fi

time make $FLAGS || exit 2

mkdir -p CAFAna/ups
jenkins/make_table.sh > CAFAna/ups/cafanacore.table
