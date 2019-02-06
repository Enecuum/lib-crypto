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
#include <openssl/obj_mac.h>
#include <openssl/bio.h>
#include "BigNumber.h"
//#pragma comment(lib,"libcrypto.lib")
using namespace std;

struct eccrypt_curve_t curve;
int cntr = 0;
BN_CTX *ctx;

/* Set up the BN_CTX */
/*
class BigNumber {
public:
	BigNumber() {

	}
	BigNumber(const BigNumber &bn) 
	{
		memcpy(rslt, bn.rslt, sizeof(bignum_digit_t[BIGNUM_MAX_DIGITS]));;
		cntr++;
	}
	BigNumber(int number) {
		this->numberDec = number;
		//bignum_digit_t rslt[BIGNUM_MAX_DIGITS];
		char buf[65];
		_itoa(number, buf, 16);
		bignum_fromhex(rslt, buf, BIGNUM_MAX_DIGITS);
	}
	BigNumber(char* number) {
		//this->numberDec = number;
		//bignum_digit_t rslt[BIGNUM_MAX_DIGITS];
		//char buf[65];
		//_itoa(number, buf, 16);
		bignum_fromhex(rslt, number, BIGNUM_MAX_DIGITS);
	}
	BigNumber(string number) {
		//this->numberDec = number;
		//bignum_digit_t rslt[BIGNUM_MAX_DIGITS];
		//char buf[65];
		//_itoa(number, buf, 16);
		bignum_fromhex(rslt, &number[0], BIGNUM_MAX_DIGITS);
	}

	BigNumber(bignum_digit_t number[BIGNUM_MAX_DIGITS]) {
		memcpy(rslt, number, sizeof(bignum_digit_t[BIGNUM_MAX_DIGITS]));
		//*rslt = *number;
	}
	int dec() {
		return this->numberDec;
	}
	bignum_digit_t* hex() {
		return rslt;
	}
	string toString() {
		char buff[ECCRYPT_BIGNUM_DIGITS];
		bignum_tohex(rslt, buff, sizeof(buff), ECCRYPT_BIGNUM_DIGITS);
		string str = string(buff);
		return str;
	}
	char* toChar() {
		char buff[ECCRYPT_BIGNUM_DIGITS];
		bignum_tohex(rslt, buff, sizeof(buff), ECCRYPT_BIGNUM_DIGITS);
		return buff;
	}

private:
	bignum_digit_t rslt[BIGNUM_MAX_DIGITS];
	int numberDec;
};

BigNumber operator + (BigNumber c1, BigNumber c2)
{
	bignum_digit_t *res = c1.hex();
	memcpy(res, c1.hex(), sizeof(bignum_digit_t));
	bignum_madd(res, c2.hex(), curve.m, ECCRYPT_BIGNUM_DIGITS);
	return BigNumber(res);
}

BigNumber operator - (BigNumber c1, BigNumber c2)
{
	bignum_digit_t *res = c1.hex();
	memcpy(res, c1.hex(), sizeof(bignum_digit_t));
	bignum_msub(res, c2.hex(), curve.m, ECCRYPT_BIGNUM_DIGITS);
	return BigNumber(res);
}

BigNumber operator / (BigNumber c1, BigNumber c2)
{
	bignum_digit_t *res = c1.hex();
	memcpy(res, c1.hex(), sizeof(bignum_digit_t));
	bignum_mdiv(res, c2.hex(), curve.m, ECCRYPT_BIGNUM_DIGITS);
	return BigNumber(res);
}

BigNumber operator % (BigNumber c1, BigNumber c2)
{
	bignum_digit_t *res = c1.hex();
	bignum_digit_t *r = c1.hex();
	memcpy(res, c1.hex(), sizeof(bignum_digit_t));
	bignum_div(res, c2.hex(), curve.m, r, ECCRYPT_BIGNUM_DIGITS);
	return BigNumber(r);
}

BigNumber operator * (BigNumber c1, BigNumber c2)
{
	bignum_digit_t *res = c1.hex();
	memcpy(res, c1.hex(), sizeof(bignum_digit_t));
	bignum_mmul(res, c2.hex(), curve.m, ECCRYPT_BIGNUM_DIGITS);
	return BigNumber(res);
}

int operator == (BigNumber c1, BigNumber c2)
{
	int res = bignum_cmp(c1.hex(), c2.hex(), ECCRYPT_BIGNUM_DIGITS);
	if (res == 0)
		return 1;
	else
		return 0;
}

class EPoint {
public:
	EPoint() {
		this->curve = curve;
	}
	EPoint(BigNumber x, BigNumber y) {
		this->x = x;
		this->y = y;
		this->curve = curve;
		is_inf = bignum_iszero(x.hex(), ECCRYPT_BIGNUM_DIGITS) &&
			bignum_iszero(y.hex(), ECCRYPT_BIGNUM_DIGITS);
	}

	BigNumber getOrder() {
		return order;
	}

	bignum_digit_t* toDigit(BigNumber b) {
		return b.hex();
	}

	EPoint convert(eccrypt_point_t p) {
		BigNumber x(*p.x);
		BigNumber y(*p.y);
		EPoint res(x, y);
		res.curve = curve;
		return res;
	}
	eccrypt_point_t convert(EPoint P) {
		eccrypt_point_t res;
		bignum_fromhex(res.x, &P.x.toString()[0], ECCRYPT_BIGNUM_DIGITS);
		bignum_fromhex(res.y, &P.y.toString()[0], ECCRYPT_BIGNUM_DIGITS);
		res.is_inf = is_inf;
		return res;
	}
	EPoint mul(BigNumber n, EPoint P) {
		eccrypt_point_t rslt;
		eccrypt_point_t p = P.convert(P);
		eccrypt_point_mul(&rslt, &p, n.hex(), curve);
		return convert(rslt);
	}
	EPoint add(EPoint a, EPoint b) {
		eccrypt_point_t rslt;
		eccrypt_point_t ac = a.convert(a);
		eccrypt_point_t bc = b.convert(b);
		eccrypt_point_add(&rslt, &ac, &bc, curve);
		return convert(rslt);
	}
	EPoint sub(EPoint a, EPoint b) {
		eccrypt_point_t rslt;
		//BigNumber m(-1);
		// get -b
		//EPoint *reverse = mul(BigNumber(-1), b);

		// P * -1 = размер поля минус Py ???
		eccrypt_point_t ac = a.convert(a);
		eccrypt_point_t bc = b.convert(b);
		//ac.is_inf = 1;
		bignum_digit_t q[BIGNUM_MAX_DIGITS];// = curve->m;
		memcpy(q, curve->m, sizeof(bignum_digit_t));
		bignum_sub(q, bc.y, ECCRYPT_BIGNUM_DIGITS);
		memcpy(bc.y, q, sizeof(bignum_digit_t));
		//*bc.y = *q;
		eccrypt_point_add(&rslt, &ac, &bc, curve);
		return convert(rslt);
	}
	void setCurve(eccrypt_curve_t *curve) {
		this->curve = curve;
	}
	BigNumber x, y;
private:
	eccrypt_curve_t *curve;
	BigNumber order;
	int is_inf;
};

BigNumber weilPairing(EPoint P, EPoint Q, EPoint S);

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

EPoint keyRecovery(vector<EPoint> proj, int* coalition, int q) {
	EPoint secret(BigNumber(0), BigNumber(0));
	secret.setCurve(&curve);
	//bignum_fromhex(secret.x, "0", ECCRYPT_BIGNUM_DIGITS);
	//bignum_fromhex(secret.y, "0", ECCRYPT_BIGNUM_DIGITS);
	//secret.is_inf = 0;

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
		EPoint buff;// = new EPoint();
		buff = (proj.at(i).mul(BigNumber(lambda), proj.at(i)));
		buff.setCurve(&curve);

		// Ну потому что точка 0.0 это не настоящий прям ноль, поэтому костылек
		if (i == 0) {
			memcpy(&secret, &buff, sizeof(EPoint));
			//eccrypt_point_cpy(&secret, &secBuf);
			continue;
		}
		secret = (secret.add(secret, buff));
		secret.setCurve(&curve);
	}
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

vector<EPoint> keyProj(int* coalition, vector<int> shares, EPoint *G) {
	vector<EPoint> res;
	for (int i = 0; i < 6; i++) {
		BigNumber c(shares.at(coalition[i] - 1));
		EPoint p = G->mul(c, *G);

		p.setCurve(&curve);
		//eccrypt_point_mul(&buff, &G, c, &curve);
		res.insert(res.end(), p);
	}
	return res;
}

void getHash() {
	char ibuf[] = "compute sha1";
	unsigned char obuf[20];

	SHA1((unsigned char*)ibuf, strlen(ibuf), obuf);

	int i;
	for (i = 0; i < 20; i++) {
		printf("%02x ", obuf[i]);
	}
	printf("\n");
}

*/

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
	if (!BN_div(r, NULL, a.bn, b.bn, ctx))
		return NULL;
	return BigNumber(r);
}

BigNumber operator % (BigNumber a, BigNumber b)
{
	BIGNUM *r = BN_new();
	if (!BN_div(NULL, r, a.bn, b.bn, ctx))
		return NULL;
	return BigNumber(r);
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

	/* Binary data for the curve parameters */
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
	fprintf(stdout, "\n%s", desc);
	BN_print_fp(stdout, bn);
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
	BigNumber msk(9);
	printBN("bn_key: ", msk.bn);

	// Chech Q = rG
	BigNumber gx(1158);
	BigNumber gy(92);

	EC_POINT *Q = EC_POINT_new(curve1);

	//if (NULL == (rand = EC_POINT_new(curve1))) handleErrors();
	//if (NULL == (EC_POINT_is_on_curve(curve1, rand, ctx))) handleErrors();
	if (NULL == (EC_POINT_is_on_curve(curve1, Q, ctx))) handleErrors();

	if (1 != EC_POINT_set_affine_coordinates_GFp(curve1, Q, gx.bn, gy.bn, NULL)) handleErrors();
	std::cout << "Q before mul" << endl;
	std::cout << "(" << gx.decimal() << " : " << gy.decimal() << ")" << endl;
	
	if (1 != EC_POINT_mul(curve1, Q, NULL, Q, msk.bn, NULL)) handleErrors();
	if (!EC_POINT_get_affine_coordinates_GFp(curve1, Q, gx.bn, gx.bn, NULL)) handleErrors();
	std::cout << "Q = r * G" << endl;
	std::cout << "(" << gx.decimal() << " : " << gy.decimal() << ")" << endl;


	BigNumber a(6);
	BigNumber b(3);
	std::cout << "a + b = " << (a + b).decimal() << endl;
	std::cout << "a - b = " << (a - b).decimal() << endl;
	std::cout << "a * b = " << (a * b).decimal() << endl;
	std::cout << "a / b = " << (a / b).decimal() << endl;
	std::cout << "a % b = " << (a % b).decimal() << endl;
	std::cout << "a == b = " << (a == b) << endl;
	//if (1 != EC_KEY_set_group(myecc, curve1)) handleErrors();
	//if (1 != EC_KEY_generate_key(myecc)) handleErrors();

	//myecc = create_private_key(key, curve1);
	//int len = EC_KEY_priv2buf(key1, check);
	//check = ecies_key_private_get_hex(myecc);
	/* инициализируем параметры кривой */
	BN_CTX_free(ctx);


	
//	BN_free(gx);
//	BN_free(gy);
	//BN_free(bn_key);
	int eee = 1;
	// Точка-генератор пока задаается вручную
	// Надо считать порядок кривой по Шуфу
/*
	bignum_fromhex(curve.a, a, ECCRYPT_BIGNUM_DIGITS);
	bignum_fromhex(curve.b, b, ECCRYPT_BIGNUM_DIGITS);
	bignum_fromhex(curve.m, p, ECCRYPT_BIGNUM_DIGITS);
	bignum_fromhex(curve.g.x, gx, ECCRYPT_BIGNUM_DIGITS);
	bignum_fromhex(curve.g.y, gy, ECCRYPT_BIGNUM_DIGITS);
	curve.g.is_inf = 0;

	// Точка P - генератор порядка 91 (G0, gens[0])
	// (972, 795)

	BigNumber k(10);

	EPoint g0(972, 795);
	g0.setCurve(&curve);

	EPoint mpk;

	std::cout << "MPK = MSK * G0" << "\r\n";
	mpk = g0.mul(k, g0);
	std::cout << mpk.x.toString() << " " << mpk.y.toString() << "\r\n";

	std::cout << "\r\n      PKG keys generation \r\n";
	// Вычисляем публичный ключ на этот сеанс
	// Второй? генератор G (gens[1], 1158, 92)
	// Соучайный элемент из поля q = 13 (поля порядка gens[1])

	// TODO: set_random_seed(LPoSID + KblockID)
	int KblockID = 123;
	int LPoSID = 677321;
	// r следует брать соучайно r = ZZ.random_element(q)
	BigNumber* r = new BigNumber(9);
	EPoint Q;
	EPoint G(BigNumber("486"), BigNumber("5c"));
	G.setCurve(&curve);
	Q = G.mul(*r, G);
	Q.setCurve(&curve);

	std::cout << "Q = r * G " << "\r\n";
	std::cout << Q.x.toString() << " " << Q.y.toString() << "\r\n";

	//
	// Сборка ключа
	//

	std::cout << "Key sharing: " << "\r\n";

	vector<int> shares = shamir(k.dec(), 10, 6);
	for (int i = 0; i < shares.size(); i++)
		std::cout << "(" << shares[i] << "), ";
	std::cout << "\r\nKey recovery" << "\r\n";

	int coalition[] = { 1,3,5,7,9,10 };
	std::cout << "Shadows: " << "\r\n";
	vector<EPoint> proj = keyProj(coalition, shares, &Q);
	for (int i = 0; i < 6; i++) {
		std::cout << "(" << proj[i].x.toString() << ", " << proj[i].y.toString() << "), ";
	}

	std::cout << "\r\nRecovered secret SK" << "\r\n";

	EPoint secret = keyRecovery(proj, coalition, 13);
	std::cout << secret.x.toString() << " " << secret.y.toString() << "\r\n";

	// Проверка полученного секрета
	std::cout << "Check SK = MSK * Q " << "\r\n";
	EPoint check = Q.mul(k, Q);
	std::cout << check.x.toString() << " " << check.y.toString() << "\r\n";

	//
	//	Постановка подписи
	//

	std::cout << "\r\n      Create signature" << "\r\n";

	BigNumber M(200);
	BigNumber *r2 = new BigNumber(7);
	EPoint s1;
	// R = rP
	s1 = g0.mul(*r2, g0);
	s1.setCurve(&curve);
	std::cout << "S1: " << s1.x.toString() << " " << s1.y.toString() << "\r\n";

	// set_random_seed(LPoSID+M)
	// H = E.random_point()
	// Тут хеширование, но пока берется "случайная" точка кривой

	EPoint H(681, 256);
	H.setCurve(&curve);
	// S2 = r*H + SecKey
	EPoint s2;
	// S = sQ + rH
	s2 = H.mul(*r2, H);
	s2.setCurve(&curve);
	s2 = secret.add(s2, secret);
	std::cout << "S2: " << s2.x.toString() << " " << s2.y.toString() << "\r\n";

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

bool prime(long long n)
{
	for (long long i = 2; i <= sqrt(n); i++)
		if (n%i == 0)
			return false;
	return true;
}

*/