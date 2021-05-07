#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE  

#include "BigNumber.h"
#include <iostream>
//#include <execinfo.h>

BigNumber::BigNumber(const BigNumber& bn) {
	this->bn = BN_dup(bn.bn);	
	//std::cout << "Constructor copy: \t" << this->bn << " " << (BN_bn2hex(this->bn)) << std::endl;
	//this->dec = decimal();
}

BigNumber::BigNumber() {
	this->bn = BN_new();
	//std::cout << "Constructor empty: \t" << this->bn << " " << (BN_bn2hex(this->bn)) << std::endl;
}

BigNumber::BigNumber(BIGNUM * x){
	this->bn = BN_dup(x);
	//std::cout << "Constructor BIGNUM: \t" << this->bn << " " << (BN_bn2hex(this->bn)) << std::endl;
	//this->dec = decimal();
}

BigNumber::BigNumber(unsigned char * x, int size) {
	
	//BIGNUM *bn = NULL;
	if (NULL == (this->bn = BN_bin2bn(x, size, NULL))) return;
	//std::cout << "Constructor char: \t" << this->bn << " " << (BN_bn2hex(this->bn)) << std::endl;
	//this->dec = decimal();
	//this->bn = bn;
}

BigNumber::BigNumber(std::string str) {

	if ((str.size() % 2) != 0) {
		str.insert(0, "0");
	}
	for (size_t i = 2; i < str.size(); i += 3) {
		str.insert(i, " ");
	}

	std::istringstream hex_chars_stream(str);
	std::vector<unsigned char> bytes;

	unsigned int c;
	while (hex_chars_stream >> std::hex >> c)
	{
		bytes.push_back(c);
	}

	//BIGNUM *bn = NULL;
	if (NULL == (this->bn = BN_bin2bn(&bytes[0], bytes.size(), NULL))) return;
	//std::cout << "Constructor string: \t" << this->bn << " " << (BN_bn2hex(this->bn)) << std::endl;
	//this->dec = decimal();
	//this->bn = bn;
}

BigNumber::BigNumber(int number) {
	std::vector<unsigned char> buf(4);
	for (size_t i = 0; i < 4; i++)
		buf[3 - i] = (number >> (i * 8));
	if (NULL == (this->bn = BN_bin2bn(buf.data(), buf.size(), NULL))) return;

	//std::cout << "Constructor int: \t" << this->bn << " " << (BN_bn2hex(this->bn)) << std::endl;
}

BigNumber::~BigNumber(){
	//std::cout << "Destructor    : \t" << this->bn << " " << (BN_bn2hex(this->bn)) << std::endl;
	BN_free(bn);
	bn = NULL;
}

int BigNumber::decimal() {
//IK added/modified
	//int x = strtol(BN_bn2hex(bn), NULL, 16);
	char* str = BN_bn2hex(bn);
	int x = strtol(str, NULL, 16);
	OPENSSL_free(str);
//IK added/modified
	return x;
}
char* BigNumber::toDecString() {
//IK added/modified	
	//return BN_bn2dec(bn);
	char* str = BN_bn2dec(bn);
	strcpy(BigNumberDecString, str);
	OPENSSL_free(str);
	return BigNumberDecString;
//IK added/modified		
}
std::string BigNumber::toHexString() {
//IK added/modified
	//std::string bnhex(BN_bn2hex(bn));
	char* str = BN_bn2hex(bn);
	std::string bnhex(str);
	OPENSSL_free(str);
//IK added/modified
	return bnhex;
}

BigNumber& BigNumber::operator += (const BigNumber& b)
{
	BN_add(this->bn, this->bn, b.bn);
	return *this;
}

BigNumber& BigNumber::operator*=(const BigNumber& b)
{
	BN_CTX* ctx = BN_CTX_new();
	BN_mul(this->bn, this->bn, b.bn, ctx);
//IK added
	BN_CTX_free(ctx); 
//IK added
	return *this;
}

BigNumber& BigNumber::operator=(const BigNumber& bn)
{
//IK added
	if (this == &bn)
		return *this;
//IK added

	BN_free(this->bn);	

	this->bn = BN_dup(bn.bn);	
	return *this;
}