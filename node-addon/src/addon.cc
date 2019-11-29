#include <napi.h>
#include "node-bignumber.h"
#include <sstream>
//using namespace Napi;

Napi::Object BNumber(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	//BigNumber bn;
	if(info[0].IsObject()){
		Napi::Array a = info[0].As<Napi::Array>();
		//Napi::Error::New(env, "qweqweqwe").ThrowAsJavaScriptException();
		//std::vector<unsigned char> buf(a.Length());
		std::vector<unsigned char> buf(a.Length());
		for(int i  = 0; i < a.Length(); i++){
			Napi::Value val = a[i];
			int ival = val.ToNumber();
			buf[i] = (unsigned char)ival;
		}

		BigNumber bn(&buf[0], a.Length());
		return NodeBN::NewInstance(Napi::External<BigNumber>::New(info.Env(), &bn));
	}
	if (info[0].IsNumber()){
		BigNumber bn(info[0].As<Napi::Number>().Int32Value());
		return NodeBN::NewInstance(Napi::External<BigNumber>::New(info.Env(), &bn));
	}
	if(info[0].IsString()){
		Napi::String a = info[0].As<Napi::String>();
		std::string str(a.Utf8Value());

		BigNumber bn(str);
		return NodeBN::NewInstance(Napi::External<BigNumber>::New(info.Env(), &bn));
		//Napi::Error::New(env, std::to_string("String")).ThrowAsJavaScriptException();
	}
	else{
		Napi::Error::New(env, std::to_string(info[0].Type())).ThrowAsJavaScriptException();
	}

	return Napi::Object::New(info.Env());
	
}

std::string addSpaces(std::string str) {
	if ((str.size() % 2) != 0) {
		str.insert(0, "0");
	}
	for (int i = 2; i < str.size(); i += 3) {
		str.insert(i, " ");
	}
	return str;
}

Napi::Object Point(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	NodeBN* x = Napi::ObjectWrap<NodeBN>::Unwrap(info[0].As<Napi::Object>());
	NodeBN* y = Napi::ObjectWrap<NodeBN>::Unwrap(info[1].As<Napi::Object>());
	NCurve* curve1 = Napi::ObjectWrap<NCurve>::Unwrap(info[2].As<Napi::Object>());
	EC_POINT *res;
	if (NULL == (res = EC_POINT_new(curve1->crv.curve)))
		Napi::Error::New(env, "EC_POINT_new error").ThrowAsJavaScriptException();
	if (1 != EC_POINT_set_affine_coordinates_GFp(curve1->crv.curve, res, x->bn.bn, y->bn.bn, NULL))
		Napi::Error::New(env, "EC_POINT_set_affine_coordinates_GFp error").ThrowAsJavaScriptException();
	return NodePT::NewInstance(Napi::External<EC_POINT*>::New(info.Env(), &res));
}

Napi::Object ECurve(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    NodeBN* a = Napi::ObjectWrap<NodeBN>::Unwrap(info[0].As<Napi::Object>());
    NodeBN* b = Napi::ObjectWrap<NodeBN>::Unwrap(info[1].As<Napi::Object>());
    NodeBN* p = Napi::ObjectWrap<NodeBN>::Unwrap(info[2].As<Napi::Object>());
    NodeBN* order = Napi::ObjectWrap<NodeBN>::Unwrap(info[3].As<Napi::Object>());
    NodeBN* gx = Napi::ObjectWrap<NodeBN>::Unwrap(info[4].As<Napi::Object>());
    NodeBN* gy = Napi::ObjectWrap<NodeBN>::Unwrap(info[5].As<Napi::Object>());
    
    Curve cur(a->bn, b->bn, p->bn, order->bn, gx->bn, gy->bn);
    
    Napi::Object obj = NCurve::NewInstance(Napi::External<Curve>::New(info.Env(), &cur));
    obj.Set(Napi::String::New(env, "msg"), "hello");
	return obj;
}

Napi::Object ECurve_Fq(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    Integer Ip, Im;
	Ip = Integer(((std::string)info[0].As<Napi::String>()).data());
	Im = Integer(info[1].As<Napi::Number>().Int32Value());
	std::string strIrred(info[2].As<Napi::String>().Utf8Value());
	std::string strA(info[3].As<Napi::String>().Utf8Value());
	std::string strB(info[4].As<Napi::String>().Utf8Value());

	ellipticCurve *ec = new ellipticCurve(Ip, Im, strIrred, strA, strB);
	ellipticCurveFq *E_Fq = new ellipticCurveFq(ec);
    
    Napi::Object obj = NCurve_Fq::NewInstance(Napi::External<ellipticCurveFq>::New(info.Env(), E_Fq));
    obj.Set(Napi::String::New(env, "msg"), "hello_elliptic");
	return obj;
}

Napi::Object Point_Fq(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	NodeBN* x = Napi::ObjectWrap<NodeBN>::Unwrap(info[0].As<Napi::Object>());
	NodeBN* y = Napi::ObjectWrap<NodeBN>::Unwrap(info[1].As<Napi::Object>());
	NCurve* curve1 = Napi::ObjectWrap<NCurve>::Unwrap(info[2].As<Napi::Object>());
	EC_POINT *res;
	if (NULL == (res = EC_POINT_new(curve1->crv.curve)))
		Napi::Error::New(env, "EC_POINT_new error").ThrowAsJavaScriptException();
	if (1 != EC_POINT_set_affine_coordinates_GFp(curve1->crv.curve, res, x->bn.bn, y->bn.bn, NULL))
		Napi::Error::New(env, "EC_POINT_set_affine_coordinates_GFp error").ThrowAsJavaScriptException();
	return NodePT::NewInstance(Napi::External<EC_POINT*>::New(info.Env(), &res));
}

Napi::Object SignTate(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    NodeBN* h = Napi::ObjectWrap<NodeBN>::Unwrap(info[0].As<Napi::Object>());
	NodePT* secret = Napi::ObjectWrap<NodePT>::Unwrap(info[1].As<Napi::Object>());
	NodePT* G = Napi::ObjectWrap<NodePT>::Unwrap(info[2].As<Napi::Object>());
	NCurve* curve = Napi::ObjectWrap<NCurve>::Unwrap(info[3].As<Napi::Object>());
    NCurve_Fq* ecurve = Napi::ObjectWrap<NCurve_Fq>::Unwrap(info[4].As<Napi::Object>());

	BigNumber r2 = getRandom(curve->crv.order);
	//std::cout << "r2: " << r2.toDecString() << std::endl;

	EC_POINT *s1 = mul(r2.bn, G->p, &curve->crv);

	// S2 = r*H + SecKey

	ecPoint rH_Fq;	
	
	ecPoint secret_fq = mapToFq(secret->p, &curve->crv, *ecurve->E_Fq);
	
	ecPoint H_fq = hashToPointFq(secret_fq, h->bn, *ecurve->E_Fq);
	//std::cout << "\n H_fq: " << std::endl;
	//ecurve->E_Fq->show(H_fq);
	ecurve->E_Fq->scalarMultiply(rH_Fq, H_fq, (Integer)(r2.toDecString()), -1);//R=6*P, order of P is not required

	ecPoint S2_fq;
	ecurve->E_Fq->add(S2_fq, rH_Fq, secret_fq);//R=P+Q
	//let s2 = addon.addPoints(rH, secret, curve);

	//ecurve->E_Fq->show(S2_fq);
	std::stringstream xx, yy;
	ecurve->E_Fq->field->writeElement(S2_fq.x, xx);
	ecurve->E_Fq->field->writeElement(S2_fq.y, yy);
	//std::cout << xx.str() << std::endl;
	std::vector<std::string> elementsX, elementsY;
	std::string temp;
	//int degree = S2_fq.x.size();
	for(int i = 0; i < S2_fq.x.size(); i++)
		elementsX.push_back(S2_fq.x[i]);
	for(int i = 0; i < S2_fq.y.size(); i++)
		elementsY.push_back(S2_fq.y[i]);

	BigNumber s1_x;
	BigNumber s1_y;
	if (!EC_POINT_get_affine_coordinates_GFp(curve->crv.curve, s1, s1_x.bn, s1_y.bn, NULL)) handleErrors();
	Napi::Object resR = Napi::Object::New(env);

    resR.Set(Napi::String::New(env, "x"), s1_x.toDecString());
    resR.Set(Napi::String::New(env, "y"), s1_y.toDecString());

    Napi::Object resS = Napi::Object::New(env);
    Napi::Array arrX = Napi::Array::New(env, elementsX.size());
    for(int i = 0; i < elementsX.size(); i++){
    	arrX[i] = elementsX[elementsX.size() - i - 1];
    }
    Napi::Array arrY = Napi::Array::New(env, elementsX.size());
    for(int i = 0; i < elementsY.size(); i++){
    	arrY[i] = elementsY[elementsX.size() - i - 1];
    }
    
    resS.Set(Napi::String::New(env, "x"), arrX);
    resS.Set(Napi::String::New(env, "y"), arrY);

    Napi::Object res = Napi::Object::New(env);
    res.Set(Napi::String::New(env, "r"), resR);
    res.Set(Napi::String::New(env, "s"), resS);
	return res;
}

Napi::Object MapToFq(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	NodePT* P = Napi::ObjectWrap<NodePT>::Unwrap(info[0].As<Napi::Object>());
	NCurve* curve = Napi::ObjectWrap<NCurve>::Unwrap(info[1].As<Napi::Object>());
	NCurve_Fq* ecurve = Napi::ObjectWrap<NCurve_Fq>::Unwrap(info[2].As<Napi::Object>());
	
	ecPoint res = mapToFq(P->p, &curve->crv, *ecurve->E_Fq);
	//BigNumber res = a->bn + b->bn;
	return NodePT_Fq::NewInstance(Napi::External<ecPoint>::New(info.Env(), &res));
}

Napi::Object Add(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	NodeBN* a = Napi::ObjectWrap<NodeBN>::Unwrap(info[0].As<Napi::Object>());
	NodeBN* b = Napi::ObjectWrap<NodeBN>::Unwrap(info[1].As<Napi::Object>());

	BigNumber res = a->bn + b->bn;
	return NodeBN::NewInstance(Napi::External<BigNumber>::New(info.Env(), &res));
}

Napi::Object Mmul(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	NodeBN* a = Napi::ObjectWrap<NodeBN>::Unwrap(info[0].As<Napi::Object>());
	NodeBN* b = Napi::ObjectWrap<NodeBN>::Unwrap(info[1].As<Napi::Object>());
	NodeBN* m = Napi::ObjectWrap<NodeBN>::Unwrap(info[2].As<Napi::Object>());

	BigNumber res = (a->bn * b->bn) % m->bn;
	return NodeBN::NewInstance(Napi::External<BigNumber>::New(info.Env(), &res));
}

Napi::Object AddPoints(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	NodePT* a = Napi::ObjectWrap<NodePT>::Unwrap(info[0].As<Napi::Object>());
	NodePT* b = Napi::ObjectWrap<NodePT>::Unwrap(info[1].As<Napi::Object>());
	NCurve* curve = Napi::ObjectWrap<NCurve>::Unwrap(info[2].As<Napi::Object>());

   	EC_POINT *res = EC_POINT_new(curve->crv.curve);
	if (1 != EC_POINT_add(curve->crv.curve, res, a->p, b->p, NULL))
		Napi::Error::New(env, "EC_POINT_add error").ThrowAsJavaScriptException();
  	return NodePT::NewInstance(Napi::External<EC_POINT*>::New(info.Env(), &res));
}

Napi::Object Mul(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	NodeBN* a = Napi::ObjectWrap<NodeBN>::Unwrap(info[0].As<Napi::Object>());
	NodePT* P = Napi::ObjectWrap<NodePT>::Unwrap(info[1].As<Napi::Object>());
	NCurve* curve = Napi::ObjectWrap<NCurve>::Unwrap(info[2].As<Napi::Object>());

	EC_POINT *res = mul(a->bn, P->p, &curve->crv);
	return NodePT::NewInstance(Napi::External<EC_POINT*>::New(info.Env(), &res));
}

Napi::Object HashToPoint(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	NodeBN* a = Napi::ObjectWrap<NodeBN>::Unwrap(info[0].As<Napi::Object>());
	NCurve* curve = Napi::ObjectWrap<NCurve>::Unwrap(info[1].As<Napi::Object>());

	EC_POINT *res = mul(a->bn, curve->crv.G, &curve->crv);
	return NodePT::NewInstance(Napi::External<EC_POINT*>::New(info.Env(), &res));
}

Napi::Object CreateMPK(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	NodeBN* a = Napi::ObjectWrap<NodeBN>::Unwrap(info[0].As<Napi::Object>());
	NodePT* P = Napi::ObjectWrap<NodePT>::Unwrap(info[1].As<Napi::Object>());
	NCurve* curve = Napi::ObjectWrap<NCurve>::Unwrap(info[2].As<Napi::Object>());
	EC_POINT *res;
	try{
		res = createMPK(a->bn, P->p, &curve->crv);
	}
	catch(unsigned long err){
        Napi::Error::New(env, ERR_error_string(err, NULL)).ThrowAsJavaScriptException();
    }
  	return NodePT::NewInstance(Napi::External<EC_POINT*>::New(info.Env(), &res));
}

Napi::Array Shamir(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	NodeBN* secret = Napi::ObjectWrap<NodeBN>::Unwrap(info[0].As<Napi::Object>());
	Napi::Array nids = info[1].As<Napi::Array>();
	int n = info[2].As<Napi::Number>().Int32Value();
	int k = info[3].As<Napi::Number>().Int32Value();
	NodeBN* q = Napi::ObjectWrap<NodeBN>::Unwrap(info[4].As<Napi::Object>());
	
	std::vector<int> ids;
	for(uint32_t i  = 0; i < nids.Length(); i++){
		ids.push_back(nids.Get(i).As<Napi::Number>().Int32Value());
	}
	std::vector<BigNumber> shares = shamir(secret->bn, ids, n, k, q->bn);

	Napi::Array res = Napi::Array::New(env, shares.size());
	for(uint32_t i  = 0; i < shares.size(); i++){
		res.Set(i, NodeBN::NewInstance(Napi::External<BigNumber>::New(info.Env(), &shares[i])));
	}
  	return res;
}

Napi::Array KeyProj(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();

	Napi::Array ncoal = info[0].As<Napi::Array>();
	Napi::Array nshares = info[1].As<Napi::Array>();
	NodePT* P = Napi::ObjectWrap<NodePT>::Unwrap(info[2].As<Napi::Object>());
	NCurve* curve = Napi::ObjectWrap<NCurve>::Unwrap(info[3].As<Napi::Object>());
	
	std::vector<int> coalition;
	for(uint32_t i  = 0; i < ncoal.Length(); i++){
		coalition.push_back(ncoal.Get(i).As<Napi::Number>().Int32Value());
	}
	std::vector<BigNumber> shares;
	for(uint32_t i  = 0; i < nshares.Length(); i++){
		NodeBN* number = Napi::ObjectWrap<NodeBN>::Unwrap(nshares.Get(i).As<Napi::Object>());
		shares.push_back(number->bn);
	}

	std::vector<EC_POINT*> proj = keyProj(coalition, shares, P->p, &curve->crv);
	
	Napi::Array res = Napi::Array::New(env, proj.size());
	for(uint32_t i  = 0; i < proj.size(); i++){
		EC_POINT* buf = proj[i];
		res.Set(i, NodePT::NewInstance(Napi::External<EC_POINT*>::New(info.Env(), &buf)));
	}
  	return res;
}

//keyRecovery(proj, coalition, q, curve);
Napi::Object KeyRecovery(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();

	Napi::Array nproj = info[0].As<Napi::Array>();
	Napi::Array ncoal = info[1].As<Napi::Array>();
	NodeBN* q = Napi::ObjectWrap<NodeBN>::Unwrap(info[2].As<Napi::Object>());
	NCurve* curve = Napi::ObjectWrap<NCurve>::Unwrap(info[3].As<Napi::Object>());
	
	std::vector<int> coalition;
	for(uint32_t i  = 0; i < ncoal.Length(); i++){
		coalition.push_back(ncoal.Get(i).As<Napi::Number>().Int32Value());
	}
	std::vector<EC_POINT*> proj;
	for(uint32_t i  = 0; i < nproj.Length(); i++){
		NodePT* pt = Napi::ObjectWrap<NodePT>::Unwrap(nproj.Get(i).As<Napi::Object>());
		proj.push_back(pt->p);
	}
	EC_POINT* res = keyRecovery(proj, coalition, q->bn, &curve->crv);

  	return NodePT::NewInstance(Napi::External<EC_POINT*>::New(info.Env(), &res));
}

Napi::Object WeilPairing(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	NodePT* P = Napi::ObjectWrap<NodePT>::Unwrap(info[0].As<Napi::Object>());
	NodePT* Q = Napi::ObjectWrap<NodePT>::Unwrap(info[1].As<Napi::Object>());
	NodePT* S = Napi::ObjectWrap<NodePT>::Unwrap(info[2].As<Napi::Object>());
	NCurve* curve = Napi::ObjectWrap<NCurve>::Unwrap(info[3].As<Napi::Object>());
	
	BigNumber res = weilPairing(P->p, Q->p, S->p, &curve->crv);
  	return NodeBN::NewInstance(Napi::External<BigNumber>::New(info.Env(), &res));
}

Napi::Number VerifyTate(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	
	Napi::Object sign = info[0].As<Napi::Object>();
	NodePT* s1 = Napi::ObjectWrap<NodePT>::Unwrap(info[1].As<Napi::Object>());
	NodeBN* h = Napi::ObjectWrap<NodeBN>::Unwrap(info[2].As<Napi::Object>());
	NodePT* Q = Napi::ObjectWrap<NodePT>::Unwrap(info[3].As<Napi::Object>());
	NodePT* G0 = Napi::ObjectWrap<NodePT>::Unwrap(info[4].As<Napi::Object>());
	NodePT* MPK = Napi::ObjectWrap<NodePT>::Unwrap(info[5].As<Napi::Object>());
	
	NCurve* curve = Napi::ObjectWrap<NCurve>::Unwrap(info[6].As<Napi::Object>());
    NCurve_Fq* ecurve = Napi::ObjectWrap<NCurve_Fq>::Unwrap(info[7].As<Napi::Object>());
	
	Napi::Object s2 = sign.Get("s").As<Napi::Object>();
	Napi::Array s2x = s2.Get("x").As<Napi::Array>();
	Napi::Array s2y = s2.Get("y").As<Napi::Array>();
	//
	// -------- TODO: rewrite hardcode
	//
	std::string strS2x("1"), strS2y("1");
	for(int i = 0; i < s2x.Length(); i++){
		Napi::Value tmpx = s2x[i];
		Napi::Value tmpy = s2y[i];
		strS2x.append(" ");
		strS2x.append(tmpx.As<Napi::String>());
		strS2y.append(" ");
		strS2y.append(tmpy.As<Napi::String>());
	}
	ExtensionField::Element S2Fq_x, S2Fq_y;
	ecurve->E_Fq->field->readElement(strS2x, S2Fq_x);
	ecurve->E_Fq->field->readElement(strS2y, S2Fq_y);
	ecPoint S2_fq(S2Fq_x, S2Fq_y);

	BigNumber seed = getRandom(curve->crv.order);
	//std::cout << "seed: " << seed.toDecString() << std::endl;
	ecPoint S_fq = hashToPointFq(S2_fq, seed, *ecurve->E_Fq);

	ecPoint G0_fq = mapToFq(G0->p, &curve->crv, *ecurve->E_Fq);

	ecPoint MPK_fq = mapToFq(MPK->p, &curve->crv, *ecurve->E_Fq);
	ecPoint Q_Fq = mapToFq(Q->p, &curve->crv, *ecurve->E_Fq);

	ecPoint H_fq = hashToPointFq(S2_fq, h->bn, *ecurve->E_Fq);
	//ecPoint H_fq = mapToFq(H->p, &curve->crv, *ecurve->E_Fq);
	ecPoint S1_fq = mapToFq(s1->p, &curve->crv, *ecurve->E_Fq);

	ExtensionField::Element rr = tatePairing(G0_fq, S2_fq, S_fq, *ecurve->E_Fq);

	ExtensionField::Element bb = tatePairing(Q_Fq, MPK_fq, S_fq, *ecurve->E_Fq);

	ExtensionField::Element cc = tatePairing(S1_fq, H_fq, S_fq, *ecurve->E_Fq);

	std::string strEta = "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001000000000000000010100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000100000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000010";

	ExtensionField::Element r1, b1, c1;
	ecurve->E_Fq->field->pow(r1, rr, strEta);
	ecurve->E_Fq->field->pow(b1, bb, strEta);
	ecurve->E_Fq->field->pow(c1, cc, strEta);

	ExtensionField::Element b1c1;
	ecurve->E_Fq->field->mul(b1c1, b1, c1);
	
	//std::cout << "\n S_fq: " << std::endl;
	//ecurve->E_Fq->show(S_fq);
	//std::cout << "\n H_fq: " << std::endl;
	//ecurve->E_Fq->show(H_fq);

	//std::cout << "\n r1: " << std::endl;
	//ecurve->E_Fq->field->writeElement(r1);
	//std::cout << "\n b1: " << std::endl;
	//ecurve->E_Fq->field->writeElement(b1);
	//std::cout << "\n c1: " << std::endl;
	//ecurve->E_Fq->field->writeElement(c1);

	//std::cout << "\n b1c1: " << std::endl;
	//ecurve->E_Fq->field->writeElement(b1c1);

	bool res = ecurve->E_Fq->field->areEqual(r1, b1c1);
	//BigNumber res(1);
	//std::cout << "\n res: " << res << std::endl;

  	return Napi::Number::New(info.Env(), res);
}

Napi::Object GetRandom(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  NodeBN* obj1 = Napi::ObjectWrap<NodeBN>::Unwrap(info[0].As<Napi::Object>());
  BigNumber bn = getRandom(obj1->bn);

  return NodeBN::NewInstance(Napi::External<BigNumber>::New(info.Env(), &bn));
}

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
	NodeBN::Init(env, exports);
	NCurve::Init(env, exports);
	NodePT::Init(env, exports);
	NodePT_Fq::Init(env, exports);
	NCurve_Fq::Init(env, exports);
	exports.Set(Napi::String::New(env, "BigNumber"),
		Napi::Function::New(env, BNumber));
	exports.Set(Napi::String::New(env, "Point"),
		Napi::Function::New(env, Point));
	exports.Set(Napi::String::New(env, "Curve"),
	  Napi::Function::New(env, ECurve));
	exports.Set(Napi::String::New(env, "Curve_Fq"),
	  Napi::Function::New(env, ECurve_Fq));
	exports.Set(Napi::String::New(env, "Point_Fq"),
	  Napi::Function::New(env, Point_Fq));
	// exports.Set(Napi::String::New(env, "add"),
	// 	Napi::Function::New(env, Add));
	exports.Set(Napi::String::New(env, "mul"),
		Napi::Function::New(env, Mul));
	exports.Set(Napi::String::New(env, "mmul"),
		Napi::Function::New(env, Mmul));
	exports.Set(Napi::String::New(env, "createMPK"),
		Napi::Function::New(env, CreateMPK));
	exports.Set(Napi::String::New(env, "shamir"),
		Napi::Function::New(env, Shamir));
	exports.Set(Napi::String::New(env, "keyProj"),
		Napi::Function::New(env, KeyProj));
	exports.Set(Napi::String::New(env, "keyRecovery"),
		Napi::Function::New(env, KeyRecovery));
	exports.Set(Napi::String::New(env, "addPoints"),
		Napi::Function::New(env, AddPoints));
	exports.Set(Napi::String::New(env, "hashToPoint"),
		Napi::Function::New(env, HashToPoint));
	exports.Set(Napi::String::New(env, "weilPairing"),
		Napi::Function::New(env, WeilPairing));
	exports.Set(Napi::String::New(env, "getRandom"),
		Napi::Function::New(env, GetRandom));

	exports.Set(Napi::String::New(env, "mapPoint"),
		Napi::Function::New(env, MapToFq));
	exports.Set(Napi::String::New(env, "signTate"),
		Napi::Function::New(env, SignTate));
	exports.Set(Napi::String::New(env, "verifyTate"),
		Napi::Function::New(env, VerifyTate));
	return exports;
}

NODE_API_MODULE(addon, InitAll)