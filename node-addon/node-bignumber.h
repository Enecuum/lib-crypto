#pragma once

#include <napi.h>
#include "ecc/crypto/crypto.h"

class NodeBN : public Napi::ObjectWrap<NodeBN> {
	public:
		static void Init(Napi::Env env, Napi::Object exports);
		static Napi::Object NewInstance(Napi::Value arg);
		NodeBN(const Napi::CallbackInfo& info);
		BigNumber bn;
	private:
		static Napi::FunctionReference constructor;
		Napi::Value GetValue(const Napi::CallbackInfo& info);
		void setVal(BigNumber a);
};

class NCurve : public Napi::ObjectWrap<NCurve> {
	public:
		static void Init(Napi::Env env, Napi::Object exports);
		static Napi::Object NewInstance(Napi::Value arg);
		NCurve(const Napi::CallbackInfo& info);
		EC_GROUP* crv;
		double d;
	private:
		static Napi::FunctionReference constructor;
		Napi::Value GetValue(const Napi::CallbackInfo& info);
};

class NodePT : public Napi::ObjectWrap<NodePT> {
	public:
		static void Init(Napi::Env env, Napi::Object exports);
		static Napi::Object NewInstance(Napi::Value arg);
		NodePT(const Napi::CallbackInfo& info);
		EC_POINT* p;
	private:
		static Napi::FunctionReference constructor;
		Napi::Value GetCoords(const Napi::CallbackInfo& info);
		Napi::Value SetCoords(const Napi::CallbackInfo& info);
};
