#!/bin/bash

# Centralize all the logic about what versions to depend on in this one file

if [ $# != 1 ]
then
    echo Usage: dependencies QUALIFIER >&2
    exit 1
fi

QUAL=$1

echo root v6_18_04d -q$QUAL
echo stan_math v4_0_1 -q$QUAL
echo eigen v3_3_9a
echo boost v1_70_0 -q$QUAL
echo ifdhc v2_5_7 -q${QUAL}:p372
