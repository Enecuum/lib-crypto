#pragma once

#define EXPORTED 


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
#include "ellipticCurve.h"

extern "C" EXPORTED BigNumber weilPairing(EC_POINT *P, EC_POINT *Q, EC_POINT *S, Curve *crv);
//extern "C" EXPORTED BigNumber tatePairing();
extern "C" EXPORTED EC_POINT *mul(BigNumber n, EC_POINT *P, Curve *crv);
extern "C" EXPORTED void printBN(char* desc, BIGNUM * bn);
extern "C" EXPORTED void handleErrors();
extern "C" EXPORTED void printPoint(EC_POINT *P, Curve *crv);
extern "C" EXPORTED EC_POINT *createMPK(BigNumber msk, EC_POINT *P, Curve *crv);
extern "C" EXPORTED BigNumber getRandom(BigNumber max);
EXPORTED std::vector<EC_POINT*> keyProj(std::vector<int> coalition, std::vector<BigNumber> shares, EC_POINT *Q, Curve *crv);
EXPORTED std::vector<BigNumber> shamir(BigNumber secretM, std::vector<int> ids, int participantN, int sufficientK, BigNumber q);
extern "C" EXPORTED EC_POINT* keyRecovery(std::vector<EC_POINT*> proj, std::vector<int> coalition, BigNumber q, Curve *crv);
EXPORTED std::vector<int> generatePoly(int power);
extern "C" EXPORTED BigNumber operator * (const BigNumber &a, const BigNumber &b);
extern "C" EXPORTED BigNumber operator % (const BigNumber &a, const BigNumber &b);
//extern "C" CRYPTOLIBRARY_API BigNumber operator / (const BigNumber &a, const BigNumber &b);
extern "C" EXPORTED BigNumber operator - (const BigNumber &a, const BigNumber &b);
extern "C" EXPORTED BigNumber operator + (const BigNumber &a, const BigNumber &b);
extern "C" EXPORTED EC_POINT* hashToPoint(BigNumber hash, Curve *curve);
extern "C" EXPORTED int test(int a, int b);
EXPORTED ExtensionField::Element tatePairing(ecPoint &P, ecPoint &Q, ecPoint &S, ellipticCurveFq &EF_q);
EXPORTED ellipticCurveFq::Point mapToFq(EC_POINT *P, Curve *curve, ellipticCurveFq& E_Fq);
//EXPORTED ellipticCurveFq::Point hashToPointFq(ecPoint &P, BigNumber hash, ellipticCurveFq& E_Fq);
void verify_mobile(
	std::string p,
	std::string a,
	std::string b,
	std::string order,
	std::string irred,
	std::string gx,
	std::string gy,
	int k,
	std::string s1x,
	std::string s1y,
	std::string s2x,
	std::string s2y,
	std::string pk_lpos,
	std::string mhash,
	std::string mpkx,
	std::string mpky,
	std::string strEta
	);