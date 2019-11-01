#pragma once

#define EXPORTED 

#include <openssl/bn.h>

class EXPORTED BigNumber
{
public:
	BigNumber();
	BigNumber(const BigNumber& bn);
	BigNumber(BIGNUM * x);
	BigNumber(unsigned char * x, int size);
	BigNumber(int x);
	~BigNumber();
	int decimal();
	char* toDecString();
	BIGNUM* bn;
	long dec;
};