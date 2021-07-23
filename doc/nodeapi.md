## NodeJS Addon API Documentation

- **[addon](#addon)**
- **[BigNumber](#bn)**
  - **[value](#value)**
- **[Curve](#curve)**
- **[Point](#point)**
  - **[xy](#xy)**
  - **[x](#x)**
  - **[y](#y)**
  - **[isInfinity](#isInfinity)**
- **[mul](#mul)**
- **[createMPK](#creatempk)**
- **[getRandom](#getrandom)**
- **[shamir](#shamir)**
- **[keyProj](#keyproj)**
- **[keyRecovery](#keyrecovery)**
- **[weilPairing](#weilpairing)**

In progress

Node.JS addons is a way to use C++ libraries from Javascript code. It can be done by using Node.JS V8-related libraries such as `node-api` or `napi`. More info about NodeJS addons here.
In this project node-addon-api is used. This npm module contains C++ library called N-API that provides functions to write your own code and compile it into .node modules.
The main idea is to wrap all `libecc` functions calls into N-API objects and build .node module by using `node-gyp` tool.
`addon.cc` is a "glue" code that calls library functions and converts JS and C++ objects both ways.
For each library object such as `BigNumber` or `Curve` there is a N-API wrapping class with constructor that takes `Napi::Ecternal<T>` as input parameter to simplify object creation.

<a name="addon"></a>
#### addon
```js
let addon = require('./build/Release/addon');
```
Basic N-API JS-object represents compiled `addon.cc` code.
<a name="bn"></a>
#### BigNumber
```js
addon.BigNumber(6)
addon.BigNumber([ 0x4, 0x86 ])
````
`BigNumber` is a method that creates an instance of `NodeBN` class which is a wrap of `BigNumber` C++ class.
<a name="value"></a>
#### value
```js
let num = addon.BigNumber(6)
num.value()
> 6
````
Returns a decimal number of `BigNumber`
<a name="curve"></a>
#### NCurve
```js
let curve = addon.Curve(BigNumber a, BigNumber b, BigNumber p, BigNumber order, BigNumber gx, BigNumber gy)
````
`Curve` is a method that creates an instance of `NCurve` class which is a wrap of `Curve` C++ class.
<a name="point"></a>
#### Point
```js
addon.Point(BigNumber x, BigNumber y, Curve curve)
````
Creates a point on a elliptic curve
<a name="xy"></a>
#### xy
```js
let pt = addon.Point(1, 1, curve)
pt.xy(curve)
> (1 : 1)
````
Returns a string with point's coordinates.
<a name="x"></a>
#### x
```js
let pt = addon.Point(1, 2, curve)
pt.x(curve)
> 1
````
Returns a X coordinate in decimal format.
<a name="y"></a>
#### y
```js
let pt = addon.Point(1, 2, curve)
pt.y(curve)
> 2
````
Returns a Y coordinate in decimal format.
<a name="isInfinity"></a>
#### isInfinity
```js
let pt = addon.Point(0, 0, curve)
pt.isInfinity(curve)
> 1
````
Check if point is at infinity or not. Returns 1 at infinity, 0 otherwise
<a name="mul"></a>
#### mul
Wrap of [mul](../doc/cppapi.md#mul)
<a name="getrandom"></a>
#### getRandom
Wrap of [getRandom](../doc/cppapi.md#getrandom)
<a name="creatempk"></a>
#### createMPK
Wrap of [createMPK](../doc/cppapi.md#creatempk)
<a name="shamir"></a>
#### shamir
Wrap of [shamir](../doc/cppapi.md#shamir)
<a name="keyproj"></a>
#### keyProj
Wrap of [keyProj](../doc/cppapi.md#keyproj)
<a name="keyrecovery"></a>
#### keyRecovery
Wrap of [keyRecovery](../doc/cppapi.md#keyrecovery)
<a name="weilpairing"></a>
#### weilPairing
Wrap of [weilPairing](../doc/cppapi.md#weilpairing)

This functions are in progress 
#### mmul
```js
addon.mmul(a, b, p);
````
Modular multiplication. `a * b mod p`. Wrap of `*` operator of `BigNumber`
#### addPoints
```js
addon.addPoints(Point a, Point b, Curve curve)
````
Adds two points of elliptic curve
#### hashToPoint

#### createPK
```js
addon.createPK(pkey, G, curve)
````
Uses to generate PK_LPoS. Returns `pkey * G`
#### keySharing
```js
addon.keySharing(coalition, Q, msk, curve)
````
Returns an array of key shares. Result od `Shamir` and `KeyProj` work. This is just an wxample, because this functions are running separately in different nodes.
#### sign
```js
addon.sign(M, leadID, G, G0, secret, curve)
```
Sing a `M` message. Returns an object of sign as two points coordinates
```js
  s1 : R = random * G0,
  s2 : S = random * H + SK_LPoS
Returned object:
{
	"r":{
		"x": decimal,
		"y": decimal
	},
	"s":{
		"x": decimal,
		"y": decimal
	}
}
```
#### verify
```js
addon.verify(sign, M, Q, G, G0, MPK, leadID, p, curve)
```
Verify signature `sign` of message `M`. Runs on every node with public paramaters. Should be simplified later.
`sign` is an object such as:
```js
{
	"r":{
		"x": decimal,
		"y": decimal
	},
	"s":{
		"x": decimal,
		"y": decimal
	}
}
```