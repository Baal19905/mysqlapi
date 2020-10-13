#!/bin/bash

ln -sf ../include include
ln -sf ../source source
mkdir -p build
cd build && cmake .. && make clean && make && make install && cd -
export LD_LIBRARY_PATH=/usr/lib64:/usr/lib64/mysql
g++ test_stmt.cpp  -std=c++11 -lmysqlapi -L./lib -L/usr/lib64/mysql -lmysqlclient -lpthread -lz -lm -lssl -lcrypto -ldl -L/usr/lib64 -I./include -I/usr/include/mysql -o test_stmt -g
g++ test_normal.cpp  -std=c++11 -lmysqlapi -L./lib -L/usr/lib64/mysql -lmysqlclient -lpthread -lz -lm -lssl -lcrypto -ldl -L/usr/lib64 -I./include -I/usr/include/mysql -o test_normal -g
g++ test_autoconnect.cpp  -std=c++11 -lmysqlapi -L./lib -L/usr/lib64/mysql -lmysqlclient -lpthread -lz -lm -lssl -lcrypto -ldl -L/usr/lib64 -I./include -I/usr/include/mysql -o test_autoconnect -g

