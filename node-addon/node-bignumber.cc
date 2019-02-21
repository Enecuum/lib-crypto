#include <napi.h>
#include "ecc/crypto/crypto.h"
#include <iostream>
#include <stdio.h>
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

//
//	Point wrap
//
NodePT::NodePT(const Napi::CallbackInfo& info) : Napi::ObjectWrap<NodePT>(info) {
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  this->val_ = info[0].As<Napi::Number>().DoubleValue();
  //this->p = EC_POINT_new(curve1);
};

Napi::FunctionReference NodePT::constructor;

void NodePT::Init(Napi::Env env, Napi::Object exports) {
  Napi::HandleScope scope(env);

  Napi::Function func = DefineClass(env, "NodePT", {});

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();

  exports.Set("NodePT", func);
}

Napi::Object NodePT::NewInstance(Napi::Value arg) {
  Napi::Object obj = constructor.New({ arg });
  return obj;
}

//
//	Curve wrap
//
NCurve::NCurve(const Napi::CallbackInfo& info) : Napi::ObjectWrap<NCurve>(info) {
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  NodeBN* a = Napi::ObjectWrap<NodeBN>::Unwrap(info[0].As<Napi::Object>());
  NodeBN* b = Napi::ObjectWrap<NodeBN>::Unwrap(info[1].As<Napi::Object>());
  NodeBN* p = Napi::ObjectWrap<NodeBN>::Unwrap(info[2].As<Napi::Object>());
  NodeBN* order = Napi::ObjectWrap<NodeBN>::Unwrap(info[3].As<Napi::Object>());
  NodeBN* gx = Napi::ObjectWrap<NodeBN>::Unwrap(info[4].As<Napi::Object>());
  NodeBN* gy = Napi::ObjectWrap<NodeBN>::Unwrap(info[5].As<Napi::Object>());

   this->curve = create_curve(a->bn, b->bn, p->bn, order->bn, gx->bn, gy->bn);

};

Napi::FunctionReference NCurve::constructor;

void NCurve::Init(Napi::Env env, Napi::Object exports) {
  Napi::HandleScope scope(env);

  Napi::Function func = DefineClass(env, "NCurve", {});

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();

  exports.Set("NCurve", func);
}

Napi::Object NCurve::NewInstance(Napi::Value arg) {
  Napi::Object obj = constructor.New({ arg });
  return obj;
}
