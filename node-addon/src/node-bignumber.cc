#include <napi.h>
#include <iostream>
#include "node-bignumber.h"

NodeBN::NodeBN(const Napi::CallbackInfo& info) : Napi::ObjectWrap<NodeBN>(info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    BigNumber* ext = info[0].As<Napi::External<BigNumber>>().Data();
    this->bn = *ext;
};

Napi::FunctionReference NodeBN::constructor;

void NodeBN::Init(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "NodeBN", {
        InstanceMethod("value", &NodeBN::GetValue),
        InstanceMethod("decString", &NodeBN::GetDecString),
        InstanceMethod("hexString", &NodeBN::GetHexString)
    });
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
    exports.Set("NodeBN", func);
}

Napi::Object NodeBN::NewInstance(Napi::Value arg) {
    Napi::Object obj = constructor.New({ arg });
    return obj;
}

Napi::Value NodeBN::GetValue(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), this->bn.decimal());
}

Napi::Value NodeBN::GetDecString(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), this->bn.toDecString());
}
Napi::Value NodeBN::GetHexString(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), this->bn.toHexString());
}