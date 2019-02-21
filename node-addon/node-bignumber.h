#ifndef MYOBJECT_H
#define MYOBJECT_H

#include <napi.h>
#include "ecc/crypto/BigNumber.h"
#include "ecc/crypto/crypto.h"

class NodeBN : public Napi::ObjectWrap<NodeBN> {
 public:
  static void Init(Napi::Env env, Napi::Object exports);
  static Napi::Object NewInstance(Napi::Value arg);
  double Val() const { return val_; }
  NodeBN(const Napi::CallbackInfo& info);
  BigNumber bn;
 private:
  static Napi::FunctionReference constructor;
  double val_;

};

class NodePT : public Napi::ObjectWrap<NodePT> {
 public:
  static void Init(Napi::Env env, Napi::Object exports);
  static Napi::Object NewInstance(Napi::Value arg);
  double Val() const { return val_; }
  NodePT(const Napi::CallbackInfo& info);
  EC_POINT* p;
 private:
  static Napi::FunctionReference constructor;
  double val_;

};

class NCurve : public Napi::ObjectWrap<NCurve> {
 public:
  static void Init(Napi::Env env, Napi::Object exports);
  static Napi::Object NewInstance(Napi::Value arg);
  double Val() const { return val_; }
  NCurve(const Napi::CallbackInfo& info);
  EC_GROUP* curve;
 private:
  static Napi::FunctionReference constructor;
  double val_;

};
#endif