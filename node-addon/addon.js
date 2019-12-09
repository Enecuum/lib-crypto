let addon = require('./src/build/Release/addon.node');
var crypto = require('crypto');

module.exports = addon;
module.exports.createPK = createPK;
module.exports.getHash = getHash;
module.exports.toPoint = toPoint;

function toPoint(hash, G, curve){
	//let slice = hash.slice(0, 5);
	let r = addon.BigNumber(hash);
	//console.log(r.decString());
	let H = addon.mul(r, G, curve);
	//console.log("H: " + H.xy(curve));
	return H;
}

function createPK(pkey, G, curve){
	//let slice = pkey.slice(0, 5);
	let r = addon.BigNumber(pkey);
	//console.log(r.decString());
	let Q = addon.mul(r, G, curve);
	//console.log("Q: " + Q.xy(curve));
	return Q;
} 

function getHash(str){
	 return crypto.createHash('sha256').update(str).digest('hex');
} 

module.exports.keySharing = function (coalition, ids, Q, msk, curve){
 	var q = addon.BigNumber(13);
 	var shares = addon.shamir(msk, ids, 3, 2, q);
 	// r = hash
	var proj = addon.keyProj(coalition, shares, Q, curve);
	return proj;
}

module.exports.sign = function (M, leadID, G, G0, secret, curve){
	var H = toPoint(getHash(M.toString() + leadID.toString()), G, curve);
	let isInfinity = 0;
	var q = addon.BigNumber(13);
	do {
		var r2 = addon.getRandom(q);
		var s1 = addon.mul(r2, G0, curve);
		// S2 = r*H + SecKey
		var s2 = addon.mul(r2, H, curve);
		s2 = addon.addPoints(s2, secret, curve);
		isInfinity = s2.isInfinity(curve)
	} while(isInfinity)
	return {
		r : {
			x : s1.x(curve),
			y : s1.y(curve)
		},
		s : {
			x : s2.x(curve),
			y : s2.y(curve)
		}
	};
}

module.exports.sign_tate = function (M, leadID, G, G0, secret, curve, ecurve){
	console.log("hash = " + getHash(M.toString() + leadID.toString()));
	var h = addon.BigNumber(getHash(M.toString() + leadID.toString()));

	let res = addon.signTate(h, secret, G, G0, curve, ecurve);
	return res;
}

module.exports.verify = function (sign, M, Q, G, G0, MPK, leadID, p, curve){
	var sx = addon.BigNumber(0);
	var sy = addon.BigNumber(522);
	var S = addon.Point(sx, sy, curve)

	var H = toPoint(getHash(M.toString() + leadID.toString()), G, curve);

	var s1 = addon.Point(addon.BigNumber(sign.r.x), addon.BigNumber(sign.r.y), curve)
	var s2 = addon.Point(addon.BigNumber(sign.s.x), addon.BigNumber(sign.s.y), curve)

	var r1 = addon.weilPairing(G0, s2, S, curve);
	//console.log("r1 = e(P, S):\t" + r1.value());

	var b1 = addon.weilPairing(MPK, Q, S, curve);
	//console.log("b1 = e(MPK, Q):\t" + b1.value());

	var c1 = addon.weilPairing(s1, H, S, curve);
	//console.log("c1 = e(R, H1):\t" + c1.value());
	
	var b1c1 = addon.mmul(b1, c1, p);
	//console.log("r1 = b1 * c1:\t" + b1c1.value());
	if(r1.value() == b1c1.value())
		return 1;
	else 
		return 0;
}

module.exports.verify_tate = function (sign, M, PK_LPoS, G, G0, MPK, leadID, p, curve, ecurve ){
	var h = addon.BigNumber(getHash(M.toString() + leadID.toString()));

	let Q = addon.toPoint(PK_LPoS, G, curve);
	console.log("Qa: " + Q.xy(curve));
	let res = addon.verifyTate(sign, h, Q, G0, MPK, curve, ecurve);
	return res;
}