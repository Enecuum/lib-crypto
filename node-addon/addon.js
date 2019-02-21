var addon = require('bindings')('addon');

var obj1 = addon.CreateObject(10);
var obj2 = addon.CreateObject(20);
var result = addon.add(obj1, obj2);

console.log(result); // 30

var r = addon.getRandom(obj1);

console.log(r);