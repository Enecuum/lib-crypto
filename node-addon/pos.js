var addon = require('./addon');


	let a = addon.BigNumber(25);
	let b = addon.BigNumber(978);
	let p = addon.BigNumber(1223);
	let order = addon.BigNumber(1183);
	let g0x = addon.BigNumber(972);
	let g0y = addon.BigNumber(795);
	let gx = addon.BigNumber(1158);
	let gy = addon.BigNumber(92);

	let q = addon.BigNumber(13);

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
	var G0 = addon.Point(g0x, g0y, curve);
	var G = addon.Point(gx, gy, curve);
	curve.G = G;
	console.log(curve.G)
	var msk = addon.BigNumber(8);

 	console.log("Creating MPK");
	var MPK = addon.createMPK(msk, G0, curve);
	console.log("MPK: " + MPK.xy(curve));
	let ids = [1, 55, 10]
	let shares = addon.shamir(msk, ids, 3, 2, q);
	console.log("Key shares: ");
	for (var i = 0; i < shares.length; i++){
		console.log(shares[i].value());
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

	req.data.leader_sign = addon.sign(req.data.m_hash, LPoSID, G, G0, secret, curve);
	
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
	console.log("Block verified: " + addon.verify(req.data.leader_sign, req.data.m_hash, Qa, G, G0, MPK, req.data.leader_id, p, curve));