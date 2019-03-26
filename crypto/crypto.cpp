
#include <iostream>
#include <bitset>
#include "crypto.h"
using namespace std;
BN_CTX *ctx = BN_CTX_new();

BigNumber operator + (const BigNumber &a, const BigNumber &b)
{
	BIGNUM *r = BN_new();
	if (!BN_add(r, a.bn, b.bn))
		return nullptr;
	return BigNumber(r);
}
BigNumber operator - (const BigNumber &a, const BigNumber &b)
{
	BIGNUM *r = BN_new();
	if (!BN_mod_sub(r, a.bn, b.bn, BigNumber(1223).bn, ctx))
		return nullptr;
	return BigNumber(r);
}
BigNumber operator * (const BigNumber &a, const BigNumber &b)
{
	BIGNUM *r = BN_new();
	if (BN_mul(r, a.bn, b.bn, ctx)) {
		return BigNumber(r);
	}
	return nullptr;
}
//BigNumber operator / (const BigNumber &a, const BigNumber &b)
//{
//	BigNumber res;
//	BIGNUM *r = BN_new();
//	if (BN_mod_inverse(r, b.bn, BigNumber(1223).bn, ctx))
//		if (BN_mod_mul(r, r, a.bn, BigNumber(1223).bn, ctx))
//			return BigNumber(r);
//	return nullptr;
//}
BigNumber operator % (const BigNumber &a, const BigNumber &b)
{
	BIGNUM *r = BN_new();
	if (BN_div(NULL, r, a.bn, b.bn, ctx)) {
		return BigNumber(r);
	}
	return nullptr;
}
int operator == (const BigNumber &a, const BigNumber &b)
{
	return BN_cmp(a.bn, b.bn);
	//if (res == 0)
	//	return 1;
	//else
	//	return 0;
}

/* @brief Modulo operation
	@param BigNumber nom
	@param BigNumber den
	@param BigNumber mod
*/
BigNumber mdiv(BigNumber nom, BigNumber den, BigNumber mod) {
	BIGNUM *r = BN_new();
	if (NULL == (BN_mod_inverse(r, den.bn, BigNumber(mod).bn, ctx)))
		handleErrors();
	//printBN("res-inv: ", r);
	if (NULL == (BN_mod_mul(r, nom.bn, r, BigNumber(mod).bn, ctx)))
		handleErrors();
	//printBN("res: ", r);
	return BigNumber(r);
}

//
BigNumber msub(BigNumber a, BigNumber b, BigNumber m) {
	BIGNUM *r = BN_new();
	if (!BN_mod_sub(r, a.bn, b.bn, m.bn, ctx))
		return nullptr;
	return BigNumber(r);
}

EC_POINT* keyRecovery(vector<EC_POINT*> proj, vector<int> coalition, BigNumber q, Curve *curve) {
	EC_POINT *secret = EC_POINT_new(curve->curve);
	EC_POINT *buff = EC_POINT_new(curve->curve);
	for (int i = 0; i < proj.size(); i++) {
		BigNumber lambda(1);
		for (int j = 0; j < proj.size(); j++) {
			if (i != j) {
				//lamb = (lamb * (0-coalition[j]))/(coalition[i]-coalition[j]) % q
				BigNumber nom = msub(BigNumber(0), BigNumber(coalition[j]), q);
				BigNumber den = msub(BigNumber(coalition[i]), BigNumber(coalition[j]), q);
				lambda = (lambda * (mdiv(nom, den, q))) % q;
				//lambda = l % q;//BigNumber(0 - coalition[j]) / BigNumber(coalition[i] - coalition[j])) % q;
				//cout << lambda.dec << endl;
			}
		}
		if ((lambda == BigNumber(0)) == 0)
			continue;

		buff = mul(lambda, proj.at(i), curve);

		if (i == 0) {
			if (!EC_POINT_copy(secret, buff)) handleErrors();
			continue;
		}
		if (!EC_POINT_add(curve->curve, secret, secret, buff, NULL)) handleErrors();
	}
	EC_POINT_free(buff);
	return secret;
}

EC_POINT* hashToPoint(BigNumber hash, Curve *curve) {
	EC_POINT *ret = mul(hash, curve->G, curve);
	return ret;
}

vector<int> generatePoly(int power) {
	//srand(time(0));
	// x^5 + 8x^4 + 6x^3 + 5x^2 + 10x
	vector<int> arrayA = { 10 };
	return arrayA;
}

vector<BigNumber> shamir(BigNumber secretM, vector<int> ids, int participantN, int sufficientK, BigNumber q)
{
	
	//secretM = 10;
	//participantN = 10;
	//sufficientK = 6;

	int power = sufficientK - 1;

	vector<int> arrayA = generatePoly(power);

	vector<BigNumber> arrayK;
	for (int i = 0; i < ids.size(); i++)
	{
		int temp = 0;
		for (int j = 0; j < power; j++)
			temp += arrayA[j] * (pow(ids[i], power - j));
		arrayK.insert(arrayK.end(), (BigNumber(temp) + secretM) % q);
	}
	return arrayK;
}

/* Get shadows of secret key (ss_i = coalition[i] * Q)*/
vector<EC_POINT*> keyProj(vector<int> coalition, vector<BigNumber> shares, EC_POINT *Q, Curve *curve) {
	vector<EC_POINT*> res;
	for (int i = 0; i < coalition.size(); i++) {
		EC_POINT *p = mul(shares[coalition[i] - 1].bn, Q, curve);
		res.push_back(p);
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

void handleErrors()
{
	printf("\n%s\n", ERR_error_string(ERR_get_error(), NULL));
	throw ERR_get_error();
	//system("pause");
	//abort();
}

int test(int a, int b) {
	throw 1;
}

void printBN(char* desc, BIGNUM * bn) {
	fprintf(stdout, "%s", desc);
	BN_print_fp(stdout, bn);
	fprintf(stdout, "\n", desc);
}

void printPoint(EC_POINT *P, Curve *curve) {
	BigNumber x;
	BigNumber y;
	if (!EC_POINT_get_affine_coordinates_GFp(curve->curve, P, x.bn, y.bn, NULL)) handleErrors();
	std::cout << "(" << x.decimal() << " : " << y.decimal() << ")" << endl;
}

EC_POINT *mul(BigNumber n, EC_POINT *P, Curve *curve) {
	EC_POINT *R = EC_POINT_new(curve->curve);
	if (1 != EC_POINT_mul(curve->curve, R, NULL, P, n.bn, NULL)) handleErrors();
	return R;
}

EC_POINT *createMPK(BigNumber msk, EC_POINT *P, Curve *curve) {
	EC_POINT *R = mul(msk, P, curve);
	return R;
}

BigNumber getRandom(BigNumber max) {
	BIGNUM *r = BN_secure_new();
	do {
		if (!BN_rand_range(r, max.bn)) handleErrors();
	} while (BN_is_zero(r));
	return BigNumber(r);
}

BigNumber g(EC_POINT *P, EC_POINT *Q, const BigNumber &x1, const BigNumber &y1, Curve *curve) {
	BigNumber px;
	BigNumber py;
	if (!EC_POINT_get_affine_coordinates_GFp(curve->curve, P, px.bn, py.bn, NULL)) handleErrors();

	BigNumber qx;
	BigNumber qy;
	if (!EC_POINT_get_affine_coordinates_GFp(curve->curve, Q, qx.bn, qy.bn, NULL)) handleErrors();

	BigNumber slope;
	//cout << "px:\t" << px.decimal() << " py:\t" << py.decimal() << endl;
	//cout << "qx:\t" << qx.decimal() << " qy:\t" << qy.decimal() << endl;
	if (((px == qx) == 0) && ((((py + qy) % curve->field) == 0) == 0)) {
		return (x1 - px);
	}
	if (((px == qx) == 0) && ((py == qy) == 0)) {
		slope = mdiv((BigNumber(3) * (px * px) + curve->a), (py + py), curve->field);
	}
	else {
		slope = mdiv((py - qy), (px - qx), curve->field);
	}
	int s = slope.decimal();
	BigNumber num = (y1 - py - (slope * (x1 - px)));
	BigNumber den = (x1 + px + qx - (slope * slope));
	return mdiv(num, den, curve->field);
}

BigNumber miller(string m, EC_POINT *P, const BigNumber &x1, const BigNumber &y1, Curve *curve) {
	EC_POINT *T = EC_POINT_new(curve->curve);
	if (1 != EC_POINT_copy(T, P)) handleErrors();
	BigNumber gret;
	BigNumber f(1);
	for (int i = 0; i < m.size(); i++) {
		f = (f * (f * g(T, T, x1, y1, curve))) % curve->field;
		//T = T + T;
		if (1 != EC_POINT_dbl(curve->curve, T, T, NULL)) handleErrors();
		if (m[i] == '1') {
			f = (f * g(T, P, x1, y1, curve)) % curve->field;
			//T = T + P;
			if (1 != EC_POINT_add(curve->curve, T, T, P, NULL)) handleErrors();
		}
	}
	EC_POINT_free(T);
	return f;
}

BigNumber evalMiller(EC_POINT *P, EC_POINT *Q, Curve *curve) {
	// Порядок подгруппы G0 в двоичном представлении без первого бита (???)
	// TODO: Перевод в бинарную строку
	string m = "011011";
	//std::string binary = std::bitset<32>(91).to_string(); //to binary
	//std::cout << binary << "\n";
	BigNumber x;
	BigNumber y;
	if (!EC_POINT_get_affine_coordinates_GFp(curve->curve, Q, x.bn, y.bn, NULL)) handleErrors();
	BigNumber res = miller(m, P, x, y, curve);
	//cout << "res: " << res.decimal() << endl;
	return res;
}

BigNumber weilPairing(EC_POINT *P, EC_POINT *Q, EC_POINT *S, Curve *curve) {

	//int num = eval_miller(P, Q + S) / eval_miller(P, S);
	//int den = eval_miller(Q, P - S) / eval_miller(Q, -S)
	//return (num / den)
	EC_POINT *QS = EC_POINT_new(curve->curve);
	// QS = Q + S
	if (1 != EC_POINT_add(curve->curve, QS, Q, S, NULL)) handleErrors();
	EC_POINT *invS = EC_POINT_new(curve->curve);
	// invS = -S
	if (1 != EC_POINT_copy(invS, S)) handleErrors();
	if (1 != EC_POINT_invert(curve->curve, invS, NULL)) handleErrors();
	EC_POINT *PS = EC_POINT_new(curve->curve);
	// PS = P - S = P + (-S)
	if (1 != EC_POINT_add(curve->curve, PS, P, invS, NULL)) handleErrors();

	BigNumber nom = mdiv(evalMiller(P, QS, curve), evalMiller(P, S, curve), curve->field);
	BigNumber den = mdiv(evalMiller(Q, PS, curve), evalMiller(Q, invS, curve), curve->field);
	EC_POINT_free(QS);
	EC_POINT_free(invS);
	EC_POINT_free(PS);
	return mdiv(nom, den, curve->field);
}