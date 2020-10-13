#!/bin/bash

export LANG=C
SVN_VER=".r"`svn info | grep Revision | cut -d " " -f 2`
mkdir -p build
cd build && rm * -fr && cmake .. -D_SVN_VER=${SVN_VER} && make clean && make  && make install && cd -
