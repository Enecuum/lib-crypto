#ifndef EC_H
#define EC_H
    #include "ellipticCurve.h"
#endif

using namespace Givaro;
using namespace std;

ExtensionField::ExtensionField(Integer p, Integer m)
{
    this->p=p;
    this->m=m;
    Fp   = primeField((primeField::Residu_t)p);
    Fp_X = Poly1Dom< primeField, Dense >( Fp, Indeter("X") );
    if(m==1)
        Fp_X.assign(irred,(Element)p);

    primeField::Element tmp;
    Fp_X.assign(zero,Fp_X.zero);
    Fp_X.assign(one,Fp_X.one);
    Fp_X.assign(mOne,Fp_X.mOne);
}
ExtensionField::ExtensionField(Integer p, Integer m, std::string strIrred)
{
	this->p = p;
	this->m = m;
	Fp = primeField((primeField::Residu_t)p);
	Fp_X = Poly1Dom< primeField, Dense >(Fp, Indeter("X"));
	if (m == 1)
		Fp_X.assign(irred, (Element)p);
	else
	{
		std::istringstream stream(strIrred);
		Fp_X.read(stream, irred);
	}
	primeField::Element tmp;
	Fp_X.assign(zero, Fp_X.zero);
	Fp_X.assign(one, Fp_X.one);
	Fp_X.assign(mOne, Fp_X.mOne);
}
ExtensionField ExtensionField::operator=(const ExtensionField& E)
{
    p=E.p;
    m=E.m;
    Fp=E.Fp;
    Fp_X=E.Fp_X;
    irred=E.irred;
    zero=E.zero;
    one=E.one;
    mOne=E.mOne;
    return *this;
}

//Return R=(A+B)%p
ExtensionField::Element& ExtensionField::add(Element& R, const Element& A, const Element& B) const
{
    Fp_X.add(R,A,B);

    return R;
}
//Return R=(A-B)%p
ExtensionField::Element& ExtensionField::sub(Element& R, const Element& A, const Element& B) const
{
    Fp_X.sub(R,A,B);
    return R;
}
//Return R=-B
ExtensionField::Element& ExtensionField::neg(Element& R, const Element& A) const
{
    Fp_X.neg(R,A);
    return R;
}

bool ExtensionField::areEqual(Element& A, const Element& B) const
{
	return Fp_X.areEqual(A, B);;
}

ExtensionField::Element& ExtensionField::pow(Element& R, const Element& A, string eta) const
{
	Element tmp;

	R = one;
	int pow = eta.size() - 1;
	int size = pow;
	int i = 0;
	tmp = A;
	while(pow >= 0){
		if (eta[pow] == '1') {
			while(i < size - pow){
				sqr(tmp, tmp);
				i++;
			}
			mul(R, R, tmp);
		}
		pow--;
	}
	return R;
}

//Return R=A*B%irred
ExtensionField::Element& ExtensionField::mul(Element& R, const Element& A, const Element& B) const
{
    if(m>1)
    {
        Element tmp,Q;
        Fp_X.mul(tmp,A,B);
        Fp_X.divmod(Q,R,tmp,irred);
    }
    else
        Fp_X.mul(R,A,B);
    return R;
}
//Return R=A*A%irred
ExtensionField::Element& ExtensionField::sqr(Element& R, const Element& A) const
{
    if(m>1)
    {
        Element tmp,Q;
        Fp_X.sqr(tmp,A);
        Fp_X.divmod(Q,R,tmp,irred);
    }
    else
        Fp_X.sqr(R,A);
    return R;
}
//Return inverse of (A) with irred polynomial, if gcd(A,irred)!=1, then it return 0
ExtensionField::Element& ExtensionField::inv(Element& I, const Element& A) const
{
    Element tmp,D;
    D=Fp_X.gcd(D,I,tmp,A,irred);
    if(Fp_X.isOne(D))
        return I;
    Fp_X.assign(I,Fp_X.zero);
    return I;
}
ExtensionField::Element& ExtensionField::additiveInv(Element& I, const Element& A) const
{
    Fp_X.sub(I,Fp_X.zero,A);
    return I;
}

//Return Q, such that Q=A/B=A*(inverse of B),
ExtensionField::Element& ExtensionField::div(Element& Q, const Element& A, const Element& B) const
{
    Element I;
    I=inv(I,B);
    mul(Q,A,I);
    return Q;
}
ExtensionField::Element& ExtensionField::scalarMultiply(Element& R,const Element& A,Integer k) const
{
    k=k%p;
    if(m==1)
    {
        Element eK;
        primeField::Element tmp;
        Fp_X.assign(eK,Fp.init(tmp,k));
        Fp_X.mul(R,A,eK);
        return R;
    }
    Element tmp,acc=A;
    Fp_X.assign(R,zero);
    while(k>0)
    {
        if(k%2)
            Fp_X.addin(R,acc);
        acc=Fp_X.add(tmp,acc,acc); //acc=tmp=2*acc
        k=k/2;
    }
    return R;
}
void ExtensionField::readElement(Element& P, bool flag)
{
    Fp_X.read(cin,P);
}
void ExtensionField::readElement(std::string str, Element& P)
{
	std::istringstream stream(str);
	Fp_X.read(stream, P);
}
void ExtensionField::writeElement(Element& A)
{
    cout<<"( ";
    Fp_X.write(cout<<"",A); 
    cout<<" )";  
}
void ExtensionField::writeElement(Element& A, std::stringstream& ss)
{
	Fp_X.write(ss, A);
}

ecPoint::ecPoint(bool b)
{
    identity=b;
}
ecPoint::ecPoint(Element x1, Element y1)
: identity(false) {
    x=x1;
    y=y1;
}
ecPoint ecPoint::operator=( const ecPoint& P)
{
    if(P.identity)
    {
        this->identity=true;
        return *this;
    }
    
    this->identity=P.identity;
    this->x=P.x;
    this->y=P.y;
    return *this;
}
bool ecPoint::operator==(const ecPoint& t) const
{return (identity && t.identity) || (!identity && !t.identity && x==t.x && y==t.y);}
bool ecPoint::operator< (const ecPoint& t) const
{return identity ? !t.identity : (!t.identity && (x<t.x || (x==t.x && y<t.y)));}

ellipticCurve::ellipticCurve(Integer p, Integer m, std::string strIrred, std::string strA, std::string strB)
{
	Kptr = new ExtensionField(p, m, strIrred);
	Kptr->readElement(strA, A);
	Kptr->readElement(strB, B);
	type = 0;
	cout << endl;
}
ellipticCurve::~ellipticCurve()
{
    free(Kptr);
}

ellipticCurve ellipticCurve::operator=( const ellipticCurve& F)
{
    Kptr=F.Kptr;
    A=F.A;
    B=F.B;
    C=F.C;
    type=F.type;
    return *this;
}
void ellipticCurve::print()
{
    cout<<"A:";
    Kptr->writeElement(A);
    cout<<endl;
    cout<<"B:";
    Kptr->writeElement(B);
    cout<<endl;
    cout<<"C:";
    Kptr->writeElement(C);
    cout<<endl;
    cout<<"type"<<type<<endl;
}

ellipticCurveFq::ellipticCurveFq(ellipticCurve* e)
{
    ec=e;
    field=ec->Kptr;
    identity.identity=true;
    this->d=d;
}
ellipticCurveFq::~ellipticCurveFq()
{  
    free(ec);
    free(field);
}
const ellipticCurveFq::Point& ellipticCurveFq::inv(Point& Q, const Point &P) 
{
    if(P.identity)
    {
        Q.identity=true;
        return Q;
    }
    Q.identity=false;
    Point T;
    Q.x=P.x;
    field->additiveInv(Q.y,P.y); //Q.y+P.y=0
    return Q;
}
bool ellipticCurveFq::isInv(const Point& Q, const Point &P) //is Q+P=point at inifinity?
{
    Point R;
    inv(R,P);
    if(R==Q)
        return true;
    return false;
}

ellipticCurveFq::Point& ellipticCurveFq::Double(Point &R,Point &P) 
{
    if (P.identity||isInv(P,P)) 
    {
        R.identity=true;
        return R;
    }

    R.identity=false;
    fieldElement x,y,x12;
    fieldElement slopeSquare, slope;
    field->sqr(x12,P.x);
    fieldElement _3x12,_2y,_3x12pA,_2x,_x,slope_x;
    fieldElement xpy,Bdx12,slopex,slopex_x;
    fieldElement x12pA,ypC,xpx,slopexpx;

    field->scalarMultiply(_3x12,x12,3);
    field->scalarMultiply(_2y,P.y,2);
    field->add(_3x12pA,_3x12,ec->A);
    field->div(slope,_3x12pA,_2y);
    field->sqr(slopeSquare,slope);
    field->scalarMultiply(_2x,P.x,2);
    field->sub(x,slopeSquare,_2x);
    field->sub(_x,P.x,x);
    field->sub(y,field->mul(slope_x,slope,_x),P.y);
        
    R.x=x;
    R.y=y;
    return R;
}
ellipticCurveFq::Point& ellipticCurveFq::add(Point &R,Point &P, Point &Q) 
{
    if(P.identity&&Q.identity||isInv(P,Q))
    {
        R.identity=true;
        return R;
    }
    R.identity=false;
    if (P.identity) 
    {
        R.x=Q.x;
        R.y=Q.y;
        return R;
    }
    if (Q.identity) 
    {
        R.x=P.x;
        R.y=P.y;
        return R;
    }
    if(P==Q)
        return Double(R,P);
    
    fieldElement x,y,x12;
    fieldElement slopeSquare, slope;
    fieldElement y2m1,x2m1,x1p2,x1m3,slopex1m3,y1pC,x1p2pA,x1p2pApS,slopex1m3px3;
    field->sub(y2m1,Q.y,P.y);
    field->sub(x2m1,Q.x,P.x);
    field->div(slope,y2m1,x2m1);
    field->sqr(slopeSquare,slope);
    field->add(x1p2,P.x,Q.x);

    field->sub(x,slopeSquare,x1p2);
    field->sub(x1m3,P.x,x);
    field->mul(slopex1m3,slope,x1m3);
    field->sub(y,slopex1m3,P.y);
    R.x=x;
    R.y=y;
    return R;
}
void ellipticCurveFq::show(Point& P)
{
    if(P.identity)
    {
        cout<<"o";
        return;
    }

    cout<<"(";
    field->writeElement(P.x);
    cout<<", ";  
    field->writeElement(P.y);  
    cout<<")";
}
//R=kP
ellipticCurveFq::Point& ellipticCurveFq::scalarMultiply(Point&R, Point& P, Integer k, Integer order)//order of P 
{
    if(P.identity)
    {
        R.identity=true;
        return R;
    }
    Point temp(false);
    R.identity=true;
    Point acc(P);
    if(order!=-1)//if order is known
        k=k%order;//better: k=k%order(P)
    while(k>0)
    {
        if(k%2)
        {
            add(temp,R,acc);
            R=temp;
        }
        Double(temp,acc);    
        acc=temp;
        k=k/2;
    }
    return R;
}
/*type 0: E/K, char(K)!=2: y2 = x3+ax+b,
 type 1: non-supersingular E/F2m: y2 + xy = x3+ax2+b,
 tyep 2: supersingular E/F2m: y2 + cy = x3 + ax + b*/
void ellipticCurveFq::show()
{
    cout<<"Elliptic Curve Defined by ";

    cout<<"y^2 = x^3 + "; 
    field->writeElement(ec->A);
    cout<<"x + ";
    field->writeElement(ec->B);
    cout<<endl;

    cout<<" over finite field in X of size "<<field->p<<"^"<<field->m;
    cout<<" with irreducible polynomial ";
    field->writeElement(field->irred);
    cout<<endl;
}