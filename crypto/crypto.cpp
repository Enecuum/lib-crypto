#include <iostream>
#include <bitset>
#include "crypto.h"

using namespace std;


BigNumber operator + (const BigNumber &a, const BigNumber &b)
{
	BIGNUM *r = nullptr;

	if (nullptr == (r = BN_new())) {
		handleError(NO_MEMORY);
		return BigNumber();
	}
		
	if (!BN_add(r, a.bn, b.bn)) {
		BN_free(r);
		handleError(CALC_FAILED);
		return BigNumber();
	}	

	BigNumber res(r);
	BN_free(r);
	return res;
}

BigNumber operator - (const BigNumber &a, const BigNumber &b)
{
	BIGNUM *r = nullptr; 
    BN_CTX* ctx = nullptr; 

	if (nullptr == (r = BN_new())) {
		handleError(NO_MEMORY);
		return BigNumber();
	}

	if (nullptr == (ctx = BN_CTX_new())) {
		BN_free(r);
		handleError(NO_MEMORY);
		return BigNumber();
	}

	if (!BN_mod_sub(r, a.bn, b.bn, BigNumber(1223).bn, ctx)) {
		BN_CTX_free(ctx);
		BN_free(r);
		handleError(CALC_FAILED);
		return BigNumber();
	}	
	BigNumber res(r);
	BN_free(r);
	BN_CTX_free(ctx);
	return res;
}

BigNumber operator * (const BigNumber &a, const BigNumber &b)
{
	BIGNUM* r = nullptr;
	BN_CTX* ctx = nullptr;

	if (nullptr == (r = BN_new())) {
		handleError(NO_MEMORY);
		return BigNumber();
	}

	if (nullptr == (ctx = BN_CTX_new())) {
		BN_free(r);
		handleError(NO_MEMORY);
		return BigNumber();
	}

	if (BN_mul(r, a.bn, b.bn, ctx)) {		
		BigNumber res(r);
		BN_free(r);
		BN_CTX_free(ctx);
		return res;
	}

	BN_free(r);
	BN_CTX_free(ctx);
	handleError(CALC_FAILED);
	return BigNumber();
}


BigNumber operator % (const BigNumber &a, const BigNumber &b)
{
	BIGNUM* r = nullptr;
	BN_CTX* ctx = nullptr;

	if (nullptr == (r = BN_new())) {
		handleError(NO_MEMORY);
		return BigNumber();
	}

	if (nullptr == (ctx = BN_CTX_new())) {
		BN_free(r);
		handleError(NO_MEMORY);
		return BigNumber();
	}

	if (BN_div(nullptr, r, a.bn, b.bn, ctx)) {
		BigNumber res(r);
		BN_free(r);
		BN_CTX_free(ctx);
		return res;
	}

	BN_free(r);
	BN_CTX_free(ctx);
	handleError(CALC_FAILED);
	return BigNumber();
}
int operator == (const BigNumber &a, const BigNumber &b)
{
	return BN_cmp(a.bn, b.bn);
}

/* @brief Modulo operation
	@param BigNumber nom
	@param BigNumber den
	@param BigNumber mod
*/
BigNumber mdiv(const BigNumber& nom, const BigNumber& den, const BigNumber& mod) {
	BIGNUM* r = nullptr;
	BN_CTX* ctx = nullptr;

	if (nullptr == (r = BN_new())) {
		handleError(NO_MEMORY);
		return BigNumber();
	}

	if (nullptr == (ctx = BN_CTX_new())) {
		BN_free(r);
		handleError(NO_MEMORY);
		return BigNumber();
	}

	if (nullptr == (BN_mod_inverse(r, den.bn, BigNumber(mod).bn, ctx))) {
		BN_free(r);
		BN_CTX_free(ctx);
		handleError(CALC_FAILED);
		return BigNumber();
	}
		
	if (0 == (BN_mod_mul(r, nom.bn, r, BigNumber(mod).bn, ctx))) {
		BN_free(r);
		BN_CTX_free(ctx);
		handleError(CALC_FAILED);
		return BigNumber();
	}

	BigNumber res(r);
	BN_free(r);
	BN_CTX_free(ctx);
	return res;
}

BigNumber msub(const BigNumber& a, const BigNumber& b, const BigNumber& m) {
	BIGNUM* r = nullptr;
	BN_CTX* ctx = nullptr;

	if (nullptr == (r = BN_new())) {
		handleError(NO_MEMORY);
		return BigNumber();
	}

	if (nullptr == (ctx = BN_CTX_new())) {
		BN_free(r);
		handleError(NO_MEMORY);
		return BigNumber();
	}

	if (!BN_mod_sub(r, a.bn, b.bn, m.bn, ctx)) {
		BN_free(r);
		BN_CTX_free(ctx);
		handleError(CALC_FAILED);
		return BigNumber();
	}
	
	BigNumber res(r);
	BN_free(r);
	BN_CTX_free(ctx);
	return res;
}

EC_POINT* keyRecovery(const vector<EC_POINT*>& proj, const vector<int>& coalition, const BigNumber& q, const Curve *curve) {
	EC_POINT *secret = EC_POINT_new(curve->curve);	
	EC_POINT* buff = nullptr;

	for (size_t i = 0; i < proj.size(); i++) {
		BigNumber lambda(1);
		for (size_t j = 0; j < proj.size(); j++) {
			if (i != j) {
				BigNumber nom = msub(BigNumber(0), BigNumber(coalition[j]), q);
				BigNumber den = msub(BigNumber(coalition[i]), BigNumber(coalition[j]), q);
				lambda = (lambda * (mdiv(nom, den, q))) % q;
			}
		}
		if ((lambda == BigNumber(0)) == 0)
			continue;

		buff = mul(lambda, proj.at(i), curve);

		if (i == 0) {
			if (!EC_POINT_copy(secret, buff)) {
				EC_POINT_free(buff);
				handleError(NO_MEMORY);				
			}
			EC_POINT_free(buff);
			continue;
		}
		if (!EC_POINT_add(curve->curve, secret, secret, buff, nullptr)) {
			EC_POINT_free(buff);
			handleError(CALC_FAILED);
		}
		EC_POINT_free(buff);
	}
	return secret;
}

EC_POINT* hashToPoint(const BigNumber& hash, const Curve *curve) {
	EC_POINT *ret = mul(hash, curve->G, curve);
	return ret;
}

vector<BigNumber> generatePoly(const int power) {
	// x^5 + 8x^4 + 6x^3 + 5x^2 + 10x
	// Poly: 1077x + 5x^2 + 6x^3 + 8x^4 + x^5
	// {1, 8, 6, 5, 1077}
	vector<BigNumber> arrayA;
	for (int i = 0; i < power; i++)
		arrayA.push_back(getRandom(BigNumber(MAX_NUMBER_256)));
	// = { 5, 1077 };
	return arrayA;
}

vector<BigNumber> shamir(const BigNumber &secretM, const vector<BigNumber> &ids, const int participantN, const int sufficientK, const BigNumber& q)
{
	size_t power = sufficientK - 1;

	vector<BigNumber> arrayA = generatePoly(power);
	vector<BigNumber> arrayK;
	for (size_t i = 0; i < ids.size(); i++)
	{
		BigNumber temp(0);
		for (size_t j = 0; j < power; j++) {
			temp = temp + (arrayA[j] * (bpow(ids[i], power - j)));			
		}
		arrayK.insert(arrayK.end(), (temp + secretM) % q);
	}
	return arrayK;
}

BigNumber bpow(const BigNumber &a, const int n) {
	BigNumber res(1);
	for (int i = 0; i < n; i++) {
		res = res * a;
	}
	return res;
}

/* Get shadows of secret key (ss_i = coalition[i] * Q)*/
vector<EC_POINT*> keyProj(const vector<int>& coalition, const vector<BigNumber> shares, const EC_POINT *Q, const Curve *curve) {
	vector<EC_POINT*> res;
	for (size_t i = 0; i < coalition.size(); i++) {
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

int test(const int a, const int b) {
	throw 1;
}

void printBN(const char* desc, const BIGNUM * bn) {
	fprintf(stdout, "%s", desc);
	BN_print_fp(stdout, bn);
	fprintf(stdout, "\n");
}

char* printPoint(const EC_POINT *P, const Curve *curve) {
	BigNumber x;
	BigNumber y;
	if (!EC_POINT_get_affine_coordinates_GFp(curve->curve, P, x.bn, y.bn, nullptr)) {
		handleError(CALC_FAILED);
		return POINTER_ERROR_RET;
	}
	
	string str = "(x: " + string(x.toHexString()) + " : y: " + string(y.toHexString()) + ")";
	strcpy(charBuff, str.c_str());
	return charBuff;
}

EC_POINT *mul(const BigNumber& n, const EC_POINT *P, const Curve *curve) {
	EC_POINT *R = nullptr;
	if (nullptr == (R = EC_POINT_new(curve->curve))) {
		handleError(NO_MEMORY);
		return POINTER_ERROR_RET;
	}
	if (1 != EC_POINT_mul(curve->curve, R, nullptr, P, n.bn, nullptr)) {
		handleError(CALC_FAILED);
		return POINTER_ERROR_RET;
	}
	return R;
}

EC_POINT *createMPK(const BigNumber& msk, const EC_POINT *P, const Curve *curve) {
	EC_POINT *R = mul(msk, P, curve);
	return R;
}

BigNumber getRandom(const BigNumber &max) {
	BIGNUM * r = nullptr;
	if (nullptr == (r = BN_secure_new())) {
		handleError(NO_MEMORY);
		return BigNumber();
	}
	
	do {
		if (!BN_rand_range(r, max.bn)) {
			handleError(CALC_FAILED);
			return BigNumber();
		}
	} while (BN_is_zero(r));
	BigNumber res(r);
	BN_free(r);
	return res;
}

BigNumber g(const EC_POINT *P, const EC_POINT *Q, const BigNumber &x1, const BigNumber &y1, const Curve *curve) {
	BigNumber px;
	BigNumber py;
	if (!EC_POINT_get_affine_coordinates_GFp(curve->curve, P, px.bn, py.bn, nullptr)) {
		handleError(CALC_FAILED);
		return BigNumber();
	}

	BigNumber qx;
	BigNumber qy;
	if (!EC_POINT_get_affine_coordinates_GFp(curve->curve, Q, qx.bn, qy.bn, nullptr)) {
		handleError(CALC_FAILED);
		return BigNumber();
	}

	BigNumber slope;

	if (((px == qx) == 0) && ((((py + qy) % curve->field) == 0) == 0)) {
		return (x1 - px);
	}
	if (((px == qx) == 0) && ((py == qy) == 0)) {
		slope = mdiv((BigNumber(3) * (px * px) + curve->a), (py + py), curve->field);
	}
	else {
		slope = mdiv((py - qy), (px - qx), curve->field);
	}
	BigNumber num = (y1 - py - (slope * (x1 - px)));
	BigNumber den = (x1 + px + qx - (slope * slope));
	return mdiv(num, den, curve->field);
}

BigNumber miller(const string& mm, const EC_POINT *P, const BigNumber &x1, const BigNumber &y1, const Curve *curve) {
	string m = mm;
	EC_POINT * T = nullptr;
	if (nullptr == (T = EC_POINT_new(curve->curve))) {
		handleError(NO_MEMORY);
		return BigNumber();
	}
		
	if (1 != EC_POINT_copy(T, P)) { 
		handleError(CALC_FAILED);
		return BigNumber();
	}

	BigNumber gret;
	BigNumber f(1);
	for (size_t i = 0; i < m.size(); i++) {
		f = (f * (f * g(T, T, x1, y1, curve))) % curve->field;
		//T = T + T;
		if (1 != EC_POINT_dbl(curve->curve, T, T, nullptr)) {
			handleError(CALC_FAILED);
			return BigNumber();
		}

		if (m[i] == '1') {
			f = (f * g(T, P, x1, y1, curve)) % curve->field;
			//T = T + P;
			if (1 != EC_POINT_add(curve->curve, T, T, P, nullptr)) {
				handleError(CALC_FAILED);
				return BigNumber();
			}
		}
	}
	EC_POINT_free(T);
	return f;
}

BigNumber evalMiller(const EC_POINT *P, const EC_POINT *Q, const Curve *curve) {
	string m = "1011011";
	BigNumber x;
	BigNumber y;
	if (!EC_POINT_get_affine_coordinates_GFp(curve->curve, Q, x.bn, y.bn, nullptr)) {
		handleError(CALC_FAILED);
		return BigNumber();
	}
	BigNumber res = miller(m, P, x, y, curve);
	return res;
}

BigNumber weilPairing(const EC_POINT *P, const EC_POINT *Q, const EC_POINT *S, const Curve *curve) {

	//int num = eval_miller(P, Q + S) / eval_miller(P, S);
	//int den = eval_miller(Q, P - S) / eval_miller(Q, -S)
	//return (num / den)
	//eta = (p^k - 1)/n
	//num = eval_miller(P, Q+S)/eval_miller(P,  S)
	//return (num^eta)
	
	
	EC_POINT *QS = nullptr;
	if (nullptr == (QS = EC_POINT_new(curve->curve))) {
		handleError(NO_MEMORY);
		return BigNumber();
	}

	// QS = Q + S
	if (1 != EC_POINT_add(curve->curve, QS, Q, S, nullptr)) {
		handleError(CALC_FAILED);
		return BigNumber();
	}


	EC_POINT *invS = nullptr;
	if (nullptr == (invS = EC_POINT_new(curve->curve))) {
		handleError(NO_MEMORY);
		return BigNumber();
	}
	// invS = -S	
	if (1 != EC_POINT_copy(invS, S)) {
		handleError(CALC_FAILED);
		return BigNumber();
	}

	if (1 != EC_POINT_invert(curve->curve, invS, nullptr)) {
		handleError(CALC_FAILED);
		return BigNumber();
	}

	EC_POINT *PS = nullptr;
	if (nullptr == (PS = EC_POINT_new(curve->curve))) {
		handleError(NO_MEMORY);
		return BigNumber();
	}

	// PS = P - S = P + (-S)
	if (1 != EC_POINT_add(curve->curve, PS, P, invS, nullptr)) {
		handleError(CALC_FAILED);
		return BigNumber();
	}

	BigNumber nom = mdiv(evalMiller(P, QS, curve), evalMiller(P, S, curve), curve->field);
	BigNumber den = mdiv(evalMiller(Q, PS, curve), evalMiller(Q, invS, curve), curve->field);
	EC_POINT_free(QS);
	EC_POINT_free(invS);
	EC_POINT_free(PS);
	return mdiv(nom, den, curve->field);
}

ExtensionField::Element g(const ecPoint& P, const ecPoint& Q, const ExtensionField::Element& x1, const ExtensionField::Element& y1, ellipticCurveFq& EF_q) {

	ExtensionField::Element px, py, qx, qy, slope;
	ExtensionField::Element  x1_minus_px, x1_minus_qx, negqy;
	ecPoint zero;
	EF_q.scalarMultiply(zero, P, (Integer)(0), -1);
	px = P.x;
	py = P.y;
	qx = Q.x;
	qy = Q.y;
	if (P == zero || Q == zero) {
		if(P == Q)
			return EF_q.field->one;
		if (P == zero) {
			EF_q.field->sub(x1_minus_qx, x1, qx);
			return x1_minus_qx;
		}
		if (Q == zero) {
			EF_q.field->sub(x1_minus_px, x1, px);
			return x1_minus_px;
		}
	}
	
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

ExtensionField::Element miller(const ecPoint& P, const ecPoint& Q, ellipticCurveFq& EF_q) {
	BigNumber Bm("80000000000000000000000000000000000200014000000000000000000000000000000000010000800000020000000000000000000000000000000000080004");
	
	int len = BN_num_bits(Bm.bn) - 1; // Minus first sign bit

	ExtensionField::Element f, gret, fgret, x1, y1;
	x1 = Q.x;
	y1 = Q.y;
	f = EF_q.field->one;
	ecPoint T;
	T = P;
	for (int i = 0; i < len; i++) {
		gret = g(T, T, x1, y1, EF_q);
		EF_q.field->mul(fgret, f, gret);
		EF_q.field->mul(f, f, fgret);

		EF_q.add(T, T, T);
		if (BN_is_bit_set(Bm.bn, len - i - 1)) {
			gret = g(T, P, x1, y1, EF_q);
			EF_q.field->mul(f, f, gret);
			//T = T + P;
			EF_q.add(T, T, P);
		}
	}
	return f;
}

ExtensionField::Element tatePairing(const ecPoint& P, const ecPoint& Q, const ecPoint& S, ellipticCurveFq& EF_q) {
	ecPoint QS;
	// QS = Q + S
	EF_q.add(QS, Q, S);
	ExtensionField::Element num, den, res, eta;
	num = miller(P, QS, EF_q);
	den = miller(P, S, EF_q);
	EF_q.field->div(res, num, den);
	return res;
}

ellipticCurveFq::Point mapToFq(const EC_POINT *P, const Curve *curve, ellipticCurveFq& E_Fq) {
	BigNumber x;
	BigNumber y;
	if (!EC_POINT_get_affine_coordinates_GFp(curve->curve, P, x.bn, y.bn, nullptr)) {
		handleError(CALC_FAILED);
		ecPoint();
	}
	ExtensionField::Element fq_x, fq_y;

	stringstream stream1, stream2;
	stream1 << "0 " << x.toDecString();
	stream2 << "0 " << y.toDecString();

	E_Fq.field->readElement(stream1.str(), fq_x);
	E_Fq.field->readElement(stream2.str(), fq_y);
	ecPoint res(fq_x, fq_y);
	return res;
}

ellipticCurveFq::Point hashToPointFq(const ecPoint &G, const BigNumber& cnum, ellipticCurveFq& E_Fq) {
	BigNumber num = cnum;
	ExtensionField::Element x, x1, x2, x2powed;
	string strX("1 1 0");
	E_Fq.field->readElement(strX, x);
	// (p ^ 2 - 1) / 2)
	string residue("10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000101000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100000000000000011000000000000000100001000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000001001000000000000001010000000001000000000000000010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000001000000000010000001000000001001000000000000001001100000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100000000000000001110000000000000001100000000001000000000000000001100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000110000000000000000100");

	// ((p + 1) / 4)
	BigNumber eta3("20000000000000000000000000000000000080005000000000000000000000000000000000004000200000008000000000000000000000000000000000020001");

	// Residue check
	while (true) {
		int len = BN_num_bits(num.bn); // Minus first sign bit
		string stream("");
		for (int j = 0; j < len; j++) {
			stream.append(to_string(BN_is_bit_set(num.bn, len - j - 1)));
		}

		// x1 = x ^ hash
		E_Fq.field->pow(x1, x, stream);

		//x2 = x1 ^ 3 + x1
		E_Fq.field->mul(x2, x1, x1);
		E_Fq.field->mul(x2, x2, x1);
		E_Fq.field->add(x2, x2, x1);
		E_Fq.field->pow(x2powed, x2, residue);
		if (x2powed == E_Fq.field->one) {
			break;
		}
		else {
			num = num + BigNumber(1);
		}
	}
	
	// x1 = x ^ hash
	// x2 = a1x + b1
	// y coord = ax + b
	// x2 = x1 ^ 3 + x1 // = (a1 * x + b1)

	//D = (2 * a1 - 4 * b1) ^ 2 + 4 * (4 * f0 - 1) * a1 ^ 2
	//y = (2 * a1 - 4 * b1 + D.sqrt()) / (2 * (4 * f0 - 1))
	//
	//a = y ^ ((p + 1) / 4) mod p
	//b = (a ^ 2 + a1) / (2 * a) mod p

	Integer a1(x2[1]);
	Integer b1(x2[0]);
	Integer Ip("6703903964971298549787012499102923063739684112761466562144343758833001675653841939454385015500446199477853424663597373826728056308768000892499915006541827");
	Integer f0("6703903964971298549787012499102923063739684112761466562144343758833001675653841939454385015500446199477853424663597373826728056308768000892499915006541826");

	Integer D = ((2 * a1 - 4 * b1) * (2 * a1 - 4 * b1) + (4 * (4 * f0 - 1) * (a1 * a1))) % Ip;

	IntSqrtModDom<> ISM;
	Integer root;
	ISM.sqrootmod(root, D, Ip);
	Integer nom = (2 * a1 - 4 * b1 + root);
	Integer den = (2 * (4 * f0 - 1));

	ExtensionField Fp(Ip, (Integer)1);
	ExtensionField::Element y_fp, nom_fp, den_fp;
	string strnom("0 ");
	string strden("0 ");
	strnom.append((string)nom);
	strden.append((string)den);
	Fp.readElement(strnom, nom_fp);
	Fp.readElement(strden, den_fp);

	Fp.div(y_fp, nom_fp, den_fp);

	int len2 = BN_num_bits(eta3.bn); // Minus first sign bit
	string strEta3("");
	for (int j = 0; j < len2; j++) {
		strEta3.append(to_string(BN_is_bit_set(eta3.bn, len2 - j - 1)));
	}
	ExtensionField::Element a_fq, a1_fp;
	Fp.pow(a_fq, y_fp, strEta3);

	string stra1("0 ");
	stra1.append((string)a1);
	Fp.readElement(stra1, a1_fp);
	

	//b = (a ^ 2 + a1) / (2 * a) mod p
	ExtensionField::Element b_fq, asqr, adbl;
	Fp.scalarMultiply(adbl, a_fq, (Integer)2);
	Fp.sqr(asqr, a_fq);
	Fp.add(asqr, asqr, a1_fp);
	Fp.div(b_fq, asqr, adbl);

	// Final point is two polynomials:
	// res_x(x1)
	// res_y(a * x + b)
	ExtensionField::Element res_x, res_y;
	Integer a_tmp, b_tmp;
	a_tmp = a_fq[0];
	b_tmp = b_fq[0];
	res_x = x1;
	E_Fq.field->scalarMultiply(res_y, x, a_tmp);
	E_Fq.field->add(res_y, res_y, b_fq);
	ecPoint res(res_x, res_y);
	return res;
}

bool verify_mobile(
	const string& p,
	const string& a,
	const string& b,
	const string& order,
	const string& irred,
	const string& gx,
	const string& gy,
	const int k,
	const string& s1x,
	const string& s1y,
	const string& s2x,
	const string& s2y,
	const string& pk_lpos,
	const string& mhash,
	const string& mpkx,
	const string& mpky
	) {
	// H = id_hash * G
	// Q = pk_lpos * G
	// MPK
	Integer Ip, Im;
	Ip = Integer(p.data());
	Im = Integer(k);

	ellipticCurve* ec = nullptr;
	if (nullptr == (ec = new ellipticCurve(Ip, Im, irred, a, b))) {
		handleError(NO_MEMORY);
		return false; 
	}

	ellipticCurveFq E_Fq(ec);
	ExtensionField::Element HFq_x, HFq_y, SFq_x, SFq_y;
	
	// G point
	ExtensionField::Element G_x, G_y;
	E_Fq.field->readElement(gx, G_x);
	E_Fq.field->readElement(gy, G_y);
	ecPoint G0_fq(G_x, G_y);
	cout << "\n G0_fq: " << endl;
	E_Fq.show(G0_fq);

	// s1 point
	ExtensionField::Element S1_x, S1_y;
	E_Fq.field->readElement(s1x, S1_x);
	E_Fq.field->readElement(s1y, S1_y);
	ecPoint S1_fq(S1_x, S1_y);
	cout << "\n S1_fq: " << endl;
	E_Fq.show(S1_fq);

	// s2 point
	ExtensionField::Element S2_x, S2_y;
	E_Fq.field->readElement(s2x, S2_x);
	E_Fq.field->readElement(s2y, S2_y);
	ecPoint S2_fq(S2_x, S2_y);
	cout << "\n S2_fq: " << endl;
	E_Fq.show(S2_fq);

	// MPK point
	ExtensionField::Element MPK_x, MPK_y;
	E_Fq.field->readElement(mpkx, MPK_x);
	E_Fq.field->readElement(mpky, MPK_y);
	ecPoint MPK_fq(MPK_x, MPK_y);
	cout << "\n MPK_fq: " << endl;
	E_Fq.show(MPK_fq);

	ecPoint tmpQ;
	ecPoint Q_Fq;
	BigNumber orderQ("3298c");
	BigNumber qhash(pk_lpos);
	BigNumber m(mhash);
	tmpQ = hashToPoint(qhash);
	E_Fq.scalarMultiply(Q_Fq, tmpQ, (Integer)(orderQ.toDecString()), -1);

	cout << "\n Q_Fq: " << endl;
	E_Fq.show(Q_Fq);
	bool res = verifyTate(S1_fq, S2_fq, m, MPK_fq, Q_Fq, G0_fq, E_Fq);

	delete ec; 
	return res;
}

EC_POINT* getQ(const BigNumber &qqhash, const Curve* crv, ellipticCurveFq& E_Fq) {
	cout << "---- getQ" << endl;
	BigNumber orderQ("3298c");
	ecPoint tmpQ;
	ecPoint tmp;
	ecPoint zero;
	int isZero = 1;
	BigNumber one(1);
	E_Fq.scalarMultiply(zero, tmpQ, (Integer)(0), -1);
	BigNumber qhash = qqhash;
	do {
		tmpQ = hashToPoint(qhash);
		E_Fq.scalarMultiply(tmp, tmpQ, (Integer)(orderQ.toDecString()), -1);
		isZero = (tmp == zero);
		qhash += one;
	} while (isZero);
	E_Fq.scalarMultiply(tmp, tmpQ, (Integer)(orderQ.toDecString()), -1);
	
	BigNumber qx(dectox_int(tmp.x[0]));
	BigNumber qy(dectox_int(tmp.y[0]));

	EC_POINT* Q;	
	if (nullptr == (Q = EC_POINT_new(crv->curve))) { 
		handleError(NO_MEMORY);
		return POINTER_ERROR_RET;
	}

	BN_CTX* ctx = nullptr;
	if (nullptr == (ctx = BN_CTX_new())) {
		handleError(NO_MEMORY);
		return POINTER_ERROR_RET;
	}

	if (1 != EC_POINT_set_affine_coordinates_GFp(crv->curve, Q, qx.bn, qy.bn, nullptr)) { 
		BN_CTX_free(ctx);
		handleError(CALC_FAILED);
		return POINTER_ERROR_RET;
	}

	BN_CTX_free(ctx);
	return Q;
}

ellipticCurveFq::Point hashToPoint(const BigNumber &cnum) {
	// x = a ^ ((p + 1) / 4)
	Integer Ip("6703903964971298549787012499102923063739684112761466562144343758833001675653841939454385015500446199477853424663597373826728056308768000892499915006541827");
	BigNumber eta("20000000000000000000000000000000000080005000000000000000000000000000000000004000200000008000000000000000000000000000000000020001");
	ExtensionField Fp(Ip, (Integer)1);
	ExtensionField::Element res, num_fp, den_fp;
	
	BigNumber num = cnum;
	BigNumber one(1);
	// Check Euler's criteria
	// (p ^ 2 - 1) / 2)
	string residue("1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100000000000000001010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001000000000000000010000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001000000000000000001");
	ExtensionField::Element tmp, tmp1, tmp2;
	// Residue check
	while (true) {
		string strnum("0 ");
		strnum.append(num.toDecString());
		Fp.readElement(strnum, num_fp);
		//x2 = x1 ^ 3 + x1
		Fp.pow(tmp, num_fp, "11");
		Fp.add(tmp1, tmp, num_fp);
		Fp.pow(tmp2, tmp1, residue);
		if (tmp2 == Fp.one) {
			break;
		}
		else {
			num += one;
		}
	}

	int len2 = BN_num_bits(eta.bn); // Minus first sign bit
	string strEta("");
	for (int j = 0; j < len2; j++) {
		strEta.append(to_string(BN_is_bit_set(eta.bn, len2 - j - 1)));
	}
	Fp.pow(res, tmp1, strEta);
	ecPoint P_res(num_fp, res);
	cout << "---- hashToPoint" << endl;
	return P_res;
}

bool verifyTate(const ecPoint& S1_fq, const ecPoint& S2_fq, const BigNumber& hash, const ecPoint& MPK_fq, const ecPoint& Q_fq, const ecPoint& G0_fq, ellipticCurveFq& E_Fq) {
	BigNumber max_hash(MAX_NUMBER_256);

	ecPoint H_fq = hashToPoint(hash);

	BigNumber shash = getRandom(max_hash);
	ecPoint S_fq = hashToPointFq(S1_fq, shash, E_Fq);

	

	ExtensionField::Element rr, bb, cc;
	rr = tatePairing( S2_fq,  G0_fq, S_fq, E_Fq);
	bb = tatePairing(Q_fq, MPK_fq, S_fq, E_Fq);
	cc = tatePairing(H_fq, S1_fq, S_fq, E_Fq);

	string strEta = "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001000000000000000010100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000100000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000010";

	/*
	  eta = (p^k - 1)/q
	  tate pairing return value should be in `eta` degree
	  num = eval_miller(P, Q+S)/eval_miller(P,  S)
	  return (num^eta)
	*/
	ExtensionField::Element r1, b1, c1, b1c1;
	E_Fq.field->pow(r1, rr, strEta);
	E_Fq.field->pow(b1, bb, strEta);
	E_Fq.field->pow(c1, cc, strEta);

	ExtensionField::Element bbcc;
	E_Fq.field->mul(b1c1, b1, c1);

	bool areEqual = E_Fq.field->areEqual(r1, b1c1);
	return areEqual;
}

string dectox_int(Integer num)
{	
	stringstream ss;
	vector<int> tmp;
	while (num != 0)
	{
		Integer rest = num % 16;
		num /= 16;
		tmp.push_back(rest);
	}
	for (int i = (tmp.size() - 1); i >= 0; i--)
		ss << hex << tmp[i];
	return string(ss.str());
}