#!/bin/bash

set +ex
env

source /cvmfs/nova.opensciencegrid.org/externals/setup || exit 1
# Alternate source of products
# source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh || exit 1

if [[ $QUALIFIER == *e19* ]]
then
    setup root v6_18_04d -q $QUALIFIER || exit 1
    setup stan_math v2_18_0 -q $QUALIFIER || exit 1
    setup eigen v3_3_5 || exit 1
    setup boost v1_70_0 -q $QUALIFIER || exit 1
    setup ifdhc v2_5_7 -q ${QUALIFIER}:p372 || exit 1
else
    setup root v6_16_00 -q $QUALIFIER || exit 1
    setup stan_math v2.18.0 -q $QUALIFIER || exit 1
    setup eigen v3.3.5 || exit 1
    setup boost v1_66_0a -q $QUALIFIER || exit 1
    setup ifdhc v2_5_6 -q ${QUALIFIER}:p2715a || exit 1
fi

setup cmake v3_14_3 || exit 1
setup ninja v1_8_2 || exit 1

make clean

if [[ $QUALIFIER == *prof* ]]
then
    time make CMAKE_BUILD_TYPE=Release || exit 2
else
    time make CMAKE_BUILD_TYPE=Debug || exit 2
fi
