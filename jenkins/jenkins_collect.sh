#!/bin/bash

TAG=`git describe --tags`
# clean out any previous build
rm -rf v??.*
rm -rf $TAG
rm -rf v??.*
rm -rf ${TAG}.version
mkdir $TAG

ls

once=no

for olddir in 'OS='*
do
    newdir=$TAG/${olddir/CAFAna/}
    newdir=${newdir/OS=SLF6/slf6.x86_64}
    newdir=${newdir/OS=SLF7/slf7.x86_64}
    newdir=${newdir/OS=ALMA9/slf7.x86_64}
    newdir=${newdir/QUALIFIER=/}
    newdir=${newdir//,/.}
    newdir=${newdir//:/.}
    echo mv $olddir $newdir
    mv $olddir $newdir
    # All versions should be the same
    if [ $once != yes ]
    then
        once=yes
        mv $newdir/CAFAna/ups $TAG/
        mv $newdir/inc $TAG/include
        for k in `find $newdir/CAFAna -name '*.h' -o -name '*.cxx' -o -name '*.txx'`
        do
            fname=${k/$newdir/}
            mkdir -p $TAG/src/`dirname $fname`
            echo mv $k $TAG/src/$fname
            mv $k $TAG/src/$fname
        done
    fi
    rm -r $newdir/CAFAna
    rm -r $newdir/build
    rm -r $newdir/inc
done

cp -r jenkins/version ${TAG}.version
sed -i s/vXX.YY/$TAG/g ${TAG}.version/*

# Include the version number in a header file as well as the library
echo '#define CAFANACORE_VERSION "'$TAG'"' >> $TAG/include/CAFAna/Core/Version.h

# Jenkins doesn't deal well with symlinks
# cd $TAG/include
# rm CAFAnaCore
# ln -s . CAFAnaCore
