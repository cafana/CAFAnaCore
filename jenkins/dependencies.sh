#!/bin/bash

# Centralize all the logic about what versions to depend on in this one file

if [ $# != 1 ]
then
    echo Usage: dependencies QUALIFIER >&2
    exit 1
fi

QUAL=$1

if [[ $QUAL == *:n311* ]]; then NQUAL=n311; QUAL=${QUAL/:n311/}; fi
if [[ $QUAL == *:n313* ]]; then NQUAL=n312; QUAL=${QUAL/:n313/}; fi

if [[ $NQUAL == n311 ]]
then
    # These are the current (October 2021) sbn versions (nutools v3_11_05)
    echo root v6_22_08d -q${QUAL}:p392
    echo boost v1_75_0 -q$QUAL

    echo ifdhc v2_6_4 -q${QUAL}:p392

else
    # These are the current (February 2023) nova versions (nutools v3_13_03)
    echo root v6_22_08d -q${QUAL}:p392
    echo boost v1_75_0 -q$QUAL

    echo ifdhc v2_6_11 -q${QUAL}:p392
fi

echo stan_math v4_2_1 -q$QUAL
echo eigen v3_4_0
