#!/bin/bash

TAG=`git describe --tags`

echo FILE=TABLE
echo PRODUCT=cafanacore
echo VERSION=$TAG
echo
echo

for EXPT in n319
do
    for OPT in debug prof
    do
        for COMPILER in e26 c14
        do
	    for STAN in stan stanfree
	    do
		echo FLAVOR=ANY
		echo QUALIFIERS=\"${OPT}:${COMPILER}:${EXPT}:${STAN}\"
		echo
		echo 'ACTION=SETUP'
		echo '  setupEnv()'
		echo '  proddir()'
		echo
		echo '  # for get-directory-name'
		echo '  setupRequired(cetpkgsupport)'
		# Expect to be run in the directory one above....
		jenkins/dependencies.sh ${OPT}:${COMPILER}:${EXPT}:${STAN} | while read line
		do
                    echo '  setupRequired('$line')'
		done
		echo
		echo '  EnvSet(CAFANACORE_VERSION, ${UPS_PROD_VERSION} )'
		echo '  EnvSet(CAFANACORE_INC, ${CAFANACORE_FQ_DIR}/include )'
		echo '  EnvSet(CAFANACORE_FQ_DIR, ${CAFANACORE_DIR}`get-directory-name subdir`.`echo ${UPS_PROD_QUALIFIERS} | tr ":" "."` )'
		echo '  EnvSet(CAFANACORE_LIB, ${CAFANACORE_FQ_DIR}/lib )'
		echo '  pathPrepend(LD_LIBRARY_PATH, ${CAFANACORE_LIB})'
		echo
		echo
	    done
        done
    done
done
