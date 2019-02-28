#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE  

#include "BigNumber.h"
#include <sstream>
#include <vector>

BigNumber::BigNumber(const BigNumber& bn) {
	this->bn = bn.bn;
	//this->dec = decimal();
}

BigNumber::BigNumber() {
	this->bn = BN_new();
}

BigNumber::BigNumber(BIGNUM * x){
	this->bn = x;
	//this->dec = decimal();
}

BigNumber::BigNumber(unsigned char * x, int size) {
	
	//BIGNUM *bn = NULL;
	if (NULL == (this->bn = BN_bin2bn(x, size, NULL))) return;
	//this->dec = decimal();
	//this->bn = bn;
}

BigNumber::BigNumber(int number) {
	std::vector<unsigned char> buf(4);
	for (int i = 0; i < 4; i++)
		buf[3 - i] = (number >> (i * 8));
	if (NULL == (this->bn = BN_bin2bn(buf.data(), buf.size(), NULL))) return;
	this->dec = number;
}

BigNumber::~BigNumber(){
	//BN_free(bn);
}

int BigNumber::decimal() {
	int x = strtol(BN_bn2hex(bn), NULL, 16);
	return x;
}
char* BigNumber::toDecString() {
	return BN_bn2dec(bn);
}
