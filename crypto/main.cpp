#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE  

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "eccrypt.h"
#include "bignum.h"
#include "ec_conf.h"
#include <iostream>
#include <vector>
#include <array>
#include <ctime>
#include <string> 
#include <math.h> 
//#include <openssl/obj_mac.h>
//#include <openssl/ec.h>
//#include <openssl/sha.h>
//#pragma comment(lib,"libcrypto.lib")
using namespace std;
struct eccrypt_curve_t curve;



class BigNumber {
public:
	BigNumber() {

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
		
		EPoint* convert(eccrypt_point_t p) {
			BigNumber x(*p.x);
			BigNumber y(*p.y);
			EPoint *res = new EPoint(x, y);
			res->curve = curve;
			return res;
		}
		eccrypt_point_t convert(EPoint P) {
			eccrypt_point_t res;
			bignum_fromhex(res.x, &P.x.toString()[0], ECCRYPT_BIGNUM_DIGITS);
			bignum_fromhex(res.y, &P.y.toString()[0], ECCRYPT_BIGNUM_DIGITS);
			res.is_inf = is_inf;
			return res;
		}
		EPoint* mul(BigNumber n, EPoint P) {
			eccrypt_point_t rslt;
			eccrypt_point_t p = P.convert(P);
			eccrypt_point_mul(&rslt, &p, n.hex(), curve);
			return convert(rslt);
		}
		EPoint* add(EPoint a, EPoint b) {
			eccrypt_point_t rslt;
			eccrypt_point_t ac = a.convert(a);
			eccrypt_point_t bc = b.convert(b);
			eccrypt_point_add(&rslt, &ac, &bc, curve);
			return convert(rslt);
		}
		EPoint* sub(EPoint a, EPoint b) {
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
	if (down < 0){
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
		buff = *(proj.at(i).mul(BigNumber(lambda), proj.at(i)));
		buff.setCurve(&curve);

		// Ну потому что точка 0.0 это не настоящий прям ноль, поэтому костылек
		if (i == 0) {
			memcpy(&secret, &buff, sizeof(EPoint));
			//eccrypt_point_cpy(&secret, &secBuf);
			continue;
		}
		secret = *(secret.add(secret, buff));
		secret.setCurve(&curve);
		//SecKey = SecKey + lambda * proj[i]
	}
	return secret;
}

vector<int> shamir(int secretM, int participantN, int sufficientK)
{
	srand(time(0));
	//int secretM;
	//int participantN;
	//int sufficientK;
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
	vector<int> arrayA = {1, 8, 6, 5, 10};

	//for (int i = 0; i < power; i++)
		//arrayA.insert(arrayA.end(), rand() % 20 + 1);

	vector<int> arrayK;
	for (int i = 0; i < participantN; i++)
	{
		int temp = 0;
		for (int j = 0; j < power; j++)
		{
			temp += arrayA[j] * (pow(i + 1, power - j));
		}
		arrayK.insert(arrayK.end(), (temp + secretM) % primeP);
	}
	//cout << "threshold scheme: (" << sufficientK << ", " << participantN << ")" << endl; //пороговая схема
	//cout << "prime number: " << primeP << ", the degree of the polynomial: " << power << endl;
	//for (int i = 0; i<participantN; i++)
		//cout << i + 1 << " participant. personal secret: " << arrayK[i] << endl;
	//system("pause");
	return arrayK;
}

vector<EPoint> keyProj(int* coalition, vector<int> shares, EPoint *G) {
	vector<EPoint> res;
	for (int i = 0; i < 6; i++) {
		BigNumber c(shares.at(coalition[i] - 1));
		EPoint *p = G->mul(c, *G);

		p->setCurve(&curve);
		//eccrypt_point_mul(&buff, &G, c, &curve);
		res.insert(res.end(), *p);
	}
	return res;
}
/*
void hash() {
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
int main(int argc, char ** argv)
{
	// OpenSSL curve test
	/*
	EC_GROUP *curve1;

	if (NULL == (curve1 = EC_GROUP_new_by_curve_name(NID_secp224r1)))
		std::cout << "error" << "\r\n";
	*/
	/* инициализируем параметры кривой */

	// Точка-генератор пока задаается вручную
	// Надо считать порядок кривой по Шуфу

	bignum_fromhex(curve.a, a, ECCRYPT_BIGNUM_DIGITS);
	bignum_fromhex(curve.b, b, ECCRYPT_BIGNUM_DIGITS);
	bignum_fromhex(curve.m, p, ECCRYPT_BIGNUM_DIGITS);
	bignum_fromhex(curve.g.x, gx, ECCRYPT_BIGNUM_DIGITS);
	bignum_fromhex(curve.g.y, gy, ECCRYPT_BIGNUM_DIGITS);
	curve.g.is_inf = 0;

	// Точка P - генератор порядка 91 (G0, gens[0])
	// (972, 795)

	BigNumber* k = new BigNumber(10);

	EPoint* g0 = new EPoint(972, 795);
	g0->setCurve(&curve);

	EPoint *mpk;

	std::cout << "MPK = MSK * G0" << "\r\n";
	mpk = g0->mul(*k, *g0);
	std::cout << mpk->x.toString() << " " << mpk->y.toString() << "\r\n";

	std::cout << "\r\n      PKG keys generation \r\n";
	// Вычисляем публичный ключ на этот сеанс
	// Второй? генератор G (gens[1], 1158, 92)
	// Соучайный элемент из поля q = 13 (поля порядка gens[1])
	
	// TODO: set_random_seed(LPoSID + KblockID)
	int KblockID = 123;
	int LPoSID = 677321;
	// r следует брать соучайно r = ZZ.random_element(q)
	BigNumber* r = new BigNumber(9);
	EPoint *Q;
	EPoint *G = new EPoint(BigNumber("486"), BigNumber("5c"));
	G->setCurve(&curve);
	Q = G->mul(*r, *G);
	Q->setCurve(&curve);

	std::cout << "Q = r * G " << "\r\n";
	std::cout << Q->x.toString() << " " << Q->y.toString() << "\r\n";

	//
	// Сборка ключа
	//

	std::cout << "Key sharing: " << "\r\n";
	
	vector<int> shares = shamir(k->dec(), 10, 6);
	for (int i = 0; i < shares.size(); i++)
		std::cout << shares.at(i) << " ";
	std::cout << "\r\nKey recovery" << "\r\n";

	int coalition[] = { 1,3,5,7,9,10 };
	std::cout << "Shadows: " << "\r\n";
	vector<EPoint> proj = keyProj(coalition, shares, Q);
	for (int i = 0; i < 6; i++) {
		std::cout << "(" << proj[i].x.toString() << ", " << proj[i].y.toString() << "), ";
	}

	std::cout << "\r\nRecovered secret: SK" << "\r\n";

	EPoint secret = keyRecovery(proj, coalition, 13);
	std::cout << secret.x.toString() << " " << secret.y.toString() << "\r\n";

	// Проверка полученного секрета
	std::cout << "Check: SK = MSK * Q " << "\r\n";
	EPoint *check = Q->mul(*k, *Q);
	std::cout << check->x.toString() << " " << check->y.toString() << "\r\n";

	//
	//	Постановка подписи
	//

	std::cout << "\r\n      Create signature" << "\r\n";

	BigNumber M(200);
	BigNumber *r2 = new BigNumber(7);
	EPoint *s1;
	// R = rP
	s1 = g0->mul(*r2, *g0);
	s1->setCurve(&curve);
	std::cout << "S1: " << s1->x.toString() << " " << s1->y.toString() << "\r\n";
	
	// set_random_seed(LPoSID+M)
	// H = E.random_point()
	// Тут хеширование, но пока берется "случайная" точка кривой
	
	EPoint* H = new EPoint(681, 256);
	H->setCurve(&curve);
	// S2 = r*H + SecKey
	EPoint *s2;
	// S = sQ + rH
	s2 = H->mul(*r2, *H);
	s2->setCurve(&curve);
	s2 = secret.add(*s2, secret);
	std::cout << "S2: " << s2->x.toString() << " " << s2->y.toString() << "\r\n";
	
	std::cout << "      Verification" << "\r\n";
	std::cout << "Weil pairing" << "\r\n";
	// Спаривания...
	
	EPoint *S = new EPoint(BigNumber(0), BigNumber(522));
	S->setCurve(&curve);
	s2->setCurve(&curve);
	BigNumber r1(weilPairing(*g0, *s2, *S));
	std::cout << "r1 = e(P, S)\t" << r1.toString() << "\r\n";

	BigNumber b1(weilPairing(*mpk, *Q, *S));
	std::cout << "b1\t" << b1.toString() << "\r\n";

	BigNumber c1(weilPairing(*s1, *H, *S));
	std::cout << "c1\t" << c1.toString() << "\r\n";

	BigNumber b1c1 = b1 * c1;
	std::cout << "r1 = b1 * c1\t" << b1c1.toString() << "\r\n";
	//EPoint *sub = g0->sub(*g0, *S);
	
	//std::cout << "G0 - S: " << sub->x.toString() << " " << sub->y.toString() << "\r\n";
	
	system("pause");
	return 0;
}

BigNumber g(EPoint P, EPoint Q, BigNumber x1, BigNumber y1) {
	BigNumber x_P = P.x;
	BigNumber y_P = P.y;
	BigNumber x_Q = Q.x;
	BigNumber y_Q = Q.y;
	BigNumber a(a);
	BigNumber slope;
	if((x_P == x_Q) && ((y_P + y_Q) == 0)) {
		return (x1 - x_P);
	}
	if (((x_P == x_Q) && (y_P == y_Q))) {
		slope = (BigNumber(3) * (x_P * x_P) + a) / (y_P + y_P);
	}
	else {
		slope = (y_P - y_Q) / (x_P - x_Q);
	}
	BigNumber test = ((x1 - x_P));
	return (y1 - y_P - (slope * (x1 - x_P))) / (x1 + x_P + x_Q - (slope * slope));
}

BigNumber miller(int n, EPoint P, BigNumber x1, BigNumber y1) {
	// TODO: Перевод в бинарную строку
	// Почему-то обрубается первый бит

	char m[] = "011011";

	BigNumber *res = new BigNumber(1);

	EPoint T(BigNumber(1), BigNumber(1));
	T.setCurve(&curve);
	EPoint buf(BigNumber(1), BigNumber(1));
	buf.setCurve(&curve);
	memcpy(&T, &P, sizeof(EPoint));
	BigNumber f(1);
	for (int i = 0; i < n; i++) {
		f = f * (f * g(T, T, x1, y1));
		T = *(T.mul(BigNumber(2), T));
		if (m[i] == '1') {
			f = f * g(T, P, x1, y1);
			T = *(T.add(T, P));
		}
		int qq = 1;
	}
	return f;
}

BigNumber evalMiller(EPoint P, EPoint Q) {
	BigNumber res = miller(6, P, Q.x, Q.y);
	return res;
}

BigNumber weilPairing(EPoint P, EPoint Q, EPoint S) {

	//int num = eval_miller(P, Q + S) / eval_miller(P, S);
	//int den = eval_miller(Q, P - S) / eval_miller(Q, -S)
	//return (num / den)

	BigNumber num = ((evalMiller(P, *Q.add(Q, S))) / (evalMiller(P, S)));
	BigNumber den = ((evalMiller(Q, *P.sub(P, S))) / (evalMiller(Q, *S.sub(EPoint(BigNumber(0), BigNumber(0)) , S))));
	return (num / den);
}

bool prime(long long n)
{
	for (long long i = 2; i <= sqrt(n); i++)
		if (n%i == 0)
			return false;
	return true;
}