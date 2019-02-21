#include <napi.h>
#include "ecc/crypto/crypto.h"

#include <uv.h>
#include "node-bignumber.h"

NodeBN::NodeBN(const Napi::CallbackInfo& info) : Napi::ObjectWrap<NodeBN>(info) {
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  this->val_ = info[0].As<Napi::Number>().DoubleValue();
  this->bn = BigNumber(info[0].As<Napi::Number>().Int32Value());
};

Napi::FunctionReference NodeBN::constructor;

void NodeBN::Init(Napi::Env env, Napi::Object exports) {
  Napi::HandleScope scope(env);

  Napi::Function func = DefineClass(env, "NodeBN", {});

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();

  exports.Set("NodeBN", func);
}

Napi::Object NodeBN::NewInstance(Napi::Value arg) {
  Napi::Object obj = constructor.New({ arg });
  return obj;
}