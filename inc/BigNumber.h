#ifndef __BIGNUMBER_H__
#define __BIGNUMBER_H__

#ifdef _WIN32
# ifdef WIN_EXPORT
#   define EXPORTED  __declspec( dllexport )
# else
#   define EXPORTED  __declspec( dllimport )
# endif
#else
# define EXPORTED
#endif

#include <stdio.h>
#include <sstream>
#include <iostream>
#include <vector>
#include <cstring>
#include <openssl/bn.h>
#include "crypto_defs.h"

class EXPORTED BigNumber
{
public:
	BigNumber();
	BigNumber(const BigNumber& bn);
	BigNumber(BIGNUM* x);
	BigNumber(unsigned char* x, int size);
	BigNumber(const std::string& str);
	BigNumber(const int x);
	~BigNumber();
	unsigned long decimal();
	char* toDecString();
	char* toHexString();
	BIGNUM* bn;

	BigNumber& operator+=(const BigNumber& b);
	BigNumber& operator*=(const BigNumber& b);
	BigNumber& operator=(const BigNumber& bn);
};

#endif