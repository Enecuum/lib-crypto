#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE  

#include "BigNumber.h"



BigNumber::BigNumber(const BigNumber& bn) {
	this->bn = nullptr;
	if (nullptr == (this->bn = BN_dup(bn.bn)))
		handleError(NO_MEMORY);
}

BigNumber::BigNumber() {
	this->bn = nullptr;
	if (nullptr == (this->bn = BN_new()))
		handleError(NO_MEMORY);
}

BigNumber::BigNumber(BIGNUM * x){
	this->bn = nullptr;
	if (nullptr == (this->bn = BN_dup(x)))
		handleError(NO_MEMORY);
}

BigNumber::BigNumber(unsigned char * x, int size) {
	this->bn = nullptr;
	if (nullptr == (this->bn = BN_bin2bn(x, size, nullptr)))
		handleError(NO_MEMORY);
}

BigNumber::BigNumber(const std::string& str) {
	std::string newStr = str;
	if ((newStr.size() % 2) != 0)
		newStr.insert(0, "0");
	
	for (size_t i = 2; i < newStr.size(); i += 3)
		newStr.insert(i, " ");

	std::istringstream hex_chars_stream(newStr);
	std::vector<unsigned char> bytes;

	unsigned int c;
	while (hex_chars_stream >> std::hex >> c) {
		bytes.push_back(c);
	}

	if (nullptr == (this->bn = BN_bin2bn(&bytes[0], bytes.size(), nullptr))) 
		handleError(NO_MEMORY);
}

BigNumber::BigNumber(int number) {
	std::vector<unsigned char> buf(4);
	for (size_t i = 0; i < 4; i++)
		buf[3 - i] = (number >> (i * 8));
	if (nullptr == (this->bn = BN_bin2bn(buf.data(), buf.size(), nullptr))) 
		handleError(NO_MEMORY);
}

BigNumber::~BigNumber(){
	BN_free(bn);
	bn = nullptr;	
}

unsigned long BigNumber::decimal() {
	char* str = BN_bn2hex(bn);
	unsigned long long x = strtol(str, nullptr, 16);
	OPENSSL_free(str);
	return x;
}
char* BigNumber::toDecString() {
	char* str = BN_bn2dec(bn);
	strcpy(charBuff, str);
	OPENSSL_free(str);
	return charBuff;
}
char* BigNumber::toHexString() {	
	char* str = BN_bn2hex(bn);
	charBuff[0] = '0';
	charBuff[1] = 'x';	
	strcpy(&charBuff[2], str);	
	OPENSSL_free(str); 
	str = nullptr;
	return charBuff;
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
	BN_CTX_free(ctx); 
	return *this;
}

BigNumber& BigNumber::operator=(const BigNumber& bn)
{
	if (this == &bn)
		return *this;

	BN_free(this->bn);	
	this->bn = BN_dup(bn.bn);	
	return *this;
}