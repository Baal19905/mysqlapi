#!/bin/bash

cd .. && sh build.sh && cd -
export LD_LIBRARY_PATH=/usr/lib64:/usr/lib64/mysql
cp ../../table_model/* .
g++ *.cpp  -std=c++11 -lmysqlapi -L../lib -L/usr/lib64/mysql -lmysqlclient -lpthread -lz -lm -lssl -lcrypto -ldl -L/usr/lib64 -I../include -I/usr/include/mysql -o test_model -g


