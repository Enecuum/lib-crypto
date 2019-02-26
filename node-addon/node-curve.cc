//
//	Curve wrap
//
#include <napi.h>
//#include "ecc/crypto/crypto.h"
#include <iostream>
#include <stdio.h>
#include <uv.h>
#include "node-bignumber.h"

NCurve::NCurve(const Napi::CallbackInfo& info) : Napi::ObjectWrap<NCurve>(info) {
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  NodeBN* a = Napi::ObjectWrap<NodeBN>::Unwrap(info[0].As<Napi::Object>());
  NodeBN* b = Napi::ObjectWrap<NodeBN>::Unwrap(info[1].As<Napi::Object>());
  NodeBN* p = Napi::ObjectWrap<NodeBN>::Unwrap(info[2].As<Napi::Object>());
  NodeBN* order = Napi::ObjectWrap<NodeBN>::Unwrap(info[3].As<Napi::Object>());
  NodeBN* gx = Napi::ObjectWrap<NodeBN>::Unwrap(info[4].As<Napi::Object>());
  NodeBN* gy = Napi::ObjectWrap<NodeBN>::Unwrap(info[5].As<Napi::Object>());
// EC_GROUP *curve = create_curve(a->bn, b->bn, p->bn, order->bn, gx->bn, gy->bn);
//    this->crv = curve;

 BN_CTX *ctx = BN_CTX_new();
	EC_GROUP *curve;
	EC_POINT *G;

	if (NULL == (curve = EC_GROUP_new_curve_GFp(p->bn.bn, a->bn.bn, b->bn.bn, ctx)))
		Napi::Error::New(env, "EC_POINT_new error").ThrowAsJavaScriptException();

	/* Create the generator */
	if (NULL == (G = EC_POINT_new(curve)))
		Napi::Error::New(env, "EC_POINT_new error").ThrowAsJavaScriptException();
	if (1 != EC_POINT_set_affine_coordinates_GFp(curve, G, gx->bn.bn, gy->bn.bn, ctx))
		Napi::Error::New(env, "EC_POINT_new error").ThrowAsJavaScriptException();

	/* Set the generator and the order */
	if (1 != EC_GROUP_set_generator(curve, G, order->bn.bn, NULL))
		Napi::Error::New(env, "EC_POINT_new error").ThrowAsJavaScriptException();

	// OpenSSL curve test
	if (1 != EC_GROUP_check(curve, NULL))
		Napi::Error::New(env, "EC_POINT_new error").ThrowAsJavaScriptException();
	EC_POINT_free(G);

	this->crv = curve;
   this->d = 10;
};

Napi::FunctionReference NCurve::constructor;

void NCurve::Init(Napi::Env env, Napi::Object exports) {
  Napi::HandleScope scope(env);

Napi::Function func = DefineClass(env, "NCurve", {
    InstanceMethod("value", &NCurve::GetValue)
  });
  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();
  exports.Set("NCurve", func);
}

Napi::Object NCurve::NewInstance(Napi::Value arg) {
  Napi::Object obj = constructor.New({ arg });
  return obj;
}

Napi::Value NCurve::GetValue(const Napi::CallbackInfo& info) {
  double num = this->d;

  return Napi::Number::New(info.Env(), num);
}