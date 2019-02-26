#include <napi.h>
#include "node-bignumber.h"
//#include "ecc/crypto/crypto.h"
#include <iostream> 
using namespace Napi;

Napi::Object BNumber(const Napi::CallbackInfo& info) {
	BigNumber bn(info[0].As<Napi::Number>().Int32Value());
	//auto instance = Page::constructor.New({  });
  return NodeBN::NewInstance(Napi::External<BigNumber>::New(info.Env(), &bn));
}

Napi::Object Point(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  NodeBN* x = Napi::ObjectWrap<NodeBN>::Unwrap(info[0].As<Napi::Object>());
  NodeBN* y = Napi::ObjectWrap<NodeBN>::Unwrap(info[1].As<Napi::Object>());
	  NCurve* curve1 = Napi::ObjectWrap<NCurve>::Unwrap(info[2].As<Napi::Object>());
  //int len = (int)curve1->d;
  //Napi::Error::New(env, std::to_string(len)).ThrowAsJavaScriptException();

   EC_POINT *res;
 if (NULL == (res = EC_POINT_new(curve1->crv)))
  Napi::Error::New(env, "EC_POINT_new error").ThrowAsJavaScriptException();
if (1 != EC_POINT_set_affine_coordinates_GFp(curve1->crv, res, x->bn.bn, y->bn.bn, NULL))
    Napi::Error::New(env, "EC_POINT_set_affine_coordinates_GFp error").ThrowAsJavaScriptException();


  return NodePT::NewInstance(Napi::External<EC_POINT*>::New(info.Env(), &res));
}

// Napi::Object Curve(const Napi::CallbackInfo& info) {
//   return NCurve::NewInstance(info[0]);
// }

Napi::Object Add(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  NodeBN* a = Napi::ObjectWrap<NodeBN>::Unwrap(info[0].As<Napi::Object>());
  NodeBN* b = Napi::ObjectWrap<NodeBN>::Unwrap(info[1].As<Napi::Object>());

  BigNumber res = a->bn + b->bn;
  return NodeBN::NewInstance(Napi::External<BigNumber>::New(info.Env(), &res));
}

Napi::Object Mmul(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	NodeBN* a = Napi::ObjectWrap<NodeBN>::Unwrap(info[0].As<Napi::Object>());
	NodeBN* b = Napi::ObjectWrap<NodeBN>::Unwrap(info[1].As<Napi::Object>());
	NodeBN* m = Napi::ObjectWrap<NodeBN>::Unwrap(info[2].As<Napi::Object>());

	BigNumber res = (a->bn * b->bn) % m->bn;
	return NodeBN::NewInstance(Napi::External<BigNumber>::New(info.Env(), &res));
}

Napi::Object PtAdd(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	NodePT* a = Napi::ObjectWrap<NodePT>::Unwrap(info[0].As<Napi::Object>());
	NodePT* b = Napi::ObjectWrap<NodePT>::Unwrap(info[1].As<Napi::Object>());
	NCurve* curve = Napi::ObjectWrap<NCurve>::Unwrap(info[2].As<Napi::Object>());

   EC_POINT *res;
	if (1 != EC_POINT_add(curve->crv, res, a->p, b->p, NULL))
		Napi::Error::New(env, "EC_POINT_add error").ThrowAsJavaScriptException();
//if (!res)
	//Napi::Error::New(env, "EC_POINT_mul error").ThrowAsJavaScriptException();
	 //EC_POINT *res = EC_POINT_new(curve->crv);
	 //if (1 != EC_POINT_mul(curve->crv, res, NULL, P->p, a->bn.bn, NULL))
	// 	Napi::Error::New(env, "EC_POINT_mul error").ThrowAsJavaScriptException();

  return NodePT::NewInstance(Napi::External<EC_POINT*>::New(info.Env(), &res));
}

Napi::Object Mul(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  NodeBN* a = Napi::ObjectWrap<NodeBN>::Unwrap(info[0].As<Napi::Object>());
  NodePT* P = Napi::ObjectWrap<NodePT>::Unwrap(info[1].As<Napi::Object>());
	NCurve* curve = Napi::ObjectWrap<NCurve>::Unwrap(info[2].As<Napi::Object>());
  
   EC_POINT *res = mul(a->bn, P->p, curve->crv);
//if (!res)
	//Napi::Error::New(env, "EC_POINT_mul error").ThrowAsJavaScriptException();
	 //EC_POINT *res = EC_POINT_new(curve->crv);
	 //if (1 != EC_POINT_mul(curve->crv, res, NULL, P->p, a->bn.bn, NULL))
	// 	Napi::Error::New(env, "EC_POINT_mul error").ThrowAsJavaScriptException();

  return NodePT::NewInstance(Napi::External<EC_POINT*>::New(info.Env(), &res));
}

Napi::Object CreateMPK(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	NodeBN* a = Napi::ObjectWrap<NodeBN>::Unwrap(info[0].As<Napi::Object>());
	NodePT* P = Napi::ObjectWrap<NodePT>::Unwrap(info[1].As<Napi::Object>());
	NCurve* curve = Napi::ObjectWrap<NCurve>::Unwrap(info[2].As<Napi::Object>());
	EC_POINT *res = createMPK(a->bn, P->p, curve->crv);
  	return NodePT::NewInstance(Napi::External<EC_POINT*>::New(info.Env(), &res));
}

Napi::Array Shamir(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	NodeBN* secret = Napi::ObjectWrap<NodeBN>::Unwrap(info[0].As<Napi::Object>());
	int n = info[1].As<Napi::Number>().Int32Value();
	int k = info[2].As<Napi::Number>().Int32Value();
	NodeBN* q = Napi::ObjectWrap<NodeBN>::Unwrap(info[3].As<Napi::Object>());
	//std::vector<BigNumber> shares = shamir(secret->bn, n, k, q->bn);
	// ----------------------------------------------------------
		const int power = k - 1;

	// x^5 + 8x^4 + 6x^3 + 5x^2 + 10x
	std::vector<int> arrayA = { 1, 8, 6, 5, 10 };

	std::vector<BigNumber> shares;
	for (int i = 0; i < n; i++)
	{
		int temp = 0;
		for (int j = 0; j < power; j++)
			temp += arrayA[j] * (pow(i + 1, power - j));
		shares.insert(shares.end(), (BigNumber(temp) + secret->bn) % q->bn);
	}
	//--------------------------------------------------------------------
	Napi::Array res = Napi::Array::New(env, shares.size());
	for(uint32_t i  = 0; i < shares.size(); i++){
		res.Set(i, NodeBN::NewInstance(Napi::External<BigNumber>::New(info.Env(), &shares[i])));
	}
  	return res;
}

Napi::Array KeyProj(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();

	Napi::Array ncoal = info[0].As<Napi::Array>();
	Napi::Array nshares = info[1].As<Napi::Array>();
	NodePT* P = Napi::ObjectWrap<NodePT>::Unwrap(info[2].As<Napi::Object>());
	NCurve* curve = Napi::ObjectWrap<NCurve>::Unwrap(info[3].As<Napi::Object>());
	
	std::vector<int> coalition;
	for(uint32_t i  = 0; i < ncoal.Length(); i++){
		coalition.push_back(ncoal.Get(i).As<Napi::Number>().Int32Value());
	}
	std::vector<BigNumber> shares;
	for(uint32_t i  = 0; i < nshares.Length(); i++){
		NodeBN* number = Napi::ObjectWrap<NodeBN>::Unwrap(nshares.Get(i).As<Napi::Object>());
		shares.push_back(number->bn);
	}

	std::vector<EC_POINT*> proj = keyProj(coalition, shares, P->p, curve->crv);
	
	Napi::Array res = Napi::Array::New(env, proj.size());
	for(uint32_t i  = 0; i < proj.size(); i++){
		EC_POINT* buf = proj[i];
		res.Set(i, NodePT::NewInstance(Napi::External<EC_POINT*>::New(info.Env(), &buf)));
	}
  	return res;
}

//keyRecovery(proj, coalition, q, curve);
Napi::Object KeyRecovery(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();

	Napi::Array nproj = info[0].As<Napi::Array>();
	Napi::Array ncoal = info[1].As<Napi::Array>();
	NodeBN* q = Napi::ObjectWrap<NodeBN>::Unwrap(info[2].As<Napi::Object>());
	NCurve* curve = Napi::ObjectWrap<NCurve>::Unwrap(info[3].As<Napi::Object>());
	
	std::vector<int> coalition;
	for(uint32_t i  = 0; i < ncoal.Length(); i++){
		coalition.push_back(ncoal.Get(i).As<Napi::Number>().Int32Value());
	}
	std::vector<EC_POINT*> proj;
	for(uint32_t i  = 0; i < nproj.Length(); i++){
		NodePT* pt = Napi::ObjectWrap<NodePT>::Unwrap(nproj.Get(i).As<Napi::Object>());
		proj.push_back(pt->p);
	}

	EC_POINT* res = keyRecovery(proj, coalition, q->bn, curve->crv);

  	return NodePT::NewInstance(Napi::External<EC_POINT*>::New(info.Env(), &res));
}

Napi::Object WeilPairing(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	NodePT* P = Napi::ObjectWrap<NodePT>::Unwrap(info[0].As<Napi::Object>());
	NodePT* Q = Napi::ObjectWrap<NodePT>::Unwrap(info[1].As<Napi::Object>());
	NodePT* S = Napi::ObjectWrap<NodePT>::Unwrap(info[2].As<Napi::Object>());
	NCurve* curve = Napi::ObjectWrap<NCurve>::Unwrap(info[3].As<Napi::Object>());
	
	BigNumber res = weilPairing(P->p, Q->p, S->p, curve->crv);
  	return NodeBN::NewInstance(Napi::External<BigNumber>::New(info.Env(), &res));
}

Napi::Object GetRandom(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  NodeBN* obj1 = Napi::ObjectWrap<NodeBN>::Unwrap(info[0].As<Napi::Object>());
  BigNumber bn = getRandom(obj1->bn);

  //double sum = obj1->Val() + obj2->Val();
  return NodeBN::NewInstance(Napi::External<BigNumber>::New(info.Env(), &bn));
}

Napi::Number SetCurve(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  NodeBN* obj1 = Napi::ObjectWrap<NodeBN>::Unwrap(info[0].As<Napi::Object>());
  BigNumber bn = getRandom(obj1->bn);
  
  NodeBN* a = Napi::ObjectWrap<NodeBN>::Unwrap(info[0].As<Napi::Object>());
  NodeBN* b = Napi::ObjectWrap<NodeBN>::Unwrap(info[1].As<Napi::Object>());
  NodeBN* p = Napi::ObjectWrap<NodeBN>::Unwrap(info[2].As<Napi::Object>());
  NodeBN* order = Napi::ObjectWrap<NodeBN>::Unwrap(info[3].As<Napi::Object>());
  NodeBN* gx = Napi::ObjectWrap<NodeBN>::Unwrap(info[4].As<Napi::Object>());
  NodeBN* gy = Napi::ObjectWrap<NodeBN>::Unwrap(info[5].As<Napi::Object>());

  //EC_GROUP* curve = create_curve(a->bn, b->bn, p->bn, order->bn, gx->bn, gy->bn);
  //Napi::Object obj = constructor.New({ Napi::Number::New(info.Env(), this->value_ * multiple.DoubleValue()) });
  //double sum = obj1->Val() + obj2->Val();
  return Napi::Number::New(env, bn.decimal());
}



  // this->curve = create_curve(a->bn, b->bn, p->bn, order->bn, gx->bn, gy->bn);

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
	NodeBN::Init(env, exports);
	NCurve::Init(env, exports);
	NodePT::Init(env, exports);

	exports.Set(Napi::String::New(env, "BigNumber"),
		Napi::Function::New(env, BNumber));
	exports.Set(Napi::String::New(env, "Point"),
		Napi::Function::New(env, Point));
	// exports.Set(Napi::String::New(env, "Curve"),
	//   Napi::Function::New(env, Curve));
	exports.Set(Napi::String::New(env, "add"),
		Napi::Function::New(env, Add));
	exports.Set(Napi::String::New(env, "mul"),
		Napi::Function::New(env, Mul));
	exports.Set(Napi::String::New(env, "mmul"),
		Napi::Function::New(env, Mmul));
	exports.Set(Napi::String::New(env, "createMPK"),
		Napi::Function::New(env, CreateMPK));
	exports.Set(Napi::String::New(env, "shamir"),
		Napi::Function::New(env, Shamir));
	exports.Set(Napi::String::New(env, "keyProj"),
		Napi::Function::New(env, KeyProj));
	exports.Set(Napi::String::New(env, "keyRecovery"),
		Napi::Function::New(env, KeyRecovery));
	exports.Set(Napi::String::New(env, "addPoints"),
		Napi::Function::New(env, PtAdd));
	exports.Set(Napi::String::New(env, "weilPairing"),
		Napi::Function::New(env, WeilPairing));

	// exports.Set(Napi::String::New(env, "setCurve"),
	//   Napi::Function::New(env, SetCurve));
	exports.Set(Napi::String::New(env, "getRandom"),
		Napi::Function::New(env, GetRandom));
	return exports;
}

NODE_API_MODULE(addon, InitAll)