var addon = require('./addon');


	let a = addon.BigNumber(1);
	let b = addon.BigNumber(0);
	let p = addon.BigNumber("80000000000000000000000000000000000200014000000000000000000000000000000000010000800000020000000000000000000000000000000000080003");
			//console.log(b.decString())
		console.log(p.decString())
	let order = addon.BigNumber("80000000000000000000000000000000000200014000000000000000000000000000000000010000800000020000000000000000000000000000000000080004");

	let gx = addon.BigNumber("2920f2e5b594160385863841d901a3c0a73ba4dca53a8df03dc61d31eb3afcb8c87feeaa3f8ff08f1cca6b5fec5d3f2a4976862cf3c83ebcc4b78ebe87b44177");
	let gy = addon.BigNumber("2c022abadb261d2e79cb693f59cdeeeb8a727086303285e5e629915e665f7aebcbf20b7632c824b56ed197f5642244f3721c41c9d2e2e4aca93e892538cd198a");
	let g0x = gx;
	let g0y = gy;
	let q = order;//addon.BigNumber("2370fb049d410fbe4e761a9886e502411dc1af70120000017e80600000000001");

	let k_hash = '000063052130512cd908edf25d2abccb8dc0d40af1bee23a5d980afb3b9b012d';//addon.getHash(data);

	let LPoSID = 14532
	let PK_LPoS = addon.getHash(k_hash.toString() + LPoSID.toString() + (5).toString());
		console.log("kblockId: " + k_hash.toString())
		console.log("lposId: " + LPoSID.toString())
		console.log("kblockId + lposId + nonce: " + k_hash.toString() + LPoSID.toString() + (5).toString())
		console.log("PK_LPoS: " + PK_LPoS)

//
// ------------------ PKG part
//
	console.log("Creating curve");
 	let curve = addon.Curve(a, b, p, order, g0x, g0y);
 	console.log(curve)

	let strIrred = "2 1 1 6703903964971298549787012499102923063739684112761466562144343758833001675653841939454385015500446199477853424663597373826728056308768000892499915006541826";
	let strA = "0 1";
	let strB = "0 0";
	let e_fq = addon.Curve_Fq(p.decString(), 2, strIrred, strA, strB);
	console.log(e_fq);
	var G0 = addon.Point(g0x, g0y, curve);
	console.log("G0: " + G0.xy(curve));
	var G = addon.Point(gx, gy, curve);
	console.log("G: " + G.xy(curve));
	curve.G = G;
	//console.log(curve.G)
	var msk = addon.BigNumber(8);

 	console.log("Creating MPK");
	var MPK = addon.createMPK(msk, G0, curve);
	console.log("MPK: " + MPK.xy(curve));
	let ids = [1, 55, 10]
	let shares = addon.shamir(msk, ids, 3, 2, q);
	console.log("Key shares: ");
	for (var i = 0; i < shares.length; i++){
		console.log(shares[i].decString());
	}

	let posId_1 = 1;
	let posId_2 = 55;
	let posId_3 = 10;

//
// ------------------ PoS1 part
//
	let keyPart1 = shares[0];	// Recieved from PKG
	let Q1 = addon.createPK(PK_LPoS, G, curve);
	let ss1 = addon.mul(keyPart1, Q1, curve);
	// Send ss1 to LPoS...
//
// ------------------ PoS2 part
//

	let keyPart2 = shares[1];	// Recieved from PKG
	let Q2 = addon.createPK(PK_LPoS, G, curve);
	let ss2 = addon.mul(keyPart2, Q2, curve);
	// Send ss2 to LPoS...
//
// ------------------ PoS3 part
//

	let keyPart3 = shares[2];	// Recieved from PKG
	let Q3 = addon.createPK(PK_LPoS, G, curve);
	let ss3 = addon.mul(keyPart3, Q3, curve);
	// Send ss3 to LPoS...

//
// ------------------ LPoS (someone from PoS_1 - 3, lets be PoS_3 )
//
	let coalition = [ posId_1, posId_3 ];
	let proj = [ss1, ss3]; // Collected from other PoSes
	
	console.log("Key shadows");
	//for (var i = 0; i < proj.length; i++){
	//	console.log(proj[i].xy(curve));
	//}
	let secret = addon.keyRecovery(proj, coalition, q, curve);
	
	console.log("Recovered secret SK:\t" + secret.xy(curve));
	let Q = addon.createPK(PK_LPoS, G, curve);
	// Просто проверка, не выполняется на ПоСе
	var check = addon.mul(msk, Q, curve);
	console.log("Check secret MSK * Q:\t" + check.xy(curve));

	console.log("Create signature");
	//let M = data;
	let req = {
	"ver":1,
	"method":"on_leader_beacon",
	"data": {
		"leader_id": LPoSID,
		"m_hash": "e3ef5e0cb7f89dfc1744003d9927bf588936e5a348641cf3984643a96014e22a",
		"mblock_data" : {
				"k_hash": "000063052130512cd908edf25d2abccb8dc0d40af1bee23a5d980afb3b9b012d",
				"nonce" : 5
			}
		}
	}

	req.data.leader_sign = addon.sign_tate(req.data.m_hash, LPoSID, G, G0, secret, curve, e_fq);
	console.log(req.data.leader_sign);
	//req.data.leader_sign = {"r":{"x":1199,"y":966},"s":{"x":1039,"y":885}}


	//console.log("S1: " + sign.r.x + " " + sign.r.y);
	//console.log("S2: " + sign.s.x + " " + sign.s.y);

//
// ------------------ PoA
//
	PK_LPoS = addon.getHash(req.data.mblock_data.k_hash.toString() + LPoSID.toString() + req.data.mblock_data.nonce.toString());
	console.log(PK_LPoS)
	//let Qa = addon.createPK(PK_LPoS, G, curve);
	let Qa = addon.toPoint(PK_LPoS, G, curve);
	console.log("Qa: " + Qa.xy(curve));
	console.log("Verification...");
	//console.log("Block verified: " + addon.verify(req.data.leader_sign, req.data.m_hash, Qa, G, G0, MPK, req.data.leader_id, p, curve));
	console.log("Block verified: " + addon.verify_tate(req.data.leader_sign, req.data.m_hash, Qa, G, G0, MPK, req.data.leader_id, p, curve, e_fq));
