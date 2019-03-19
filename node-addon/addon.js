//let addon = require('./node_modules/win64/addon');
let addon = require('./node_modules/win64/addon');
var crypto = require('crypto');

module.exports = addon;
//module.exports.hashToPoint = hashToPoint; 
module.exports.createPK = createPK;


function hash(hash, G, curve){
	//console.log("hash: " + hash); // 30
	//let num = hash.slice(0, 1);
	// var buffer = new ArrayBuffer(num.length);
	// console.log(num); // 30
	// let r = Buffer.from(num);
	// console.log(r); // 30
	// var hx = addon.BigNumber(681);
	// var hy = addon.BigNumber(256);
	// var H = addon.Point(hx, hy, curve);
	//console.log("r: " + h.value())
	//var H = addon.hashToPoint(h, curve);
	var r = addon.BigNumber(4);
	
	var H = addon.mul(r, G, curve);
	console.log("H1: " + H.xy(curve))
	return H;
}

function createPK(pkey, G, curve){
	//var r = addon.BigNumber(9);//addon.getRandom(obj1);
	var r = addon.BigNumber(7);
	
	var Q = addon.mul(r, G, curve);
	console.log("Q: " + Q.xy(curve)); // 30
	return Q; //hash(pkey, curve);
} 

module.exports.getHash = function (str){
	 return crypto.createHash('sha1').update(str).digest('hex');
} 

module.exports.verify = function (sign, G, G0, MPK, kblock, leadID, p, curve){
	var sx = addon.BigNumber(0);
	var sy = addon.BigNumber(522);
	var S = addon.Point(sx, sy, curve)

	var H = hash(kblock.m.toString() + leadID.toString(), G, curve);
	let Q = createPK(kblock.id.toString() + leadID.toString(), G, curve);

	var r1 = addon.weilPairing(G0, sign.s2, S, curve);
	console.log("r1 = e(P, S):\t" + r1.value());

	var b1 = addon.weilPairing(MPK, Q, S, curve);
	console.log("b1 = e(MPK, Q):\t" + b1.value());

	var c1 = addon.weilPairing(sign.s1, H, S, curve);
	console.log("c1 = e(R, H1):\t" + c1.value());
	
	var b1c1 = addon.mmul(b1, c1, p);
	console.log("r1 = b1 * c1:\t" + b1c1.value());
	if(r1.value() == b1c1.value())
		return 1;
	else 
		return 0;
}

module.exports.keySharing = function (coalition, Q, msk, curve){
 	var q = addon.BigNumber(13);
 	var shares = addon.shamir(msk, 10, 6, q);
 	// r = hash

	var proj = addon.keyProj(coalition, shares, Q, curve);
	return proj;
}

module.exports.sign = function (kblock, leadID, G, G0, secret, curve){
	var r2 = addon.BigNumber(7);//addon.getRandom(q);
	//console.log("r2: " + r2.value());

	var s1 = addon.mul(r2, G0, curve);

	var H = hash(kblock.m.toString() + leadID.toString(), G, curve);

	// S2 = r*H + SecKey
	// S = sQ + rH
	var s2 = addon.mul(r2, H, curve);
	s2 = addon.addPoints(s2, secret, curve);
	return {
		s1 : s1,
		s2 : s2
	};
}