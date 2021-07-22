var addon = require('./addon');

	console.log('Load crypto addon');
	let cryptoAddon = addon.getCryptoAddon(1);

	console.log('Creating big numbers');
	let a = cryptoAddon.newBigNumber(1);
	let b = cryptoAddon.newBigNumber(0);
cryptoAddon.eraseBigNumber([a, b]) //example how to secure erase bigNumber

    a = cryptoAddon.newBigNumber(1);
    b = cryptoAddon.newBigNumber(0);
	let p = cryptoAddon.newBigNumber("80000000000000000000000000000000000200014000000000000000000000000000000000010000800000020000000000000000000000000000000000080003");
	let order = cryptoAddon.newBigNumber("80000000000000000000000000000000000200014000000000000000000000000000000000010000800000020000000000000000000000000000000000080004");
	let gx = cryptoAddon.newBigNumber("2920f2e5b594160385863841d901a3c0a73ba4dca53a8df03dc61d31eb3afcb8c87feeaa3f8ff08f1cca6b5fec5d3f2a4976862cf3c83ebcc4b78ebe87b44177");
	let gy = cryptoAddon.newBigNumber("2c022abadb261d2e79cb693f59cdeeeb8a727086303285e5e629915e665f7aebcbf20b7632c824b56ed197f5642244f3721c41c9d2e2e4aca93e892538cd198a");
	let g0x = gx;
	let g0y = gy;
	let q = order;
	let q1 = cryptoAddon.newBigNumber("287a1a55f1c28b1c23a27eef69b6a537e5dfd068d43a34951ed645e049d6be0ac805e3c45501be831afe2d40a2395d8c72edb186c6d140bb85ae022a074b");

	let k_hash = '000063052130512cd908edf25d2abccb8dc0d40af1bee23a5d980afb3b9b012d';
	let LPoSID = 14532;
	let PK_LPoS = addon.getHash(k_hash.toString() + LPoSID.toString() + (5).toString());

	console.log("Creating curve");
	let curve = cryptoAddon.newCurve(a, b, p, order, g0x, g0y);

	let strIrred = "2 1 1 6703903964971298549787012499102923063739684112761466562144343758833001675653841939454385015500446199477853424663597373826728056308768000892499915006541826";
	let strA = "0 1";
	let strB = "0 0";
	console.log("Creating point on curve");
	var G = cryptoAddon.newPoint(gx, gy, curve);
	cryptoAddon.setPointToCurve(G, curve);

	let e_fq = cryptoAddon.newCurveFq(cryptoAddon.bigNumberToDec(p), 2, strIrred, strA, strB);

	var msk = cryptoAddon.newBigNumber(10000000);

	let ids = [];
	for (let i = 0; i < 100; i++)
		ids[i] = cryptoAddon.newBigNumber(i + 1);
	let shares = cryptoAddon.shamir(msk, ids, 100, 3, q);
	console.log('Do Shamir')
	console.log('shares = ', shares);

	let posId_1 = 1;
	let posId_2 = 55;
	let posId_3 = 10;
	PK_LPoS = cryptoAddon.newBigNumber(PK_LPoS);

	//
	// ------------------ PoS1 part
	//
	let keyPart1 = shares[0];	// Recieved from PKG
	let Q1 = cryptoAddon.getQ(PK_LPoS, curve, e_fq);
	let ss1 = cryptoAddon.mul(keyPart1, Q1, curve);
	// Send ss1 to LPoS...
	//
	// ------------------ PoS2 part
	//

	let keyPart2 = shares[54];	// Recieved from PKG
	let Q2 = cryptoAddon.getQ(PK_LPoS, curve, e_fq);
	let ss2 = cryptoAddon.mul(keyPart2, Q2, curve);
	// Send ss2 to LPoS...
	//
	// ------------------ PoS3 part
	//

	let keyPart3 = shares[9];	// Recieved from PKG	
	let Q3 = cryptoAddon.getQ(PK_LPoS, curve, e_fq);
	let ss3 = cryptoAddon.mul(keyPart3, Q3, curve);
	// Send ss3 to LPoS...

	//
	// ------------------ LPoS (someone from PoS_1 - 3, lets be PoS_3 )
	//
	let coalition = [cryptoAddon.newBigNumber(posId_1), cryptoAddon.newBigNumber(posId_2), cryptoAddon.newBigNumber(posId_3)];
	let proj = [ss1, ss2, ss3]; // Collected from other PoSes	
	let secret = cryptoAddon.keyRecovery(proj, coalition, q1, curve);

	console.log("Recovered secret SK:\t" + cryptoAddon.xy(secret, curve));
	console.log('proj = ', proj);


	let Q = cryptoAddon.getQ(PK_LPoS, curve, e_fq);

	// Просто проверка, не выполняется на ПоСе
	var check = cryptoAddon.mul(msk, Q, curve);
	console.log("Check secret MSK * Q:\t" + cryptoAddon.xy(check, curve));

	console.log("Create signature");
	//let M = data;
	let req = {
		"ver": 1,
		"method": "on_leader_beacon",
		"data": {
			"leader_id": LPoSID,
			"m_hash": "e3ef5e0cb7f89dfc1744003d9927bf588936e5a348641cf3984643a96014e22a",
			"mblock_data": {
				"k_hash": "000063052130512cd908edf25d2abccb8dc0d40af1bee23a5d980afb3b9b012d",
				"nonce": 5
			}
		}
	}
	let G0_fq = {
		"x": "1 1971424652593645857677685913504949042673180456464917721388355467732670356866868453718540344482523620218083146279366045128738893020712321933640175997249379 4296897641464992034676854814757495000621938623767876348735377415270791885507945430568382535788680955541452197460367952645174915991662132695572019313583345",
		"y": "1 5439973223440119070103328012315186243431766339870489830477397472399815594412903491893756952248783128391927052429939035290789135974932506387114453095089572 3254491657578196534138971223937186183707778225921454196686815561535427648524577315556854258504535233566592842007776061702323300678216177012235337721726634"
	}
	let MPK_fq = {
		"x": "1 5553161562309620134204294307547179611636685754291535214076054128059515506033993893714735433610207337513297007943745903960736280984281319771387455094363055 6689291821874770449783470798967833024384908153897387642241791007779898182075451742944983283446597677757107561868230869942692013197487003982057559827210919",
		"y": "1 1409647240359222769554276508179480635408566023738878356661595724530348198250043359650835609016647773316137383233659454103710745566316894573620890364518372 3885560331261099024236364110126283369754589356789796979215968181927252095476961502262491073346156231135118163091525459453361435352300956275516792143616474"
	}

	console.log('sign_tate')
	req.data.leader_sign = addon.sign_tate(req.data.m_hash, LPoSID, G0_fq, secret, curve, e_fq);
	console.log('req.data.leader_sign = ', req.data.leader_sign);


	//
	// ------------------ PoA
	//
	PK_LPoS = addon.getHash(req.data.mblock_data.k_hash.toString() + LPoSID.toString() + req.data.mblock_data.nonce.toString());
	console.log("PK_LPoS: " + PK_LPoS)


	console.log("Verification...");
	console.log("Block verified: " + addon.verify_tate(req.data.leader_sign, req.data.m_hash, PK_LPoS, G0_fq, MPK_fq, req.data.leader_id, curve, e_fq));
	console.log("==========================================================================");
