#!/bin/sh
rm -rf build/
mkdir build
cd build
cmake ..
make install
cd ..
rm -rf build/
#cp bin/Joiner /Users/haonan/Desktop/y2s2/database/sigmod/submission/workloads/small
cp bin/Joiner /home/aposta/projects/other/sigmod-contest/workloads/small
cp bin/Joiner /home/aposta/projects/other/sigmod-contest/workloads/big
