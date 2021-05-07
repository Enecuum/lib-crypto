#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE  

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "crypto.h"
using namespace std;

//class Point
//{
//	private:
//		Point(BigNumber x, BigNumber y, EC_GROUP *curve) {
//			this->P = EC_POINT_new(curve);
//			if (1 != EC_POINT_set_affine_coordinates_GFp(curve, P, x.bn, y.bn, NULL)) return;
//		}
//		EC_POINT *P;
//		friend class Curve;
//		
//};

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

	//EC_GROUP *curve1;
	int maxi = 400000000;
	int homManyPrint = maxi*0.001;
	int printEachi = maxi/(homManyPrint);
	for (int ii = 0; ii < maxi; ++ii)
	{
		
		BigNumber a(25);
		BigNumber b(978);
		BigNumber p(1223);
		BigNumber order(1183);
		BigNumber g0x(972);
		BigNumber g0y(795);

																						//cout << "a: " << a.decimal() << endl;
																						//cout << "b: " << b.decimal() << endl;
																						//cout << "p: " << p.decimal() << endl;
																						//cout << "G0: (" << g0x.decimal() << " " << g0y.decimal() << ")" << endl;
																						//cout << "order: " << order.decimal() << endl;
		Curve* curve = NULL; 
		curve = new Curve(a, b, p, order, g0x, g0y);
																						//if (NULL == (curve1 = create_curve(a, b, p, order, g0x, g0y)))
																						//	std::cout << "error" << endl;

//IK added
		EC_POINT *Q = NULL;
		EC_POINT* MPK = NULL;
		EC_POINT* G = NULL;
		EC_POINT* G0 = NULL;
		EC_POINT* check = NULL;
		EC_POINT* s1 = NULL;
		EC_POINT* s2 = NULL;
		EC_POINT* H = NULL;
		vector<EC_POINT*> proj;
		EC_POINT* secret = NULL;
		EC_POINT* S = NULL;		
//IK added

		BigNumber msk(8);// = getRandom(BigNumber(1223));

		cout << "MSK: " << msk.decimal() << endl;

		BigNumber q(13);	// G0 order
		unsigned char ch[2] = { 0x4, 0x86 };
		BigNumber gx(ch, 2);
		BigNumber gy(92);

																						//Curve cv(a, b, p, order, g0x, g0y);
																						//EC_POINT *tst = cv.createPoint(gx, gy);
																						//std::cout << "===============tst: ";
																						//printPoint(tst, cv.getCurve());

//IK modified
		//EC_POINT* G0 = EC_POINT_new(curve->curve);	
		G0 = EC_POINT_new(curve->curve);
		
//IK modified
		if (1 != EC_POINT_set_affine_coordinates_GFp(curve->curve, G0, g0x.bn, g0y.bn, NULL)) handleErrors();
//IK modified

		//EC_POINT* G = EC_POINT_new(curve->curve);
		G = EC_POINT_new(curve->curve);
//IK modified
		if (1 != EC_POINT_set_affine_coordinates_GFp(curve->curve, G, gx.bn, gy.bn, NULL)) handleErrors();


		std::cout << "G0: ";
		printPoint(G0, curve);

//IK modified
		//EC_POINT* MPK = createMPK(msk, G0, curve);
		MPK = createMPK(msk, G0, curve);
//IK modified


		std::cout << "MPK: ";
		printPoint(MPK, curve);

		std::cout << "\r\n      PKG keys generation \r\n";

																			// Вычисляем публичный ключ на этот сеанс
																			// Второй генератор G (gens[1], 1158, 92)
																			// Соучайный элемент из поля q = 13 (поля порядка gens[1])
																			// TODO: set_random_seed(LPoSID + KblockID)
		int KblockID = 123;
		int LPoSID = 677321;
																			// r следует брать соучайно r = ZZ.random_element(q)
		BigNumber r(7);// = getRandom(q);

		std::cout << "Random r: " << r.decimal() << endl;
//IK modified
		//EC_POINT* Q = mul(r, G, curve);
		Q = mul(r, G, curve);
//IK modified

		std::cout << "Q = r * G: ";
		printPoint(Q, curve);
		
		
		

		
		std::cout << "Key sharing: " << endl;
		vector<int> ids = { 1, 55, 10 };
		vector<BigNumber> shares = shamir(msk, ids, 3, 2, q);


		for (int i = 0; i < shares.size(); i++)
			std::cout << "(" << shares[i].decimal() << "), ";


		vector<int> coalition = { 1, 3 };
		std::cout << "\r\nShadows: " << "\r\n";
//IK modified
		//vector<EC_POINT*> proj = keyProj(coalition, shares, Q, curve);
		proj = keyProj(coalition, shares, Q, curve);
//IK modified
		for (int i = 0; i < proj.size(); i++) {
			printPoint(proj[i], curve);
		}



		vector<int> coalition2 = { 1, 10 };
		std::cout << "\r\n      Key recovery" << endl;
//IK modified
		//EC_POINT* secret = keyRecovery(proj, coalition2, q, curve);
		secret = keyRecovery(proj, coalition2, q, curve);
//IK modified
		
		std::cout << "Recovered secret SK: \t";
		printPoint(secret, curve);

		
		std::cout << "Check secret MSK * Q: \t";
//IK modified
		//EC_POINT* check = mul(msk, Q, curve);
		 check = mul(msk, Q, curve);

//IK modified
		printPoint(check, curve);

		std::cout << "\r\n      Create signature" << endl;


		BigNumber M(200);
		BigNumber r2(7);// = getRandom(q);
		cout << "r2: " << r2.decimal() << endl;
//IK modified
		//EC_POINT* s1;
//IK modified
		// R = rP
		s1 = mul(r2, G0, curve);

		std::cout << "S1: ";
		printPoint(s1, curve);

																					// set_random_seed(LPoSID+M)
																					// H = E.random_point()
																					// Тут хеширование, но пока берется "случайная" точка кривой


																					//BigNumber hx(681);
																					//BigNumber hy(256);
//IK modified
		//EC_POINT* H = hashToPoint(BigNumber(7), curve);
		H = hashToPoint(BigNumber(7), curve);
//IK modified
		printPoint(H, curve);
																					// S2 = r*H + SecKey
																					// S = sQ + rH
//IK modified
		//EC_POINT* s2 = mul(r2, H, curve);
		s2 = mul(r2, H, curve);
//IK modified
		if (1 != EC_POINT_add(curve->curve, s2, s2, secret, NULL)) handleErrors();

		std::cout << "S2: ";
		printPoint(s2, curve);

		std::cout << "      Verification" << "\r\n";
		std::cout << "Weil pairing" << "\r\n";

		BigNumber sx(0);
		BigNumber sy(522);
//IK modified
		//EC_POINT* S = EC_POINT_new(curve->curve);
		S = EC_POINT_new(curve->curve);
//IK modified
		if (1 != EC_POINT_set_affine_coordinates_GFp(curve->curve, S, sx.bn, sy.bn, NULL)) handleErrors();

		BigNumber r1 = weilPairing(G0, s2, S, curve);
		std::cout << "r1 = e(P, S)\t" << r1.decimal() << "\r\n";

		BigNumber b1 = weilPairing(MPK, Q, S, curve);
		std::cout << "b1 = e(MPK, Q)\t" << b1.decimal() << "\r\n";

		BigNumber c1 = weilPairing(s1, H, S, curve);
		std::cout << "c1 = e(R, H1)\t" << c1.decimal() << "\r\n";

		BigNumber b1c1 = (b1 * c1) % p;
		std::cout << "r1 = b1 * c1\t" << b1c1.decimal() << "\r\n";
		
		/************/

		
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
//IK added
		EC_POINT_free(S);
		for (int j = 0; j < proj.size(); ++j)
			EC_POINT_free(proj.at(j));		
//IK added
		delete(curve);
		

		if (ii% printEachi == 0) {
			std::cout << "[" << ii << /*"/" << maxi <<*/ "] ";
			//system("pause");
			std::cout << endl;
		}
	}
	//EC_GROUP_free(curve.curve);
	//}
	//std::cout.clear();
	//cout << "runtime = " << clock() / 1000.0 << endl; // время работы программы         
	system("pause");
	return 0;
}