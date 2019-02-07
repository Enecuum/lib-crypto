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
	void print();
	int decimal();
	BIGNUM* getBn();
	BIGNUM* bn;
	long dec;

	
};