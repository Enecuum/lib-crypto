#!/bin/bash

# our comment is here
echo "Build library"

# Path to repo
repoPath="/mnt/c/korobkov/project/repo/lib-crypto"
# Path to all node addon files
addonPath="$repoPath/node-addon"
# Path to addon source
srcAddonPath="$addonPath/src"
libName="libenq.a"
naddonName="addon.node"
binPackName="enq-bin"
# Path to final bin files
nmPath="$addonPath/node_modules/$binPackName"

cd $repoPath
# Build start
g++  \
 -DHAVE_CONFIG_H \
 -DWIN_EXPORT \
 -I. \
 -I/tmp/givaro-build/include \
 -O2   -Wall -g -DNDEBUG -UGIVARO_DEBUG -UDEBUG -std=gnu++11 \
 -fPIC \
 -c \
 ./crypto/BigNumber.cpp \
 ./crypto/Curve.cpp \
 ./crypto/ellipticCurve.cpp \
 ./crypto/crypto.cpp \
 ./crypto/crypto_defs.cpp \
 -lgivaro -lgmpxx -lgmp -lcrypto

 ar rvs libenq.a crypto.o Curve.o BigNumber.o ellipticCurve.o crypto_defs.o

ranlib libenq.a

cd $srcAddonPath

node-gyp clean
node-gyp configure
mkdir -p $srcAddonPath/build/node_modules/$binPackName

cp $repoPath/$libName $srcAddonPath/build/node_modules/$binPackName/$libName
node-gyp build

mkdir -p $nmPath

cp $srcAddonPath/build/node_modules/$binPackName/$libName $nmPath/$libName
cp $srcAddonPath/build/Release/addon.node $nmPath/$naddonName
