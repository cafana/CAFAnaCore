#!/bin/bash

set +ex
env

if [[ $QUALIFIER == *c7* ]]
then
    # NOvA doesn't seem to have all the c7 products
    source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh || exit 1    
else
    source /cvmfs/nova.opensciencegrid.org/externals/setup || exit 1
fi

setup root v6_18_04d -q $QUALIFIER || exit 1
setup stan_math v4_0_1 -q $QUALIFIER || exit 1
setup eigen v3_3_9a || exit 1
setup boost v1_70_0 -q $QUALIFIER || exit 1
setup ifdhc v2_5_7 -q ${QUALIFIER}:p372 || exit 1

setup cmake v3_14_3 || exit 1
setup ninja v1_8_2 || exit 1

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
