/**
 * Enecuum Crypto Library source code
 * See LICENCE file at the top of the source tree
 *
 * ******************************************
 *
 * Curve.h
 *
 * ******************************************
 *
 * Authors: A. Prudanov, I. Korobkov
 */

#ifndef __CURVE_H__
#define __CURVE_H__

#ifdef _WIN32
# ifdef WIN_EXPORT
#   define EXPORTED  __declspec( dllexport )
# else
#   define EXPORTED  __declspec( dllimport )
# endif
#else
# define EXPORTED
#endif

#include <openssl/ec.h>
#include "BigNumber.h"
#include "crypto_defs.h"

class EXPORTED Curve
{
public:
	Curve();
	Curve(const Curve& C);
	Curve(const BigNumber& a, const BigNumber& b, const BigNumber& p, const BigNumber& order, const BigNumber& gx, const BigNumber& gy);
	~Curve();
	EC_POINT* getPoint();
	EC_GROUP* getCurve();
	void setPointByCopy(const EC_POINT* G);
	BigNumber order;
	BigNumber field;
	BigNumber a;
	BigNumber gx;
	BigNumber gy;
	EC_POINT* G;
	EC_GROUP* curve;
	std::string toDecString();
	Curve& operator=(const Curve& C);
private:
	void createDefault();
};
EC_GROUP* create_curve(const BigNumber& a, const BigNumber& b, const BigNumber& p, const BigNumber& order, const BigNumber& gx, const BigNumber& gy);

#endif