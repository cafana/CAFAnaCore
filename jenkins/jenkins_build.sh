#!/bin/bash

set +ex
env

if [[ $QUALIFIER == *e19* ]]
then
    # DUNE lblpwgtools versions
    source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh || exit 1
    setup root v6_18_04d -q ${QUALIFIER}:py2 || exit 1
    setup stan_math v2_18_0 -q $QUALIFIER || exit 1
    setup eigen v3_3_5 || exit 1
    setup boost v1_70_0 -q $QUALIFIER || exit 1
else
    # NOvA versions
    source /cvmfs/nova.opensciencegrid.org/externals/setup || exit 1
    setup root v6_16_00 -q $QUALIFIER || exit 1
    setup stan_math v2.18.0 -q $QUALIFIER || exit 1
    setup eigen v3.3.5 || exit 1
    setup boost v1_66_0a -q $QUALIFIER || exit 1
fi

setup cmake v3_14_3 || exit 1
setup ninja v1_8_2 || exit 1

export PRODUCTS=/cvmfs/nova.opensciencegrid.org/externals/:$PRODUCTS
setup osclib v00.03 -q ${QUALIFIER}:stan
setup srproxy v00.12 -q ${QUALIFIER}:py2

make clean
time make || exit 2
