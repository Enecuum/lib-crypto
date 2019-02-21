#include <napi.h>
#include "node-bignumber.h"
#include "ecc/crypto/crypto.h"

using namespace Napi;

Napi::Object CreateObject(const Napi::CallbackInfo& info) {
  return NodeBN::NewInstance(info[0]);
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


Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
  NodeBN::Init(env, exports);

  exports.Set(Napi::String::New(env, "CreateObject"),
      Napi::Function::New(env, CreateObject));

  exports.Set(Napi::String::New(env, "add"),
      Napi::Function::New(env, Add));
   exports.Set(Napi::String::New(env, "getRandom"),
       Napi::Function::New(env, GetRandom));
  return exports;
}

NODE_API_MODULE(addon, InitAll)