#include <napi.h>
#include "node-bignumber.h"
#include <stdexcept>
#include <iostream>

NCurve::NCurve(const Napi::CallbackInfo& info) : Napi::ObjectWrap<NCurve>(info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    Curve *curve = info[0].As<Napi::External<Curve>>().Data();
    this->crv = *curve;
    
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