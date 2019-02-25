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

BigNumber weilPairing(EC_POINT *P, EC_POINT *Q, EC_POINT *S, EC_GROUP *curve);
extern "C" CRYPTOLIBRARY_API EC_POINT *mul(BigNumber n, EC_POINT *P, EC_GROUP *curve);
void printBN(char* desc, BIGNUM * bn);
void handleErrors();
extern "C" CRYPTOLIBRARY_API EC_GROUP *create_curve(BigNumber a, BigNumber b, BigNumber p, BigNumber order, BigNumber gx, BigNumber gy);
void printPoint(EC_POINT *P, EC_GROUP *curve);
EC_POINT *createMPK(BigNumber msk, EC_POINT *P, EC_GROUP *curve);
extern "C" CRYPTOLIBRARY_API BigNumber getRandom(BigNumber max);
std::vector<EC_POINT*> keyProj(std::vector<int> coalition, std::vector<BigNumber> shares, EC_POINT *G, EC_GROUP *curve);
std::vector<BigNumber> shamir(BigNumber secretM, int participantN, int sufficientK, BigNumber q);
EC_POINT* keyRecovery(std::vector<EC_POINT*> proj, std::vector<int> coalition, BigNumber q, EC_GROUP *curve);
BigNumber operator * (const BigNumber &a, const BigNumber &b);
BigNumber operator % (const BigNumber &a, const BigNumber &b);
BigNumber operator / (const BigNumber &a, const BigNumber &b);
BigNumber operator - (const BigNumber &a, const BigNumber &b);
extern "C" CRYPTOLIBRARY_API BigNumber operator + (const BigNumber &a, const BigNumber &b);

extern "C" CRYPTOLIBRARY_API int ecc_sum(int a, int b);

extern "C" CRYPTOLIBRARY_API BigNumber createObject(int a);