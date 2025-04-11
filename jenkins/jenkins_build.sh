#!/bin/bash

set +ex
env

if [[ $QUALIFIER != *:n319* ]]
then
    echo Unspecified nutools version in qualifier $QUALIFIER -- must be n319
    exit 1
fi

if [[ $QUALIFIER != *e26* && $QUALIFIER != *c14* ]]
then
    echo Unknown compiler in qualifier $QUALIFIER -- must be e26, or c14
    exit 1
fi

if [[ $QUALIFIER != *debug* && $QUALIFIER != *prof* ]]
then
    echo Unknown optimization level in qualifier $QUALIFIER -- must be debug or prof
    exit 1
fi

source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh || exit 1

# Looping over lines is a total pain in bash. Easier to just send it to a file
TMPFILE=`mktemp`
# Expect to be run in the directory one above....
jenkins/dependencies.sh $QUALIFIER | sed 's/^/setup /' > $TMPFILE
cat $TMPFILE
source $TMPFILE

setup cmake v3_27_4 || exit 1
setup ninja v1_11_1 || exit 1

make clean

FLAGS=''

if [[ $QUALIFIER == *prof* ]]
then
    FLAGS=$FLAGS' CMAKE_BUILD_TYPE=Release' || exit 2
else
    FLAGS=$FLAGS' CMAKE_BUILD_TYPE=Debug' || exit 2
fi

if [[ $QUALIFIER == *c14* ]]
then
    FLAGS=$FLAGS' CMAKE_CXX_COMPILER=clang++' || exit 2
else
    FLAGS=$FLAGS' CMAKE_CXX_COMPILER=g++' || exit 2
fi

time make $FLAGS || exit 2

mkdir -p CAFAna/ups
jenkins/make_table.sh > CAFAna/ups/cafanacore.table
