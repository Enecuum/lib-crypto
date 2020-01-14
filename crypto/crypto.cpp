
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
	for (size_t i = 0; i < proj.size(); i++) {
		BigNumber lambda(1);
		for (size_t j = 0; j < proj.size(); j++) {
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
	// Poly: 1077x + 5x^2 + 6x^3 + 8x^4 + x^5
	// {1, 8, 6, 5, 1077}
	vector<int> arrayA = { 5, 1077 };
	return arrayA;
}

vector<BigNumber> shamir(BigNumber secretM, vector<int> ids, int participantN, int sufficientK, BigNumber q)
{
	
	//secretM = 10;
	//participantN = 10;
	//sufficientK = 6;

	size_t power = sufficientK - 1;

	vector<int> arrayA = generatePoly(power);

	vector<BigNumber> arrayK;
	for (size_t i = 0; i < ids.size(); i++)
	{
		BigNumber temp(0);
		for (int j = 0; j < power; j++)
			temp = temp + (BigNumber(arrayA[j]) * (bpow(BigNumber(ids[i]), power - j)));
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
	Integer Ip = Integer("6703903964971298549787012499102923063739684112761466562144343758833001675653841939454385015500446199477853424663597373826728056308768000892499915006541827");
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

void verify_mobile(
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
	std::string mpky,
	std::string strEta
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
	gx.insert(0, "0 ");
	gy.insert(0, "0 ");
	E_Fq.field->readElement(gx, G_x);
	E_Fq.field->readElement(gy, G_y);
	ecPoint G0_fq(G_x, G_y);
	cout << "\n G0_fq: " << endl;
	E_Fq.show(G0_fq);
	// s1 point
	ExtensionField::Element S1_x, S1_y;
	s1x.insert(0, "0 ");
	s1y.insert(0, "0 ");
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
	mpkx.insert(0, "0 ");
	mpky.insert(0, "0 ");
	E_Fq.field->readElement(mpkx, MPK_x);
	E_Fq.field->readElement(mpky, MPK_y);
	ecPoint MPK_fq(MPK_x, MPK_y);
	cout << "\n MPK_fq: " << endl;
	E_Fq.show(MPK_fq);

	string strHx_fq("11 12795200616325864128301600439533238625836570855973778800724207558943117513205 12130884351496920759643149235993696557935956626275118316469563330330421956954 6283040097693372918304058385692122958293949981328347340387761786450421554751 4899237897159181067640145765574944595870830883586148580522329018622189949897 8846355464997914784016362869867541795213329883451326397888469916812886128424 14129079141784273041581234757445313819197902405344241350499624439513305758998 13352072847043331449752665150855039962671167605050063905273191449284813579560 6393127232507087088463231637082626347554201215151264493277478426389978548816 11055793717890002963273053758670586391406977509500385190412071151130460455343 6377301085569399016620998198775940872073165203580132449119152990662002772535 11447275512728877891081113656170964162742321407569066846904189083117611540998 5937852374468625217547352015728789862599812933954999569332681032009432549840");
	string strHy_fq("11 3218839677179312609018856855655013274124364702869282379100183166286769237998 15044457283554428446408480119819863018909478296385264354499712915354078803313 10836697868833735308741152896657122315989750961978455786725014823625823018718 6252900733548808857614294347803602330676526195807488170093773710720709936698 9072164750976273151044609579754931421152548695706084093022299941929293303770 1581051531791168413564622242556945736630412913604996002010381621887765731409 3095295538339275550800957843390877304418396432741414862000545449582323768268 11825058412863805355749018175020855939988339734006085226357534946021033274444 664800404171033514083577273183587063464881026711383308491155722761791501750 11913374880181628896317996635569558849448830303277386417285222453712730872323 11428928723220629553432075124518828787664163337109018680143588209208797492122 1104801707771267947748586133547196191205177949442762583878743413246435741153");
	E_Fq.field->readElement(strHx_fq, HFq_x);
	E_Fq.field->readElement(strHy_fq, HFq_y);
	ecPoint H_fq(HFq_x, HFq_y);
	//cout << "\n H_fq: " << endl;
	//E_Fq.show(H_fq);

	string strSx_fq("11 7463519085493365693394121199190059745988511279031308831763630965575497714973 3213479576728694612873129502574939146123496605844840557128765993775758126890 13752713972998144419152467731240251127964542833056708196491325125872366091087 15553372254183114510739439757246731793806383848608048309939148850948110186888 12258644319749650037144571587825036927155675497479416759205860608047734134062 3532101921007362030794325086925105560686780697780826036915262551502427550164 4355439161355160537858443362077205542687166906735462310058911937938440403416 3802272190851511210630147460266578785161879155516601953628860280715370308621 1317637193017543797636378441570532755116450388205874274160281092658933403629 5473798230129279492859744018818548971274864114987064021826091310815335879733 8851270884955316309046441361625390056442140408234485954600071180679479553237 15132257993022606200919632833688451974231808385319226086684277348612204805425");
	string strSy_fq("11 5402906328658268916194312952865326995905954868409589434741588571597549983634 391870371242143485522013522668372963939928916979327764650555183010158927409 8388867712234558980469512863505598308802851258476952237041272427075151744109 7495609234823091861100624264137916927683614152015976278448757663646649941991 5658906808188150074100165944955289756224642167129582121658117411584296860873 13878886333438371495866267885612108469039825873739305210045820592485652966585 6963650853987273077816544478754914531960456246032726948867288748211675615415 8830885856243348641350531158188320567660408310040255566312664501804444943080 6106755154037675810581056224691089803765559530574180661442422341382742080274 5485664565367343310789520189176634801281297522570962649298265524017950485938 9805891012360959361564020302204274589333018098732386052032042786398105438146 15198026600655527523115482280007159805633263781115846119482141707705953976273");
	E_Fq.field->readElement(strSx_fq, SFq_x);
	E_Fq.field->readElement(strSy_fq, SFq_y);
	ecPoint S_fq(SFq_x, SFq_y);
	//cout << "\n S_fq: " << endl;
	//E_Fq.show(S_fq);


	//cout << "\n G0_fq: " << endl;
	//E_Fq.show(G0_fq);

	ecPoint Q_Fq;
	E_Fq.scalarMultiply(Q_Fq, G0_fq, (Integer)(pk_lpos.data()), -1);

	//std::stringstream ss;
	//E_Fq.field->writeElement(r1, ss);

	//cout << ss.str() << endl;
	//vector<string> elements;
	//string temp;
	//while (!ss.eof()) {
	//	ss >> temp;
	//	if (temp == "+")
	//		continue;
	//	std::size_t start = temp.find("(");
	//	std::size_t end = temp.find(")");
	//	std::string str3 = temp.substr(start + 1, end - 1);
	//	elements.push_back(str3);
	//	//cout << str3 << endl;
	//	temp = "";
	//}

	ExtensionField::Element rr = tatePairing(G0_fq, S2_fq, S_fq, E_Fq);
	ExtensionField::Element bb = tatePairing(MPK_fq, Q_Fq, S_fq, E_Fq);
	ExtensionField::Element cc = tatePairing(S1_fq, H_fq, S_fq, E_Fq);

	// TODO: cacl instead of hard-code
	
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

	//E_Fq.field->pow(b1c1, bbcc, strEta);
	cout << "\n r1: " << endl;
	E_Fq.field->writeElement(r1);
	//cout << "\n b1: " << endl;
	//E_Fq.field->writeElement(b1);
	//cout << "\n c1: " << endl;
	//E_Fq.field->writeElement(c1);
	cout << "\n b1c1: " << endl;
	E_Fq.field->writeElement(b1c1);
}

ellipticCurveFq::Point hashToPoint(BigNumber num) {
	// x = a ^ ((p + 1) / 4)
	Integer Ip = Integer("6703903964971298549787012499102923063739684112761466562144343758833001675653841939454385015500446199477853424663597373826728056308768000892499915006541827");
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
	BigNumber max_hash("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
	//BigNumber hash = getRandom(max_hash);
	cout << "\n hash: " << hash.toDecString() << endl;

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
	BigNumber bnEta("80000000000000000000000000000000000200014000000000000000000000000000000000010000800000020000000000000000000000000000000000080002");

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
	cout << "\n r1: " << endl;
	E_Fq.field->writeElement(r1);
	cout << "\n b1: " << endl;
	E_Fq.field->writeElement(b1);
	cout << "\n c1: " << endl;
	E_Fq.field->writeElement(c1);
	cout << "\n b1c1: " << endl;
	E_Fq.field->writeElement(b1c1);
	cout << "\n Verified: " << areEqual << endl;
	return areEqual;
}