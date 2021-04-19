#!/bin/bash

# Centralize all the logic about what versions to depend on in this one file

if [ $# != 1 ]
then
    echo Usage: dependencies QUALIFIER >&2
    exit 1
fi

QUAL=$1

if [ $QUAL == *:n308* ]; then NQUAL=n308; QUAL=${QUAL/:n308/}; fi
if [ $QUAL == *:n308* ]; then NQUAL=n309; QUAL=${QUAL/:n309/}; fi

if [[ $NQUAL == *n308* ]]
then
    # These are the current (Apr 2021) nova versions (nutools v3_08_00)
    echo root v6_18_04d -q$QUAL
    echo boost v1_70_0 -q$QUAL
    echo ifdhc v2_5_7 -q${QUAL}:p372
else
    # These are the current (Apr 2021) sbn versions (nutools v3_09_02)
    echo root v6_22_06a -q${QUAL}:p383b
    echo boost v1_73_0 -q$QUAL
    echo ifdhc v2_5_12 -q${QUAL}:p383b
fi

echo stan_math v4_0_1 -q$QUAL
echo eigen v3_3_9a
