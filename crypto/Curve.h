#pragma once

#ifdef CRYPTOLIBRARY_EXPORTS
#define  CRYPTOLIBRARY_API __declspec(dllexport)
#else
#define  CRYPTOLIBRARY_API __declspec(dllimport)
#endif

#include <openssl/ec.h>
#include "BigNumber.h"

class CRYPTOLIBRARY_API Curve
{
public:
	Curve();
	Curve(BigNumber a, BigNumber b, BigNumber p, BigNumber order, BigNumber gx, BigNumber gy);
	~Curve();
	EC_POINT *getPoint();
	EC_GROUP* getCurve();
	BigNumber order;
	BigNumber field;
	BigNumber a;
	BigNumber gx;
	BigNumber gy;
	EC_GROUP *curve;
};
EC_GROUP *create_curve(BigNumber a, BigNumber b, BigNumber p, BigNumber order, BigNumber gx, BigNumber gy);

