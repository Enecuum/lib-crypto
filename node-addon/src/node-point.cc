#include <napi.h>
#include "node-bignumber.h"

NodePT::NodePT(const Napi::CallbackInfo& info) : Napi::ObjectWrap<NodePT>(info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    EC_POINT* res = *(info[0].As<Napi::External<EC_POINT*>>().Data());
    this->p = res;
};

Napi::FunctionReference NodePT::constructor;

void NodePT::Init(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);

    Napi::Function func = DefineClass(env, "NodePT", {
        InstanceMethod("xy", &NodePT::GetCoords),
        InstanceMethod("SetCoords", &NodePT::SetCoords),
        InstanceMethod("x", &NodePT::getX),
        InstanceMethod("y", &NodePT::getY),
        InstanceMethod("isInfinity", &NodePT::isInfinity)
    });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("NodePT", func);
}

Napi::Object NodePT::NewInstance(Napi::Value arg) {
    Napi::Object obj = constructor.New({ arg });
    return obj;
}

Napi::Value NodePT::GetCoords(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    BigNumber x;
    BigNumber y;
    NCurve* curve = Napi::ObjectWrap<NCurve>::Unwrap(info[0].As<Napi::Object>());
    if (!EC_POINT_get_affine_coordinates_GFp(curve->crv.curve, this->p, x.bn, y.bn, NULL))
        Napi::Error::New(env, "EC_POINT_new error").ThrowAsJavaScriptException();
    std::string str = "(" + std::to_string(x.decimal()) + " " + std::to_string(y.decimal()) + ")";
    return Napi::String::New(info.Env(), str);
}

Napi::Value NodePT::SetCoords(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    NodeBN* x = Napi::ObjectWrap<NodeBN>::Unwrap(info[0].As<Napi::Object>());
    NodeBN* y = Napi::ObjectWrap<NodeBN>::Unwrap(info[1].As<Napi::Object>());
    NCurve* curve = Napi::ObjectWrap<NCurve>::Unwrap(info[2].As<Napi::Object>());
    if (1 != EC_POINT_set_affine_coordinates_GFp(curve->crv.curve, this->p, x->bn.bn, y->bn.bn, NULL))
        Napi::Error::New(env, "EC_POINT_set_affine_coordinates_GFp error").ThrowAsJavaScriptException();
    return Napi::Number::New(info.Env(), 1);
}

Napi::Value NodePT::getX(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    BigNumber x;
    BigNumber y;
    NCurve* curve = Napi::ObjectWrap<NCurve>::Unwrap(info[0].As<Napi::Object>());
    if (!EC_POINT_get_affine_coordinates_GFp(curve->crv.curve, this->p, x.bn, y.bn, NULL))
        Napi::Error::New(env, "getX EC_POINT_new error").ThrowAsJavaScriptException();
    return Napi::Number::New(info.Env(), x.decimal());
}

Napi::Value NodePT::getY(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    BigNumber x;
    BigNumber y;
    NCurve* curve = Napi::ObjectWrap<NCurve>::Unwrap(info[0].As<Napi::Object>());
    if (!EC_POINT_get_affine_coordinates_GFp(curve->crv.curve, this->p, x.bn, y.bn, NULL))
        Napi::Error::New(env, "getY EC_POINT_new error").ThrowAsJavaScriptException();
    return Napi::Number::New(info.Env(), y.decimal());
}

Napi::Value NodePT::isInfinity(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    NCurve* curve = Napi::ObjectWrap<NCurve>::Unwrap(info[0].As<Napi::Object>());
    int ret = 0;
    if (EC_POINT_is_at_infinity(curve->crv.curve, this->p))
        ret = 1;
    return Napi::Number::New(info.Env(), ret);
}