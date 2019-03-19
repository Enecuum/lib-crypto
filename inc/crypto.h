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

//#include "ec_conf.h"
#include <vector>
#include <array>
#include <ctime>
#include <cstring> 
#include <math.h>
#include <openssl/ec.h>
#include <openssl/sha.h>
#include <openssl/err.h>
#include "BigNumber.h"
#include "Curve.h"

extern "C" EXPORTED BigNumber weilPairing(EC_POINT *P, EC_POINT *Q, EC_POINT *S, Curve *crv);
extern "C" EXPORTED EC_POINT *mul(BigNumber n, EC_POINT *P, Curve *crv);
extern "C" EXPORTED void printBN(char* desc, BIGNUM * bn);
extern "C" EXPORTED void handleErrors();
extern "C" EXPORTED void printPoint(EC_POINT *P, Curve *crv);
extern "C" EXPORTED EC_POINT *createMPK(BigNumber msk, EC_POINT *P, Curve *crv);
extern "C" EXPORTED BigNumber getRandom(BigNumber max);
EXPORTED std::vector<EC_POINT*> keyProj(std::vector<int> coalition, std::vector<BigNumber> shares, EC_POINT *Q, Curve *crv);
EXPORTED std::vector<BigNumber> shamir(BigNumber secretM, int participantN, int sufficientK, BigNumber q);
extern "C" EXPORTED EC_POINT* keyRecovery(std::vector<EC_POINT*> proj, std::vector<int> coalition, BigNumber q, Curve *crv);
EXPORTED std::vector<int> generatePoly(int power);
extern "C" EXPORTED BigNumber operator * (const BigNumber &a, const BigNumber &b);
extern "C" EXPORTED BigNumber operator % (const BigNumber &a, const BigNumber &b);
//extern "C" CRYPTOLIBRARY_API BigNumber operator / (const BigNumber &a, const BigNumber &b);
extern "C" EXPORTED BigNumber operator - (const BigNumber &a, const BigNumber &b);
extern "C" EXPORTED BigNumber operator + (const BigNumber &a, const BigNumber &b);
extern "C" EXPORTED EC_POINT* hashToPoint(BigNumber hash, Curve *curve);
extern "C" EXPORTED int test(int a, int b);