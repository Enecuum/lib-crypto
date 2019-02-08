#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE  

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
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

int cntr = 0;
BN_CTX *ctx;

BigNumber weilPairing(EC_POINT *P, EC_POINT *Q, EC_POINT *S, EC_GROUP *curve);
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
BigNumber operator + (const BigNumber &a, const BigNumber &b)
{
	BIGNUM *r = BN_new();
	if (!BN_mod_add(r, a.bn, b.bn, BigNumber(1223).bn, ctx))
		return NULL;
	return BigNumber(r);
}

BigNumber operator - (const BigNumber &a, const BigNumber &b)
{
	BIGNUM *r = BN_new();
	if (!BN_mod_sub(r, a.bn, b.bn, BigNumber(1223).bn, ctx))
		return NULL;
	return BigNumber(r);
}

BigNumber operator * (const BigNumber &a, const BigNumber &b)
{
	BIGNUM *r = BN_new();
	if (BN_mod_mul(r, a.bn, b.bn, BigNumber(1223).bn, ctx)) {
		return BigNumber(r);
	}
	return NULL;
}

BigNumber operator / (const BigNumber &a, const BigNumber &b)
{
	BigNumber res;
	BIGNUM *r = BN_new();
	if (BN_mod_inverse(r, b.bn, BigNumber(1223).bn, ctx))
	if (BN_mod_mul(r, r, a.bn, BigNumber(1223).bn, ctx)) 
		return BigNumber(r);
	return NULL;
}

BigNumber operator % (const BigNumber &a, const BigNumber &b)
{
	BIGNUM *r = BN_new();
	if (BN_div(NULL, r, a.bn, b.bn, ctx)){
		return BigNumber(r);
	}
	return NULL;
}

int operator == (const BigNumber &a, const BigNumber &b)
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
	EC_GROUP *curve;
	EC_POINT *G;

	BigNumber a(25);
	BigNumber b(978);
	BigNumber p(1223);
	BigNumber order(1183);
	BigNumber g0x(972);
	BigNumber g0y(795);

	cout << "a: " << a.decimal() << endl;
	cout << "b: " << b.decimal() << endl;
	cout << "p: " << p.decimal() << endl;
	cout << "G0: (" << g0x.decimal() << " " << g0y.decimal() << ")" << endl;
	cout << "order: " << order.decimal() << endl;

	if (NULL == (curve = EC_GROUP_new_curve_GFp(p.bn, a.bn, b.bn, ctx))) handleErrors();

	/* Create the generator */
	if (NULL == (G = EC_POINT_new(curve))) handleErrors();
	if (1 != EC_POINT_set_affine_coordinates_GFp(curve, G, g0x.bn, g0y.bn, ctx))
		handleErrors();

	/* Set the generator and the order */
	if (1 != EC_GROUP_set_generator(curve, G, order.bn, NULL))
		handleErrors();

	EC_POINT_free(G);
	return curve;
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
	//srand(time(0));
	//for (int i = 0; i < 1000; i++) {
		//if ((i % 100) == 0)
			//cout << i << endl;
	//std::cout.setstate(std::ios_base::failbit);
	if (NULL == (ctx = BN_CTX_new())) handleErrors();

	EC_GROUP *curve1;
	if (NULL == (curve1 = create_curve()))
		std::cout << "error" << endl;

	if (1 != EC_GROUP_check(curve1, NULL)) handleErrors();
	// OpenSSL curve test
	BigNumber msk(10);
	cout << "MSK: " << msk.decimal() << endl;

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

	std::cout << "\r\n      Key recovery" << endl;
	EC_POINT *secret = keyRecovery(proj, coalition, 13, curve1);

	std::cout << "Recovered secret SK: \t";
	printPoint(secret, curve1);

	std::cout << "Check secret MSK * Q: \t";
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

	std::cout << "      Verification" << "\r\n";
	std::cout << "Weil pairing" << "\r\n";

	BigNumber sx(0);
	BigNumber sy(522);
	EC_POINT *S = EC_POINT_new(curve1);
	if (1 != EC_POINT_set_affine_coordinates_GFp(curve1, S, sx.bn, sy.bn, NULL)) handleErrors();

	BigNumber r1 = weilPairing(G0, s2, S, curve1);
	std::cout << "r1 = e(P, S)\t" << r1.decimal() << "\r\n";

	BigNumber b1 = weilPairing(MPK, Q, S, curve1);
	std::cout << "b1 = e(MPK, Q)\t" << b1.decimal() << "\r\n";

	BigNumber c1 = weilPairing(s1, H, S, curve1);
	std::cout << "c1 = e(R, H1)\t" << c1.decimal() << "\r\n";

	BigNumber b1c1 = b1 * c1;
	std::cout << "r1 = b1 * c1\t" << b1c1.decimal() << "\r\n";

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
	//}
	//std::cout.clear();
	cout << "runtime = " << clock() / 1000.0 << endl; // время работы программы         
	system("pause");
	return 0;
}

BigNumber g(EC_POINT *P, EC_POINT *Q, const BigNumber &x1, const BigNumber &y1, EC_GROUP *curve) {
	BigNumber px;
	BigNumber py;
	if (!EC_POINT_get_affine_coordinates_GFp(curve, P, px.bn, py.bn, NULL)) handleErrors();

	BigNumber qx;
	BigNumber qy;
	if (!EC_POINT_get_affine_coordinates_GFp(curve, Q, qx.bn, qy.bn, NULL)) handleErrors();

	BigNumber a(25);
	BigNumber slope;

	if ((px == qx) && ((py + qy) == 0)) {
		return (x1 - px);
	}
	if (((px == qx) && (py == qy))) {
		slope = (BigNumber(3) * (px * px) + a) / (py + py);
	}
	else {
		slope = (py - qy) / (px - qx);
	}
	return (y1 - py - (slope * (x1 - px))) / (x1 + px + qx - (slope * slope));
}

BigNumber miller(string m, EC_POINT *P, const BigNumber &x1, const BigNumber &y1, EC_GROUP *curve) {
	EC_POINT *T = EC_POINT_new(curve);
	if (1 != EC_POINT_copy( T, P)) handleErrors();
	
	BigNumber f(1);
	for (int i = 0; i < m.size(); i++) {
		f = f * (f * g(T, T, x1, y1, curve));
		//T = T + T;
		if (1 != EC_POINT_dbl(curve, T, T, NULL)) handleErrors();
		if (m[i] == '1') {
			f = f * g(T, P, x1, y1, curve);
			//T = T + P;
			if (1 != EC_POINT_add(curve, T, T, P, NULL)) handleErrors();
		}
	}
	EC_POINT_free(T);
	return f;
}

BigNumber evalMiller(EC_POINT *P, EC_POINT *Q, EC_GROUP *curve) {
	// Порядок подгруппы G0 в двоичном представлении без первого бита (???)
	// TODO: Перевод в бинарную строку
	string m = "011011";
	BigNumber x;
	BigNumber y;
	if (!EC_POINT_get_affine_coordinates_GFp(curve, Q, x.bn, y.bn, NULL)) handleErrors();
	BigNumber res = miller(m, P, x, y, curve);

	return res;
}

BigNumber weilPairing(EC_POINT *P, EC_POINT *Q, EC_POINT *S, EC_GROUP *curve) {

	//int num = eval_miller(P, Q + S) / eval_miller(P, S);
	//int den = eval_miller(Q, P - S) / eval_miller(Q, -S)
	//return (num / den)
	EC_POINT *QS = EC_POINT_new(curve);
	// QS = Q + S
	if (1 != EC_POINT_add(curve, QS, Q, S, NULL)) handleErrors();
	EC_POINT *invS = EC_POINT_new(curve);
	// invS = -S
	if (1 != EC_POINT_copy(invS, S)) handleErrors();
	if (1 != EC_POINT_invert(curve, invS, NULL)) handleErrors();
	EC_POINT *PS = EC_POINT_new(curve);
	// PS = P - S = P + (-S)
	if (1 != EC_POINT_add(curve, PS, P, invS, NULL)) handleErrors();

	BigNumber num = evalMiller(P, QS, curve) / evalMiller(P, S, curve);
	BigNumber den = evalMiller(Q, PS, curve) / evalMiller(Q, invS, curve);
	EC_POINT_free(QS);
	EC_POINT_free(invS);
	EC_POINT_free(PS);
	return (num / den);
}

bool prime(long long n)
{
	for (long long i = 2; i <= sqrt(n); i++)
		if (n%i == 0)
			return false;
	return true;
}