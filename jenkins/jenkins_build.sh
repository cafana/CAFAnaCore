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

make clean # don't trust my build system
time make -j || exit 2
