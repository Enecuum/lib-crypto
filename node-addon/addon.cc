#include <napi.h>
#include "node-bignumber.h"
//#include "ecc/crypto/crypto.h"

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
  //Napi::Error::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
//return env.Null();
  NodeBN* a = Napi::ObjectWrap<NodeBN>::Unwrap(info[0].As<Napi::Object>());
  NodeBN* b = Napi::ObjectWrap<NodeBN>::Unwrap(info[1].As<Napi::Object>());
  //int res = a->bn.decimal() + b->bn.decimal();
  BigNumber res = a->bn + b->bn;
  //return Napi::External::New(env, new BigNumber(1));
  Napi::External<BigNumber> ext = Napi::External<BigNumber>::New(info.Env(), &res);
  //Napi::Object ret = BNumber(info);

  //ret.setVal(res);
 //  ret.setValue(res);
	// ret->bn = res;
  //ret.Set(Napi::String::New(env, "num"), res);
  return NodeBN::NewInstance(ext);
}

Napi::Object Mult(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  NodeBN* a = Napi::ObjectWrap<NodeBN>::Unwrap(info[0].As<Napi::Object>());
  NodePT* P = Napi::ObjectWrap<NodePT>::Unwrap(info[1].As<Napi::Object>());
	NCurve* curve = Napi::ObjectWrap<NCurve>::Unwrap(info[2].As<Napi::Object>());
  
  // EC_POINT *res = mul(a->bn, P->p, curve->crv);
	EC_POINT *res = EC_POINT_new(curve->crv);
	if (1 != EC_POINT_mul(curve->crv, res, NULL, P->p, a->bn.bn, NULL))
		Napi::Error::New(env, "EC_POINT_mul error").ThrowAsJavaScriptException();

  return NodePT::NewInstance(Napi::External<EC_POINT*>::New(info.Env(), &res));
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
    exports.Set(Napi::String::New(env, "mult"),
      Napi::Function::New(env, Mult));
     // exports.Set(Napi::String::New(env, "setCurve"),
     //   Napi::Function::New(env, SetCurve));
   exports.Set(Napi::String::New(env, "getRandom"),
       Napi::Function::New(env, GetRandom));
  return exports;
}

NODE_API_MODULE(addon, InitAll)