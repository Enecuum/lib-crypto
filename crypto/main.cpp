#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE  

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "crypto.h"

using namespace std;

class Point
{
	private:
		Point(BigNumber x, BigNumber y, EC_GROUP *curve) {
			this->P = EC_POINT_new(curve);
			if (1 != EC_POINT_set_affine_coordinates_GFp(curve, P, x.bn, y.bn, NULL)) return;
		}
		EC_POINT *P;
		friend class Curve;
		
};

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
	unsigned char primeCh[32] = { 0x23, 0x70, 0xfb, 0x04, 0x9d, 0x41, 0x0f, 0xbe, 0x4e, 0x76, 0x1a, 0x98, 0x86, 0xe5, 0x02, 0x41, 0x7d, 0x02, 0x3f, 0x40, 0x18, 0x00, 0x00, 0x01, 0x7e, 0x80, 0x60, 0x00, 0x00, 0x00, 0x00, 0x01 };
	BigNumber a(0);
	BigNumber b(5);
	unsigned char orderCh[32] = { 0x23, 0x70, 0xfb, 0x04, 0x9d, 0x41, 0x0f, 0xbe, 0x4e, 0x76, 0x1a, 0x98, 0x86, 0xe5, 0x02, 0x41, 0x1d, 0xc1, 0xaf, 0x70, 0x12, 0x00, 0x00, 0x01, 0x7e, 0x80, 0x60, 0x00, 0x00, 0x00, 0x00, 0x01 };
	BigNumber p(primeCh, 32);
	BigNumber order(orderCh, 32);

	unsigned char gxCh[32] = { 0x1e, 0x98, 0x4a, 0xc1, 0x56, 0xde, 0x86, 0x90, 0xd0, 0xe1, 0xee, 0x61, 0xf4, 0x1a, 0x8c, 0xd2, 0xb0, 0xd9, 0x9e, 0x9d, 0x50, 0x94, 0x84, 0xb2, 0x00, 0xdd, 0x6d, 0x14, 0x1d, 0x68, 0x56, 0x95 };
	unsigned char gyCh[32] = { 0x01, 0x57, 0x56, 0x79, 0x2c, 0xc8, 0x16, 0x71, 0x48, 0x1b, 0xb4, 0x83, 0x0f, 0x0d, 0x3f, 0xd7, 0x20, 0xba, 0x97, 0xb3, 0x10, 0x60, 0xc6, 0xec, 0x91, 0x1a, 0x9a, 0xeb, 0x1c, 0x27, 0x52, 0x29 };
	BigNumber g0x(gxCh, 32);
	BigNumber g0y(gyCh, 32);

	BigNumber gx(gxCh, 32);
	BigNumber gy(gyCh, 32);
	cout << "a: " << a.toDecString() << endl;
	cout << "b: " << b.toDecString() << endl;
	cout << "p: " << p.toDecString() << endl;
	cout << "G0: (" << g0x.toDecString() << " " << g0y.toDecString() << ")" << endl;
	cout << "order: " << order.toDecString() << endl;
	Curve *curve = new Curve(a, b, p, order, g0x, g0y);
	//if (NULL == (curve1 = create_curve(a, b, p, order, g0x, g0y)))
	//	std::cout << "error" << endl;

	BigNumber msk(8);// = getRandom(BigNumber(1223));
	cout << "MSK: " << msk.toDecString() << endl;
	BigNumber q = order;	// G0 order


	//Curve cv(a, b, p, order, g0x, g0y);
	//EC_POINT *tst = cv.createPoint(gx, gy);
	//std::cout << "===============tst: ";
	//printPoint(tst, cv.getCurve());


	EC_POINT *G0 = EC_POINT_new(curve->curve);
	if (1 != EC_POINT_set_affine_coordinates_GFp(curve->curve, G0, g0x.bn, g0y.bn, NULL)) handleErrors();
	EC_POINT *G = EC_POINT_new(curve->curve);
	if (1 != EC_POINT_set_affine_coordinates_GFp(curve->curve, G, gx.bn, gy.bn, NULL)) handleErrors();

	std::cout << "G0: ";
	printPoint(G0, curve);
	EC_POINT *MPK = createMPK(msk, G0, curve);

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
	BigNumber r(4);// = getRandom(q);

	std::cout << "Random r: " << r.toDecString() << endl;
	EC_POINT *Q = mul(r, G, curve);

	std::cout << "Q = r * G: ";
	printPoint(Q, curve);
	/*
	std::cout << "Key sharing: " << endl;
	vector<int> ids = { 1, 55, 10 };
	vector<BigNumber> shares = shamir(msk, ids, 3, 2, q);
	for (int i = 0; i < shares.size(); i++)
		std::cout << "(" << shares[i].decimal() << "), ";

	vector<int> coalition = {1, 3 };
	std::cout << "\r\nShadows: " << "\r\n";
	vector<EC_POINT*> proj = keyProj(coalition, shares, Q, curve);
	for (int i = 0; i < proj.size(); i++) {
		printPoint(proj[i], curve);
	}
	vector<int> coalition2 = { 1, 10 };
	std::cout << "\r\n      Key recovery" << endl;
	EC_POINT *secret = keyRecovery(proj, coalition2, q, curve);

	std::cout << "Recovered secret SK: \t";
	printPoint(secret, curve);
	*/
	std::cout << "Check secret MSK * Q: \t";
	EC_POINT *check = mul(msk, Q, curve);
	printPoint(check, curve);

	std::cout << "\r\n      Create signature" << endl;
	
	BigNumber M(200);
	BigNumber r2(2);// = getRandom(q);
	cout << "r2: " << r2.toDecString() << endl;
	EC_POINT *s1;
	// R = rP
	s1 = mul(r2, G0, curve);

	std::cout << "S1: ";
	printPoint(s1, curve);
/*
	// set_random_seed(LPoSID+M)
	// H = E.random_point()
	// Тут хеширование, но пока берется "случайная" точка кривой
	
	
	//BigNumber hx(681);
	//BigNumber hy(256);
	EC_POINT *H = hashToPoint(BigNumber(7), curve);
	printPoint(H, curve);
	// S2 = r*H + SecKey
	// S = sQ + rH
	EC_POINT *s2 = mul(r2, H, curve);
	if (1 != EC_POINT_add(curve->curve, s2, s2, secret, NULL)) handleErrors();

	std::cout << "S2: ";
	printPoint(s2, curve);

	std::cout << "      Verification" << "\r\n";
	std::cout << "Weil pairing" << "\r\n";

	BigNumber sx(0);
	BigNumber sy(522);
	EC_POINT *S = EC_POINT_new(curve->curve);
	if (1 != EC_POINT_set_affine_coordinates_GFp(curve->curve, S, sx.bn, sy.bn, NULL)) handleErrors();

	BigNumber r1 = weilPairing(G0, s2, S, curve);
	std::cout << "r1 = e(P, S)\t" << r1.decimal() << "\r\n";

	BigNumber b1 = weilPairing(MPK, Q, S, curve);
	std::cout << "b1 = e(MPK, Q)\t" << b1.decimal() << "\r\n";

	BigNumber c1 = weilPairing(s1, H, S, curve);
	std::cout << "c1 = e(R, H1)\t" << c1.decimal() << "\r\n";

	BigNumber b1c1 = (b1 * c1) % p;
	std::cout << "r1 = b1 * c1\t" << b1c1.decimal() << "\r\n";

	*/
	//BN_CTX_free(ctx);
	//EC_POINT_free(secret);
	
	//EC_GROUP_free(curve.curve);
	//}
	//std::cout.clear();
	//cout << "runtime = " << clock() / 1000.0 << endl; // время работы программы         
	//system("pause");
	Integer Ip, Im;
	Ip = Integer("16030569034403128277756688287498649515636838101184337499778392980116222246913");
	Im = Integer(12);
	string strIrred("12 1 7 16030569034403128277756688287498649515636838101184337499778392980116222246896 16030569034403128277756688287498649515636838101184337499778392980116222246710 16030569034403128277756688287498649515636838101184337499778392980116222246885 2309 2992 16030569034403128277756688287498649515636838101184337499778392980116222237244 16030569034403128277756688287498649515636838101184337499778392980116222225365 3429 48555 63122 37991");
	string strA("0 0");
	string strB("0 5");
	
	ellipticCurve *ec = new ellipticCurve(Ip, Im, strIrred, strA, strB);
	ec->print();
	ellipticCurveFq E_Fq(ec);
	E_Fq.show();
	ExtensionField::Element HFq_x, HFq_y, SFq_x, SFq_y;

	ecPoint secret_fq = mapToFq(check, curve, E_Fq);
	cout << "\n secret fq: " << endl;
	E_Fq.show(secret_fq);

	string strHx_fq("11 12795200616325864128301600439533238625836570855973778800724207558943117513205 12130884351496920759643149235993696557935956626275118316469563330330421956954 6283040097693372918304058385692122958293949981328347340387761786450421554751 4899237897159181067640145765574944595870830883586148580522329018622189949897 8846355464997914784016362869867541795213329883451326397888469916812886128424 14129079141784273041581234757445313819197902405344241350499624439513305758998 13352072847043331449752665150855039962671167605050063905273191449284813579560 6393127232507087088463231637082626347554201215151264493277478426389978548816 11055793717890002963273053758670586391406977509500385190412071151130460455343 6377301085569399016620998198775940872073165203580132449119152990662002772535 11447275512728877891081113656170964162742321407569066846904189083117611540998 5937852374468625217547352015728789862599812933954999569332681032009432549840");
	string strHy_fq("11 3218839677179312609018856855655013274124364702869282379100183166286769237998 15044457283554428446408480119819863018909478296385264354499712915354078803313 10836697868833735308741152896657122315989750961978455786725014823625823018718 6252900733548808857614294347803602330676526195807488170093773710720709936698 9072164750976273151044609579754931421152548695706084093022299941929293303770 1581051531791168413564622242556945736630412913604996002010381621887765731409 3095295538339275550800957843390877304418396432741414862000545449582323768268 11825058412863805355749018175020855939988339734006085226357534946021033274444 664800404171033514083577273183587063464881026711383308491155722761791501750 11913374880181628896317996635569558849448830303277386417285222453712730872323 11428928723220629553432075124518828787664163337109018680143588209208797492122 1104801707771267947748586133547196191205177949442762583878743413246435741153");
	E_Fq.field->readElement(strHx_fq, HFq_x);
	E_Fq.field->readElement(strHy_fq, HFq_y);
	ecPoint H_fq(HFq_x, HFq_y);
	cout << "\n H_fq: " << endl;
	E_Fq.show(H_fq);
	ecPoint rH, S2_fq;
	E_Fq.scalarMultiply(rH, H_fq, (Integer)2, -1);//R=6*P, order of P is not required
	cout << "\n rH: " << endl;
	E_Fq.show(rH);

	E_Fq.add(S2_fq,rH, secret_fq);//R=P+Q
	cout << "\n S2_fq: " << endl;
	E_Fq.show(S2_fq);
	string strSx_fq("11 7463519085493365693394121199190059745988511279031308831763630965575497714973 3213479576728694612873129502574939146123496605844840557128765993775758126890 13752713972998144419152467731240251127964542833056708196491325125872366091087 15553372254183114510739439757246731793806383848608048309939148850948110186888 12258644319749650037144571587825036927155675497479416759205860608047734134062 3532101921007362030794325086925105560686780697780826036915262551502427550164 4355439161355160537858443362077205542687166906735462310058911937938440403416 3802272190851511210630147460266578785161879155516601953628860280715370308621 1317637193017543797636378441570532755116450388205874274160281092658933403629 5473798230129279492859744018818548971274864114987064021826091310815335879733 8851270884955316309046441361625390056442140408234485954600071180679479553237 15132257993022606200919632833688451974231808385319226086684277348612204805425");
	string strSy_fq("11 5402906328658268916194312952865326995905954868409589434741588571597549983634 391870371242143485522013522668372963939928916979327764650555183010158927409 8388867712234558980469512863505598308802851258476952237041272427075151744109 7495609234823091861100624264137916927683614152015976278448757663646649941991 5658906808188150074100165944955289756224642167129582121658117411584296860873 13878886333438371495866267885612108469039825873739305210045820592485652966585 6963650853987273077816544478754914531960456246032726948867288748211675615415 8830885856243348641350531158188320567660408310040255566312664501804444943080 6106755154037675810581056224691089803765559530574180661442422341382742080274 5485664565367343310789520189176634801281297522570962649298265524017950485938 9805891012360959361564020302204274589333018098732386052032042786398105438146 15198026600655527523115482280007159805633263781115846119482141707705953976273");
	E_Fq.field->readElement(strSx_fq, SFq_x);
	E_Fq.field->readElement(strSy_fq, SFq_y);
	ecPoint S_fq(SFq_x, SFq_y);
	cout << "\n S_fq: " << endl;
	E_Fq.show(S_fq);

	ecPoint G0_fq = mapToFq(G, curve, E_Fq);
	cout << "\n G0_fq: " << endl;
	E_Fq.show(G0_fq);

	ecPoint Q_Fq;
	E_Fq.scalarMultiply(Q_Fq, G0_fq, (Integer)(msk.toDecString()), -1);

	ExtensionField::Element r1 = tatePairing(G0_fq, S2_fq, S_fq, E_Fq);
	cout << "\n r1: " << endl;
	//E_Fq.field->writeElement(r1);

	std::stringstream ss;
	E_Fq.field->writeElement(r1, ss);
	std::string ss1, ss2, ss3;
	//ss >> ss1 >> ss2 >> ss3;
	cout << ss.str() << endl;

	string temp;
	while (!ss.eof()) {
		ss >> temp;
		if (temp == "+")
			continue;
		std::size_t start = temp.find("(");
		std::size_t end = temp.find(")");
		std::string str3 = temp.substr(start + 1, end - 1);
		cout << str3 << endl;
		temp = "";
	}
	
	ecPoint MPK_fq = mapToFq(MPK, curve, E_Fq);
	ecPoint S1_fq = mapToFq(s1, curve, E_Fq);

/*
	ExtensionField::Element b1 = tatePairing(MPK_fq, Q_Fq, S_fq, E_Fq);
	//cout << "\n b1: " << endl;
	//E_Fq.field->writeElement(b1);

	ExtensionField::Element c1 = tatePairing(S1_fq, H_fq, S_fq, E_Fq);
	//cout << "\n c1: " << endl;
	//E_Fq.field->writeElement(c1);

	ExtensionField::Element b1c1;
	E_Fq.field->mul(b1c1, b1, c1);
	cout << "\n b1c1: " << endl;
	E_Fq.field->writeElement(b1c1);
	*/
	//int verify = (r1 == b1c1)
	//cout << "\n Verified: " << verify << endl;
	cout << "\r\nruntime is: " << clock() / 1000.0 << endl; // время работы программы  

	EC_POINT_free(Q);
	EC_POINT_free(MPK);
	EC_POINT_free(G);
	EC_POINT_free(G0);
	EC_POINT_free(check);
	EC_POINT_free(s1);
	delete(curve);
	return 0;
}