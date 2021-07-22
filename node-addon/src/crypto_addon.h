#ifndef __CRYPTO_ADDON_H__
#define __CRYPTO_ADDON_H__

#include <napi.h>
#include "../../inc/crypto.h"
#include <stdio.h>
#include <map>
#include <vector>
#include <openssl/bn.h>
#include <openssl/ec.h>



typedef std::map<uint, BigNumber*>       type_big_number_container;
typedef std::map<uint, Curve*>		     type_curve_container;
typedef std::map<uint, ellipticCurveFq*> type_curve_fq_container;
typedef std::map<uint, EC_POINT*>		 type_point_container;
typedef std::map<uint, ecPoint*>         type_point_fq_container;



using namespace std;

enum element_type {
    BIG_NUM = 0,
    CURVE,
    CURVE_FQ,
    POINT,
    POINT_FQ
};

const  uint  MAX_LAST_ID = 0xFFFFFF;

class crypto_addon: public Napi::ObjectWrap<crypto_addon> {
public:
    crypto_addon(const Napi::CallbackInfo& info);
    ~crypto_addon();

    static Napi::Object init(Napi::Env env, Napi::Object exports);
    static Napi::Object newInstance(const Napi::CallbackInfo& info);

    Napi::Value newBigNumber(const Napi::CallbackInfo& info);
    Napi::Value newCurve(const Napi::CallbackInfo& info);
    Napi::Value newCurveFq(const Napi::CallbackInfo& info);
    Napi::Value newPoint(const Napi::CallbackInfo& info);
    Napi::Value bigNumberToDec(const Napi::CallbackInfo& info);
    Napi::Value bigNumberToHex(const Napi::CallbackInfo& info);
    Napi::Value xy(const Napi::CallbackInfo& info);
    Napi::Value setPointToCurve(const Napi::CallbackInfo& info);
    Napi::Value doShamir(const Napi::CallbackInfo& info);
    Napi::Value doGetQ(const Napi::CallbackInfo& info);
    Napi::Value doMul(const Napi::CallbackInfo& info);
    Napi::Value doKeyRecovery(const Napi::CallbackInfo& info);
    Napi::Value doSignTate(const Napi::CallbackInfo& info);
    Napi::Value doVerifyTate(const Napi::CallbackInfo& info);
    Napi::Value doEraseBigNumber(const Napi::CallbackInfo& info);    
    Napi::Value doEraseCurve(const Napi::CallbackInfo& info);
    Napi::Value doEraseCurveFq(const Napi::CallbackInfo& info);
    Napi::Value doErasePoint(const Napi::CallbackInfo& info);
    Napi::Value doErasePointFq(const Napi::CallbackInfo& info);


private:
    static type_big_number_container m_bigNumberAll;
    static uint m_bigNumberLastID;
    static type_curve_container m_curveAll;
    static uint m_curveLastID;
    static type_curve_fq_container m_curveFqAll;
    static uint m_curveFqLastID;
    static type_point_container m_pointAll;
    static uint m_pointLastID;
    static type_point_fq_container  m_pointFqAll;
    static uint m_pointFqLastID;
    static BN_CTX* m_ctx;
    static bool m_isLog;

    void  clear();
    void  handleErrorThrowJSexception(Napi::Env env, const string& where, const uint errorCode);
   
    
    void* getElement(Napi::Env env, const uint id, const uint type, bool needsErase = false);
    uint  putNewElement(Napi::Env env, const void* src, const uint type, const void* extra = nullptr);
    void  secureFreeElement(Napi::Env env, void* src, const uint type);

    Napi::Array polyToArr(const ExtensionField::Element el, Napi::Env env);
    ExtensionField::Element arrToPoly(const Napi::Array arr, ellipticCurveFq& E_Fq);

    void printMess(const string& mess);
};

#endif