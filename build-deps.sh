#!/bin/bash

# hoedown
git clone https://github.com/hoedown/hoedown.git
cd hoedown
make
cd ..

# libnucommon
git clone https://github.com/nu-dev/libnucommon.git
cd libnucommon
make
cd ..