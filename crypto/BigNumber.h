#include <openssl/bn.h>
#pragma once
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
	BIGNUM* bn;
	long dec;
};