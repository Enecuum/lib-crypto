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

	//EC_GROUP *curve1;
	BigNumber a(1);
	BigNumber b(0);
	BigNumber p("80000000000000000000000000000000000200014000000000000000000000000000000000010000800000020000000000000000000000000000000000080003");

	BigNumber order("80000000000000000000000000000000000200014000000000000000000000000000000000010000800000020000000000000000000000000000000000080004");

	//BigNumber g0x("2920f2e5b594160385863841d901a3c0a73ba4dca53a8df03dc61d31eb3afcb8c87feeaa3f8ff08f1cca6b5fec5d3f2a4976862cf3c83ebcc4b78ebe87b44177");
	//BigNumber g0y("2c022abadb261d2e79cb693f59cdeeeb8a727086303285e5e629915e665f7aebcbf20b7632c824b56ed197f5642244f3721c41c9d2e2e4aca93e892538cd198a");

	BigNumber gx("2920f2e5b594160385863841d901a3c0a73ba4dca53a8df03dc61d31eb3afcb8c87feeaa3f8ff08f1cca6b5fec5d3f2a4976862cf3c83ebcc4b78ebe87b44177");
	BigNumber gy("2c022abadb261d2e79cb693f59cdeeeb8a727086303285e5e629915e665f7aebcbf20b7632c824b56ed197f5642244f3721c41c9d2e2e4aca93e892538cd198a");
	BigNumber max_hash("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
	BigNumber orderQ("3298c");
	//BigNumber gx = g0x;
	//BigNumber gy = g0y;
	cout << "a: " << a.toDecString() << endl;
	cout << "b: " << b.toDecString() << endl;
	cout << "p: " << p.toDecString() << endl;
	//cout << "G0: (" << g0x.toDecString() << " " << g0y.toDecString() << ")" << endl;
	cout << "order: " << order.toDecString() << endl;
	Curve *curve = new Curve(a, b, p, order, gx, gy);
	//if (NULL == (curve1 = create_curve(a, b, p, order, g0x, g0y)))
	//	std::cout << "error" << endl;

	BigNumber msk(10000000);// = getRandom(BigNumber(1223));
	cout << "MSK: " << msk.toDecString() << endl;
	BigNumber q = order;	// G0 order


	//Curve cv(a, b, p, order, g0x, g0y);
	//EC_POINT *tst = cv.createPoint(gx, gy);
	//std::cout << "===============tst: ";
	//printPoint(tst, cv.getCurve());


	//EC_POINT *G0 = EC_POINT_new(curve->curve);
	//if (1 != EC_POINT_set_affine_coordinates_GFp(curve->curve, G0, g0x.bn, g0y.bn, NULL)) handleErrors();
	EC_POINT *G = EC_POINT_new(curve->curve);
	if (1 != EC_POINT_set_affine_coordinates_GFp(curve->curve, G, gx.bn, gy.bn, NULL)) handleErrors();

	//std::cout << "G0: ";
	//printPoint(G0, curve);
	//EC_POINT *MPK = createMPK(msk, G0, curve);

	//std::cout << "MPK: ";
	//printPoint(MPK, curve);

	std::cout << "\r\n      PKG keys generation \r\n";

	// Вычисляем публичный ключ на этот сеанс
	// Второй генератор G (gens[1], 1158, 92)
	// Соучайный элемент из поля q = 13 (поля порядка gens[1])
	// TODO: set_random_seed(LPoSID + KblockID)

	// r следует брать соучайно r = ZZ.random_element(q)
	BigNumber r = getRandom(max_hash);
	std::cout << "Random r: " << r.toDecString() << endl;

	Integer Ip, Im;
	Ip = Integer("6703903964971298549787012499102923063739684112761466562144343758833001675653841939454385015500446199477853424663597373826728056308768000892499915006541827");
	Im = Integer(2);
	string strIrred("2 1 1 6703903964971298549787012499102923063739684112761466562144343758833001675653841939454385015500446199477853424663597373826728056308768000892499915006541826");
	string strA("0 1");
	string strB("0 0");
	string strG0_x("1 1971424652593645857677685913504949042673180456464917721388355467732670356866868453718540344482523620218083146279366045128738893020712321933640175997249379 4296897641464992034676854814757495000621938623767876348735377415270791885507945430568382535788680955541452197460367952645174915991662132695572019313583345");
	string strG0_y("1 5439973223440119070103328012315186243431766339870489830477397472399815594412903491893756952248783128391927052429939035290789135974932506387114453095089572 3254491657578196534138971223937186183707778225921454196686815561535427648524577315556854258504535233566592842007776061702323300678216177012235337721726634");
	ellipticCurve *ec = new ellipticCurve(Ip, Im, strIrred, strA, strB);
	//ec->print();
	ellipticCurveFq E_Fq(ec);

	//ecPoint tmpQ;// = hashToPoint(max_hash);
	//ecPoint tmp;
	//ecPoint zero;
	//int isZero = 1;
	//E_Fq.scalarMultiply(zero, tmpQ, (Integer)(0), -1);
	//do {
		BigNumber qhash("7cd925afaffb8466029213a05ae0faaff9c533dfb3ae446dbfcb971e45e2cacf");// = getRandom(max_hash);
	//	tmpQ = hashToPoint(qhash);
	//	E_Fq.scalarMultiply(tmp, tmpQ, (Integer)(orderQ.toDecString()), -1);
	//	isZero = (tmp == zero);
	//	std::cout << "qhash: " << qhash.toDecString() << " " << isZero << endl;
	//} while (isZero);
	//cout << "\n tmpQ: " << endl;
	//E_Fq.show(tmpQ);
	//E_Fq.scalarMultiply(tmp, tmpQ, (Integer)(orderQ.toDecString()), -1);

	//BigNumber qx(dectox_int(tmp.x[0]));
	//BigNumber qy(dectox_int(tmp.y[0]));

	EC_POINT *Q = getQ(qhash, curve, E_Fq);
	//if (1 != EC_POINT_set_affine_coordinates_GFp(curve->curve, Q, qx.bn, qy.bn, NULL)) handleErrors();

	std::cout << "Q: ";
	printPoint(Q, curve);

	std::cout << "Key sharing: " << endl;
	vector<BigNumber> ids;// = { 1, 55, 10 };
	for (int i = 0; i < 100; i++)
		ids.push_back(BigNumber(i + 1));
	vector<BigNumber> shares = shamir(msk, ids, 100, 3, q);
	for (int i = 0; i < shares.size(); i++)
		std::cout << "(" << shares[i].toDecString() << "), ";

	vector<int> coalition = { 1, 55, 10 };
	//for (int i = 0; i < 5; i++)
	//	coalition.push_back(i + 1);
	std::cout << "\r\nShadows: " << "\r\n";
	vector<EC_POINT*> proj = keyProj(coalition, shares, Q, curve);
	for (int i = 0; i < proj.size(); i++) {
		printPoint(proj[i], curve);
	}
	vector<BigNumber> coalition2 = { BigNumber(1), BigNumber(55), BigNumber(10) };
	std::cout << "\r\n      Key recovery" << endl;
	BigNumber q1("287a1a55f1c28b1c23a27eef69b6a537e5dfd068d43a34951ed645e049d6be0ac805e3c45501be831afe2d40a2395d8c72edb186c6d140bb85ae022a074b");
	EC_POINT *secret = keyRecovery(proj, coalition2, q1, curve);

	std::cout << "Recovered secret SK: \t";
	printPoint(secret, curve);
	
	std::cout << "Check secret MSK * Q: \t";
	EC_POINT *check = mul(msk, Q, curve);
	printPoint(check, curve);

	//return 0;
	std::cout << "\r\n      Create signature" << endl;
	
	BigNumber M(200);
	BigNumber r2 = getRandom(q);
	//cout << "r2: " << r2.toDecString() << endl;
	EC_POINT *s1;
	// R = rP
	s1 = mul(r2, G, curve);

	//std::cout << "S1: ";
	//printPoint(s1, curve);
	bool rres = verify_mobile(
		"6703903964971298549787012499102923063739684112761466562144343758833001675653841939454385015500446199477853424663597373826728056308768000892499915006541827",
		"0 1",
		"0 0",
		"80000000000000000000000000000000000200014000000000000000000000000000000000010000800000020000000000000000000000000000000000080004",
		"2 1 1 6703903964971298549787012499102923063739684112761466562144343758833001675653841939454385015500446199477853424663597373826728056308768000892499915006541826",
		"1 1971424652593645857677685913504949042673180456464917721388355467732670356866868453718540344482523620218083146279366045128738893020712321933640175997249379 4296897641464992034676854814757495000621938623767876348735377415270791885507945430568382535788680955541452197460367952645174915991662132695572019313583345",
		"1 5439973223440119070103328012315186243431766339870489830477397472399815594412903491893756952248783128391927052429939035290789135974932506387114453095089572 3254491657578196534138971223937186183707778225921454196686815561535427648524577315556854258504535233566592842007776061702323300678216177012235337721726634",
		2,
		"1 6463583063506853453352145388557562342045097230688492424308762960259148626679694233304792496993896826204859767286406402028661773263552351316088010972635992 1416364819800415093000147933796337994391339019800078311831844813166613331367052012412062135770454209668583451908837134818641138833984169440926085257140004",
		"1 6078398104078171632215695320773804401035385252644159046477855921450504663081503930748506940646128360937810792876085571074711657927225554046396682784191095 684613704912171420676283070735540821105033103251441886765466710413059666511493248245492678777726184493459839136863344011812422098470079101205394677191475",
		"0 3470831550237819672838077551471498136543133545444604390853161848692005042308124460409689740770641992155090555806050873579947700883623574639834581493212985",
		"0 1733574993285795463951614608926130592634635270276769479113634444427354889391360909387986984065006253554975343438496577407919375391296206198317458722434660",
		"7cd925afaffb8466029213a05ae0faaff9c533dfb3ae446dbfcb971e45e2cacf",
		"e3ef5e0cb7f89dfc1744003d9927bf588936e5a348641cf3984643a96014e22a",
		"1 5553161562309620134204294307547179611636685754291535214076054128059515506033993893714735433610207337513297007943745903960736280984281319771387455094363055 6689291821874770449783470798967833024384908153897387642241791007779898182075451742944983283446597677757107561868230869942692013197487003982057559827210919",
		"1 1409647240359222769554276508179480635408566023738878356661595724530348198250043359650835609016647773316137383233659454103710745566316894573620890364518372 3885560331261099024236364110126283369754589356789796979215968181927252095476961502262491073346156231135118163091525459453361435352300956275516792143616474"
	);
	return 0;
	
	cout << "\n ------------------------------------------------------------------------------------ " << endl;
	ExtensionField::Element HFq_x, HFq_y, SFq_x, SFq_y, G0_x, G0_y;

	E_Fq.field->readElement(strG0_x, G0_x);
	E_Fq.field->readElement(strG0_y, G0_y);
	ecPoint G0_fq(G0_x, G0_y);
	ecPoint MPK_fq;
	E_Fq.scalarMultiply(MPK_fq, G0_fq, (Integer)(msk.toDecString()), -1);
	cout << "\n MPK_fq: " << endl;
	E_Fq.show(MPK_fq);
	ecPoint secret_fq = mapToFq(secret, curve, E_Fq);

	/*
	BigNumber hash("c8411fd7002be15c6d266bdf516fbd49ef101e0e6454c4fcac0a0736e7ad15aa");
	ecPoint eee = hashToPointFq(secret_fq, hash, E_Fq);
	cout << "\n eee " << endl;
	E_Fq.show(eee);
		return 0;
*/


	cout << "\n secret fq: " << endl;
	E_Fq.show(secret_fq);
	BigNumber hash("e3ef5e0cb7f89dfc1744003d9927bf588936e5a348641cf3984643a96014e22a");// = getRandom(max_hash);
	cout << "\n hash: " << hash.toDecString() << endl;
	//string strHx_fq("1 6585938874884161190249790176567180373159829994480512034157897828690094321702398082583836641936540925052205593236857739108779400699876416733619250033001574 1114870903498799919300311051614230487702143776715306484413118861977974710144846490697848621314514171343204111400320074712855582002282753809846905090592511");
	//string strHy_fq("1 5977369586974353026773600315229776563237092478572059568886286293271512008099964983767401181053880836241479041663079843436368298305692856067127602342710521 2161202195522849472617767678421059087248221940509956469640162759356556534804221688741829365093541793519766900881300788512903369980421169424075071068242390");
	//E_Fq.field->readElement(strHx_fq, HFq_x);
	//E_Fq.field->readElement(strHy_fq, HFq_y);

	//BigNumber yy = hashToPoint(hash);
	//std::cout << std::endl << "yy: " << yy.toDecString() << std::endl;
	//EC_POINT *H = EC_POINT_new(curve->curve);
	//if (1 != EC_POINT_set_affine_coordinates_GFp(curve->curve, H, hash.bn, yy.bn, NULL)) handleErrors();
	
	
	ecPoint H_fq = hashToPoint(hash);//hashToPointFq(secret_fq, hash, E_Fq);
	cout << "\n H_fq: " << endl;
	E_Fq.show(H_fq);
	ecPoint rH, S2_fq;
	
	E_Fq.scalarMultiply(rH, H_fq, (Integer)(r2.toDecString()), -1);//R=6*P, order of P is not required
	//cout << "\n rH: " << endl;
	//E_Fq.show(rH);

	E_Fq.add(S2_fq,rH, secret_fq);//R=P+Q
	cout << "\n S2_fq: " << endl;
	E_Fq.show(S2_fq);

	//ecPoint G0_fq = mapToFq(G, curve, E_Fq);
	//ecPoint MPK_fq = mapToFq(MPK, curve, E_Fq);
	ecPoint S1_fq;
	E_Fq.scalarMultiply(S1_fq, G0_fq, (Integer)(r2.toDecString()), -1);//R=6*P, order of P is not required
	cout << "\n S1_fq: " << endl;
	E_Fq.show(S1_fq);
	//mapToFq(s1, curve, E_Fq);

	//cout << "\n G0_fq: " << endl;
	//E_Fq.show(G0_fq);

	ecPoint Q_Fq = mapToFq(Q, curve, E_Fq);
	ecPoint QQ_Fq;
	E_Fq.scalarMultiply(QQ_Fq, G0_fq, (Integer)(r.toDecString()), -1);//R=6*P, order of P is not required

	BigNumber shash = getRandom(q);
	//string strSx_fq("1 6159497620935257906557343540898005704075905690051056620736859984179681011970149193326830486125500760419791791564998991498449345753926314997987382523405921 3955639758400305255197132500245584107122348438917038996230137507748635098851063891808983517470373013192290448706270855321350612334250594749922405365544392");
	//string strSy_fq("1 1358517738138214687498297006528551934170768973346051715607110811974480465953218306945989515494073925868991969272612769952234634613804574004024545343325167 617848479534351383674347258603898500438322921182801022785593855537092883000984751823642698365488639632813733693500782440637339604880634710794074753004823");
	//E_Fq.field->readElement(strSx_fq, SFq_x);
	//E_Fq.field->readElement(strSy_fq, SFq_y);
	ecPoint S_fq = hashToPointFq(secret_fq, shash, E_Fq);
	cout << "\n S_fq: " << endl;
	E_Fq.show(S_fq);

	cout << "\n Q_Fq: " << endl;
	E_Fq.show(Q_Fq);

	bool res = verifyTate(S1_fq, S2_fq, hash, MPK_fq, Q_Fq, G0_fq, E_Fq);
	return 0;
	//E_Fq.scalarMultiply(Q_Fq, G0_fq, (Integer)(msk.toDecString()), -1);
	//cout << "\n Q_Fq: " << endl;
	//E_Fq.show(Q_Fq);
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
	ExtensionField::Element rr, bb, cc;
	rr = tatePairing(S2_fq, G0_fq, S_fq, E_Fq);
	bb = tatePairing(Q_Fq, MPK_fq, S_fq, E_Fq);
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

	//E_Fq.field->pow(b1c1, bbcc, strEta);
	cout << "\n r1: " << endl;
	E_Fq.field->writeElement(r1);
	cout << "\n b1: " << endl;
	E_Fq.field->writeElement(b1);
	cout << "\n c1: " << endl;
	E_Fq.field->writeElement(c1);
	cout << "\n b1c1: " << endl;
	E_Fq.field->writeElement(b1c1);
	cout << "\n Verified: " << E_Fq.field->areEqual(r1, b1c1) << endl;
	/**/
	//int verify = (r1 == b1c1)
	//cout << "\n Verified: " << verify << endl;
	cout << "\r\nruntime is: " << clock() / 1000.0 << endl; // время работы программы  

	//ExtensionField::Element rand;
	//E_Fq.field->random(rand);
	//cout << "\n rand: " << endl;
	//E_Fq.field->writeElement(rand);
	EC_POINT_free(Q);
	//EC_POINT_free(MPK);
	EC_POINT_free(G);
	//EC_POINT_free(G0);
	EC_POINT_free(check);
	EC_POINT_free(s1);
	delete(curve);
	return 0;
}