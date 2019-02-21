var addon = require('bindings')('addon');

var obj1 = addon.BigNumber(10);
var obj2 = addon.BigNumber(20);
var result = addon.add(obj1, obj2);

console.log(result); // 30

var r = addon.getRandom(obj1);

var pt = addon.Point(10);

	var a = addon.BigNumber(25);
	var b = addon.BigNumber(978);
	var p = addon.BigNumber(1223);
	var order = addon.BigNumber(1183);
	var g0x = addon.BigNumber(972);
	var g0y = addon.BigNumber(795);


var curve = new addon.NCurve(a, b, p, order, g0x, g0y);

console.log(r);