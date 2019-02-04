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
#include <openssl/obj_mac.h>
#include <openssl/ec.h>
#include <openssl/sha.h>
#include "main.h"
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
		char buf[65];
		//_itoa(number, buf, 16);
		bignum_fromhex(rslt, number, BIGNUM_MAX_DIGITS);
	}
	BigNumber(string number) {
		//this->numberDec = number;
		//bignum_digit_t rslt[BIGNUM_MAX_DIGITS];
		char buf[65];
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

			// P * -1 = ������ ���� ����� Py ???
			eccrypt_point_t ac = a.convert(a);
			eccrypt_point_t bc = b.convert(b);
			//ac.is_inf = 1;
			bignum_digit_t q[BIGNUM_MAX_DIGITS];// = curve->m;
			memcpy(q, curve->m, sizeof(bignum_digit_t));
			bignum_sub(q, bc.y, ECCRYPT_BIGNUM_DIGITS);
			*bc.y = *q;
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

BigNumber* weilPairing(EPoint P, EPoint Q, EPoint S);

//struct eccrypt_curve_t curve;

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
		eccrypt_point_t secBuf, resBuf, projBuf;
		if (lambda == 0)
			continue;
		EPoint buff;// = new EPoint();
		buff = *(proj.at(i).mul(BigNumber(lambda), proj.at(i)));
		buff.setCurve(&curve);

		// �� ������ ��� ����� 0.0 ��� �� ��������� ���� ����, ������� ��������
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
	//cout << "threshold scheme: (" << sufficientK << ", " << participantN << ")" << endl; //��������� �����
	//cout << "prime number: " << primeP << ", the degree of the polynomial: " << power << endl;
	//for (int i = 0; i<participantN; i++)
		//cout << i + 1 << " participant. personal secret: " << arrayK[i] << endl;
	//system("pause");
	return arrayK;
}

int usage(char* progname) {
	printf("Usage: %s add x1 y1 x2 y2\n"
		"       %s mul x  y  k\n", progname, progname);
	system("pause");
	return 0;
}

vector<EPoint> keyProj(int* coalition, vector<int> shares, EPoint *G) {
	EPoint buff();
	vector<EPoint> res;
	char buffer[256];
	//bignum_digit_t c[ECCRYPT_BIGNUM_DIGITS];
	
	for (int i = 0; i < 6; i++) {
		char share[65];
		BigNumber c(shares.at(coalition[i] - 1));
		//_itoa(shares.at(coalition[i] - 1), share, 16);
		//bignum_fromhex(c, share, ECCRYPT_BIGNUM_DIGITS);
		EPoint *p;
		p = G->mul(c, *G);
		p->setCurve(&curve);
		//eccrypt_point_mul(&buff, &G, c, &curve);
		res.insert(res.end(), *p);
	}
	return res;
}

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

int main(int argc, char ** argv)
{
	// OpenSSL curve test
	EC_GROUP *curve1;

	if (NULL == (curve1 = EC_GROUP_new_by_curve_name(NID_secp224r1)))
		std::cout << "error" << "\r\n";
	/* �������������� ��������� ������ */

	// �����-��������� ���� ��������� �������
	// ���� ������� ������� ������ �� ����

	bignum_fromhex(curve.a, a, ECCRYPT_BIGNUM_DIGITS);
	bignum_fromhex(curve.b, b, ECCRYPT_BIGNUM_DIGITS);
	bignum_fromhex(curve.m, p, ECCRYPT_BIGNUM_DIGITS);
	bignum_fromhex(curve.g.x, gx, ECCRYPT_BIGNUM_DIGITS);
	bignum_fromhex(curve.g.y, gy, ECCRYPT_BIGNUM_DIGITS);
	curve.g.is_inf = 0;

	// ����� P - ��������� ������� 91 (G0, gens[0])
	// (972, 795)

	BigNumber* k = new BigNumber(10);

	EPoint* p3 = new EPoint(972, 795);
	p3->setCurve(&curve);

	EPoint *res;

	std::cout << "MPK = MSK * G0" << "\r\n";
	res = p3->mul(*k, *p3);
	std::cout << res->x.toString() << " " << res->y.toString() << "\r\n";

	std::cout << "\r\n      PKG keys generation \r\n";
	// ��������� ��������� ���� �� ���� �����
	// ������? ��������� G (gens[1], 1158, 92)
	// ��������� ������� �� ���� q = 13 (���� ������� gens[1])
	
	// TODO: set_random_seed(LPoSID + KblockID)
	int KblockID = 123;
	int LPoSID = 677321;
	// r ������� ����� �������� r = ZZ.random_element(q)
	BigNumber* r = new BigNumber(9);
	EPoint *Q;
	EPoint *G = new EPoint(BigNumber("486"), BigNumber("5c"));
	G->setCurve(&curve);
	Q = G->mul(*r, *G);
	Q->setCurve(&curve);

	std::cout << "Q = r * G " << "\r\n";
	std::cout << Q->x.toString() << " " << Q->y.toString() << "\r\n";

	//
	// ������ �����
	//

	std::cout << "Key sharing: " << "\r\n";
	
	vector<int> shares = shamir(10, 10, 6);
	for (int i = 0; i < shares.size(); i++)
		std::cout << shares.at(i) << " ";
	std::cout << "\r\nKey recovery" << "\r\n";

	int coalition[] = { 1,3,5,7,9,10 };
	std::cout << "Shadows: " << "\r\n";
	vector<EPoint> proj = keyProj(coalition, shares, Q);
	for (int i = 0; i < 6; i++) {
		std::cout << "(" << proj[i].x.toString() << ", " << proj[i].y.toString() << "), ";
	}

	std::cout << "\r\nRecovered secret: " << "\r\n";

	EPoint secret = keyRecovery(proj, coalition, 13);
	std::cout << secret.x.toString() << " " << secret.y.toString() << "\r\n";

	// �������� ����������� �������
	std::cout << "Check: MSK * Q " << "\r\n";
	res = Q->mul(*k, *Q);
	std::cout << res->x.toString() << " " << res->y.toString() << "\r\n";

	//
	//	���������� �������
	//

	std::cout << "\r\n      Create signature" << "\r\n";

	BigNumber M(200);
	BigNumber *r2 = new BigNumber(7);
	EPoint *s1;
	s1 = p3->mul(*r2, *p3);
	s1->setCurve(&curve);
	std::cout << "S1: " << s1->x.toString() << " " << s1->y.toString() << "\r\n";
	
	// set_random_seed(LPoSID+M)
	// H = E.random_point()
	// ��� �����������, �� ���� ������� "���������" ����� ������
	
	EPoint* H = new EPoint(681, 256);
	H->setCurve(&curve);
	// S2 = r*H + SecKey
	EPoint *s2;
	s2 = H->mul(*r2, *H);
	s2->setCurve(&curve);
	s2 = secret.add(*s2, secret);
	std::cout << "S2: " << s2->x.toString() << " " << s2->y.toString() << "\r\n";
	
	std::cout << "Verification" << "\r\n";
	
	// ����������...
	/*
	EPoint *S = new EPoint(BigNumber(0), BigNumber(522));
	BigNumber *weil = weilPairing(*p3, *s2, *S);
	EPoint *sub = p3->sub(*p3, *S);
	//std::cout << "weil pairing " << weil.toString() << "\r\n";
	std::cout << "G0 - S: " << sub->x.toString() << " " << sub->y.toString() << "\r\n";
	std::cout << "weil: " << weil->toString() << "\r\n";
	*/
	system("pause");
	return 0;
}
BigNumber* operator + (BigNumber c1, BigNumber c2)
{
	bignum_digit_t *res = c1.hex();
	//memcpy(res, c1.hex(), sizeof(bignum_digit_t));

	bignum_add(res, c2.hex(), ECCRYPT_BIGNUM_DIGITS);
	return &BigNumber(res);
}
/*

BigNumber* g(EPoint P, EPoint Q, BigNumber x1, BigNumber y1) {
	//(x_P, y_P) = P.xy()
		//(x_Q, y_Q) = Q.xy()
		//R, (x, y) = PolynomialRing(GF(p), 'x, y').objgens()
	BigNumber x_P = P.x;
	BigNumber y_P = P.y;
	BigNumber x_Q = Q.x;
	BigNumber y_Q = Q.y;
	if(x_P == x_Q && y_P + y_Q == 0) {
		return x1 - x_P;
	}
	if (P == Q) {
		slope = (3 * x_P ^ 2 + a) / (2 * y_P);
	}
	else {
		slope = (y_P - y_Q) / (x_P - x_Q);
	}
	return (y1 - y_P - slope * (x1 - x_P)) / (x1 + x_P + x_Q - slope ^ 2);
}
*/
BigNumber* miller(int n, EPoint P, BigNumber x1, BigNumber y1) {
	// TODO: ������� � �������� ������
	// ������-�� ���������� ������ ���

	char m[] = "011011";
	//int n = 6;

	//m = bin(m)[3:]
	//	n = len(m)
	//	T = P
	//	f = 1
	//	for i in range(n) :

	//		f = f ^ 2 * g(T, T, x1, y1)
	//		T = T + T
	//		if int(m[i]) == 1 :
	//			f = f * g(T, P, x1, y1)
	//			T = T + P
	//			return f
	// ���������� �������
	BigNumber *res = new BigNumber(1);
	return res;
}

BigNumber* evalMiller(EPoint P, EPoint Q) {

	//(x1, y1) = Q.xy()
	//	return (miller(n, P, x1, y1))
	BigNumber* res = miller(6, P, Q.x, Q.y);
	return res;
}

BigNumber* weilPairing(EPoint P, EPoint Q, EPoint S) {

	//int num = eval_miller(P, Q + S) / eval_miller(P, S);
	//int den = eval_miller(Q, P - S) / eval_miller(Q, -S)
	//return (num / den)

	BigNumber a{ 20 };
	BigNumber b{ 34 };
	BigNumber *num = a + b;
	//BigNumber* num = evalMiller(P, *Q.add(Q, S));

	return num;
}

bool prime(long long n)
{
	for (long long i = 2; i <= sqrt(n); i++)
		if (n%i == 0)
			return false;
	return true;
}

