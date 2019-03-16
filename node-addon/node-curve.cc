#include <napi.h>
#include "node-bignumber.h"
#include <stdexcept>
#include <iostream>

NCurve::NCurve(const Napi::CallbackInfo& info) : Napi::ObjectWrap<NCurve>(info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    NodeBN* a = Napi::ObjectWrap<NodeBN>::Unwrap(info[0].As<Napi::Object>());
    NodeBN* b = Napi::ObjectWrap<NodeBN>::Unwrap(info[1].As<Napi::Object>());
    NodeBN* p = Napi::ObjectWrap<NodeBN>::Unwrap(info[2].As<Napi::Object>());
    NodeBN* order = Napi::ObjectWrap<NodeBN>::Unwrap(info[3].As<Napi::Object>());
    NodeBN* gx = Napi::ObjectWrap<NodeBN>::Unwrap(info[4].As<Napi::Object>());
    NodeBN* gy = Napi::ObjectWrap<NodeBN>::Unwrap(info[5].As<Napi::Object>());
    
    Curve curve(a->bn, b->bn, p->bn, order->bn, gx->bn, gy->bn);
    this->crv = curve;
    
    try{
        //int i = test(1, 2);
        printPoint(curve.G, &curve);
        std::cout << "Caught NAPI exception\n";
    }
    catch(...){
        //Napi::Error::New(env, "ass").ThrowAsJavaScriptException();
        throw Napi::Error::New(env, "Example exception");
    }

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