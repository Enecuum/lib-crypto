var addon = require('./addon');


var number = [ 0x4, 0x86 ];
	var test = addon.BigNumber(number);

console.log(test.value());
	var a = addon.BigNumber(25);
	var b = addon.BigNumber(978);
	var p = addon.BigNumber(1223);
	var order = addon.BigNumber(1183);
	var g0x = addon.BigNumber(972);
	var g0y = addon.BigNumber(795);
	var gx = addon.BigNumber(1158);
	var gy = addon.BigNumber(92);
	var q = addon.BigNumber(13);

	let data = 'f7bc6eb9f0425cee9c4278cfb179a824490a82e82d4b67768b41dc484f09061f045e2a61d287759f85fd641b1c3489e56093a677cbfe344efc33a616477b7d1231e1b385f2151fb53d74bb81fbdf4ab2b9c4f22e34ea4b666c0e9e36c4beb421e5cf77c31bde1eac317230749ef0144781749083bbc1da351db2d6647f15c412233d74746c1c78f04ca63017f36df69782aea85207ca387e96d1741ce675200266f23c1fd9e20bec430c531c94ead1edfbc8e0c7cc734907586568b6cf2a0b71c68b84a65b48fc1dc05e195db7f2a70270fee3508e5f68003e60e6a7670ab5ae7ef76d6417c3742fe8262ae163bd79ad3fff3b8769914020ee05b3e467368cb7';
	let blockID = addon.getHash(data);
	
	let kblock = {
		m : data,
		id : blockID
	}

	let LPoSID = 677321
	var PK_LPoS = addon.getHash(kblock.id.toString() + LPoSID.toString());
	console.log("PK_LPoS: " + PK_LPoS);
console.log(a.value());
console.log("Creating 1111 curve");
 	var curve = new addon.NCurve(a, b, p, order, g0x, g0y);

	var G0 = addon.Point(g0x, g0y, curve);
	var G = addon.Point(gx, gy, curve);
	var msk = addon.BigNumber(10);
	console.log("G0: " + G0.xy(curve));
	console.log("G: " + G.xy(curve));

	console.log("Creating MPK");
	var MPK = addon.createMPK(msk, G0, curve);
	console.log("MPK: " + MPK.xy(curve));

	let Q = addon.createPK(PK_LPoS, G, curve);
	console.log("Q: " + Q.xy(curve));

	console.log("PKG keys generation");	
	
	let ids = [1, 55, 10]
	var coalition = [ 1, 2 ];
	var participants = [ 1, 55, 10 ];

	var proj = addon.keySharing(coalition, ids, Q, msk, curve);
	console.log("Key shadows");
	for (var i = 0; i < proj.length; i++){
		console.log(proj[i].xy(curve));
	}

	var secret = addon.keyRecovery(proj, participants, q, curve);

	console.log("Recovered secret SK:\t" + secret.xy(curve));

	var check = addon.mul(msk, Q, curve);
	console.log("Check secret MSK * Q:\t" + check.xy(curve));

	console.log("Create signature");

	let sign = addon.sign(data, LPoSID, G, G0, secret, curve);

    console.log("S1: " + sign.r.p.xy(curve));
    console.log("S2: " + sign.s.p.xy(curve));

	console.log("-------- Verification");
	console.log("Weil pairing");

	console.log("Block verified: " + addon.verify(sign, data, Q, G, G0, MPK, LPoSID, p, curve));