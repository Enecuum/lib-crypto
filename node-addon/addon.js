//let addon = require('./Build/Release/addon');
let addon = require('./node_modules/enecuum-crypto/addon');
var crypto = require('crypto');

module.exports = addon;
//module.exports.hashToPoint = hashToPoint; 
module.exports.createPK = createPK;
module.exports.getHash = getHash;

function hash(hash, G, curve){
	//console.log("hash: " + hash); // 30
	
	let slice = hash.slice(0, 1);
	//console.log(parseInt(slice, 16)); // 30
	let r = addon.BigNumber(parseInt(slice, 16));
	// console.log(num); // 30

	// console.log(r); // 30
	// var hx = addon.BigNumber(681);
	// var hy = addon.BigNumber(256);
	// var H = addon.Point(hx, hy, curve);
	//console.log("r: " + r.value())
	//var H = addon.hashToPoint(h, curve);
	//var r = addon.BigNumber(4);
	
	var H = addon.mul(r, G, curve);
	//console.log("\tH1: " + H.xy(curve))
	return H;
}

function createPK(pkey, G, curve){
	//var r = addon.BigNumber(9);//addon.getRandom(obj1);
	let slice = pkey.slice(0, 1);
	let r = addon.BigNumber(parseInt(slice, 16));

	//var r = addon.BigNumber(7);
	
	var Q = addon.mul(r, G, curve);
	//console.log("\tQ: " + Q.xy(curve)); // 30
	return Q; //hash(pkey, curve);
} 

function getHash(str){
	 return crypto.createHash('sha1').update(str).digest('hex');
} 

module.exports.verify = function (sign, M, Q, G, G0, MPK, leadID, p, curve){
	var sx = addon.BigNumber(0);
	var sy = addon.BigNumber(522);
	var S = addon.Point(sx, sy, curve)

	var H = hash(getHash(M.toString() + leadID.toString()), G, curve);

	var r1 = addon.weilPairing(G0, sign.s2, S, curve);
	//console.log("r1 = e(P, S):\t" + r1.value());

	var b1 = addon.weilPairing(MPK, Q, S, curve);
	//console.log("b1 = e(MPK, Q):\t" + b1.value());

	var c1 = addon.weilPairing(sign.s1, H, S, curve);
	//console.log("c1 = e(R, H1):\t" + c1.value());
	
	var b1c1 = addon.mmul(b1, c1, p);
	//console.log("r1 = b1 * c1:\t" + b1c1.value());
	if(r1.value() == b1c1.value())
		return 1;
	else 
		return 0;
}

module.exports.keySharing = function (coalition, ids, Q, msk, curve){
 	var q = addon.BigNumber(13);
 	var shares = addon.shamir(msk, ids, 3, 2, q);
 	// r = hash
	var proj = addon.keyProj(coalition, shares, Q, curve);
	return proj;
}

module.exports.sign = function (M, leadID, G, G0, secret, curve){
	var r2 = addon.BigNumber(7);//addon.getRandom(q);
	//console.log("r2: " + r2.value());

	var s1 = addon.mul(r2, G0, curve);

	var H = hash(getHash(M.toString() + leadID.toString()), G, curve);

	// S2 = r*H + SecKey
	// S = sQ + rH
	var s2 = addon.mul(r2, H, curve);
	s2 = addon.addPoints(s2, secret, curve);
	return {
		s1 : s1,
		s2 : s2
	};
}