#pragma once
#include <openssl/bn.h>

class BigNumber
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