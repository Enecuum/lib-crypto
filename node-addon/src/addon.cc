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

Napi::Array polyToArr(ExtensionField::Element el, Napi::Env env) {
	Napi::Array res = Napi::Array::New(env, el.size());
	int len = el.size();
	for(int i = 0; i < len; i++)
    	res[i] = el[len - i - 1];
	return res;
}
ExtensionField::Element arrToPoly(Napi::Array arr, ellipticCurveFq& E_Fq) {
	std::string str = std::to_string(arr.Length() - 1);
	for(int i = 0; i < arr.Length(); i++){
		Napi::Value tmpx = arr[i];
		str.append(" ");
		str.append(tmpx.As<Napi::String>());
	}
	ExtensionField::Element res;
	E_Fq.field->readElement(str, res);
	return res;
}

Napi::Object SignTate(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    NodeBN* h = Napi::ObjectWrap<NodeBN>::Unwrap(info[0].As<Napi::Object>());
	NodePT* secret = Napi::ObjectWrap<NodePT>::Unwrap(info[1].As<Napi::Object>());
	NodePT* G = Napi::ObjectWrap<NodePT>::Unwrap(info[2].As<Napi::Object>());
	Napi::Object G0_obj = info[3].As<Napi::Object>();
	NCurve* curve = Napi::ObjectWrap<NCurve>::Unwrap(info[4].As<Napi::Object>());
    NCurve_Fq* ecurve = Napi::ObjectWrap<NCurve_Fq>::Unwrap(info[5].As<Napi::Object>());

	Napi::String G0x = G0_obj.Get("x").As<Napi::String>();
	Napi::String G0y = G0_obj.Get("y").As<Napi::String>();

	ExtensionField::Element G0_x, G0_y;
	ecurve->E_Fq->field->readElement(G0x.Utf8Value(), G0_x);
	ecurve->E_Fq->field->readElement(G0y.Utf8Value(), G0_y);
	ecPoint G0_fq(G0_x, G0_y);
	
	BigNumber r2 = getRandom(curve->crv.order);
	//std::cout << "r2: " << r2.toDecString() << std::endl;

	ecPoint S1_fq;
	ecurve->E_Fq->scalarMultiply(S1_fq, G0_fq, (Integer)(r2.toDecString()), -1);
	
	ecPoint secret_fq = mapToFq(secret->p, &curve->crv, *ecurve->E_Fq);
	ecPoint H_fq = hashToPoint(h->bn);
	//std::cout << "\n H_fq: " << std::endl;
	//ecurve->E_Fq->show(H_fq);
	// S2 = r*H + SecKey

	ecPoint rH_Fq;	
	ecurve->E_Fq->scalarMultiply(rH_Fq, H_fq, (Integer)(r2.toDecString()), -1);

	ecPoint S2_fq;
	ecurve->E_Fq->add(S2_fq, rH_Fq, secret_fq);

	//ecurve->E_Fq->show(S2_fq);

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
	NodeBN* h = Napi::ObjectWrap<NodeBN>::Unwrap(info[1].As<Napi::Object>());
	NodePT* Q = Napi::ObjectWrap<NodePT>::Unwrap(info[2].As<Napi::Object>());
	Napi::Object G0_obj = info[3].As<Napi::Object>();
	Napi::Object MPK_obj = info[4].As<Napi::Object>();
	
	NCurve* curve = Napi::ObjectWrap<NCurve>::Unwrap(info[5].As<Napi::Object>());
    NCurve_Fq* ecurve = Napi::ObjectWrap<NCurve_Fq>::Unwrap(info[6].As<Napi::Object>());
	
	Napi::String G0x = G0_obj.Get("x").As<Napi::String>();
	Napi::String G0y = G0_obj.Get("y").As<Napi::String>();
	ExtensionField::Element G0_x, G0_y;
	ecurve->E_Fq->field->readElement(G0x.Utf8Value(), G0_x);
	ecurve->E_Fq->field->readElement(G0y.Utf8Value(), G0_y);
	ecPoint G0_fq(G0_x, G0_y);

	ExtensionField::Element MPK_x, MPK_y;
	Napi::String MPKx = MPK_obj.Get("x").As<Napi::String>();
	Napi::String MPKy = MPK_obj.Get("y").As<Napi::String>();
	ecurve->E_Fq->field->readElement(MPKx.Utf8Value(), MPK_x);
	ecurve->E_Fq->field->readElement(MPKy.Utf8Value(), MPK_y);
	ecPoint MPK_fq(MPK_x, MPK_y);
	
	Napi::Object s2 = sign.Get("s").As<Napi::Object>();
	Napi::Object s1 = sign.Get("r").As<Napi::Object>();

	Napi::Array s1x = s1.Get("x").As<Napi::Array>();
	Napi::Array s1y = s1.Get("y").As<Napi::Array>();
	ExtensionField::Element S1Fq_x = arrToPoly(s1x, *ecurve->E_Fq);
	ExtensionField::Element S1Fq_y = arrToPoly(s1y, *ecurve->E_Fq);
	ecPoint S1_fq(S1Fq_x, S1Fq_y);

	Napi::Array s2x = s2.Get("x").As<Napi::Array>();
	Napi::Array s2y = s2.Get("y").As<Napi::Array>();
	ExtensionField::Element S2Fq_x = arrToPoly(s2x, *ecurve->E_Fq);
	ExtensionField::Element S2Fq_y = arrToPoly(s2y, *ecurve->E_Fq);
	ecPoint S2_fq(S2Fq_x, S2Fq_y);


	BigNumber seed = getRandom(curve->crv.order);
	std::cout << "seed: " << seed.toDecString() << std::endl;
	ecPoint S_fq = hashToPointFq(S2_fq, seed, *ecurve->E_Fq);

	//ecPoint G0_fq = mapToFq(G0->p, &curve->crv, *ecurve->E_Fq);

	//ecPoint MPK_fq = mapToFq(MPK->p, &curve->crv, *ecurve->E_Fq);
	ecPoint Q_Fq = mapToFq(Q->p, &curve->crv, *ecurve->E_Fq);

	ecPoint H_fq = hashToPoint(h->bn);
	//ecPoint H_fq = mapToFq(H->p, &curve->crv, *ecurve->E_Fq);
	//ecPoint S1_fq = mapToFq(s1->p, &curve->crv, *ecurve->E_Fq);

	ExtensionField::Element rr, bb, cc;

	rr = tatePairing(S2_fq, G0_fq, S_fq, *ecurve->E_Fq);
	bb = tatePairing(Q_Fq, MPK_fq, S_fq, *ecurve->E_Fq);
	cc = tatePairing(H_fq, S1_fq, S_fq, *ecurve->E_Fq);
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