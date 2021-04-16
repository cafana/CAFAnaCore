#!/bin/bash

TAG=`git describe --tags`

echo FILE=TABLE
echo PRODUCT=cafanacore
echo VERSION=$TAG
echo
echo

for QUAL in debug:e19 e19:prof debug:e20 e20:prof c7:debug c7:prof
do
    echo FLAVOR=ANY
    echo QUALIFIERS=\"$QUAL\"
    echo
    echo 'ACTION=SETUP'
    echo '  setupEnv()'
    echo '  proddir()'
    echo
    echo '  # for get-directory-name'
    echo '  setupRequired(cetpkgsupport)'
    # Expect to be run in the directory one above....
    jenkins/dependencies.sh $QUAL | while read line
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
