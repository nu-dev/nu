#!/bin/bash

# lua 5.3.3
LUA_VERSION=5.2.4
curl -R -O http://www.lua.org/ftp/lua-$LUA_VERSION.tar.gz
tar zxf lua-$LUA_VERSION.tar.gz
rm lua-$LUA_VERSION.tar.gz
mv lua-$LUA_VERSION lua
cd lua
OS="generic"
case "$OSTYPE" in
  solaris*) OS="solaris" ;;
  darwin*)  OS="macosx" ;; 
  linux*)   OS="linux" ;;
  bsd*)     OS="bsd" ;;
  msys*)    OS="mingw" ;;
  *)        OS="generic" ;;
esac
echo "Detected OS as $OS"

# make for the OS
make $OS

cd src
mv luac ..
mv *.h ../
mv liblua.a ../
cd ..
cd ..

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
