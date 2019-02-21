#include <napi.h>
#include "node-bignumber.h"
#include "ecc/crypto/crypto.h"

using namespace Napi;

Napi::Object BNumber(const Napi::CallbackInfo& info) {
  return NodeBN::NewInstance(info[0]);
}

Napi::Object Point(const Napi::CallbackInfo& info) {
  return NodePT::NewInstance(info[0]);
}

Napi::Object Curve(const Napi::CallbackInfo& info) {
  return NCurve::NewInstance(info[0]);
}

Napi::Number Add(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  NodeBN* a = Napi::ObjectWrap<NodeBN>::Unwrap(info[0].As<Napi::Object>());
  NodeBN* b = Napi::ObjectWrap<NodeBN>::Unwrap(info[1].As<Napi::Object>());
  int res = a->bn.decimal() + b->bn.decimal();
  double sum = res;//obj1->Val() + obj2->Val();
  return Napi::Number::New(env, sum);
}

Napi::Number GetRandom(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  NodeBN* obj1 = Napi::ObjectWrap<NodeBN>::Unwrap(info[0].As<Napi::Object>());
  BigNumber bn = getRandom(obj1->bn);

  //double sum = obj1->Val() + obj2->Val();
  return Napi::Number::New(env, bn.decimal());
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

  EC_GROUP* curve = create_curve(a->bn, b->bn, p->bn, order->bn, gx->bn, gy->bn);
  //Napi::Object obj = constructor.New({ Napi::Number::New(info.Env(), this->value_ * multiple.DoubleValue()) });
  //double sum = obj1->Val() + obj2->Val();
  return Napi::Number::New(env, bn.decimal());
}



  // this->curve = create_curve(a->bn, b->bn, p->bn, order->bn, gx->bn, gy->bn);

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
  NodeBN::Init(env, exports);
  NodePT::Init(env, exports);
  NCurve::Init(env, exports);
  exports.Set(Napi::String::New(env, "BigNumber"),
      Napi::Function::New(env, BNumber));
  exports.Set(Napi::String::New(env, "Point"),
      Napi::Function::New(env, Point));
    // exports.Set(Napi::String::New(env, "Curve"),
    //   Napi::Function::New(env, Curve));
  exports.Set(Napi::String::New(env, "add"),
      Napi::Function::New(env, Add));
     // exports.Set(Napi::String::New(env, "setCurve"),
     //   Napi::Function::New(env, SetCurve));
   exports.Set(Napi::String::New(env, "getRandom"),
       Napi::Function::New(env, GetRandom));
  return exports;
}

NODE_API_MODULE(addon, InitAll)