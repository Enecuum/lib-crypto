#ifndef __CRYPTO_H__
#define __CRYPTO_H__

#ifdef _WIN32
# ifdef WIN_EXPORT
#   define EXPORTED  __declspec( dllexport )
# else
#   define EXPORTED  __declspec( dllimport )
# endif
#else
# define EXPORTED						 
#endif

#include <vector>
#include <array>
#include <ctime>
#include <cstring> 
#include <math.h>
#include <openssl/ec.h>
#include <openssl/sha.h>
#include <openssl/err.h>
#include "crypto_defs.h"
#include "BigNumber.h"
#include "Curve.h"
#include "ellipticCurve.h"
#include "constants.h"


extern "C" EXPORTED BigNumber weilPairing(const EC_POINT * P, const EC_POINT * Q, const EC_POINT * S, const Curve * crv);
extern "C" EXPORTED EC_POINT * mul(const BigNumber & n, const EC_POINT * P, const Curve * crv);
extern "C" EXPORTED void printBN(const char* desc, const BIGNUM * bn);
extern "C" EXPORTED char* printPoint(const EC_POINT * P, const Curve * crv);
extern "C" EXPORTED EC_POINT * createMPK(const BigNumber & msk, const EC_POINT * P, const Curve * crv);
extern "C" EXPORTED BigNumber getRandom(const BigNumber & max);
EXPORTED std::vector<EC_POINT*> keyProj(const std::vector<int>& coalition, const std::vector<BigNumber> shares, const EC_POINT* Q, const Curve* crv);
EXPORTED std::vector<BigNumber> shamir(const BigNumber& secretM, const std::vector<BigNumber>& ids, const int participantN, const int sufficientK, const BigNumber& q);
extern "C" EXPORTED EC_POINT * keyRecovery(const std::vector<EC_POINT*>&proj, const std::vector<int>&coalition, const BigNumber & q, const Curve * curve);
EXPORTED std::vector<BigNumber> generatePoly(const int power);
extern "C" EXPORTED BigNumber operator * (const BigNumber & a, const BigNumber & b);
extern "C" EXPORTED BigNumber operator % (const BigNumber & a, const BigNumber & b);
extern "C" EXPORTED BigNumber operator - (const BigNumber & a, const BigNumber & b);
extern "C" EXPORTED BigNumber operator + (const BigNumber & a, const BigNumber & b);
extern "C" EXPORTED int test(const int a, const int b);
extern "C" EXPORTED BigNumber bpow(const BigNumber & a, const int n);
EXPORTED ExtensionField::Element tatePairing(const ecPoint& P, const ecPoint& Q, const ecPoint& S, ellipticCurveFq& EF_q);
EXPORTED ellipticCurveFq::Point mapToFq(const EC_POINT* P, const Curve* curve, ellipticCurveFq& E_Fq);
EXPORTED ellipticCurveFq::Point hashToPointFq(const ecPoint& G, const BigNumber& cnum, ellipticCurveFq& E_Fq);
EXPORTED ellipticCurveFq::Point hashToPoint(const BigNumber& cnum);
std::string dectox_int(Integer num);
EXPORTED EC_POINT* getQ(const BigNumber& qqhash, const Curve* crv, ellipticCurveFq& E_Fq);
bool verifyTate(const ecPoint& S1_fq, const ecPoint& S2_fq, const BigNumber& hash, const ecPoint& MPK_fq, const ecPoint& Q_fq, const ecPoint& G0_fq, ellipticCurveFq& E_Fq);
bool verify_mobile(const std::string& p, const std::string& a, const std::string& b, const std::string& order, const std::string& irred,
	const std::string& gx, const std::string& gy, const int k, const std::string& s1x, const std::string& s1y, const std::string& s2x,
	const std::string& s2y, const std::string& pk_lpos, const std::string& mhash, const std::string& mpkx,
	const std::string& mpky);

#endif