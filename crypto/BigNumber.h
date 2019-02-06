#include <openssl/bn.h>
#pragma once
class BigNumber
{
public:
	BigNumber();
	BigNumber(BIGNUM * x, BIGNUM * y);
	BigNumber(unsigned char * x, int size);
	BigNumber(int x);
	~BigNumber();
	void print();
	int decimal();
	BIGNUM* getBn();
	BIGNUM* bn;
private:
	int dec;
	
};

