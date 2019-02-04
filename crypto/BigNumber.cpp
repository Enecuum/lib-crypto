#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE 
#include "bignum.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
class BigNumber {
	public:
		BigNumber(int number) {
			this->numberDec = number;
			bignum_digit_t rslt[BIGNUM_MAX_DIGITS];
			char buf[65];
			_itoa(number, buf, 16);
			bignum_fromhex(rslt, buf, BIGNUM_MAX_DIGITS);
		}
		int dec() {
			return this->numberDec;
		}
		bignum_digit_t* hex() {
			return this->rslt;
		}
	private:
		bignum_digit_t rslt[BIGNUM_MAX_DIGITS];
		int numberDec;
};