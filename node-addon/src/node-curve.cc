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

// -----------------  E_Fq  -----------------------------

NCurve_Fq::NCurve_Fq(const Napi::CallbackInfo& info) : Napi::ObjectWrap<NCurve_Fq>(info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    ellipticCurveFq *E_Fq = info[0].As<Napi::External<ellipticCurveFq>>().Data();
    this->E_Fq = E_Fq;
    //ellipticCurveFq E_Fq;
    // try{
    //     //int i = test(1, 2);
    //     printPoint(curve.G, &curve);
    // }
    // catch(unsigned long err){
    //     //Napi::Error::New(env, "ass").ThrowAsJavaScriptException();
    //     throw Napi::Error::New(env, ERR_error_string(err, NULL));
    // }

};

Napi::FunctionReference NCurve_Fq::constructor;

void NCurve_Fq::Init(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);

    Napi::Function func = DefineClass(env, "NCurve_Fq", {});
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
    exports.Set("NCurve_Fq", func);
}

Napi::Object NCurve_Fq::NewInstance(Napi::Value arg) {
    Napi::Object obj = constructor.New({ arg });
    return obj;
}