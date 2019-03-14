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

#include <openssl/ec.h>
#include "BigNumber.h"

class EXPORTED Curve
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
	EC_POINT *G;
};
EC_GROUP *create_curve(BigNumber a, BigNumber b, BigNumber p, BigNumber order, BigNumber gx, BigNumber gy);

