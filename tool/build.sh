#!/bin/bash

mkdir -p build
cd build && rm * -fr && cmake .. -DCMAKE_BUILD_TYPE=Debug && make clean && make  && make install && cd -