
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
	std::cout << "(" << x.toDecString() << " : " << y.toDecString() << ")" << endl;
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
	return BigNumber(4);
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
	/*
	  eta = (p^k - 1)/n
	  num = eval_miller(P, Q+S)/eval_miller(P,  S)
	  return (num^eta)
	*/
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

ExtensionField::Element g(ecPoint& P, ecPoint& Q, ExtensionField::Element& x1, ExtensionField::Element& y1, ellipticCurveFq& EF_q) {

	ExtensionField::Element px, py, qx, qy, slope;
	ExtensionField::Element py_plus_qy = NULL, x1_minus_px, negqy;
	px = P.x;
	py = P.y;
	qx = Q.x;
	qy = Q.y;

	EF_q.field->neg(negqy, qy);

	if ((px == qx) && (py == negqy)) {

		EF_q.field->sub(x1_minus_px, x1, px);
		return x1_minus_px;
	}
	if ( P == Q ) {
		ExtensionField::Element px2, _3px2, _2py, _3px2_plus_a;
		EF_q.field->sqr(px2, px);
		EF_q.field->scalarMultiply(_3px2, px2, 3);
		EF_q.field->add(_3px2_plus_a, _3px2, EF_q.ec->A);
		EF_q.field->scalarMultiply(_2py, py, 2);
		EF_q.field->div(slope, _3px2_plus_a, _2py);
	}
	else {
		ExtensionField::Element py_minux_qy, px_minus_qx;
		EF_q.field->sub(py_minux_qy, py, qy);
		EF_q.field->sub(px_minus_qx, px, qx);
		EF_q.field->div(slope, py_minux_qy, px_minus_qx);
	}
	ExtensionField::Element y1_minus_py, slope_x1px, slope2, num, den, num_div_den;

	EF_q.field->sub(x1_minus_px, x1, px);

	EF_q.field->mul(slope_x1px, x1_minus_px, slope );
	EF_q.field->sub(y1_minus_py, y1, py);
	EF_q.field->sub(num, y1_minus_py, slope_x1px);

	EF_q.field->sqr(slope2, slope);
	EF_q.field->add(den, x1, px);
	EF_q.field->add(den, den, qx);
	EF_q.field->sub(den, den, slope2);

	EF_q.field->div(num_div_den, num, den);
	return num_div_den;
}

ExtensionField::Element miller(ecPoint& P, ecPoint& Q, ellipticCurveFq& EF_q) {
	string m = "0001101110000111110110000010010011101010000010000111110111110010011100111011000011010100110001000011011100101000000100100000100011101110000011010111101110000000100100000000000000000000000010111111010000000011000000000000000000000000000000000000000000001";
	ExtensionField::Element f, gret, fgret, x1, y1;
	x1 = Q.x;
	y1 = Q.y;
	f = EF_q.field->one;
	ecPoint T;
	T = P;
	for (int i = 0; i < m.size(); i++) {
		gret = g(T, T, x1, y1, EF_q);
		EF_q.field->mul(fgret, f, gret);
		EF_q.field->mul(f, f, fgret);

		EF_q.add(T, T, T);
		if (m[i] == '1') {
			gret = g(T, P, x1, y1, EF_q);
			EF_q.field->mul(f, f, gret);
			//T = T + P;
			EF_q.add(T, T, P);
		}
	}
	return f;
}

ExtensionField::Element tatePairing(ecPoint& P, ecPoint& Q, ecPoint& S, ellipticCurveFq& EF_q) {
	ecPoint QS;
	// QS = Q + S
	EF_q.add(QS, Q, S);
	ExtensionField::Element num, den, res, eta;
	num = miller(P, QS, EF_q);
	den = miller(P, S, EF_q);
	EF_q.field->div(res, num, den);
	/*
	  eta = (p^k - 1)/q
	  num = eval_miller(P, Q+S)/eval_miller(P,  S)
	  return (num^eta)
	*/
	string strEta = "b1100010011011010001011001001001100010110100010110001000000100010000100011101001010100101110111000010000100111101010111010010101010011000011100110001100100101010001100111010110110111100011001100101000101000101100011111100000010000010001110110111101101011001111000101001111101101000011101110110000100100010111010011111111111010000011011011100101101111001111010111000100111111010000110100101001001000011100000101010001010000000110001111111101001110101000101100001101100001110001011001010010111011101110010111100011001110100000011100100000001111110100011101000111001100000101101100110111011110001001110100110010000000111100110011010001010011010110011011011110010000011010011111010001010011111000010010100011000001010100000010110000001001001110011011101111000111110001011100110110000010001010101011010100111001101110011000110001011011110100101000001110100011001101110111110011110111011000111000110110010001101000010010001110100001011001111100010100010000111001010101101110111111010100110110000010011010111101001001110000000001101110011100000001100111100011000001110000010000111100010001110011001000011000100000000110001111100011111100101010010111011101110100000100111001000010110101000010010011000000110001111011011100111011111000000001101010011100110000000110110010011000111011001000111100100101111110101000000011010000110000110110100110010100010010101001001101010111110110110111111001011111100111011000011011101010101111011000000100111001110010010010111110010011110101101010101111110101001000111111100111010110011001010000010011000111110010110001111000010111100011011001001110000010010010010101101110000001001101101111010001110001011110101110001001011100110011111010001010110101011010000111011010110011111011101100001000001011110011100110011010000011010000111010100000101010110111001111010110010001111000101010010110010000100010000101100101011000110011011110101001011100010111100100101010011110100100001010010000001011111100110011100111110000110010111010110001001010101011011100100010010110110101010110111111010011000111110001010110110101101001011111100000010010000001110010110110001000111000000010001111100110011010110000100011110000110101001001100110000010111111110100100011100010101011010100110100011100010010000101110110010100100010001110010111000010101101011010011010110101110011010000110011100100110111000110000111000010001011110100001111011000010011111001000101100101101010111111101101111000000110010011110111111011101010100111110111001100001000001000000111000001011001011011101000110001001001010110110100100111111010101100011011111011010010000110101100011110110011001110110101110100001000010011000111111011011001000111010001101011101000110000111000101001101001110100001111101001001000101110000000000000000000000100011110111000000100100000000000000000000000000000000000000000000000";
	ExtensionField::Element rr;
	EF_q.field->superfastpow(rr, res, strEta);
	return rr;
}

ellipticCurveFq::Point mapToFq(EC_POINT *P, Curve *curve, ellipticCurveFq& E_Fq) {
	BigNumber x;
	BigNumber y;
	if (!EC_POINT_get_affine_coordinates_GFp(curve->curve, P, x.bn, y.bn, NULL)) handleErrors();
	ExtensionField::Element fq_x, fq_y;

	std::stringstream stream1, stream2;
	stream1 << "0 " << x.toDecString();
	stream2 << "0 " << y.toDecString();

	E_Fq.field->readElement(stream1.str(), fq_x);
	E_Fq.field->readElement(stream2.str(), fq_y);
	ecPoint res(fq_x, fq_y);
	return res;
}