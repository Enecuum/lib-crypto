#!/bin/bash

# HOME=mnt/d/awork/ECC/givaro-3.8.0

#  g++ -w -DHAVE_CONFIG_H -I. -I/$HOME  -I/$HOME -I./crypto \
#  -I/$HOME/src/kernel/system -I/$HOME/src/kernel/memory \
#  -I/$HOME/src/kernel/zpz -I/$HOME/src/kernel/integer \
#  -I/$HOME/src/kernel -I/$HOME/src/library/poly1 \
#  -I/$HOME/src/kernel/bstruct -I/$HOME/src/library/tools \
#  -O2   -Wall -g -DNDEBUG -UGIVARO_DEBUG -UDEBUG -std=gnu++11 \
#  -c -o test.o ./crypto/main.cpp 

# /bin/sh /usr/bin/libtool  --tag=CXX   --mode=link g++ -O2   \
# -Wall -g -DNDEBUG -UGIVARO_DEBUG -UDEBUG -std=gnu++11 \
# -static -o test test.o -L. \
# -L/$HOME/src -L/$HOME/src/.libs \
# -lenq -lgivaro -lgmpxx -lgmp -lcrypto 

HOME=mnt/d/awork/ECC/givaro-3.8.0

 g++ -w -DHAVE_CONFIG_H -I. \
 -O2   -Wall -g -DNDEBUG -UGIVARO_DEBUG -UDEBUG -std=gnu++11 \
 -c -o test.o ./crypto/main.cpp 

/bin/sh /usr/bin/libtool  --tag=CXX   --mode=link g++ -O2   \
-Wall -g -DNDEBUG -UGIVARO_DEBUG -UDEBUG -std=gnu++11 \
-static -o test test.o -L. \
-lenq -lgivaro -lgmpxx -lgmp -lcrypto 
