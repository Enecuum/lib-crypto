#!/bin/bash


g++  \
 -DHAVE_CONFIG_H \
 -DWIN_EXPORT \
 -I. \
 -O2   -Wall -g -DNDEBUG -UGIVARO_DEBUG -UDEBUG -std=gnu++11 \
 -c \
 ./crypto/*.cpp \
 -lgivaro -lgmpxx -lgmp -lcrypto

 ar rvs libenq.a crypto.o Curve.o BigNumber.o ellipticCurve.o

ranlib libenq.a