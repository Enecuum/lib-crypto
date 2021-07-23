#include "Curve.h"

Curve::Curve() {
    this->curve = nullptr;
    this->G = nullptr;
    createDefault();
}

Curve::Curve(const Curve& C) {
    this->curve = nullptr;
    this->G = nullptr;

    *this = C;
}

Curve::Curve(const BigNumber& a, const BigNumber& b, const BigNumber& p, const BigNumber& order, const BigNumber& gx, const BigNumber& gy) {
    this->order = order;
    this->field = p;
    this->a = a;
    this->gx = gx;
    this->gy = gy;
    this->curve = nullptr;
    this->G = nullptr;

    BN_CTX* ctx = nullptr;
    if (nullptr == (ctx = BN_CTX_new())) {
        handleError(NO_MEMORY);
        return;
    }

    if (nullptr == (this->curve = EC_GROUP_new_curve_GFp(p.bn, a.bn, b.bn, ctx))) {       
        BN_CTX_free(ctx);
        handleError(NO_MEMORY);
        return;
    }

    /* Create the generator */
    if (nullptr == (this->G = EC_POINT_new(this->curve))) {                
        EC_GROUP_free(this->curve);
        BN_CTX_free(ctx);
        this->curve = nullptr;
        handleError(NO_MEMORY);
        return;
    }

    if (1 != EC_POINT_set_affine_coordinates_GFp(this->curve, this->G, gx.bn, gy.bn, ctx)) {        
        EC_POINT_free(this->G);
        EC_GROUP_free(this->curve);
        BN_CTX_free(ctx);
        this->G = nullptr;
        this->curve = nullptr;
        handleError(CALC_FAILED);
        return;
    }

    BN_CTX_free(ctx);
}


Curve::~Curve() {
    EC_POINT_free(G);
    EC_GROUP_free(curve);
    G = nullptr;
    curve = nullptr;
}

EC_POINT* Curve::getPoint() {
    return G;
}

EC_GROUP* Curve::getCurve() {
    return curve;
}

void Curve::setPointByCopy(const EC_POINT* G) {
    if (G == nullptr) {
        handleError(POINTER_NULL);
        return;
    }
    
    if (1 != EC_POINT_copy(this->G, G))
        handleError(CALC_FAILED);
}


EC_GROUP* create_curve(const BigNumber& a, const BigNumber& b, const BigNumber& p, const BigNumber& order, const BigNumber& gx, const BigNumber& gy)
{
    EC_GROUP* curve = nullptr;
    EC_POINT* G = nullptr;

    if (nullptr == (curve = EC_GROUP_new_curve_GFp(p.bn, a.bn, b.bn, nullptr))) {
        handleError(NO_MEMORY);        
        return POINTER_ERROR_RET;
    }

    /* Create the generator */
    if (nullptr == (G = EC_POINT_new(curve))) {        
        EC_GROUP_free(curve);
        curve = nullptr;
        handleError(NO_MEMORY);
        return POINTER_ERROR_RET;
    }

    if (1 != EC_POINT_set_affine_coordinates_GFp(curve, G, gx.bn, gy.bn, nullptr)) {               
        EC_POINT_free(G);
        EC_GROUP_free(curve);
        curve = nullptr;
        G = nullptr;
        handleError(CALC_FAILED);
        return POINTER_ERROR_RET;
    }

    /* Set the generator and the order */
    if (1 != EC_GROUP_set_generator(curve, G, order.bn, nullptr)) {
        EC_POINT_free(G);
        EC_GROUP_free(curve);
        curve = nullptr;
        G = nullptr;
        handleError(CALC_FAILED);
        return POINTER_ERROR_RET;
    }

    // OpenSSL curve test
    if (1 != EC_GROUP_check(curve, nullptr)) {
        EC_POINT_free(G);
        EC_GROUP_free(curve);
        curve = nullptr;
        G = nullptr;
        handleError(CALC_FAILED);
        return POINTER_ERROR_RET;
    }

    EC_POINT_free(G);
    G = nullptr;
    return curve;
}


std::string Curve::toDecString() {
    static std::string str;
    str += "order = "; str.append(this->order.toDecString()); str += "\n";
    str += "field = "; str.append(this->field.toDecString()); str += "\n";
    str += "a = "; str.append(this->a.toDecString()); str += "\n";
    str += "gx = "; str.append(this->gx.toDecString()); str += "\n";
    str += "gy = "; str.append(this->gy.toDecString()); str += "\n";
    str += "curve pointer = ";
    str += toHex(curve);
    str.append("\n");

    str += "G pointer = ";
    str += toHex(G);
    return str;
}

Curve& Curve::operator=(const Curve& C)
{
    if (this == &C)
        return *this;

    EC_GROUP_free(this->curve);
    EC_POINT_free(this->G);
    this->curve = nullptr;
    this->G = nullptr;
    createDefault();

    if (1 != EC_GROUP_copy(this->curve, C.curve)) {        
        EC_GROUP_free(this->curve);
        EC_POINT_free(this->G);
        this->curve = nullptr;
        this->G = nullptr;
        handleError(NO_MEMORY);
        return *this;
    }

    if (1 != EC_POINT_copy(this->G, C.G)) {
        handleError(NO_MEMORY);
        return *this;
    }
    
    this->order = C.order;
    this->field = C.field;
    this->a = C.a;
    this->gx = C.gx;
    this->gy = C.gy;
    return *this;
}

void Curve::createDefault() {
    BigNumber a(1);
    BigNumber b(0);
    BigNumber p("70000000000000000000000000000000000200014000000000000000000000000000000000010000800000020000000000000000000000000000000000080003");    
    BN_CTX* ctx = BN_CTX_new();
    if (nullptr == (this->curve = EC_GROUP_new_curve_GFp(p.bn, a.bn, b.bn, ctx))) {
        BN_CTX_free(ctx);
        handleError(NO_MEMORY);
        return;
    }

    if (nullptr == (this->G = EC_POINT_new(this->curve))) {
        BN_CTX_free(ctx);
        EC_GROUP_free(curve);        
        curve = nullptr;
        handleError(NO_MEMORY);
        return;
    }
    BN_CTX_free(ctx);
}