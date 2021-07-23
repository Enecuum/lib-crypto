#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE  

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "crypto.h"

using namespace std;



int main(int argc, char ** argv)
{
	BN_CTX* ctx = nullptr;
	Curve* curve = nullptr;
	EC_POINT* G = nullptr;
	ellipticCurve* ec = nullptr;
	EC_POINT* Q = nullptr;
	vector<EC_POINT*> proj;
	EC_POINT* check = nullptr;
	EC_POINT* secret = nullptr;
	EC_POINT* s1 = nullptr;

	BigNumber a(1);
	BigNumber b(0);
	BigNumber p("80000000000000000000000000000000000200014000000000000000000000000000000000010000800000020000000000000000000000000000000000080003");

	BigNumber order("80000000000000000000000000000000000200014000000000000000000000000000000000010000800000020000000000000000000000000000000000080004");


	BigNumber gx("2920f2e5b594160385863841d901a3c0a73ba4dca53a8df03dc61d31eb3afcb8c87feeaa3f8ff08f1cca6b5fec5d3f2a4976862cf3c83ebcc4b78ebe87b44177");
	BigNumber gy("2c022abadb261d2e79cb693f59cdeeeb8a727086303285e5e629915e665f7aebcbf20b7632c824b56ed197f5642244f3721c41c9d2e2e4aca93e892538cd198a");
	BigNumber max_hash("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
	BigNumber orderQ("3298c");
	cout << "a: " << a.toDecString() << endl;
	cout << "b: " << b.toDecString() << endl;
	cout << "p: " << p.toDecString() << endl;
	cout << "order: " << order.toDecString() << endl;
	if (nullptr == (curve = new Curve(a, b, p, order, gx, gy))) {
		handleError(NO_MEMORY); return INT_ERROR_RET;
	}

	BigNumber msk(10000000);
	cout << "MSK: " << msk.toDecString() << endl;
	BigNumber q = order;	// G0 order

	if (nullptr == (G = EC_POINT_new(curve->curve))) {
		handleError(NO_MEMORY); return INT_ERROR_RET;
	}

	if (nullptr == (ctx = BN_CTX_new())) {
		handleError(NO_MEMORY); return INT_ERROR_RET;
	}

	if (1 != EC_POINT_set_affine_coordinates_GFp(curve->curve, G, gx.bn, gy.bn, ctx)) {
		handleError(NO_MEMORY);	return INT_ERROR_RET;
	}

	std::cout << "\r\n      PKG keys generation \r\n";

	// Сalc public key for this session
	// Second generator  G (gens[1], 1158, 92)
	// Random element gets from field of q = 13 (~ gens[1])

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
	if (nullptr == (ec = new ellipticCurve(Ip, Im, strIrred, strA, strB))) {
		handleError(NO_MEMORY); return INT_ERROR_RET;
	}	

	ellipticCurveFq E_Fq(ec);
	BigNumber qhash("7cd925afaffb8466029213a05ae0faaff9c533dfb3ae446dbfcb971e45e2cacf");
	

	Q = getQ(qhash, curve, E_Fq);
	

	std::cout << "Q: ";
	printPoint(Q, curve);

	std::cout << "Key sharing: " << endl;
	vector<BigNumber> ids;
	for (int i = 0; i < 100; i++)
		ids.push_back(BigNumber(i + 1));
	vector<BigNumber> shares = shamir(msk, ids, 100, 3, q);
	for (int i = 0; i < shares.size(); i++)
		std::cout << "(" << shares[i].toDecString() << "), ";

	vector<int> coalition = { 1, 55, 10 };
	std::cout << "\r\nShadows: " << "\r\n";
	proj = keyProj(coalition, shares, Q, curve);
	for (int i = 0; i < proj.size(); i++) {
		printPoint(proj[i], curve);
	}
	vector<int> coalition2 = { 1, 55, 10 };
	std::cout << "\r\n      Key recovery" << endl;
	BigNumber q1("287a1a55f1c28b1c23a27eef69b6a537e5dfd068d43a34951ed645e049d6be0ac805e3c45501be831afe2d40a2395d8c72edb186c6d140bb85ae022a074b");
	secret = keyRecovery(proj, coalition2, q1, curve);

	std::cout << "Recovered secret SK: \t";
	printPoint(secret, curve);
	
	std::cout << "Check secret MSK * Q: \t";
	check = mul(msk, Q, curve);
	printPoint(check, curve);

	//return 0;
	std::cout << "\r\n      Create signature" << endl;
	
	BigNumber M(200);
	BigNumber r2 = getRandom(q);
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
	cout << "Verified: " << rres << endl;
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

	cout << "\n secret fq: " << endl;
	E_Fq.show(secret_fq);
	BigNumber hash("e3ef5e0cb7f89dfc1744003d9927bf588936e5a348641cf3984643a96014e22a"); 
	cout << "\n hash: " << hash.toDecString() << endl;
	
	ecPoint H_fq = hashToPoint(hash);
	cout << "\n H_fq: " << endl;
	E_Fq.show(H_fq);
	ecPoint rH, S2_fq;
	
	E_Fq.scalarMultiply(rH, H_fq, (Integer)(r2.toDecString()), -1);//R=6*P, order of P is not required

	E_Fq.add(S2_fq,rH, secret_fq);//R=P+Q
	cout << "\n S2_fq: " << endl;
	E_Fq.show(S2_fq);

	ecPoint S1_fq;
	E_Fq.scalarMultiply(S1_fq, G0_fq, (Integer)(r2.toDecString()), -1);//R=6*P, order of P is not required
	cout << "\n S1_fq: " << endl;
	E_Fq.show(S1_fq);

	ecPoint Q_Fq = mapToFq(Q, curve, E_Fq);
	ecPoint QQ_Fq;
	E_Fq.scalarMultiply(QQ_Fq, G0_fq, (Integer)(r.toDecString()), -1);//R=6*P, order of P is not required

	BigNumber shash = getRandom(q);
	ecPoint S_fq = hashToPointFq(secret_fq, shash, E_Fq);
	cout << "\n S_fq: " << endl;
	E_Fq.show(S_fq);

	cout << "\n Q_Fq: " << endl;
	E_Fq.show(Q_Fq);

	bool res = verifyTate(S1_fq, S2_fq, hash, MPK_fq, Q_Fq, G0_fq, E_Fq);
	cout << "\n res: " << res << endl;
	
	ExtensionField::Element rr, bb, cc;
	rr = tatePairing(S2_fq, G0_fq, S_fq, E_Fq);
	bb = tatePairing(Q_Fq, MPK_fq, S_fq, E_Fq);
	cc = tatePairing(H_fq, S1_fq, S_fq, E_Fq);
		
	string strEta = "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001000000000000000010100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000100000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000010";
	BigNumber bnEta("80000000000000000000000000000000000200014000000000000000000000000000000000010000800000020000000000000000000000000000000000080002");
	
	//eta = (p^k - 1)/q
	//tate pairing return value should be in `eta` degree
	//num = eval_miller(P, Q+S)/eval_miller(P,  S)
	//return (num^eta)
	ExtensionField::Element r1, b1, c1, b1c1;
	E_Fq.field->pow(r1, rr, strEta);
	E_Fq.field->pow(b1, bb, strEta);
	E_Fq.field->pow(c1, cc, strEta);

	ExtensionField::Element bbcc;
	E_Fq.field->mul(b1c1, b1, c1);

	cout << "\n r1: " << endl;
	E_Fq.field->writeElement(r1);
	cout << "\n b1: " << endl;
	E_Fq.field->writeElement(b1);
	cout << "\n c1: " << endl;
	E_Fq.field->writeElement(c1);
	cout << "\n b1c1: " << endl;
	E_Fq.field->writeElement(b1c1);
	cout << "\n Verified: " << E_Fq.field->areEqual(r1, b1c1) << endl;
	cout << "\r\nruntime is: " << clock() / 1000.0 << endl; // время работы программы  


	EC_POINT_free(s1);	   s1 = nullptr;
	EC_POINT_free(secret); secret = nullptr;
	EC_POINT_free(check);  check = nullptr;
	for (size_t i = 0; i < proj.size(); ++i) {
		delete proj[i]; proj[i] = nullptr;
	}
	EC_POINT_free(Q);
	delete ec;			ec = nullptr;
	EC_POINT_free(G);	G = nullptr;
	delete curve;	    curve = nullptr;
	BN_CTX_free(ctx);   ctx   = nullptr;
	return 0;
}