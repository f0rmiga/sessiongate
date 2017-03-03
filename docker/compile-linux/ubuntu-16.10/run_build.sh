#!/bin/sh

if [ ! -d "docker/compile-linux/ubuntu-16.10/build" ]; then
    mkdir docker/compile-linux/ubuntu-16.10/build
fi

cd docker/compile-linux/ubuntu-16.10/build
cmake -DSTATIC_LINK=true ../../../..
make
mv libsessiongate.so ..
cd ..
rm -rf build/
