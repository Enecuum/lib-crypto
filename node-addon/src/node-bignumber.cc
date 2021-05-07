#include <napi.h>
#include <uv.h>
#include <iostream>
#include "node-bignumber.h"

NodeBN::NodeBN(const Napi::CallbackInfo& info) : Napi::ObjectWrap<NodeBN>(info) {
    std::cout << "Constructor NodeBN: \t" << std::endl;
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    this->bn = info[0].As<Napi::External<BigNumber>>().Data();
    std::cout << "bn: \t" << this->bn << std::endl;
};
NodeBN::~NodeBN() {
    std::cout << "Destructor NodeBN: \t" << std::endl;

    delete(this->bn);
};
//Napi::FunctionReference NodeBN::constructor;

Napi::Object NodeBN::Init(Napi::Env env, Napi::Object exports) {
    std::cout << "NodeBN::Init: \t" << std::endl;


    //Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "NodeBN", {
        InstanceMethod("value", &NodeBN::GetValue),
        InstanceMethod("decString", &NodeBN::GetDecString),
        InstanceMethod("hexString", &NodeBN::GetHexString)
    });
    Napi::FunctionReference* constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    exports.Set("NodeBN", func);
    env.SetInstanceData(constructor);
    return exports;
    //constructor = Napi::Persistent(func);
    //constructor.SuppressDestruct();

}

Napi::Object NodeBN::NewInstance(Napi::Env env, Napi::Value arg) {
    std::cout << "NodeBN::NewInstance: \t" << std::endl;
    Napi::Object obj = env.GetInstanceData<Napi::FunctionReference>()->New({ arg });
    return obj;

    //Napi::Object obj = constructor.New({ arg });
    //return obj;
}

Napi::Value NodeBN::GetValue(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), this->bn->decimal());
}

Napi::Value NodeBN::GetDecString(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), this->bn->toDecString());
}
Napi::Value NodeBN::GetHexString(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), this->bn->toHexString());
}