#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE  

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "crypto.h"
using namespace std;

int main(int argc, char ** argv)
{
	//OpenSSL_add_all_algorithms();
	//ERR_load_BIO_strings();
	//ERR_load_crypto_strings();

	//srand(time(0));
	//for (int i = 0; i < 1000; i++) {
	//	if ((i % 100) == 0) {
	//		std::cout.clear();
	//		cout << i << endl;
	//	}
	// std::cout.setstate(std::ios_base::failbit);
	//if (NULL == (ctx = BN_CTX_new())) handleErrors();

	EC_GROUP *curve1;
	if (NULL == (curve1 = create_curve()))
		std::cout << "error" << endl;

	
	BigNumber msk(10);// = getRandom(BigNumber(1223));
	cout << "MSK: " << msk.decimal() << endl;
	BigNumber q(13);	// G0 order

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
	BigNumber r = getRandom(q);

	std::cout << "Random r: " << r.decimal() << endl;

	EC_POINT *Q = mul(r, G, curve1);
	std::cout << "Q = r * G: ";
	printPoint(Q, curve1);

	std::cout << "Key sharing: " << endl;
	vector<BigNumber> shares = shamir(msk, 10, 6, q);
	for (int i = 0; i < shares.size(); i++)
		std::cout << "(" << shares[i].decimal() << "), ";

	int coalition[] = { 1,3,5,7,9,10 };
	std::cout << "\r\nShadows: " << "\r\n";
	vector<EC_POINT*> proj = keyProj(coalition, shares, Q, curve1);
	for (int i = 0; i < 6; i++) {
		printPoint(proj[i], curve1);
	}

	std::cout << "\r\n      Key recovery" << endl;
	EC_POINT *secret = keyRecovery(proj, coalition, BigNumber(13), curve1);

	std::cout << "Recovered secret SK: \t";
	printPoint(secret, curve1);

	std::cout << "Check secret MSK * Q: \t";
	EC_POINT *check = mul(msk, Q, curve1);
	printPoint(check, curve1);

	std::cout << "\r\n      Create signature" << endl;

	BigNumber M(200);
	BigNumber r2 = getRandom(q);
	cout << "r2: " << r2.decimal() << endl;
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

	BigNumber b1c1 = (b1 * c1) % BigNumber(1223);
	std::cout << "r1 = b1 * c1\t" << b1c1.decimal() << "\r\n";

	//BN_CTX_free(ctx);
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
	//cout << "runtime = " << clock() / 1000.0 << endl; // время работы программы         
	system("pause");
	return 0;
}