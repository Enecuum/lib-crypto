#include <napi.h>
#include <sstream>
#include "crypto_addon.h"


Napi::Object getCryptoAddon(const Napi::CallbackInfo& info) {
	return crypto_addon::newInstance(info);
}



Napi::Object init(Napi::Env env, Napi::Object exports) {
	crypto_addon::init(env, exports);
	exports.Set(Napi::String::New(env, "getCryptoAddon"), Napi::Function::New(env, getCryptoAddon));
	return exports;
}

NODE_API_MODULE(addon, init)