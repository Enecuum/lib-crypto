#pragma once

#define EXPORTED 

#include <sstream>
#include <vector>

#include <openssl/bn.h>

class EXPORTED BigNumber
{
public:
	BigNumber();
	BigNumber(const BigNumber& bn);
	BigNumber(BIGNUM * x);
	BigNumber(unsigned char * x, int size);
	BigNumber(std::string str);
	BigNumber(int x);
	~BigNumber();
	int decimal();
	char* toDecString();
	BIGNUM* bn;
	long dec;
};