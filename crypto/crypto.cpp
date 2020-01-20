
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

EC_POINT* keyRecovery(vector<EC_POINT*> proj, vector<BigNumber> coalition, BigNumber q, Curve *curve) {
	EC_POINT *secret = EC_POINT_new(curve->curve);
	EC_POINT *buff = EC_POINT_new(curve->curve);
	for (size_t i = 0; i < proj.size(); i++) {
		BigNumber lambda(1);
		for (size_t j = 0; j < proj.size(); j++) {
			if (i != j) {
				//lamb = (lamb * (0-coalition[j]))/(coalition[i]-coalition[j]) % q
				BigNumber nom = msub(BigNumber(0), coalition[j], q);
				BigNumber den = msub(coalition[i], coalition[j], q);
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

vector<BigNumber> generatePoly(int power) {
	srand(time(0));
	
	// x^5 + 8x^4 + 6x^3 + 5x^2 + 10x
	// Poly: 1077x + 5x^2 + 6x^3 + 8x^4 + x^5
	// {1, 8, 6, 5, 1077}
	vector<BigNumber> arrayA;
	for (int i = 0; i < power; i++)
		arrayA.push_back(getRandom(BigNumber(MAX_NUMBER_256)));
	// = { 5, 1077 };
	return arrayA;
}

vector<BigNumber> shamir(BigNumber secretM, vector<BigNumber> ids, int participantN, int sufficientK, BigNumber q)
{
	
	//secretM = 10;
	//participantN = 10;
	//sufficientK = 6;

	size_t power = sufficientK - 1;

	vector<BigNumber> arrayA = generatePoly(power);

	vector<BigNumber> arrayK;
	for (size_t i = 0; i < ids.size(); i++)
	{
		BigNumber temp(0);
		for (size_t j = 0; j < power; j++)
			temp = temp + (arrayA[j] * (bpow(ids[i], power - j)));
		arrayK.insert(arrayK.end(), (temp + secretM) % q);
	}
	return arrayK;
}

BigNumber bpow(BigNumber a, int n) {
	BigNumber res(1);
	for (int i = 0; i < n; i++) {
		res = res * a;
	}
	return res;
}

/* Get shadows of secret key (ss_i = coalition[i] * Q)*/
vector<EC_POINT*> keyProj(vector<int> coalition, vector<BigNumber> shares, EC_POINT *Q, Curve *curve) {
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

BigNumber miller(string m, EC_POINT *P, const BigNumber &x1, const BigNumber &y1, Curve *curve) {
	EC_POINT *T = EC_POINT_new(curve->curve);
	if (1 != EC_POINT_copy(T, P)) handleErrors();
	BigNumber gret;
	BigNumber f(1);
	for (size_t i = 0; i < m.size(); i++) {
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
	// ѕор€док подгруппы G0 в двоичном представлении без первого бита (???)
	// TODO: ѕеревод в бинарную строку
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
	ExtensionField::Element py_plus_qy = NULL, x1_minus_px, x1_minus_qx, negqy;
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
	//std::cout << std::endl;
	//EF_q.show(P);
	//EF_q.show(Q);
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

ExtensionField::Element tatePairing(ecPoint& P, ecPoint& Q, ecPoint& S, ellipticCurveFq& EF_q) {
	ecPoint QS;
	// QS = Q + S
	EF_q.add(QS, Q, S);
	ExtensionField::Element num, den, res, eta;
	num = miller(P, QS, EF_q);
	den = miller(P, S, EF_q);
	EF_q.field->div(res, num, den);
	return res;
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

ellipticCurveFq::Point hashToPointFq(ecPoint &G, BigNumber num, ellipticCurveFq& E_Fq) {
	ExtensionField::Element x, x1, x2, x2powed;
	std::string strX("1 1 0");
	E_Fq.field->readElement(strX, x);
	// (p ^ 2 - 1) / 2)
	std::string residue("10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000101000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100000000000000011000000000000000100001000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000001001000000000000001010000000001000000000000000010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000001000000000010000001000000001001000000000000001001100000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100000000000000001110000000000000001100000000001000000000000000001100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000110000000000000000100");
	//BigNumber Bp("80000000000000000000000000000000000200014000000000000000000000000000000000010000800000020000000000000000000000000000000000080003");

	// ((p + 1) / 4)
	BigNumber eta3("20000000000000000000000000000000000080005000000000000000000000000000000000004000200000008000000000000000000000000000000000020001");

	// Residue check
	while (true) {

		int len = BN_num_bits(num.bn); // Minus first sign bit
		std::string stream("");
		for (int j = 0; j < len; j++) {
			stream.append(std::to_string(BN_is_bit_set(num.bn, len - j - 1)));
		}

		// x1 = x ^ hash
		E_Fq.field->pow(x1, x, stream);

		//x2 = x1 ^ 3 + x1
		E_Fq.field->mul(x2, x1, x1);
		E_Fq.field->mul(x2, x2, x1);
		E_Fq.field->add(x2, x2, x1);
		E_Fq.field->pow(x2powed, x2, residue);
		if (x2powed == E_Fq.field->one) {
			//std::cout << std::endl << "hash is: " << num.toDecString() << std::endl;
			break;
		}
		else {
			//break;
			//std::cout << std::endl << "Increase hash... " << std::endl;
			num = num + BigNumber(1);
		}
	}
	//std::cout << std::endl << "x1 " << std::endl;
	//E_Fq.field->writeElement(x1);
	//std::cout << std::endl << "x2 " << std::endl;
	//E_Fq.field->writeElement(x2);
	// x1 = x ^ hash
	// x2 = a1x + b1
	// y coord = ax + b

	//x2 = x1 ^ 3 + x1 // = (a1 * x + b1)

	//D = (2 * a1 - 4 * b1) ^ 2 + 4 * (4 * f0 - 1) * a1 ^ 2, f0Ч свободный член примитивного полинома GF(p ^ 2)
	//y = (2 * a1 - 4 * b1 + D.sqrt()) / (2 * (4 * f0 - 1))
	//
	//a = y ^ ((p + 1) / 4) mod p
	//b = (a ^ 2 + a1) / (2 * a) mod p
	//
	//координата искомой точки(x1, a*x + b)
	//соответственно: х1 и х2 Ч элементы GF(p ^ 2), x2 = y ^ 2
	//a и b Ч значение координаты y

	Integer a1(x2[1]);
	Integer b1(x2[0]);
	Integer Ip("6703903964971298549787012499102923063739684112761466562144343758833001675653841939454385015500446199477853424663597373826728056308768000892499915006541827");
	Integer f0("6703903964971298549787012499102923063739684112761466562144343758833001675653841939454385015500446199477853424663597373826728056308768000892499915006541826");

	//std::cout << std::endl << "a1 " << a1 << std::endl;
	//std::cout << std::endl << "b1 " << b1 << std::endl;

	Integer D = ((2 * a1 - 4 * b1) * (2 * a1 - 4 * b1) + (4 * (4 * f0 - 1) * (a1 * a1))) % Ip;

	IntSqrtModDom<> ISM;
	Integer root;
	ISM.sqrootmod(root, D, Ip);
	Integer nom = (2 * a1 - 4 * b1 + root);
	Integer den = (2 * (4 * f0 - 1));

	//std::cout << std::endl << "D: " << D << std::endl;
	//std::cout << std::endl << "sqrt(D): " << sqrt(D) << std::endl;
	//std::cout << std::endl << "root: " << root << std::endl;
	//std::cout << std::endl << "nom: " << nom << std::endl;
	//std::cout << std::endl << "den: " << den << std::endl;
	ExtensionField Fp(Ip, (Integer)1);
	ExtensionField::Element y_fp, nom_fp, den_fp;
	std::string strnom("0 ");
	std::string strden("0 ");
	strnom.append((string)nom);
	strden.append((string)den);
	Fp.readElement(strnom, nom_fp);
	Fp.readElement(strden, den_fp);

	Fp.div(y_fp, nom_fp, den_fp);
	//std::cout << std::endl << "y_fp: " << std::endl;
	//Fp.writeElement(y_fp);

	int len2 = BN_num_bits(eta3.bn); // Minus first sign bit
	std::string strEta3("");
	for (int j = 0; j < len2; j++) {
		strEta3.append(std::to_string(BN_is_bit_set(eta3.bn, len2 - j - 1)));
	}
	ExtensionField::Element a_fq, a1_fp;
	Fp.pow(a_fq, y_fp, strEta3);
	//std::cout << std::endl << "a_fq: " << std::endl;
	//Fp.writeElement(a_fq);

	std::string stra1("0 ");
	stra1.append((string)a1);
	Fp.readElement(stra1, a1_fp);

	//b = (a ^ 2 + a1) / (2 * a) mod p
	ExtensionField::Element b_fq, asqr, adbl;
	Fp.scalarMultiply(adbl, a_fq, (Integer)2);
	Fp.sqr(asqr, a_fq);
	Fp.add(asqr, asqr, a1_fp);
	Fp.div(b_fq, asqr, adbl);
	//std::cout << std::endl << "b_fq: " << std::endl;
	//Fp.writeElement(b_fq);

	// Final point is two polynomials:
	// res_x(x1)
	// res_y(a * x + b)
	ExtensionField::Element res_x, res_y;
	Integer a_tmp, b_tmp;
	a_tmp = a_fq[0];
	b_tmp = b_fq[0];
	res_x = x1;
	//res_y = x1;
	E_Fq.field->scalarMultiply(res_y, x, a_tmp);
	E_Fq.field->add(res_y, res_y, b_fq);
	ecPoint res(res_x, res_y);
	return res;
}

bool verify_mobile(
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
	std::string mpky
	) {
	// H = id_hash * G
	// Q = pk_lpos * G
	// MPK
	Integer Ip, Im;
	Ip = Integer(p.data());
	Im = Integer(k);

	ellipticCurve *ec = new ellipticCurve(Ip, Im, irred, a, b);
	ellipticCurveFq E_Fq(ec);
	//E_Fq.show();
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
	tmpQ = hashToPoint(qhash);
	E_Fq.scalarMultiply(Q_Fq, tmpQ, (Integer)(orderQ.toDecString()), -1);

	cout << "\n Q_Fq: " << endl;
	E_Fq.show(Q_Fq);
	bool res = verifyTate(S1_fq, S2_fq, mhash, MPK_fq, Q_Fq, G0_fq, E_Fq);
	return res;
}

EC_POINT* getQ(BigNumber qhash, Curve* crv, ellipticCurveFq& E_Fq) {

	BigNumber orderQ("3298c");
	ecPoint tmpQ;// = hashToPoint(max_hash);
	ecPoint tmp;
	ecPoint zero;
	int isZero = 1;
	E_Fq.scalarMultiply(zero, tmpQ, (Integer)(0), -1);

	do {
		tmpQ = hashToPoint(qhash);
		E_Fq.scalarMultiply(tmp, tmpQ, (Integer)(orderQ.toDecString()), -1);
		isZero = (tmp == zero);
		qhash = qhash + BigNumber(1);
	} while (isZero);
	E_Fq.scalarMultiply(tmp, tmpQ, (Integer)(orderQ.toDecString()), -1);

	BigNumber qx(dectox_int(tmp.x[0]));
	BigNumber qy(dectox_int(tmp.y[0]));

	EC_POINT* Q;
	if (NULL == (Q = EC_POINT_new(crv->curve))) handleErrors();
	if (1 != EC_POINT_set_affine_coordinates_GFp(crv->curve, Q, qx.bn, qy.bn, NULL)) handleErrors();
	return Q;
}

ellipticCurveFq::Point hashToPoint(BigNumber num) {
	// x = a ^ ((p + 1) / 4)
	Integer Ip("6703903964971298549787012499102923063739684112761466562144343758833001675653841939454385015500446199477853424663597373826728056308768000892499915006541827");
	BigNumber eta("20000000000000000000000000000000000080005000000000000000000000000000000000004000200000008000000000000000000000000000000000020001");
	ExtensionField Fp(Ip, (Integer)1);
	ExtensionField::Element res, num_fp, den_fp;
	

	// Check Euler's criteria
		// (p ^ 2 - 1) / 2)
	std::string residue("1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100000000000000001010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001000000000000000010000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001000000000000000001");
	ExtensionField::Element tmp, tmp1, tmp2;
	// Residue check
	while (true) {
		std::string strnum("0 ");
		strnum.append(num.toDecString());
		Fp.readElement(strnum, num_fp);
		//x2 = x1 ^ 3 + x1
		Fp.pow(tmp, num_fp, "11");
		Fp.add(tmp1, tmp, num_fp);
		Fp.pow(tmp2, tmp1, residue);
		if (tmp2 == Fp.one) {
			//std::cout << std::endl << "hash is: " << num.toDecString() << std::endl;
			break;
		}
		else {
			//break;
			//std::cout << std::endl << "Increase hash... " << std::endl;
			num = num + BigNumber(1);
		}
	}

	int len2 = BN_num_bits(eta.bn); // Minus first sign bit
	std::string strEta("");
	for (int j = 0; j < len2; j++) {
		strEta.append(std::to_string(BN_is_bit_set(eta.bn, len2 - j - 1)));
	}
	Fp.pow(res, tmp1, strEta);
	ecPoint P_res(num_fp, res);
	return P_res;
}

bool verifyTate(ecPoint& S1_fq, ecPoint& S2_fq, BigNumber hash, ecPoint& MPK_fq, ecPoint& Q_fq, ecPoint& G0_fq, ellipticCurveFq& E_Fq) {
	BigNumber max_hash(MAX_NUMBER_256);
	//BigNumber hash = getRandom(max_hash);
	//cout << "\n hash: " << hash.toDecString() << endl;

	ecPoint H_fq = hashToPoint(hash);//hashToPointFq(secret_fq, hash, E_Fq);

	BigNumber shash = getRandom(max_hash);
	ecPoint S_fq = hashToPointFq(S1_fq, shash, E_Fq);

	ExtensionField::Element rr, bb, cc;
	rr = tatePairing(S2_fq, G0_fq, S_fq, E_Fq);
	bb = tatePairing(Q_fq, MPK_fq, S_fq, E_Fq);
	cc = tatePairing(H_fq, S1_fq, S_fq, E_Fq);
	//return 0;
	// TODO: cacl instead of hard-code
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
	//E_Fq.field->pow(b1c1, bbcc, strEta);
	//cout << "\n r1: " << endl;
	//E_Fq.field->writeElement(r1);
	//cout << "\n b1: " << endl;
	//E_Fq.field->writeElement(b1);
	//cout << "\n c1: " << endl;
	//E_Fq.field->writeElement(c1);
	//cout << "\n b1c1: " << endl;
	//E_Fq.field->writeElement(b1c1);
	//cout << "\n Verified: " << areEqual << endl;
	return areEqual;
}

std::string dectox_int(Integer num)
{
	std::stringstream ss;
	std::vector<int> tmp;
	while (num != 0)
	{
		Integer rest = num % 16;
		num /= 16;
		tmp.push_back(rest);
	}
	for (int i = (tmp.size() - 1); i >= 0; i--)
		ss << std::hex << tmp[i];
	return std::string(ss.str());
}