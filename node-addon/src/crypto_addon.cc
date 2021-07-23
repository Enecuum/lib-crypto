#include "crypto_addon.h"



#define CA_LOG_ENABLE  /*abbreviation: CA is crypto addon  */
type_big_number_container crypto_addon::m_bigNumberAll;
uint                      crypto_addon::m_bigNumberLastID = 0;
type_curve_container      crypto_addon::m_curveAll;
uint                      crypto_addon::m_curveLastID = 0;
type_curve_fq_container   crypto_addon::m_curveFqAll;
uint                      crypto_addon::m_curveFqLastID = 0;
type_point_container      crypto_addon::m_pointAll;
uint                      crypto_addon::m_pointLastID = 0;
type_point_fq_container   crypto_addon::m_pointFqAll;
uint                      crypto_addon::m_pointFqLastID = 0;
BN_CTX*                   crypto_addon::m_ctx = nullptr;
bool                      crypto_addon::m_isLog = false;

crypto_addon::crypto_addon(const Napi::CallbackInfo& info) : Napi::ObjectWrap<crypto_addon>(info)
{
    m_isLog = false;
    if (info.Length() > 0) {
        if (info[0].As<Napi::Number>().Int32Value() > 0)
            m_isLog = true; 
    }
        
    printMess("crypto_addon::constructor");

    m_ctx = nullptr;
    clear();
    if (nullptr == (m_ctx = BN_CTX_new())) {
        handleErrorThrowJSexception(info.Env(), "constructor", NO_MEMORY);                
    }
}

crypto_addon::~crypto_addon() {
    printMess("crypto_addon::destructor");
    clear();
}


void crypto_addon::handleErrorThrowJSexception(Napi::Env env, const string& where, const uint errorCode) {
    string str = "\nIn crypto_addon::" + where + " - " + errorDescription(errorCode) + "\n";
    cout << str;
    Napi::Error::New(env, str).ThrowAsJavaScriptException();
}


void crypto_addon::printMess(const string& mess) {
    if (m_isLog)
        cout << mess << endl;
}






Napi::Object crypto_addon::init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "crypto_addon", {
                                        InstanceMethod("newBigNumber", &crypto_addon::newBigNumber),
                                        InstanceMethod("newCurve", &crypto_addon::newCurve),
                                        InstanceMethod("newCurveFq", &crypto_addon::newCurveFq),
                                        InstanceMethod("newPoint", &crypto_addon::newPoint),
                                        InstanceMethod("bigNumberToDec", &crypto_addon::bigNumberToDec),
                                        InstanceMethod("bigNumberToHex", &crypto_addon::bigNumberToHex),
                                        InstanceMethod("xy", &crypto_addon::xy),
                                        InstanceMethod("setPointToCurve", &crypto_addon::setPointToCurve),
                                        InstanceMethod("shamir", &crypto_addon::doShamir),                                        
                                        InstanceMethod("getQ", &crypto_addon::doGetQ),
                                        InstanceMethod("mul", &crypto_addon::doMul),
                                        InstanceMethod("keyRecovery", &crypto_addon::doKeyRecovery),
                                        InstanceMethod("signTate", &crypto_addon::doSignTate),    
                                        InstanceMethod("verifyTate", &crypto_addon::doVerifyTate),
                                        InstanceMethod("eraseBigNumber", &crypto_addon::doEraseBigNumber),
                                        InstanceMethod("eraseCurve", &crypto_addon::doEraseCurve),
                                        InstanceMethod("eraseCurveFq", &crypto_addon::doEraseCurveFq),
                                        InstanceMethod("erasePoint", &crypto_addon::doErasePoint),
                                        InstanceMethod("erasePointFq", &crypto_addon::doErasePointFq)
                                    });

    Napi::FunctionReference* constructor = nullptr;
    if (nullptr == (constructor = new Napi::FunctionReference())) {
        string where = "init";
        string str = "\ncrypto_addon::" + where + " " + "Pointer(-s) is NULL" + "\n";
        Napi::Error::New(env, str).ThrowAsJavaScriptException(); 
        return env.Null().ToObject();
    }

    *constructor = Napi::Persistent(func);
    exports.Set("crypto_addon", func);
    env.SetInstanceData(constructor);
    return exports;
}


Napi::Object crypto_addon::newInstance(const Napi::CallbackInfo& info) {    
    cout << "crypto_addon::newInstance" << endl;
    Napi::Env env = info.Env();
    Napi::EscapableHandleScope scope(env);
    Napi::Value constructorArgs = Napi::Number::New(env, 0);
    if (info.Length() > 0)
        constructorArgs = info[0];
    Napi::Object obj = env.GetInstanceData<Napi::FunctionReference>()->New({ constructorArgs });    
    return scope.Escape(napi_value(obj)).ToObject();
}


Napi::Value crypto_addon::newBigNumber(const Napi::CallbackInfo& info) {
    const string methodName = "newBigNumber";
    const uint needsAmountParams = 1;
    if (info.Length() != needsAmountParams) {
        handleErrorThrowJSexception(info.Env(), methodName, PARAMETER_INVALID);
        return info.Env().Null();
    }

    BigNumber* bn = nullptr;

    if (info[0].IsObject()) {
        Napi::Array a = info[0].As<Napi::Array>();
        vector<unsigned char> buf(a.Length());
        for (size_t i = 0; i < a.Length(); i++) {
            Napi::Value val = a[i];
            int ival = val.ToNumber();
            buf[i] = (unsigned char)ival;
        }

        bn = new BigNumber(&buf[0], a.Length());
    }
    else
        if (info[0].IsNumber()) {
            bn = new BigNumber(info[0].As<Napi::Number>().Int32Value());
        }
        else
            if (info[0].IsString()) {
                string str(info[0].As<Napi::String>().Utf8Value());
                bn = new BigNumber(str);
            }
            else {
                handleErrorThrowJSexception(info.Env(), methodName, PARAMETER_INVALID);
                return info.Env().Null();
            }

    if (bn == nullptr) {
        handleErrorThrowJSexception(info.Env(), methodName, NO_MEMORY);
        return info.Env().Null();
    }
    
    m_bigNumberLastID++;
    m_bigNumberAll[m_bigNumberLastID] = bn;
    
    printMess("crypto_addon::newBigNumber\t" + to_string(m_bigNumberLastID) + "\t" + toHex(bn) + "\t" + toHex(bn->bn));    
    return Napi::Number::New(info.Env(), m_bigNumberLastID);
}


Napi::Value crypto_addon::newCurve(const Napi::CallbackInfo& info) {
    const string methodName = "newCurve";
    const uint needsAmountParams = 6;
    if (info.Length() != needsAmountParams) {
        handleErrorThrowJSexception(info.Env(), methodName, PARAMETER_INVALID);
        return info.Env().Null();
    }

    BigNumber* a = reinterpret_cast<BigNumber*>(getElement(info.Env(), info[0].As<Napi::Number>().Int32Value(), BIG_NUM));
    BigNumber* b = reinterpret_cast<BigNumber*>(getElement(info.Env(), info[1].As<Napi::Number>().Int32Value(), BIG_NUM));
    BigNumber* p = reinterpret_cast<BigNumber*>(getElement(info.Env(), info[2].As<Napi::Number>().Int32Value(), BIG_NUM));
    BigNumber* order = reinterpret_cast<BigNumber*>(getElement(info.Env(), info[3].As<Napi::Number>().Int32Value(), BIG_NUM));
    BigNumber* gx = reinterpret_cast<BigNumber*>(getElement(info.Env(), info[4].As<Napi::Number>().Int32Value(), BIG_NUM));
    BigNumber* gy = reinterpret_cast<BigNumber*>(getElement(info.Env(), info[5].As<Napi::Number>().Int32Value(), BIG_NUM));

    Curve* crv = nullptr;
    if (nullptr == (crv = new Curve(*a, *b, *p, *order, *gx, *gy))) {        
        handleErrorThrowJSexception(info.Env(), methodName, POINTER_NULL);
        return info.Env().Null();
    }

    m_curveLastID++;
    m_curveAll[m_curveLastID] = crv;
    printMess("crypto_addon::newCurve\t" + to_string(m_curveLastID) + "\t" + toHex(crv));
    return Napi::Number::New(info.Env(), m_curveLastID);
}



Napi::Value	crypto_addon::newCurveFq(const Napi::CallbackInfo& info) {
    const string methodName = "newCurveFq";
    const uint needsAmountParams = 5;
    if (info.Length() != needsAmountParams) {
        handleErrorThrowJSexception(info.Env(), methodName, PARAMETER_INVALID);
        return info.Env().Null();
    }

    Integer Ip, Im;
    Ip = Integer(((string)info[0].As<Napi::String>()).data());
    Im = Integer(info[1].As<Napi::Number>().Int32Value());
    string strIrred(info[2].As<Napi::String>().Utf8Value());
    string strA(info[3].As<Napi::String>().Utf8Value());
    string strB(info[4].As<Napi::String>().Utf8Value());

    ellipticCurve* ec = nullptr;
    if (nullptr == (ec = new ellipticCurve(Ip, Im, strIrred, strA, strB))) {
        handleErrorThrowJSexception(info.Env(), methodName, POINTER_NULL);
        return info.Env().Null();
    }

    ellipticCurveFq* E_Fq = nullptr;
    if (nullptr == (E_Fq = new ellipticCurveFq(ec))) {
        handleErrorThrowJSexception(info.Env(), methodName, POINTER_NULL);
        return info.Env().Null();
    }
    
    m_curveFqLastID++;
    m_curveFqAll[m_curveFqLastID] = E_Fq;
    
    printMess("crypto_addon::newCurveFq\t" + to_string(m_curveFqLastID) + "\t" + toHex(E_Fq));
    
    return Napi::Number::New(info.Env(), m_curveFqLastID);
}


Napi::Value	crypto_addon::newPoint(const Napi::CallbackInfo& info) {
    const string methodName = "newPoint";
    const uint needsAmountParams = 3;
    if (info.Length() != needsAmountParams) {
        handleErrorThrowJSexception(info.Env(), methodName, PARAMETER_INVALID);
        return info.Env().Null();
    }

    uint xBigNumberID = info[0].As<Napi::Number>().Int32Value();
    uint yBigNumberID = info[1].As<Napi::Number>().Int32Value();
    uint curveID = info[2].As<Napi::Number>().Int32Value();

    BigNumber* x = reinterpret_cast<BigNumber*>(getElement(info.Env(), xBigNumberID, BIG_NUM));
    BigNumber* y = reinterpret_cast<BigNumber*>(getElement(info.Env(), yBigNumberID, BIG_NUM));
    Curve* curve1 = reinterpret_cast<Curve*>(getElement(info.Env(), curveID, CURVE));

    EC_POINT * res = nullptr;
    if (nullptr == (res = EC_POINT_new(curve1->curve))) {
        handleErrorThrowJSexception(info.Env(), methodName, POINTER_NULL);
        return info.Env().Null();
    }


    if (1 != EC_POINT_set_affine_coordinates_GFp(curve1->curve, res, x->bn, y->bn, m_ctx)) {
        EC_POINT_free(res);
        handleErrorThrowJSexception(info.Env(), methodName, CALC_FAILED);
        return info.Env().Null();
    }

    m_pointLastID++;
    m_pointAll[m_pointLastID] = res;
    
    printMess("crypto_addon::newPoint\t" + to_string(m_pointLastID) + "\t" + toHex(res));
    
    return Napi::Number::New(info.Env(), m_pointLastID);
}


void crypto_addon::clear() {
    type_big_number_container::iterator bigNumberIt;
    for (bigNumberIt = m_bigNumberAll.begin(); bigNumberIt != m_bigNumberAll.end(); ++bigNumberIt) {
        printMess("crypto_addon::delete newBigNumber\t" + to_string(bigNumberIt->first) + "\t" + toHex(bigNumberIt->second) + "\t" + toHex(bigNumberIt->second->bn));
        delete bigNumberIt->second;        
        bigNumberIt->second = nullptr;
    }
    m_bigNumberAll.clear();
    m_bigNumberLastID = NO_ELEMENT;

    type_curve_container::iterator curveIt;
    for (curveIt = m_curveAll.begin(); curveIt != m_curveAll.end(); ++curveIt) {
        printMess("crypto_addon::delete newCurve\t" + to_string(curveIt->first) + "\t" + toHex(curveIt->second) + "\t" + toHex(curveIt->second->curve));
        delete curveIt->second;                
        curveIt->second = nullptr;
    }
    m_curveAll.clear();
    m_curveLastID = NO_ELEMENT;


    type_curve_fq_container::iterator curveFqIt;
    for (curveFqIt = m_curveFqAll.begin(); curveFqIt != m_curveFqAll.end(); ++curveFqIt) {
        printMess("crypto_addon::delete newCurveFq\t" + to_string(curveFqIt->first) + "\t" + toHex(curveFqIt->second));
        delete curveFqIt->second;                
        curveFqIt->second = nullptr;
    }
    m_curveFqAll.clear();
    m_curveFqLastID = NO_ELEMENT;

    type_point_container::iterator pointIt;
    for (pointIt = m_pointAll.begin(); pointIt != m_pointAll.end(); ++pointIt) {
        printMess("crypto_addon::delete newPoint\t" + to_string(pointIt->first) + "\t" + toHex(pointIt->second));
        EC_POINT_free(pointIt->second);                
        pointIt->second = nullptr;
    }
    m_pointAll.clear();
    m_pointLastID = NO_ELEMENT;

    type_point_fq_container::iterator pointFqIt;
    for (pointFqIt = m_pointFqAll.begin(); pointFqIt != m_pointFqAll.end(); ++pointFqIt) {
        printMess("crypto_addon::delete newPointFq\t" + to_string(pointFqIt->first) + "\t" + toHex(pointFqIt->second));
        delete pointFqIt->second;                
        pointIt->second = nullptr;
    }
    m_pointAll.clear();
    m_pointLastID = NO_ELEMENT;

    if (m_ctx != nullptr) {
        printMess("crypto_addon::delete m_ctx\t" + toHex(m_ctx));
        BN_CTX_free(m_ctx);                
        m_ctx = nullptr;
    }    
}


Napi::Value crypto_addon::bigNumberToDec(const Napi::CallbackInfo& info) {
    const string methodName = "bigNumberToDeC";
    const uint needsAmountParams = 1;
    if (info.Length() != needsAmountParams) {
        handleErrorThrowJSexception(info.Env(), methodName, PARAMETER_INVALID);
        return info.Env().Null();
    }
    uint id = info[0].As<Napi::Number>().Int32Value();
    BigNumber* bn = (BigNumber*) getElement(info.Env(), id, BIG_NUM);
    return Napi::String::New(info.Env(), bn->toDecString());
}


Napi::Value crypto_addon::bigNumberToHex(const Napi::CallbackInfo& info) {
    const string methodName = "bigNumberToHex";
    const uint needsAmountParams = 1;
    if (info.Length() != needsAmountParams) {
        handleErrorThrowJSexception(info.Env(), methodName, PARAMETER_INVALID);
        return info.Env().Null();
    }
    uint id = info[0].As<Napi::Number>().Int32Value();    
    BigNumber* bn = (BigNumber*) getElement(info.Env(), id, BIG_NUM);     
    return Napi::String::New(info.Env(), bn->toHexString());
}


Napi::Value crypto_addon::xy(const Napi::CallbackInfo& info) {
    const string methodName = "xy";
    const uint needsAmountParams = 2;
    if (info.Length() != needsAmountParams) {
        handleErrorThrowJSexception(info.Env(), methodName, PARAMETER_INVALID);
        return info.Env().Null();
    }

    EC_POINT* p  = (EC_POINT*) getElement(info.Env(), info[0].As<Napi::Number>().Int32Value(), POINT);
    Curve* curve = (Curve*)    getElement(info.Env(), info[1].As<Napi::Number>().Int32Value(), CURVE);
    return Napi::String::New(info.Env(), printPoint(p, curve));
}



Napi::Value crypto_addon::setPointToCurve(const Napi::CallbackInfo & info) {
    const string methodName = "setPointToCurve";
    const uint needsAmountParams = 2;
    if (info.Length() != needsAmountParams) {
        handleErrorThrowJSexception(info.Env(), methodName, PARAMETER_INVALID);
        return info.Env().Null();
    }

    EC_POINT* p  = (EC_POINT*)getElement(info.Env(), info[0].As<Napi::Number>().Int32Value(), POINT);
    Curve* curve = (Curve*)   getElement(info.Env(), info[1].As<Napi::Number>().Int32Value(), CURVE);
    curve->setPointByCopy(p);

    return Napi::String::New(info.Env(), "success setPointToCurve");
}


uint crypto_addon::putNewElement(Napi::Env env, const void* src, const uint type, const void* addon) {
    const string methodName = "putNewElement";
   
    if (src == nullptr) {
        handleErrorThrowJSexception(env, methodName, POINTER_NULL);
        return NO_ELEMENT;
    }

    switch (type)
    {
    case BIG_NUM: {
        BigNumber* newElement = nullptr;
        if (nullptr == (newElement = new BigNumber(*((BigNumber*)src)))) {
            handleErrorThrowJSexception(env, methodName, POINTER_NULL);
            return NO_ELEMENT;
        }

        m_bigNumberLastID++;
        if (m_bigNumberLastID == MAX_LAST_ID) {
            handleErrorThrowJSexception(env, methodName, CONTAINER_OVERFLOW);
            return NO_ELEMENT;
        }
        m_bigNumberAll[m_bigNumberLastID] = newElement;
        return m_bigNumberLastID;
    }

    case CURVE: {
        Curve* newElement = nullptr;
        if (nullptr == (newElement = new Curve(*((Curve*)src)))) {
            handleErrorThrowJSexception(env, methodName, POINTER_NULL);
            return NO_ELEMENT;
        }

        m_curveLastID++;        
        if (m_curveLastID == MAX_LAST_ID) {
            handleErrorThrowJSexception(env, methodName, CONTAINER_OVERFLOW);
            return NO_ELEMENT;
        }
        m_curveAll[m_curveLastID] = newElement;
        return m_curveLastID;
    }

    case CURVE_FQ: {
        ellipticCurveFq* newElement = nullptr;
        if (nullptr == (newElement = new ellipticCurveFq(*((ellipticCurveFq*)src)))) {
            handleErrorThrowJSexception(env, methodName, POINTER_NULL);
            return NO_ELEMENT;
        }

        m_curveFqLastID++;
        if (m_curveFqLastID == MAX_LAST_ID) {
            handleErrorThrowJSexception(env, methodName, CONTAINER_OVERFLOW);
            return NO_ELEMENT;
        }
        m_curveFqAll[m_curveFqLastID] = newElement;
        return m_curveFqLastID;
    }

    case POINT: {
        Curve* srcCurve = (Curve*) addon;
        if ((nullptr == srcCurve)) {
            handleErrorThrowJSexception(env, methodName, POINTER_NULL);
            return NO_ELEMENT;
        }
        if (nullptr == srcCurve->curve) {
            handleErrorThrowJSexception(env, methodName, POINTER_NULL);
            return NO_ELEMENT;
        }

        EC_POINT* newElement = nullptr;
        if (nullptr == (newElement = EC_POINT_dup((EC_POINT*)src, srcCurve->curve))) {
            handleErrorThrowJSexception(env, methodName, POINTER_NULL);
            return NO_ELEMENT;
        }
        
        m_pointLastID++;
        if (m_pointLastID == MAX_LAST_ID) {
            handleErrorThrowJSexception(env, methodName, CONTAINER_OVERFLOW);
            return NO_ELEMENT;
        }
        m_pointAll[m_pointLastID] = newElement;
        return m_pointLastID;
    }

    case POINT_FQ: {
        ecPoint* newElement = nullptr;
        if (nullptr == (newElement = new ecPoint(*((ecPoint*)src)))) {
            handleErrorThrowJSexception(env, methodName, POINTER_NULL);
            return NO_ELEMENT;
        }

        m_pointFqLastID++;
        if (m_pointFqLastID == MAX_LAST_ID) {
            handleErrorThrowJSexception(env, methodName, CONTAINER_OVERFLOW);
            return NO_ELEMENT;
        }        
        m_pointFqAll[m_pointLastID] = newElement;
        return m_pointFqLastID;
    }

    default:
        handleErrorThrowJSexception(env, methodName, NOT_DEFINED_ACTION);
        return NO_ELEMENT;
    }
}


void* crypto_addon::getElement(Napi::Env env, const uint id, const uint type, const bool needsErase) {
    const string methodName = "getElement";

    switch (type)
    {
    case BIG_NUM:{
        if ((id == NO_ELEMENT) || (id > m_bigNumberLastID)) {
            handleErrorThrowJSexception(env, methodName, ID_INVALID);
            return nullptr;
        }
        type_big_number_container::iterator it = m_bigNumberAll.find(id);
        if (it == m_bigNumberAll.end()) {
            handleErrorThrowJSexception(env, methodName, NOT_FOUND_ELEMENT);
            return nullptr;
        }
        if (it->second == nullptr) {
            handleErrorThrowJSexception(env, methodName, POINTER_NULL);
            return nullptr;
        }
        if (it->second->bn == nullptr) {
            handleErrorThrowJSexception(env, methodName, POINTER_NULL);
            return nullptr;
        }
        void *p = it->second;
        if (needsErase)
            m_bigNumberAll.erase(it);
        return p;
    }        

    case CURVE: {
        if ((id == NO_ELEMENT) || (id > m_curveLastID)) {
            handleErrorThrowJSexception(env, methodName, ID_INVALID);
            return nullptr;
        }
        type_curve_container::iterator it = m_curveAll.find(id);
        if (it == m_curveAll.end()) {
            handleErrorThrowJSexception(env, methodName, NOT_FOUND_ELEMENT);
            return nullptr;
        }
        if (it->second == nullptr) {
            handleErrorThrowJSexception(env, methodName, POINTER_NULL);
            return nullptr;
        }
        if (it->second->curve == nullptr) {
            handleErrorThrowJSexception(env, methodName, POINTER_NULL);
            return nullptr;
        }
        void* p = it->second;
        if (needsErase)
            m_curveAll.erase(it);
        return p;
    }

    case CURVE_FQ: {
        if ((id == NO_ELEMENT) || (id > m_curveFqLastID)) {
            handleErrorThrowJSexception(env, methodName, ID_INVALID);
            return nullptr;
        }
        type_curve_fq_container::iterator it = m_curveFqAll.find(id);
        if (it == m_curveFqAll.end()) {
            handleErrorThrowJSexception(env, methodName, NOT_FOUND_ELEMENT);
            return nullptr;
        }
        if (it->second == nullptr) {
            handleErrorThrowJSexception(env, methodName, POINTER_NULL);
            return nullptr;
        }
        if (it->second->ec == nullptr) {
            handleErrorThrowJSexception(env, methodName, POINTER_NULL);
            return nullptr;
        }
        void* p = it->second;
        if (needsErase)
            m_curveFqAll.erase(it);
        return p;
    }

    case POINT: {
        if ((id == NO_ELEMENT) || (id > m_pointLastID)) {
            handleErrorThrowJSexception(env, methodName, ID_INVALID);
            return nullptr;
        }
        type_point_container::iterator it = m_pointAll.find(id);
        if (it == m_pointAll.end()) {
            handleErrorThrowJSexception(env, methodName, NOT_FOUND_ELEMENT);
            return nullptr;
        }
        if (it->second == nullptr) {
            handleErrorThrowJSexception(env, methodName, POINTER_NULL);
            return nullptr;
        }
        void* p = it->second;
        if (needsErase)
            m_pointAll.erase(it);
        return p;
    }

    case POINT_FQ: {
        if ((id == NO_ELEMENT) || (id > m_pointFqLastID)) {
            handleErrorThrowJSexception(env, methodName, ID_INVALID);
            return nullptr;
        }
        type_point_fq_container::iterator it = m_pointFqAll.find(id);
        if (it == m_pointFqAll.end()) {
            handleErrorThrowJSexception(env, methodName, NOT_FOUND_ELEMENT);
            return nullptr;
        }
        if (it->second == nullptr) {
            handleErrorThrowJSexception(env, methodName, POINTER_NULL);
            return nullptr;
        }
        void* p = it->second;
        if (needsErase)
            m_pointFqAll.erase(it);
        return p;
    }

    default:
        handleErrorThrowJSexception(env, methodName, NOT_DEFINED_ACTION);
        return nullptr;
    }
}




Napi::Value crypto_addon::doShamir(const Napi::CallbackInfo& info) {
    const string methodName = "shamir";
    const uint needsAmountParams = 5;
    if (info.Length() != needsAmountParams) {
        handleErrorThrowJSexception(info.Env(), methodName, PARAMETER_INVALID);
        return info.Env().Null();
    }

    BigNumber* secret = reinterpret_cast<BigNumber*>(getElement(info.Env(), info[0].As<Napi::Number>().Int32Value(), BIG_NUM));
    Napi::Array nids = info[1].As<Napi::Array>();
    int n = info[2].As<Napi::Number>().Int32Value();
    int k = info[3].As<Napi::Number>().Int32Value();
    BigNumber* q = reinterpret_cast<BigNumber*>(getElement(info.Env(), info[4].As<Napi::Number>().Int32Value(), BIG_NUM));
    vector<BigNumber> ids;
    for (size_t i = 0; i < nids.Length(); ++i) {
        uint id = nids.Get(i).As<Napi::Number>().Int32Value();
        BigNumber* tmp = reinterpret_cast<BigNumber*>(getElement(info.Env(), id, BIG_NUM));
        ids.push_back(*tmp);
    }

    vector<BigNumber> shares = shamir(*secret, ids, n, k, *q);

    Napi::Array res = Napi::Array::New(info.Env(), shares.size());
    for (size_t i = 0; i < shares.size(); ++i) {
        uint newID = putNewElement(info.Env(), &shares[i], BIG_NUM);
        res.Set(i, Napi::Number::New(info.Env(), newID));
    }
    return  res;
}


Napi::Value crypto_addon::doGetQ(const Napi::CallbackInfo& info) {
    const string methodName = "doGetQ";
    const uint needsAmountParams = 3;
    if (info.Length() != needsAmountParams) {
        handleErrorThrowJSexception(info.Env(), methodName, PARAMETER_INVALID);
        return info.Env().Null();
    }

    BigNumber* nhash = reinterpret_cast<BigNumber*>(getElement(info.Env(), info[0].As<Napi::Number>().Int32Value(), BIG_NUM));
    Curve* curve = reinterpret_cast<Curve*>(getElement(info.Env(), info[1].As<Napi::Number>().Int32Value(), CURVE));
    ellipticCurveFq* ecurve = reinterpret_cast<ellipticCurveFq*>(getElement(info.Env(), info[2].As<Napi::Number>().Int32Value(), CURVE_FQ));
    
    EC_POINT* Q = getQ(*nhash, curve, *ecurve);    
    uint id = putNewElement(info.Env(), Q, POINT, curve);
    return Napi::Number::New(info.Env(), id);
}


Napi::Value crypto_addon::doMul(const Napi::CallbackInfo& info) {
    const string methodName = "doGetQ";
    const uint needsAmountParams = 3;
    if (info.Length() != needsAmountParams) {
        handleErrorThrowJSexception(info.Env(), methodName, PARAMETER_INVALID);
        return info.Env().Null();
    }

    BigNumber* a = reinterpret_cast<BigNumber*>(getElement(info.Env(), info[0].As<Napi::Number>().Int32Value(), BIG_NUM));
    EC_POINT*  p = reinterpret_cast<EC_POINT*> (getElement(info.Env(), info[1].As<Napi::Number>().Int32Value(), POINT));
    Curve* curve = reinterpret_cast<Curve*>    (getElement(info.Env(), info[2].As<Napi::Number>().Int32Value(), CURVE));    

    EC_POINT* point = mul(*a, p, curve);
    uint pointID = putNewElement(info.Env(), point, POINT, curve);
    return Napi::Number::New(info.Env(), pointID);
}


Napi::Value crypto_addon::doKeyRecovery(const Napi::CallbackInfo& info) {
    const string methodName = "doGetQ";
    const uint needsAmountParams = 4;
    if (info.Length() != needsAmountParams) {
        handleErrorThrowJSexception(info.Env(), methodName, PARAMETER_INVALID);
        return info.Env().Null();
    }
    Napi::Array nproj = info[0].As<Napi::Array>();
    Napi::Array ncoal = info[1].As<Napi::Array>();
    BigNumber* q = (BigNumber*) getElement(info.Env(), info[2].As<Napi::Number>().Int32Value(), BIG_NUM);
    Curve* curve = (Curve*)     getElement(info.Env(), info[3].As<Napi::Number>().Int32Value(), CURVE);
    
    vector<int> coalition;
    for (size_t i = 0; i < ncoal.Length(); i++) {
        uint id = ncoal.Get(i).As<Napi::Number>().Int32Value();
        BigNumber* tmp = (BigNumber*) getElement(info.Env(), id, BIG_NUM);
        coalition.push_back((int) tmp->decimal());
    }

    vector<EC_POINT*> proj;
    for (size_t i = 0; i < nproj.Length(); i++) {
        uint id = nproj.Get(i).As<Napi::Number>().Int32Value();
        EC_POINT* pt = (EC_POINT*) getElement(info.Env(), id, POINT);        
        proj.push_back(pt);
    }
    EC_POINT* point = keyRecovery(proj, coalition, *q, curve);
    uint pointID = putNewElement(info.Env(), point, POINT, curve);
    return Napi::Number::New(info.Env(), pointID);
}



Napi::Value crypto_addon::doSignTate(const Napi::CallbackInfo& info) {
    const string methodName = "doSignTate";    
    const uint needsAmountParams = 5;
    if (info.Length() != needsAmountParams) {
        handleErrorThrowJSexception(info.Env(), methodName, PARAMETER_INVALID);
        return info.Env().Null();
    }

    BigNumber* h      = (BigNumber*) getElement(info.Env(), info[0].As<Napi::Number>().Int32Value(), BIG_NUM);
    EC_POINT*  secret = (EC_POINT*)  getElement(info.Env(), info[1].As<Napi::Number>().Int32Value(), POINT);
    Napi::Object G0_obj = info[2].As<Napi::Object>();
    Curve* curve        = (Curve*)   getElement(info.Env(), info[3].As<Napi::Number>().Int32Value(), CURVE);
    ellipticCurveFq* ecurve = (ellipticCurveFq*) getElement(info.Env(), info[4].As<Napi::Number>().Int32Value(), CURVE_FQ);

    Napi::String G0x = G0_obj.Get("x").As<Napi::String>();
    Napi::String G0y = G0_obj.Get("y").As<Napi::String>();

    ExtensionField::Element G0_x, G0_y;
    ecurve->field->readElement(G0x.Utf8Value(), G0_x);
    ecurve->field->readElement(G0y.Utf8Value(), G0_y);
    ecPoint G0_fq(G0_x, G0_y);

    BigNumber r2 = getRandom(curve->order);

    ecPoint S1_fq;
    ecurve->scalarMultiply(S1_fq, G0_fq, (Integer)(r2.toDecString()), -1);

    ecPoint secret_fq = mapToFq(secret, curve, *ecurve);
    ecPoint H_fq = hashToPoint(*h);

    ecPoint rH_Fq;
    ecurve->scalarMultiply(rH_Fq, H_fq, (Integer)(r2.toDecString()), -1);

    ecPoint S2_fq;
    ecurve->add(S2_fq, rH_Fq, secret_fq);

    Napi::Env env = info.Env();
    Napi::Object resR = Napi::Object::New(env);
    Napi::Object resS = Napi::Object::New(env);
    Napi::Object res = Napi::Object::New(env);

    resR.Set(Napi::String::New(env, "x"), polyToArr(S1_fq.x, env));
    resR.Set(Napi::String::New(env, "y"), polyToArr(S1_fq.y, env));

    resS.Set(Napi::String::New(env, "x"), polyToArr(S2_fq.x, env));
    resS.Set(Napi::String::New(env, "y"), polyToArr(S2_fq.y, env));

    res.Set(Napi::String::New(env, "r"), resR);
    res.Set(Napi::String::New(env, "s"), resS);
    return res;
}


Napi::Value crypto_addon::doVerifyTate(const Napi::CallbackInfo& info) {
    const string methodName = "doVerifyTate";
    const uint needsAmountParams = 7;
    if (info.Length() != needsAmountParams) {
        handleErrorThrowJSexception(info.Env(), methodName, PARAMETER_INVALID);
        return info.Env().Null();
    }

    Napi::Object sign = info[0].As<Napi::Object>();
    BigNumber* h = (BigNumber*)getElement(info.Env(), info[1].As<Napi::Number>().Int32Value(), BIG_NUM);
    EC_POINT* Q = (EC_POINT*)getElement(info.Env(), info[2].As<Napi::Number>().Int32Value(), POINT);
    Napi::Object G0_obj = info[3].As<Napi::Object>();
    Napi::Object MPK_obj = info[4].As<Napi::Object>();
    Curve* curve = (Curve*)getElement(info.Env(), info[5].As<Napi::Number>().Int32Value(), CURVE);
    ellipticCurveFq* ecurve = (ellipticCurveFq*)getElement(info.Env(), info[6].As<Napi::Number>().Int32Value(), CURVE_FQ);

    Napi::String G0x = G0_obj.Get("x").As<Napi::String>();
    Napi::String G0y = G0_obj.Get("y").As<Napi::String>();
    ExtensionField::Element G0_x, G0_y;
    ecurve->field->readElement(G0x.Utf8Value(), G0_x);
    ecurve->field->readElement(G0y.Utf8Value(), G0_y);
    ecPoint G0_fq(G0_x, G0_y);

    ExtensionField::Element MPK_x, MPK_y;
    Napi::String MPKx = MPK_obj.Get("x").As<Napi::String>();
    Napi::String MPKy = MPK_obj.Get("y").As<Napi::String>();
    ecurve->field->readElement(MPKx.Utf8Value(), MPK_x);
    ecurve->field->readElement(MPKy.Utf8Value(), MPK_y);
    ecPoint MPK_fq(MPK_x, MPK_y);

    Napi::Object s1 = sign.Get("r").As<Napi::Object>();
    Napi::Array s1x = s1.Get("x").As<Napi::Array>();
    Napi::Array s1y = s1.Get("y").As<Napi::Array>();
    ExtensionField::Element S1Fq_x = arrToPoly(s1x, *ecurve);
    ExtensionField::Element S1Fq_y = arrToPoly(s1y, *ecurve);
    ecPoint S1_fq(S1Fq_x, S1Fq_y);

    Napi::Object s2 = sign.Get("s").As<Napi::Object>();
    Napi::Array s2x = s2.Get("x").As<Napi::Array>();
    Napi::Array s2y = s2.Get("y").As<Napi::Array>();
    ExtensionField::Element S2Fq_x = arrToPoly(s2x, *ecurve);
    ExtensionField::Element S2Fq_y = arrToPoly(s2y, *ecurve);
    ecPoint S2_fq(S2Fq_x, S2Fq_y);


    BigNumber seed = getRandom(curve->order);
    ecPoint S_fq = hashToPointFq(S2_fq, seed, *ecurve);

    ecPoint Q_Fq = mapToFq(Q, curve, *ecurve);

    bool res = verifyTate(S1_fq, S2_fq, *h, MPK_fq, Q_Fq, G0_fq, *ecurve);
    return Napi::String::New(info.Env(), (res == true ? "1 (true)" : "0 (false)"));
}



Napi::Array crypto_addon::polyToArr(const ExtensionField::Element el, Napi::Env env) {
    Napi::Array res = Napi::Array::New(env, el.size());
    size_t len = el.size();
    for (size_t i = 0; i < len; i++)
        res[i] = el[len - i - 1];
    return res;
}


ExtensionField::Element crypto_addon::arrToPoly(const Napi::Array arr, ellipticCurveFq& E_Fq) {
    string str = to_string(arr.Length() - 1);
    for (size_t i = 0; i < arr.Length(); i++) {
        Napi::Value tmpx = arr[i];
        str.append(" ");
        str.append(tmpx.As<Napi::String>());
    }
    ExtensionField::Element res;
    E_Fq.field->readElement(str, res);
    return res;
}


Napi::Value crypto_addon::doEraseBigNumber(const Napi::CallbackInfo& info) {
    const string methodName = "doEraseBigNumber";
    const uint needsAmountParams = 1;

    if (info.Length() != needsAmountParams) {
        handleErrorThrowJSexception(info.Env(), methodName, PARAMETER_INVALID);
        return info.Env().Null();
    }
    
    Napi::Array arrayID = info[0].As<Napi::Array>();
    for (size_t i = 0; i < arrayID.Length(); ++i) {
        uint id = arrayID.Get(i).As<Napi::Number>().Int32Value();
        void* bigNum = getElement(info.Env(), id, BIG_NUM, true);
        secureFreeElement(info.Env(), bigNum, BIG_NUM);
    }

    return Napi::String::New(info.Env(), string("success ") + methodName);
}

Napi::Value crypto_addon::doEraseCurve(const Napi::CallbackInfo& info) {
    const string methodName = "doEraseCurve";
    const uint needsAmountParams = 1;

    if (info.Length() != needsAmountParams) {
        handleErrorThrowJSexception(info.Env(), methodName, PARAMETER_INVALID);
        return info.Env().Null();
    }

    Napi::Array arrayID = info[0].As<Napi::Array>();
    for (size_t i = 0; i < arrayID.Length(); ++i) {
        uint id = arrayID.Get(i).As<Napi::Number>().Int32Value();
        void* curve = getElement(info.Env(), id, CURVE, true);
        secureFreeElement(info.Env(), curve, CURVE);
    }

    return Napi::String::New(info.Env(), string("success ")+methodName);
}


Napi::Value crypto_addon::doEraseCurveFq(const Napi::CallbackInfo& info) {
    const string methodName = "doEraseCurveFq";
    const uint needsAmountParams = 1;

    if (info.Length() != needsAmountParams) {
        handleErrorThrowJSexception(info.Env(), methodName, PARAMETER_INVALID);
        return info.Env().Null();
    }

    Napi::Array arrayID = info[0].As<Napi::Array>();
    for (size_t i = 0; i < arrayID.Length(); ++i) {
        uint id = arrayID.Get(i).As<Napi::Number>().Int32Value();
        void* curveFq = getElement(info.Env(), id, CURVE_FQ, true);
        secureFreeElement(info.Env(), curveFq, CURVE_FQ);
    }

    return Napi::String::New(info.Env(), string("success ") + methodName);
}


Napi::Value crypto_addon::doErasePoint(const Napi::CallbackInfo& info) {
    const string methodName = "doErasePoint";
    const uint needsAmountParams = 1;

    if (info.Length() != needsAmountParams) {
        handleErrorThrowJSexception(info.Env(), methodName, PARAMETER_INVALID);
        return info.Env().Null();
    }

    Napi::Array arrayID = info[0].As<Napi::Array>();
    for (size_t i = 0; i < arrayID.Length(); ++i) {
        uint id = arrayID.Get(i).As<Napi::Number>().Int32Value();
        void* point = getElement(info.Env(), id, POINT, true);
        secureFreeElement(info.Env(), point, POINT);
    }

    return Napi::String::New(info.Env(), string("success ") + methodName);
}


Napi::Value crypto_addon::doErasePointFq(const Napi::CallbackInfo& info) {
    const string methodName = "doErasePointFq";
    const uint needsAmountParams = 1;

    if (info.Length() != needsAmountParams) {
        handleErrorThrowJSexception(info.Env(), methodName, PARAMETER_INVALID);
        return info.Env().Null();
    }

    Napi::Array arrayID = info[0].As<Napi::Array>();
    for (size_t i = 0; i < arrayID.Length(); ++i) {
        uint id = arrayID.Get(i).As<Napi::Number>().Int32Value();
        void* pointFq = getElement(info.Env(), id, POINT_FQ, true);
        secureFreeElement(info.Env(), pointFq, POINT_FQ);
    }

    return Napi::String::New(info.Env(), string("success ") + methodName);
}


void  crypto_addon::secureFreeElement(Napi::Env env, void* src, const uint type) {
    const string methodName = "secureFreeElement";
    if (src == nullptr) {
        handleErrorThrowJSexception(env, methodName, POINTER_NULL);
        return;
    }

    switch (type)
    {
    case BIG_NUM: {        
        BigNumber* b = (BigNumber*) src;
        printMess("crypto_addon::secureFreeElement BigNumber\t" + toHex(b) + "\t" + toHex(b->bn));
        BigNumber tmp(0xFFFFFF);
        BN_copy(b->bn, tmp.bn);        
        BN_clear_free(b->bn);       
        b->bn = nullptr;
        delete b;
        return;
    }

    case CURVE: {
        Curve* curve = (Curve*) src;
        printMess("crypto_addon::secureFreeElement Curve\t" + toHex(curve) + "\t" + toHex(curve->curve));
        Curve tmp;
        EC_GROUP_copy(curve->curve, tmp.curve);
        EC_POINT_copy(curve->G, tmp.G);
        BN_copy(curve->order.bn,  tmp.order.bn);
        BN_copy(curve->field.bn,  tmp.field.bn);
        BN_copy(curve->a.bn,      tmp.a.bn);
        BN_copy(curve->gx.bn,     tmp.gx.bn);
        BN_copy(curve->gy.bn,     tmp.gy.bn);
        EC_GROUP_clear_free(curve->curve);
        curve->curve = nullptr;
        delete curve;
        return;
    }

    case CURVE_FQ: {                
        ellipticCurveFq* curveFq = (ellipticCurveFq*) src;
        printMess("crypto_addon::secureFreeElement CurveFq\t" + toHex(curveFq));
        ellipticCurveFq tmp;
        *curveFq->ec->Kptr = *tmp.ec->Kptr;
        *curveFq->ec = *tmp.ec;
        *curveFq->field = *tmp.field;
        curveFq->d = tmp.d;
        curveFq->identity = tmp.identity;
        delete curveFq;
        return;
    }

    case POINT: {
        EC_POINT* p = (EC_POINT*) src;
        printMess("crypto_addon::secureFreeElement Point\t" + toHex(p));
        Curve tmp;
        EC_POINT_copy(p, tmp.G);
        EC_POINT_clear_free(p);        
        return;
    }

    case POINT_FQ: {
        ecPoint* p = (ecPoint*) src;
        printMess("crypto_addon::secureFreeElement PointFq\t" + toHex(p));
        ecPoint tmp;
        *p = tmp;
        delete p;
        return;
    }

    default:
        handleErrorThrowJSexception(env, methodName, NOT_DEFINED_ACTION);
        return;
    } 
   
}













