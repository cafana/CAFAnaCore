#!/bin/bash

TAG=`git describe --tags`

echo FILE=TABLE
echo PRODUCT=cafanacore
echo VERSION=$TAG
echo
echo

for EXPT in n308 n309
do
    for OPT in debug prof
    do
        for COMPILER in e19 e20 c7
        do
            if [[ $EXPT == n308 && $COMPILER != e19 ]]; then continue; fi

            echo FLAVOR=ANY
            echo $QUALIFIERS=\"${OPT}:${COMPILER}:${EXPT}\"
            echo
            echo 'ACTION=SETUP'
            echo '  setupEnv()'
            echo '  proddir()'
            echo
            echo '  # for get-directory-name'
            echo '  setupRequired(cetpkgsupport)'
            # Expect to be run in the directory one above....
            jenkins/dependencies.sh ${OPT}:${COMPILER}:${EXPT} | while read line
            do
                echo '  setupRequired('$line')'
            done
            echo
            echo '  EnvSet(CAFANACORE_VERSION, ${UPS_PROD_VERSION} )'
            echo '  EnvSet(CAFANACORE_INC, ${UPS_PROD_DIR}/include )'
            echo '  EnvSet(CAFANACORE_FQ_DIR, ${CAFANACORE_DIR}`get-directory-name subdir`.`echo ${UPS_PROD_QUALIFIERS} | tr ":" "."` )'
            echo '  EnvSet(CAFANACORE_LIB, ${CAFANACORE_FQ_DIR}/lib )'
            echo '  pathPrepend(LD_LIBRARY_PATH, ${CAFANACORE_LIB})'
            echo
            echo
        done
    done
done
