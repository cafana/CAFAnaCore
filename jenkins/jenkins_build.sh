#!/bin/bash

set +ex
env

if [[ $QUALIFIER == *e19* || $QUALIFIER == *c7* ]]
then
    # NOvA doesn't seem to have all the c7 products
    source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh || exit 1
else
    source /cvmfs/nova.opensciencegrid.org/externals/setup || exit 1
fi

if [[ $QUALIFIER == *e19* || $QUALIFIER == *c7* ]]
then
    setup root v6_22_06a -q ${QUALIFIER}:p383b || exit 1
    setup stan_math v2_18_0 -q $QUALIFIER || exit 1
    setup eigen v3_3_9a || exit 1
    setup boost v1_73_0 -q $QUALIFIER || exit 1
    setup ifdhc v2_5_7 -q ${QUALIFIER}:p372 || exit 1
else
    # e17
    setup root v6_16_00 -q $QUALIFIER || exit 1
    setup stan_math v2.18.0 -q $QUALIFIER || exit 1
    setup eigen v3.3.5 || exit 1
    setup boost v1_66_0a -q $QUALIFIER || exit 1
    setup ifdhc v2_5_6 -q ${QUALIFIER}:p2715a || exit 1
fi

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
