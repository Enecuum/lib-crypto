#!/bin/bash

HOME=mnt/d/awork/ECC/givaro-3.8.0
LHOME=tmp/givaro/lib
GMPHOME=tmp/win-gmp/include


# g++ -w \
#  -DHAVE_CONFIG_H \
#  -DWIN_EXPORT \
#  -I. -I/$HOME \
#  -I/$HOME   -I/$HOME/src/kernel/system -I/$HOME/src/kernel/memory \
#  -I/$HOME/src/kernel/zpz -I/$HOME/src/kernel/integer \
#  -I/$HOME/src/kernel -I/$HOME/src/library/poly1 \
#  -I/$HOME/src/kernel/bstruct -I/$HOME/src/library/tools \
#  -O2   -Wall -g -DNDEBUG -UGIVARO_DEBUG -UDEBUG -std=gnu++11 \
#  ./crypto/main.cpp \
#  -o test \
#  -Ltmp/givaro/lib -L. \
#  -lgivaro -lgmpxx -lgmp -lcrypto -lenq

 g++ -w -DHAVE_CONFIG_H -I. -I/$HOME  -I/$HOME -I./crypto \
 -I/$HOME/src/kernel/system -I/$HOME/src/kernel/memory \
 -I/$HOME/src/kernel/zpz -I/$HOME/src/kernel/integer \
 -I/$HOME/src/kernel -I/$HOME/src/library/poly1 \
 -I/$HOME/src/kernel/bstruct -I/$HOME/src/library/tools \
 -O2   -Wall -g -DNDEBUG -UGIVARO_DEBUG -UDEBUG -std=gnu++11 \
 -c -o test.o ./crypto/main.cpp 

/bin/sh /usr/bin/libtool  --tag=CXX   --mode=link g++ -O2   \
-Wall -g -DNDEBUG -UGIVARO_DEBUG -UDEBUG -std=gnu++11 \
-static -o test test.o -L/mnt/c/cygwin64/home/Anton/ecc/examples \
-L/$HOME/src -L/$HOME/src/.libs \
-lenq -lgivaro -lgmpxx -lgmp -lcrypto 
