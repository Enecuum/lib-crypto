
# lib-crypto

Enecuum C++ library for Elliptic Curve cryptography. Based on OpenSSL EC methods.

- **[C++ API documentation](doc/cppapi.md)**
- **[NodeJS addon documentation](doc/nodeapi.md)**

For now it contains realization of Algorithms 4-8 from [Enecuum Blockchain TechPaper](https://new.enecuum.com/files/tp_en.pdf), and signature verification by formula 3.5.

 - Big Numbers Arithmetic - operations with numbers of any size (based on OpenSSL BIGNUM structure and methods)
 - Creating custom Elliptic Curve (EC)
 - Operations with EC points
 - Shamir's key sharing scheme
 - Key recovery
 - Signature and Verification by Weil Pairing algorithm

OpenSSL Binaries required. 

### Usage (Linux)
Download OpenSSL for Linux:

`sudo apt-get install libssl-dev`

Build and run test example:
```
cd crypto
g++ *.cpp -lcrypto -o test
./test
```
Build .so library:
```
cd crypto
g++ -fPIC *.cpp -shared -o libecc.so
```
### Usage (Windows)
There is `.sln` solution for Visual Studo 2017. 
First, download [OpenSSL v1.1.1](https://slproweb.com/products/Win32OpenSSL.html).
In project Properties set correct paths:
```
[C/C++ -> General -> Additional Include Directories]` : OpenSSL’s include directory (e.g C:\openssl\include)
[Linker -> General -> Additional Library Directories]` : OpenSSL’s lib directory (e.g C:\openssl\lib)
[Linker -> Input -> Additional Dependencies]` : libcrypto.lib
```
To build test app simply use `main.cpp` as example.
In case of error **"No openssl_applink"** go to `[C/C++ -> Code generation -> Runtime library ]` and set value as `/MD`
To build DLL setup project configuration for dll.
### Node.js Binding

This addon allows you to call C++ methods of the library from NodeJS. Written with [node-addon-api](https://github.com/nodejs/node-addon-api). 
**Node.JS version 10+ required**

#### Simple usage
```
cd node-addon
npm install
node index.js
```
This will download a pre-built files from another repo as npm-packet depends of your platform. If you have any issues with this way, you should build from sources.
#### Build addon from sources:

**Note.** Make sure your NodeJS and `libecc.so` library have the same platform (x86, x64).

In file `binding.gyp` set **absolute** path to `libecc.so`. In Windows it is `libecc.lib` path
```
...
  "libraries": [ "/home/user/lib-crypto/crypto/libecc.so" ]
...
```
In file `node-bignumber.h` set **your** path to `crypto.h`

`#include "../lib-crypto/crypto.h"`

Build and run test example:
```
cd node-addon/src
npm install
node ./addon.js
```