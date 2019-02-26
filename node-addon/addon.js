var addon = require('bindings')('addon');



var obj1 = addon.BigNumber(10);
var obj2 = addon.BigNumber(20);
var result = addon.add(obj1, obj2);

console.log(result.value()); // 30

	var a = addon.BigNumber(25);
	var b = addon.BigNumber(978);
	var p = addon.BigNumber(1223);
	var order = addon.BigNumber(1183);
	var g0x = addon.BigNumber(972);
	var g0y = addon.BigNumber(795);
	var gx = addon.BigNumber(1158);
	var gy = addon.BigNumber(92);

console.log("Creating curve");
 	var curve = new addon.NCurve(a, b, p, order, g0x, g0y);

	var G0 = addon.Point(g0x, g0y, curve);
	var G = addon.Point(gx, gy, curve);
	var msk = addon.BigNumber(10);
	console.log("G: " + G.xy(curve));
	console.log("G0: " + G0.xy(curve));
console.log("Creating MPK");

	var MPK = addon.createMPK(msk, G0, curve);
	console.log("MPK: " + MPK.xy(curve));


console.log("PKG keys generation");	

	var r = addon.BigNumber(9);//addon.getRandom(obj1);
	//console.log(r.value()); // 30
	var Q = addon.mul(r, G, curve);
	console.log("Q = r * G: " + Q.xy(curve));

 	var q = addon.BigNumber(13);
 	var shares = addon.shamir(msk, 10, 6, q);

	for (var i = 0; i < shares.length; i++){
		console.log(shares[i].value());
	}

	var coalition = [ 1,3,5,7,9,10 ];
console.log("Shadows: ");

	var proj = addon.keyProj(coalition, shares, Q, curve);
	for (var i = 0; i < proj.length; i++){
		console.log(proj[i].xy(curve));
	}

console.log("Key recovery");	
	var secret = addon.keyRecovery(proj, coalition, q, curve);

console.log("Recovered secret SK: " + secret.xy(curve));

	var check = addon.mul(msk, Q, curve);
console.log("Check secret MSK * Q: " + check.xy(curve));

console.log("Create signature");

	var M = addon.BigNumber(200);
	var r2 = addon.BigNumber(7);//addon.getRandom(q);
	console.log("r2: " + r2.value());

	var s1 = addon.mul(r2, G0, curve);

console.log("S1: " + s1.xy(curve));

	var hx = addon.BigNumber(681);
	var hy = addon.BigNumber(256);
	var H = addon.Point(hx, hy, curve);

	// S2 = r*H + SecKey
	// S = sQ + rH
	var s2 = addon.mul(r2, H, curve);
	s2 = addon.addPoints(s2, secret, curve);
	//if (1 != EC_POINT_add(curve1, s2, s2, secret, NULL)) handleErrors();

console.log("S2: " + s2.xy(curve));

console.log("-------- Verification");
console.log("Weil pairing");

	var sx = addon.BigNumber(0);
	var sy = addon.BigNumber(522);
	var S = addon.Point(sx, sy, curve)

	var r1 = addon.weilPairing(G0, s2, S, curve);
	console.log("r1 = e(P, S): " + r1.value());

	var b1 = addon.weilPairing(MPK, Q, S, curve);
	console.log("b1 = e(MPK, Q): " + b1.value());

	var c1 = addon.weilPairing(s1, H, S, curve);
	console.log("c1 = e(R, H1): " + c1.value());
	
	var b1c1 = addon.mmul(b1, c1, p);
	console.log("r1 = b1 * c1: " + b1c1.value());