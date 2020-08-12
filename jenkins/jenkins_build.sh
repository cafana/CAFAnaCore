#!/bin/bash

set +ex
env

if [[ $QUALIFIER == *e19* ]]
then
    # DUNE lblpwgtools versions
    source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh || exit 1
    setup root v6_18_04d -q ${QUALIFIER}:py2 || exit 1
else
    # NOvA versions
    source /cvmfs/nova.opensciencegrid.org/externals/setup || exit 1
    setup root v6_16_00 -q $QUALIFIER || exit 1
fi

setup cmake v3_14_3
setup ninja v1_8_2

export PRODUCTS=/cvmfs/nova.opensciencegrid.org/externals/:$PRODUCTS
setup osclib v00.03 -q ${QUALIFIER}:stan
setup srproxy v00.12 -q ${QUALIFIER}:py2

make clean
time make || exit 2
