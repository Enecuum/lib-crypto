#pragma once

#ifdef CRYPTOLIBRARY_EXPORTS
#define  CRYPTOLIBRARY_API __declspec(dllexport)
#else
#define  CRYPTOLIBRARY_API __declspec(dllimport)
#endif

#include <openssl/bn.h>

class CRYPTOLIBRARY_API BigNumber
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