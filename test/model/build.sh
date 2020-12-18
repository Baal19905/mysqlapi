#!/bin/bash

#cd .. && sh build.sh && cd -
export LD_LIBRARY_PATH=/usr/lib64:/usr/lib64/mysql

#g++ *.cpp ../../src/*.cpp  -std=c++11 -L../lib -L/usr/lib64/mysql -lpthread -lz -lm -lssl -lcrypto -ldl -L/usr/lib64 -I../../src -I/usr/include/mysql -o test_model -g
g++ *.cpp ../../src/*.cpp  -std=c++11 -L../lib -L/usr/lib64/mysql -lpthread -lmysqlclient -lz -lm -ldl -L/usr/lib64 -I../../src -I/usr/include/mysql -o test_model -g


