#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE  

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "eccrypt.h"
#include "bignum.h"
#include "ec_conf.h"
#include <vector>
#include <array>
#include <ctime>
#include <string> 
#include <math.h>
#include <openssl/obj_mac.h>
#include <openssl/ec.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include "BigNumber.h"
//#pragma comment(lib,"libcrypto.lib")
using namespace std;

struct eccrypt_curve_t curve;
int cntr = 0;
BN_CTX *ctx;

EC_POINT *mul(BigNumber n, EC_POINT *P, EC_GROUP *curve);
bool prime(long long n);
void handleErrors();
/* Set up the BN_CTX */

int finiteDiv(int up, int down, int p, vector<int> invs) {
	if (down < 0) {
		up *= -1;
		down *= -1;
	}
	up = up * invs[down];
	while (up < 0)
		up += p;
	return (up);
}

vector<int> getInv(int m) {
	vector<int> r = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
	for (int i = 2; i < m; i++)
		r[i] = (m - (m / i) * r[m%i] % m) % m;
	return r;
}

EC_POINT* keyRecovery(vector<EC_POINT*> proj, int* coalition, int q, EC_GROUP *curve) {
	EC_POINT* secret = EC_POINT_new(curve);
	EC_POINT *buff = EC_POINT_new(curve);
	vector<int> invs = getInv(13);
	for (int i = 0; i < 6; i++) {
		int lambda = 1;
		for (int j = 0; j < 6; j++) {
			if (i != j) {
				lambda = (finiteDiv(lambda * (0 - coalition[j]), coalition[i] - coalition[j], q, invs)) % q;
			}
		}
		if (lambda == 0)
			continue;
		
		buff = mul(BigNumber(lambda), proj.at(i), curve);

		if (i == 0) {
			if (!EC_POINT_copy(secret, buff)) handleErrors();
			continue;
		}
		if (!EC_POINT_add(curve, secret, secret, buff, NULL)) handleErrors();
	}
	EC_POINT_free(buff);
	return secret;
}

vector<int> shamir(int secretM, int participantN, int sufficientK)
{
	srand(time(0));
	//secretM = 10;
	//participantN = 10;
	//sufficientK = 6;
	int primeP = 13; // secretM + 1;

	bool prime(long long);
	while (primeP != 0)
	{
		if (prime(primeP))
			break;
		else
			primeP++;
	}
	const int power = sufficientK - 1;

	// x^5 + 8x^4 + 6x^3 + 5x^2 + 10x
	vector<int> arrayA = { 1, 8, 6, 5, 10 };

	vector<int> arrayK;
	for (int i = 0; i < participantN; i++)
	{
		int temp = 0;
		for (int j = 0; j < power; j++)
			temp += arrayA[j] * (pow(i + 1, power - j));
		arrayK.insert(arrayK.end(), (temp + secretM) % primeP);
	}
	return arrayK;
}

/* Get shadows of secret key (ss_i = coalition[i] * Q)*/
vector<EC_POINT*> keyProj(int* coalition, vector<int> shares, EC_POINT *G, EC_GROUP *curve) {
	vector<EC_POINT*> res;
	for (int i = 0; i < 6; i++) {
		BigNumber c(shares.at(coalition[i] - 1));
		EC_POINT *p = mul(c.bn, G, curve);
		res.insert(res.end(), p);
	}
	return res;
}

void getHash() {
	char ibuf[] = "compute sha1";
	unsigned char obuf[20];
	SHA1((unsigned char*)ibuf, strlen(ibuf), obuf);
	for (int i = 0; i < 20; i++) {
		printf("%02x ", obuf[i]);
	}
	printf("\n");
}

class EPoint {
	public:
		EPoint() {

		}
		EPoint(BigNumber x, BigNumber y) {

		}
		EPoint(BigNumber x, BigNumber y, EC_GROUP *curve) {
			this->curve = curve;
			this->x = x;
			this->y = y;
			P = EC_POINT_new(curve);
			if (1 != EC_POINT_set_affine_coordinates_GFp(curve, P, this->x.bn, this->y.bn, NULL)) return;//handleErrors();
		}
		EPoint(EC_POINT *P) {
			this->P = P;
		}
		void setCurve(EC_GROUP *curve) {
			this->curve = curve;
		}
		void print() {
			std::cout << "(" << x.decimal() << " : " << y.decimal() << ")" << endl;
		}
		BigNumber x;
		BigNumber y;
	private:
		EC_GROUP *curve;
		EC_POINT *P;


};
BigNumber operator + (BigNumber a, BigNumber b)
{
	BIGNUM *r = BN_new();
	if (!BN_add(r, a.bn, b.bn))
		return NULL;
	return BigNumber(r);
}

BigNumber operator - (BigNumber a, BigNumber b)
{
	BIGNUM *r = BN_new();
	if (!BN_sub(r, a.bn, b.bn))
		return NULL;
	return BigNumber(r);
}

BigNumber operator * (BigNumber a, BigNumber b)
{
	BIGNUM *r = BN_new();
	BN_CTX *ctx1;
	if (ctx1 = BN_CTX_new())
		if (BN_mul(r, a.bn, b.bn, ctx1)) {
			BN_CTX_free(ctx1);
			return BigNumber(r);
		}
	BN_CTX_free(ctx1);
	return NULL;
}

BigNumber operator / (BigNumber a, BigNumber b)
{
	BIGNUM *r = BN_new();
	BN_CTX *ctx1;
	if (ctx1 = BN_CTX_new())
		if (!BN_div(r, NULL, a.bn, b.bn, ctx)){
			BN_CTX_free(ctx1);
		return BigNumber(r);
		}
	BN_CTX_free(ctx1);
return NULL;
}

BigNumber operator % (BigNumber a, BigNumber b)
{
	BIGNUM *r = BN_new();
	BN_CTX *ctx1;
	if (ctx1 = BN_CTX_new())
		if (!BN_div(NULL, r, a.bn, b.bn, ctx)){
				BN_CTX_free(ctx1);
			return BigNumber(r);
		}
	BN_CTX_free(ctx1);
	return NULL;
}

int operator == (BigNumber a, BigNumber b)
{
	int res = BN_cmp(a.bn, b.bn);
	if (res == 0)
		return 1;
	else
		return 0;
}

void printBN(char* desc, BIGNUM * bn);
void handleErrors()
{
	printf("\n%s\n", ERR_error_string(ERR_get_error(), NULL));
	//system("pause");
	//abort();
}

EC_GROUP *create_curve(void)
{
	//BN_CTX *ctx;
	EC_GROUP *curve;
	BIGNUM *a, *b, *p, *order, *x, *y;
	EC_POINT *generator;

	/* инициализируем параметры кривой */
	unsigned char a_bin[1] =
	{ 0x19 };
	unsigned char b_bin[2] =
	{ 0x03, 0xD2 };
	unsigned char p_bin[2] =
	{ 0x04, 0xC7 };
	unsigned char order_bin[2] =
	{ 0x04, 0x9F };

	// G = (1158, 92)
	//unsigned char x_bin[2] =
	//{ 0x04, 0x86 };
	//unsigned char y_bin[1] =
	//{ 0x5C };

	// G0 = (972, 795)
	unsigned char x_bin[2] =
	{ 0x03, 0xCC };
	unsigned char y_bin[2] =
	{ 0x03, 0x1B };

	/* Set up the BN_CTX */
	//if (NULL == (ctx = BN_CTX_new())) handleErrors();

	/* Set the values for the various parameters */
	if (NULL == (a = BN_bin2bn(a_bin, 1, NULL))) handleErrors();
	if (NULL == (b = BN_bin2bn(b_bin, 2, NULL))) handleErrors();
	if (NULL == (p = BN_bin2bn(p_bin, 2, NULL))) handleErrors();
	if (NULL == (order = BN_bin2bn(order_bin, 2, NULL))) handleErrors();
	if (NULL == (x = BN_bin2bn(x_bin, 2, NULL))) handleErrors();
	if (NULL == (y = BN_bin2bn(y_bin, 2, NULL))) handleErrors();

	printBN("a: ", a);
	printBN("b: ", b);
	printBN("p: ", p);
	printBN("gx: ", x);
	printBN("gy: ", y);
	printBN("order: ", order);
	/* Create the curve */
	if (NULL == (curve = EC_GROUP_new_curve_GFp(p, a, b, ctx))) handleErrors();

	/* Create the generator */
	if (NULL == (generator = EC_POINT_new(curve))) handleErrors();
	if (1 != EC_POINT_set_affine_coordinates_GFp(curve, generator, x, y, ctx))
		handleErrors();

	/* Set the generator and the order */
	if (1 != EC_GROUP_set_generator(curve, generator, order, NULL))
		handleErrors();

	EC_POINT_free(generator);
	BN_free(y);
	BN_free(x);
	BN_free(order);
	BN_free(p);
	BN_free(b);
	BN_free(a);
	//BN_CTX_free(ctx);

	return curve;
}

EC_KEY * create_private_key(BIGNUM *bn, EC_GROUP *curve) {

	//EC_GROUP *group;
	//BIGNUM *bn = NULL;
	EC_KEY *key = NULL;

	if (!(key = EC_KEY_new())) {
		printf("EC_KEY_new\n");
		printf("%s\n", ERR_error_string(ERR_get_error(), NULL));
		return NULL;
	}

	EC_GROUP_set_point_conversion_form(curve, POINT_CONVERSION_COMPRESSED);

	if (EC_KEY_set_group(key, curve) != 1) {
		printf("EC_KEY_set_group\n");
		printf("%s\n", ERR_error_string(ERR_get_error(), NULL));
		EC_GROUP_free(curve);
		EC_KEY_free(key);
		return NULL;
	}

	//EC_GROUP_free(curve);

	//if (NULL == (bn = BN_bin2bn(hex, 1, NULL))) handleErrors();

	if (EC_KEY_set_private_key(key, bn) != 1) {
		printf("EC_KEY_set_private_key\n");
		printf("%s\n", ERR_error_string(ERR_get_error(), NULL));
		EC_KEY_free(key);
		BN_free(bn);
		return NULL;
	}

	//BN_free(bn);

	return key;
}

char * ecies_key_private_get_hex(EC_KEY *key) {

	char *hex;
	const BIGNUM *bn;

	if (!(bn = EC_KEY_get0_private_key(key))) {
		printf("EC_KEY_get0_private_key\n");
		printf("%s\n", ERR_error_string(ERR_get_error(), NULL));
		return NULL;
	}

	if (!(hex = BN_bn2hex(bn))) {
		printf("BN_bn2hex\n");
		printf("%s\n", ERR_error_string(ERR_get_error(), NULL));
		return NULL;
	}

	//printf("PRIV: %s\n", hex);
	return hex;
}

void printBN(char* desc, BIGNUM * bn) {
	fprintf(stdout, "%s", desc);
	BN_print_fp(stdout, bn);
	fprintf(stdout, "\n", desc);
}
void printPoint(EC_POINT *P, EC_GROUP *curve) {
	BigNumber x;
	BigNumber y;
	if (!EC_POINT_get_affine_coordinates_GFp(curve, P, x.bn, y.bn, NULL)) handleErrors();
	std::cout << "(" << x.decimal() << " : " << y.decimal() << ")" << endl;
}

EC_POINT *mul(BigNumber n, EC_POINT *P, EC_GROUP *curve) {
	EC_POINT *R = EC_POINT_new(curve);
	if (1 != EC_POINT_mul(curve, R, NULL, P, n.bn, NULL)) handleErrors();
	return R;
}

EC_POINT *createMPK(BigNumber msk, EC_POINT *P, EC_GROUP *curve) {
	EC_POINT *R = mul(msk, P, curve);
	return R;
}

int main(int argc, char ** argv)
{
	//OpenSSL_add_all_algorithms();
	//ERR_load_BIO_strings();
	//ERR_load_crypto_strings();

	if (NULL == (ctx = BN_CTX_new())) handleErrors();

	EC_GROUP *curve1;
	if (NULL == (curve1 = create_curve()))
		std::cout << "error" << "\r\n";
	
	if (1 != EC_GROUP_check(curve1, NULL)) handleErrors();
	// OpenSSL curve test
	BigNumber msk(10);
	printBN("MSK: ", msk.bn);

	// Chech Q = rG
	BigNumber gx(1158);
	BigNumber gy(92);
	BigNumber g0x(972);
	BigNumber g0y(795);
	EC_POINT *G0 = EC_POINT_new(curve1);
	if (1 != EC_POINT_set_affine_coordinates_GFp(curve1, G0, g0x.bn, g0y.bn, NULL)) handleErrors();
	EC_POINT *G = EC_POINT_new(curve1);
	if (1 != EC_POINT_set_affine_coordinates_GFp(curve1, G, gx.bn, gy.bn, NULL)) handleErrors();
	
	std::cout << "G0: ";
	printPoint(G0, curve1);
	EC_POINT *MPK = createMPK(msk, G0, curve1);

	std::cout << "MPK: ";
	printPoint(MPK, curve1);

	std::cout << "\r\n      PKG keys generation \r\n";

	// Вычисляем публичный ключ на этот сеанс
	// Второй генератор G (gens[1], 1158, 92)
	// Соучайный элемент из поля q = 13 (поля порядка gens[1])
	// TODO: set_random_seed(LPoSID + KblockID)
	int KblockID = 123;
	int LPoSID = 677321;
	// r следует брать соучайно r = ZZ.random_element(q)
	BigNumber r(9);
	std::cout << "Random r: " << r.decimal() << endl;

	EC_POINT *Q = mul(r, G, curve1);
	//if (!EC_POINT_get_affine_coordinates_GFp(curve1, Q, gx.bn, gy.bn, NULL)) handleErrors();
	std::cout << "Q = r * G: ";
	printPoint(Q, curve1);
	
	std::cout << "Key sharing: " << endl;

	vector<int> shares = shamir(msk.decimal(), 10, 6);

	for (int i = 0; i < shares.size(); i++)
		std::cout << "(" << shares[i] << "), ";
	
	int coalition[] = { 1,3,5,7,9,10 };
	std::cout << "\r\nShadows: " << "\r\n";
	vector<EC_POINT*> proj = keyProj(coalition, shares, Q, curve1);
	for (int i = 0; i < 6; i++) {
		printPoint(proj[i], curve1);
	}

	std::cout << "\r\nKey recovery" << "\r\n";
	EC_POINT *secret = keyRecovery(proj, coalition, 13, curve1);

	std::cout << "\r\nRecovered secret SK: ";
	printPoint(secret, curve1);
	
	std::cout << "\r\nCheck secret MSK * Q: ";
	EC_POINT *check = mul(msk, Q, curve1);
	printPoint(check, curve1);

	std::cout << "\r\n      Create signature" << endl;

	BigNumber M(200);
	BigNumber r2(7);
	EC_POINT *s1;
	// R = rP
	s1 = mul(r2, G0, curve1);

	std::cout << "S1: ";
	printPoint(s1, curve1);

	// set_random_seed(LPoSID+M)
	// H = E.random_point()
	// Тут хеширование, но пока берется "случайная" точка кривой
	BigNumber hx(681);
	BigNumber hy(256);
	EC_POINT *H = EC_POINT_new(curve1);
	if (1 != EC_POINT_set_affine_coordinates_GFp(curve1, H, hx.bn, hy.bn, NULL)) handleErrors();


	// S2 = r*H + SecKey
	// S = sQ + rH
	EC_POINT *s2 = mul(r2, H, curve1);
	if (1 != EC_POINT_add(curve1, s2, s2, secret, NULL)) handleErrors();
	
	std::cout << "S2: ";
	printPoint(s2, curve1);

	BN_CTX_free(ctx);
	EC_POINT_free(secret);
	EC_POINT_free(Q);
	EC_POINT_free(MPK);
	EC_POINT_free(G);
	EC_POINT_free(G0);
	EC_POINT_free(check);
	EC_POINT_free(s1);
	EC_POINT_free(s2);
	EC_POINT_free(H);
	EC_GROUP_free(curve1);

/*

	std::cout << "      Verification" << "\r\n";
	std::cout << "Weil pairing" << "\r\n";
	// Спаривания...

	EPoint *S = new EPoint(BigNumber(0), BigNumber(522));
	S->setCurve(&curve);
	s2.setCurve(&curve);
	BigNumber r1 = weilPairing(g0, s2, *S);
	std::cout << "r1 = e(P, S)\t" << r1.toString() << "\r\n";

	BigNumber b1 = weilPairing(mpk, Q, *S);
	std::cout << "b1 = e(MPK, Q)\t" << b1.toString() << "\r\n";

	BigNumber c1 = weilPairing(s1, H, *S);
	std::cout << "c1 = e(R, H1)\t" << c1.toString() << "\r\n";

	BigNumber b1c1 = b1 * c1;
	std::cout << "r1 = b1 * c1\t" << b1c1.toString() << "\r\n";
	cout << "copies: " << cntr << endl;
	*/
	system("pause");
	return 0;
}
/*
BigNumber g(EPoint P, EPoint Q, const BigNumber &x1, const BigNumber &y1) {
	BigNumber a(a);
	BigNumber slope;
	if ((P.x == Q.x) && ((P.y + Q.y) == 0)) {
		return (x1 - P.x);
	}
	if (((P.x == Q.x) && (P.y == Q.y))) {
		slope = (BigNumber(3) * (P.x * P.x) + a) / (P.y + P.y);
	}
	else {
		slope = (P.y - Q.y) / (P.x - Q.x);
	}
	return (y1 - P.y - (slope * (x1 - P.x))) / (x1 + P.x + Q.x - (slope * slope));
}

BigNumber miller(string m, EPoint P, BigNumber x1, BigNumber y1) {
	EPoint T = P;
	BigNumber f(1);
	for (int i = 0; i < m.size(); i++) {
		f = f * (f * g(T, T, x1, y1));
		T = (T.mul(BigNumber(2), T));
		if (m[i] == '1') {
			f = f * g(T, P, x1, y1);
			T = (T.add(T, P));
		}
		int qq = 1;
	}
	return f;
}

BigNumber evalMiller(EPoint P, EPoint Q) {
	// Порядок подгруппы G0 в двоичном представлении без первого бита (???)
	// TODO: Перевод в бинарную строку
	string m = "011011";
	BigNumber res = miller(m, P, Q.x, Q.y);
	return res;
}

BigNumber weilPairing(EPoint P, EPoint Q, EPoint S) {

	//int num = eval_miller(P, Q + S) / eval_miller(P, S);
	//int den = eval_miller(Q, P - S) / eval_miller(Q, -S)
	//return (num / den)

	BigNumber num = ((evalMiller(P, Q.add(Q, S))) / (evalMiller(P, S)));
	BigNumber den = ((evalMiller(Q, P.sub(P, S))) / (evalMiller(Q, S.sub(EPoint(BigNumber(0), BigNumber(0)), S))));
	return (num / den);
}
*/
bool prime(long long n)
{
	for (long long i = 2; i <= sqrt(n); i++)
		if (n%i == 0)
			return false;
	return true;
}