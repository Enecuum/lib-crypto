## C++ API Documentation

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

<a name="bn"></a>
#### class BigNumber
```cpp
class BigNumber
```
This class is a wrap of OpenSSL BIGNUM structure with additional methods
##### Fields:
```cpp
BIGNUM* bn;
```
OpenSSL BIGNUM variable to be wrapped 
##### Constructors:
```cpp
BigNumber(unsigned char * x, int n);
```
Creates object from `unsigned char` buffer of `n` size
```cpp
BigNumber(int x);
```
Creates object from int number
```cpp
BigNumber(const BigNumber& bn);
```
Copy constructor
```cpp
BigNumber(BIGNUM* x);
```
Creates object from OpenSSL BIGNUM
##### Methods:
```cpp
int decimal();
```
Returns decimal value of `bn` field
```cpp
char* toDecString();
```
Returns pointer to a char representation of `bn` field
<a name="curve"></a>
#### class Curve
```cpp
class Curve
```
This class is a wrap of OpenSSL EC_POINT structure with additional fields and methods
##### Constructor:
```cpp
Curve(BigNumber a, BigNumber b, BigNumber p, BigNumber order, BigNumber gx, BigNumber gy)
```
 - `[in] a` - `a` parameter of elliptic curve
 - `[in] b` - `p` parameter of elliptic curve
 - `[in] p` - field of elliptic cupve
 - `[in] order` - Number of points of elliptic curve
 - `[in] gx` - x-coordinate of group generator
 - `[in] gy` - y-coordinate of group generator


<a name="mul"></a>
#### mul
```cpp
EC_POINT* mul(BigNumber n, EC_POINT *P, Curve *crv);
```
Multiply point by number
 - `[in] n` - number
 - `[in] P` - point
 - `[in] crv` - Elliptic curve
 - `return` - Result of multiplication

<a name="creatempk"></a>
#### createMPK
```cpp
EC_POINT* createMPK(BigNumber msk, EC_POINT *P, Curve *crv);
```
Creates Master Public Key. Wrapper of mul() function.
 - `[in] msk` - number
 - `[in] P` - point
 - `[in] crv` - Elliptic curve
 - `return` - Master Public Key

<a name="getrandom"></a>
#### getRandom
```cpp
BigNumber getRandom(BigNumber max);
```
Generates non-zero random number in range from 1 to `max`
 - `[in] max` - Range upper bound
 - `return` - Random number

<a name="shamir"></a>
#### shamir
```cpp
vector<BigNumber> shamir(BigNumber secretM, int participantN, int sufficientK, BigNumber q);
```
Shamir's secret key sharing scheme.
 - `[in] secretM` - SecretKey to be shared
 - `[in] participantN` - total number of parts
 - `[in] sufficientK` - min number of participants
 - `[in] q` - Finite field order
 - `return` - vector of SecretKey shadows

<a name="keyproj"></a>
#### keyProj
```cpp
vector<EC_POINT*> keyProj(vector<int> coalition, vector<BigNumber> shares, EC_POINT *Q, Curve *crv);
```
Creates vector of Public Keys. Multiply elements of `shares` by `Q`
 - `[in] coalition` - vector of participant's ID's involved in Shamir's secret key sharing scheme
 - `[in] shares` - vector of secret key shares
 - `[in] Q` - point of elliptic curve
 - `[in] crv` - Elliptic curve
 - `return` - vector of Public keys

<a name="keyrecovery"></a>
#### keyRecovery
```cpp
EC_POINT* keyRecovery(std::vector<EC_POINT*> proj, vector<int> coalition, BigNumber q, Curve *crv);
```
SecretKey recovery. Gets session SecretKey SK from participants key shadows
 - `[in] proj` - vector of participant's Public keys
 - `[in] coalition` - vector of participant's ID's involved in secret key sharing scheme
 - `[in] q` - Finite field order
 - `return` - SecretKey SK - point of EC. SK = msk * Q; (Q - Public key of LPoS)

<a name="weilpairing"></a>
#### weilPairing
```cpp
BigNumber weilPairing(EC_POINT *P, EC_POINT *Q, EC_POINT *S, Curve *crv);
```
 - `[in] P` - 
 - `[in] Q` - 
 - `[in] S` - 
 - `[in] crv` - Elliptic curve

<a name="operators"></a>
#### Operators
```cpp
BigNumber operator * (const BigNumber &a, const BigNumber &b);
BigNumber operator % (const BigNumber &a, const BigNumber &b);
BigNumber operator - (const BigNumber &a, const BigNumber &b);
BigNumber operator + (const BigNumber &a, const BigNumber &b);
```
Operators overloading for BigNumber class