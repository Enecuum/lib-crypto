var addon = require('./addon');


	let pArr = [0x23,0x70,0xfb,0x04,0x9d,0x41,0x0f,0xbe,0x4e,0x76,0x1a,0x98,0x86,0xe5,0x02,0x41,0x7d,0x02,0x3f,0x40,0x18,0x00,0x00,0x01,0x7e,0x80,0x60,0x00,0x00,0x00,0x00,0x01]
	let a = addon.BigNumber(0);
	let b = addon.BigNumber(5);
	let p = addon.BigNumber("23 70 fb 04 9d 41 0f be 4e 76 1a 98 86 e5 02 41 7d 02 3f 40 18 00 00 01 7e 80 60 00 00 00 00 01");
			//console.log(b.decString())
		console.log(p.decString())
	let order = addon.BigNumber("23 70 fb 04 9d 41 0f be 4e 76 1a 98 86 e5 02 41 1d c1 af 70 12 00 00 01 7e 80 60 00 00 00 00 01");

	let gx = addon.BigNumber("1e 98 4a c1 56 de 86 90 d0 e1 ee 61 f4 1a 8c d2 b0 d9 9e 9d 50 94 84 b2 00 dd 6d 14 1d 68 56 95");
	let gy = addon.BigNumber("01 57 56 79 2c c8 16 71 48 1b b4 83 0f 0d 3f d7 20 ba 97 b3 10 60 c6 ec 91 1a 9a eb 1c 27 52 29");
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