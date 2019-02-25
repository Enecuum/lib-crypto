var addon = require('bindings')('addon');

var obj1 = addon.BigNumber(10);
var obj2 = addon.BigNumber(20);
var result = addon.add(obj1, obj2);

console.log(result.value()); // 30

var r = addon.BigNumber(1);//addon.getRandom(obj1);
console.log(r.value()); // 30


	var a = addon.BigNumber(25);
	var b = addon.BigNumber(978);
	var p = addon.BigNumber(1223);
	var order = addon.BigNumber(1183);
	var g0x = addon.BigNumber(972);
	var g0y = addon.BigNumber(795);
	var gx = addon.BigNumber(1158);
	var gy = addon.BigNumber(92);

 var curve = new addon.NCurve(a, b, p, order, g0x, g0y);

 var G0 = addon.Point(g0x, g0y, curve);
  var G = addon.Point(gx, gy, curve);
 //pt.SetCoords(g0x, g0y, curve);

// var G = pt;
var Q = addon.mult(r, G, curve);
// 	addon.mul(r, G);

// console.log(curve.value());

 console.log(G0.xy(curve));
 console.log(Q.xy(curve));