#include "Curve.h"

Curve::Curve() {

	}

Curve::Curve(BigNumber a, BigNumber b, BigNumber p, BigNumber order, BigNumber gx, BigNumber gy) {
		this->curve = create_curve(a, b, p, order, gx, gy);
		this->order = order;
		this->field = p;
		this->a = a;
		this->gx = gx;
		this->gy = gy;
		EC_POINT *G;

		if (NULL == (curve = EC_GROUP_new_curve_GFp(p.bn, a.bn, b.bn, NULL))) return;

		/* Create the generator */
		if (NULL == (G = EC_POINT_new(curve)))return;
		if (1 != EC_POINT_set_affine_coordinates_GFp(curve, G, gx.bn, gy.bn, NULL))
			return;
		this->G = G;
	}
Curve::~Curve(){}

EC_POINT *Curve::getPoint() {
	return EC_POINT_new(this->curve);
}
//Point createPoint(BigNumber x, BigNumber y) {
//	Point pt(x, y, curve);
//	return pt;
//}
EC_GROUP* Curve::getCurve() {
	return curve;
}

EC_GROUP *create_curve(BigNumber a, BigNumber b, BigNumber p, BigNumber order, BigNumber gx, BigNumber gy)
{
	EC_GROUP *curve;
	EC_POINT *G;

	if (NULL == (curve = EC_GROUP_new_curve_GFp(p.bn, a.bn, b.bn, NULL))) return nullptr;

	/* Create the generator */
	if (NULL == (G = EC_POINT_new(curve)))return nullptr;
	if (1 != EC_POINT_set_affine_coordinates_GFp(curve, G, gx.bn, gy.bn, NULL))
		return nullptr;

	/* Set the generator and the order */
	if (1 != EC_GROUP_set_generator(curve, G, order.bn, NULL))
		return nullptr;

	// OpenSSL curve test
	if (1 != EC_GROUP_check(curve, NULL)) return nullptr;
	EC_POINT_free(G);
	return curve;
}