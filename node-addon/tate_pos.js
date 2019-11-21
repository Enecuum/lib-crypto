var addon = require('./addon');


	let a = addon.BigNumber(0);
	let b = addon.BigNumber(5);
	let p = addon.BigNumber("2370fb049d410fbe4e761a9886e502417d023f40180000017e80600000000001");
			//console.log(b.decString())
		console.log(p.decString())
	let order = addon.BigNumber("2370fb049d410fbe4e761a9886e502411dc1af70120000017e80600000000001");

	let gx = addon.BigNumber("1e984ac156de8690d0e1ee61f41a8cd2b0d99e9d509484b200dd6d141d685695");
	let gy = addon.BigNumber("015756792cc81671481bb4830f0d3fd720ba97b31060c6ec911a9aeb1c275229");
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
	//PK_LPoS = "7c d9 25 af af fb 84 66 02 92 13 a0 5a e0 fa af f9 c5 33 df b3 ae 44 6d bf cb 97 1e 45 e2 ca cf";
	console.log("Creating curve");
 	let curve = addon.Curve(a, b, p, order, g0x, g0y);
 	console.log(curve)

	let strIrred = "12 1 7 16030569034403128277756688287498649515636838101184337499778392980116222246896 16030569034403128277756688287498649515636838101184337499778392980116222246710 16030569034403128277756688287498649515636838101184337499778392980116222246885 2309 2992 16030569034403128277756688287498649515636838101184337499778392980116222237244 16030569034403128277756688287498649515636838101184337499778392980116222225365 3429 48555 63122 37991";
	let strA = "0 0";
	let strB = "0 5";
	let e_fq = addon.Curve_Fq(p.decString(), 12, strIrred, strA, strB);
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
	console.log("Verification...");
	//console.log("Block verified: " + addon.verify(req.data.leader_sign, req.data.m_hash, Qa, G, G0, MPK, req.data.leader_id, p, curve));
	console.log("Block verified: " + addon.verify_tate(req.data.leader_sign, req.data.m_hash, Qa, G, G0, MPK, req.data.leader_id, p, curve, e_fq));
