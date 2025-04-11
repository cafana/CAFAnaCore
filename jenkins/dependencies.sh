#!/bin/bash

# Centralize all the logic about what versions to depend on in this one file

if [ $# != 1 ]
then
    echo Usage: dependencies QUALIFIER >&2
    exit 1
fi

QUAL=$1

if [[ $QUAL == *:n311* ]]; then NQUAL=n311; QUAL=${QUAL/:n311/}; fi
if [[ $QUAL == *:n319* ]]; then NQUAL=n319; QUAL=${QUAL/:n319/}; fi

if [[ $NQUAL == n311 ]]
then
    # These are the current (October 2021) sbn versions (nutools v3_11_05)
    echo root v6_22_08d -q${QUAL}:p392
    echo boost v1_75_0 -q$QUAL

    echo ifdhc v2_6_4 -q${QUAL}:p392

else
    # These are the current (April 2025) nova versions (nutools v3_19_01)
    echo root v6_28_12 -q${QUAL}:p3915
    echo boost v1_82_0 -q$QUAL

    echo ifdhc v2_7_2 -q${QUAL}:p3915

fi

echo stan_math v4_9_0a
echo sundials v7_1_1
echo eigen v23_08_01_66e8f
