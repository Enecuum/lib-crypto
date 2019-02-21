#ifndef MYOBJECT_H
#define MYOBJECT_H

#include <napi.h>
#include "ecc/crypto/BigNumber.h"

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

#endif