#!/bin/bash

# Centralize all the logic about what versions to depend on in this one file

if [ $# != 1 ]
then
    echo Usage: dependencies QUALIFIER >&2
    exit 1
fi

QUAL=$1
WANTSTAN=x

if [[ $QUAL == *:n311* ]]; then NQUAL=n311; QUAL=${QUAL/:n311/}; fi
if [[ $QUAL == *:n319* ]]; then NQUAL=n319; QUAL=${QUAL/:n319/}; fi

case $QUAL in
  *:stan)
    WANTSTAN=yes
    QUAL=${QUAL/:stan/}
    ;;
  *:stanthread)
    WANTSTAN=yes
    QUAL=${QUAL/:stanthread/}
    ;;
  *:stanfree)
    WANTSTAN=no
    QUAL=${QUAL/:stanfree/}
    ;;
  *)
    echo Must specify either \*:stan or \*:stanfree or \*:stanthread in QUALIFIER option
    exit 1
    ;;
esac

if [[ $NQUAL == n311 ]]
then
    # These are the current (Jul 2022) sbn versions (nutools v3_12_03)
    echo root v6_22_08d -q${QUAL}:p392
    echo boost v1_75_0 -q$QUAL

    echo ifdhc v2_6_4 -q${QUAL}:p392

else
    # These are the current (April 2025) nova/dune versions (nutools v3_19_01)
    echo root v6_28_12 -q${QUAL}:p3915
    echo boost v1_82_0 -q$QUAL

    echo ifdhc v2_7_2 -q${QUAL}:p3915

fi

if [ $WANTSTAN == yes ]
then 
    echo stan_math v4_9_0a
    echo sundials v6_1_1
fi

echo eigen v23_08_01_66e8f
