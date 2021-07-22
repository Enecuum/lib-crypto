#ifndef __ELLIPTIC_CURVE_H__
#define __ELLIPTIC_CURVE_H__

#pragma once

/*
check if "=="" operator for type Element in ecPoint is defined
*/
#ifndef GIVARO_ZPZ_H
#define GIVARO_ZPZ_H
#include <givaro/givzpz.h>
#include <givaro/givpoly1.h>
#include <givaro/givpower.h>
#include <givaro/givrandom.h>
#include <givaro/givinteger.h>
#include <givaro/givintsqrootmod.h>
//#include <givaro/modular-integer.h>
#endif

#include <iostream>
#include <sstream>
#include "crypto_defs.h"


using namespace Givaro;


class ExtensionField
{
public:
    Integer p, m;
    typedef ZpzDom<Integer> primeField;
    typedef Poly1Dom< primeField, Dense >::Element Element;
    primeField Fp;
    // Fp[X]- Polynomials over Fp, with X as indeterminate
    Poly1Dom< primeField, Dense > Fp_X;
    //irred is an irreducible polynomial of degree m in Fp_X
    Element irred, zero, one, mOne;

    ExtensionField();
    ExtensionField(const ExtensionField& E);
    ExtensionField(const Integer& p, const Integer& m);
    ExtensionField(const Integer& p, const Integer& m, const std::string& strIrred);
    ExtensionField& operator=(const ExtensionField& E);

    //Return R=(A+B)%p
    Element& add(Element& R, const Element& A, const Element& B) const;
    //Return R=(A-B)%p
    Element& sub(Element& R, const Element& A, const Element& B) const;
    //Return R=-B
    Element& neg(Element& R, const Element& A) const;

    Element& pow(Element& R, const Element& A, std::string eta) const;
    //Return R=A*B%irred
    Element& mul(Element& R, const Element& A, const Element& B) const;
    //Return R=A*A%irred
    Element& sqr(Element& R, const Element& A) const;
    //Return inverse of (A) with irred polynomial, if gcd(A,irred)!=1, then it return 0
    Element& inv(Element& I, const Element& A) const;
    //I+A=0
    Element& additiveInv(Element& I, const Element& A) const;
    //Return Q, such that Q=A/B=A*(inverse of B),
    Element& div(Element& Q, const Element& A, const Element& B) const;
    //R=k*A;
    Element& scalarMultiply(Element& R, const Element& A, Integer k) const;

    void readElement(Element& A, bool flag = true);
    void readElement(std::string str, Element& P);
    void writeElement(Element& A);
    void writeElement(Element& A, std::stringstream& ss);
    bool areEqual(Element& A, const Element& B) const;
};
class ecPoint
{
public:
    typedef ExtensionField::Element Element;
    ecPoint() : identity(true) {}
    ecPoint(bool b);
    ecPoint(const Element& x1, const Element& y1);
    ecPoint(const ecPoint& P);
    ecPoint& operator=(const ecPoint& P);
    bool operator==(const ecPoint& t) const;
    bool operator< (const ecPoint& t) const;

    bool identity;
    Element x, y;
};

/*
 Elliptic Curve over field p^m, defined by co-efficient a,b,c (3 possible equations)
 type 0: E/K, char(K)!=2: y2 = x3+ax+b,
 type 1: non-supersingular E/F2m: y2 + xy = x3+ax2+b,
 tyep 2: supersingular E/F2m: y2 + cy = x3 + ax + b
 */

class ellipticCurve
{
public:

    typedef ExtensionField::Element Element;  //element over the field
    ExtensionField* Kptr;                     //elliptic curve over field K
    Element A, B, C;                             //equation of curve, depending upon the char(K)

    ellipticCurve();
    ellipticCurve(const ellipticCurve& src);
    ellipticCurve(Integer p, Integer m, std::string strIrred, std::string strA, std::string strB);
    ~ellipticCurve();


    ellipticCurve& operator=(const ellipticCurve& F);
    void print();
    int type; //0,1,2
};

class ellipticCurveFq
{
public:
    ellipticCurve* ec;
    int d; //elliptic curve ec/Fq, abelian group of points E(Fq^d), default d=1
    typedef ExtensionField::Element fieldElement;
    typedef ecPoint Point;
    ExtensionField* field;//coordinates of point belong to this field F(q^d)
    //hence arithmetic on co-ordinates is done in this field
    Point identity;

    ellipticCurveFq();
    ellipticCurveFq(const ellipticCurveFq& F);
    ellipticCurveFq(const ellipticCurve* e);
    ~ellipticCurveFq();

    ellipticCurveFq& operator=(const ellipticCurveFq& F);

    //Q=-P
    const Point& inv(Point& Q, const Point& P);
    //is Q=-P?
    bool isInv(const Point& Q, const Point& P);
    //R=2*P
    Point& Double(Point& R, const Point& P);
    //R=P+Q          
    Point& add(Point& R, const Point& P, const Point& Q);
    //R=k*P
    Point& scalarMultiply(Point& R, const Point& P, Integer k, const Integer order);  //order of P

    void show(Point& P);
    void show();
};

#endif