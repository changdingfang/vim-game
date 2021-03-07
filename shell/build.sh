#!/bin/bash
# =======================================================================
#  FileName:     build.sh
#  Author:       dingfang
#  CreateDate:   2021-03-04 19:44:04
#  ModifyAuthor: dingfang
#  ModifyDate:   2021-03-07 14:00:55
# =======================================================================


SCRIPT=`pwd`/$0
ROOT_DIR=`dirname ${SCRIPT}`

cd ${ROOT_DIR}/..

if [[ -d build ]]; then
    rm -r build
fi


mkdir build
cd build
cmake ..
make

cd bin
cp -r ${ROOT_DIR}/maps .
./vgame
