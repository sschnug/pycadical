#!/bin/sh

echo " "
echo "Clone cadical repository"
echo " "
git clone https://github.com/arminbiere/cadical.git

echo " "
echo "Build cadical"
echo " "
cd cadical
./configure
make CXXFLAGS='-fPIC -DQUIET'

echo " "
echo "Copy lib to /usr/lib"
echo " "
cp build/libcadical.a /usr/lib
