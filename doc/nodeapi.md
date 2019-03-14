## NodeJS Addon API Documentation

- **[BigNumber](#bn)**
- **[Curve](#curve)**
- **[mul](#mul)**
- **[createMPK](#creatempk)**
- **[getRandom](#getrandom)**
- **[shamir](#shamir)**
- **[keyProj](#keyproj)**
- **[keyRecovery](#keyrecovery)**
- **[weilPairing](#weilpairing)**
- **[Operators](#operators)**

In progress

Node.JS addons is a way to use C++ libraries from Javascript code. It can be done by using Node.JS V8-related libraries such as `node-api` or `napi`. More info about NodeJS addons here.
In this project node-addon-api is used. This npm module contains C++ library called N-API that provides functions to write your own code and compile it into .node modules.
The main idea is to wrap all `libecc` functions calls into N-API objects and build .node module by using `node-gyp` tool.
`addon.cc` is a "glue" code that calls library functions and converts JS and C++ objects both ways.
For each library object such as `BigNumber` or `Curve` there is a N-API wrapping class with constructor that takes `Napi::Ecternal<T>` as input parameter to simplify object creation.

addon
BigNumber
value
NCurve
Point
xy
keySharing
keyRecovery
mul
mmul
getRandom
hashToPoint
createPK
createMPK
keySharing
sign
weilPairing