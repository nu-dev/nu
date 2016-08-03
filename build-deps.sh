#!/bin/bash

# hoedown
git clone https://github.com/hoedown/hoedown.git
cd hoedown
make
cp libhoedown.a ../
cd ..

# libnucommon
git clone https://github.com/nu-dev/libnucommon.git
cd libnucommon
make
cd ..