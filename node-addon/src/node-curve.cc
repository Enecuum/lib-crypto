#include <napi.h>
#include "node-bignumber.h"
#include <stdexcept>
#include <iostream>

NCurve::NCurve(const Napi::CallbackInfo& info) : Napi::ObjectWrap<NCurve>(info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    //Curve *curve = info[0].As<Napi::External<Curve>>().Data();
    this->crv = info[0].As<Napi::External<Curve>>().Data();
    
    // try{
    //     //int i = test(1, 2);
    //     printPoint(curve.G, &curve);
    // }
    // catch(unsigned long err){
    //     //Napi::Error::New(env, "ass").ThrowAsJavaScriptException();
    //     throw Napi::Error::New(env, ERR_error_string(err, NULL));
    // }

};

Napi::FunctionReference NCurve::constructor;

Napi::Object NCurve::Init(Napi::Env env, Napi::Object exports) {
    //Napi::HandleScope scope(env);

    Napi::Function func = DefineClass(env, "NCurve", {});

    Napi::FunctionReference* constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    exports.Set("NCurve", func);
    env.SetInstanceData(constructor);
    return exports;
}

Napi::Object NCurve::NewInstance(Napi::Env env, Napi::Value arg) {
    std::cout << "NCurve::NewInstance: \t" << std::endl;
    Napi::Object obj = env.GetInstanceData<Napi::FunctionReference>()->New({ arg });
    return obj;

    //Napi::Object obj = constructor.New({ arg });
    //return obj;
}