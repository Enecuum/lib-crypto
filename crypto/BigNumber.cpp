#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE  

#include "BigNumber.h"
#include <sstream>
#include <vector>
using namespace std;
BigNumber::BigNumber() {
	this->bn = BN_new();
}

BigNumber::BigNumber(BIGNUM * x){
	this->bn = x;
}

BigNumber::BigNumber(unsigned char * x, int size) {
	
	//BIGNUM *bn = NULL;
	if (NULL == (this->bn = BN_bin2bn(x, size, NULL))) return;
	//this->bn = bn;
}

BigNumber::BigNumber(int number) {
	vector<unsigned char> buf(4);
	for (int i = 0; i < 4; i++)
		buf[3 - i] = (number >> (i * 8));
	if (NULL == (this->bn = BN_bin2bn(buf.data(), buf.size(), NULL))) return;
}

BigNumber::~BigNumber(){
	BN_free(bn);
}

void BigNumber::print() {


}
int BigNumber::decimal() {
	long x = strtol(BN_bn2hex(bn), NULL, 16);
	return x;
}
BIGNUM* BigNumber::getBn() {
	return this->bn;
}