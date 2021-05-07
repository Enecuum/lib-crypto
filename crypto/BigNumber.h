#pragma once

#ifdef _WIN32
# ifdef WIN_EXPORT
#   define EXPORTED  __declspec( dllexport )
# else
#   define EXPORTED  __declspec( dllimport )
# endif
#else
# define EXPORTED
#endif

#include <sstream>
#include <vector>

#include <openssl/bn.h>

//IK added
const int   MAX_LEN_DEC_STRING = 10000;	  	
static char BigNumberDecString[MAX_LEN_DEC_STRING];


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
	std::string toHexString();
	BIGNUM* bn;

	BigNumber& operator+=(const BigNumber& b);
	BigNumber& operator*=(const BigNumber& b);
	BigNumber& operator=(const BigNumber& bn);
};