#pragma once

#ifdef CRYPTOLIBRARY_EXPORTS
#define  CRYPTOLIBRARY_API __declspec(dllexport)
#else
#define  CRYPTOLIBRARY_API __declspec(dllimport)
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

extern "C" CRYPTOLIBRARY_API BigNumber weilPairing(EC_POINT *P, EC_POINT *Q, EC_POINT *S, Curve *crv);
extern "C" CRYPTOLIBRARY_API EC_POINT *mul(BigNumber n, EC_POINT *P, Curve *crv);
extern "C" CRYPTOLIBRARY_API void printBN(char* desc, BIGNUM * bn);
extern "C" CRYPTOLIBRARY_API void handleErrors();
extern "C" CRYPTOLIBRARY_API void printPoint(EC_POINT *P, Curve *crv);
extern "C" CRYPTOLIBRARY_API EC_POINT *createMPK(BigNumber msk, EC_POINT *P, Curve *crv);
extern "C" CRYPTOLIBRARY_API BigNumber getRandom(BigNumber max);
CRYPTOLIBRARY_API std::vector<EC_POINT*> keyProj(std::vector<int> coalition, std::vector<BigNumber> shares, EC_POINT *G, Curve *crv);
CRYPTOLIBRARY_API std::vector<BigNumber> shamir(BigNumber secretM, int participantN, int sufficientK, BigNumber q);
extern "C" CRYPTOLIBRARY_API EC_POINT* keyRecovery(std::vector<EC_POINT*> proj, std::vector<int> coalition, BigNumber q, Curve *crv);
std::vector<int> generatePoly(int power);
extern "C" CRYPTOLIBRARY_API BigNumber operator * (const BigNumber &a, const BigNumber &b);
extern "C" CRYPTOLIBRARY_API BigNumber operator % (const BigNumber &a, const BigNumber &b);
//extern "C" CRYPTOLIBRARY_API BigNumber operator / (const BigNumber &a, const BigNumber &b);
extern "C" CRYPTOLIBRARY_API BigNumber operator - (const BigNumber &a, const BigNumber &b);
extern "C" CRYPTOLIBRARY_API BigNumber operator + (const BigNumber &a, const BigNumber &b);

