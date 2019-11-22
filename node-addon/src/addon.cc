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
		//int len = a.Length();
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
		//std::string hex_chars("E8 48 D8 FF FF 8B 0D");
		Napi::String a = info[0].As<Napi::String>();
		std::string str(a.Utf8Value());
		if ((str.size() % 2) != 0) {
			str.insert(0, "0");
		}
		for (int i = 2; i < str.size(); i += 3) {
			str.insert(i, " ");
		}
		
		std::istringstream hex_chars_stream(str);
		std::vector<unsigned char> bytes;

		unsigned int c;
		while (hex_chars_stream >> std::hex >> c)
		{
		    bytes.push_back(c);
		}
		BigNumber bn(&bytes[0], bytes.size());
		return NodeBN::NewInstance(Napi::External<BigNumber>::New(info.Env(), &bn));
		//Napi::Error::New(env, std::to_string("String")).ThrowAsJavaScriptException();
	}
	else{
		Napi::Error::New(env, std::to_string(info[0].Type())).ThrowAsJavaScriptException();
	}
	
	//
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
    //NodeBN* a = Napi::ObjectWrap<NodeBN>::Unwrap(info[0].As<Napi::Object>());
    //NodeBN* b = Napi::ObjectWrap<NodeBN>::Unwrap(info[1].As<Napi::Object>());
    //NodeBN* p = Napi::ObjectWrap<NodeBN>::Unwrap(info[2].As<Napi::Object>());
    //NodeBN* order = Napi::ObjectWrap<NodeBN>::Unwrap(info[3].As<Napi::Object>());
    //NodeBN* gx = Napi::ObjectWrap<NodeBN>::Unwrap(info[4].As<Napi::Object>());
    //NodeBN* gy = Napi::ObjectWrap<NodeBN>::Unwrap(info[5].As<Napi::Object>());
    NodePT* H = Napi::ObjectWrap<NodePT>::Unwrap(info[0].As<Napi::Object>());
	NodePT* secret = Napi::ObjectWrap<NodePT>::Unwrap(info[1].As<Napi::Object>());
	NodePT* G = Napi::ObjectWrap<NodePT>::Unwrap(info[2].As<Napi::Object>());
	NCurve* curve = Napi::ObjectWrap<NCurve>::Unwrap(info[3].As<Napi::Object>());
    NCurve_Fq* ecurve = Napi::ObjectWrap<NCurve_Fq>::Unwrap(info[4].As<Napi::Object>());

	BigNumber r2(2);

	EC_POINT *s1 = mul(r2.bn, G->p, &curve->crv);

	// S2 = r*H + SecKey
	EC_POINT *rH = mul(r2.bn, H->p, &curve->crv);
	

	ExtensionField::Element HFq_x, HFq_y;
	std::string strHx_fq("11 12795200616325864128301600439533238625836570855973778800724207558943117513205 12130884351496920759643149235993696557935956626275118316469563330330421956954 6283040097693372918304058385692122958293949981328347340387761786450421554751 4899237897159181067640145765574944595870830883586148580522329018622189949897 8846355464997914784016362869867541795213329883451326397888469916812886128424 14129079141784273041581234757445313819197902405344241350499624439513305758998 13352072847043331449752665150855039962671167605050063905273191449284813579560 6393127232507087088463231637082626347554201215151264493277478426389978548816 11055793717890002963273053758670586391406977509500385190412071151130460455343 6377301085569399016620998198775940872073165203580132449119152990662002772535 11447275512728877891081113656170964162742321407569066846904189083117611540998 5937852374468625217547352015728789862599812933954999569332681032009432549840");
	std::string strHy_fq("11 3218839677179312609018856855655013274124364702869282379100183166286769237998 15044457283554428446408480119819863018909478296385264354499712915354078803313 10836697868833735308741152896657122315989750961978455786725014823625823018718 6252900733548808857614294347803602330676526195807488170093773710720709936698 9072164750976273151044609579754931421152548695706084093022299941929293303770 1581051531791168413564622242556945736630412913604996002010381621887765731409 3095295538339275550800957843390877304418396432741414862000545449582323768268 11825058412863805355749018175020855939988339734006085226357534946021033274444 664800404171033514083577273183587063464881026711383308491155722761791501750 11913374880181628896317996635569558849448830303277386417285222453712730872323 11428928723220629553432075124518828787664163337109018680143588209208797492122 1104801707771267947748586133547196191205177949442762583878743413246435741153");
	ecurve->E_Fq->field->readElement(strHx_fq, HFq_x);
	ecurve->E_Fq->field->readElement(strHy_fq, HFq_y);
	ecPoint H_fq(HFq_x, HFq_y);

	//ecPoint H_fq = mapToFq(H->p, &curve->crv, *ecurve->E_Fq);
	ecPoint rH_Fq; // = mapToFq(rH, &curve->crv, *ecurve->E_Fq);
	ecurve->E_Fq->scalarMultiply(rH_Fq, H_fq, (Integer)2, -1);//R=6*P, order of P is not required
	
	ecPoint secret_fq = mapToFq(secret->p, &curve->crv, *ecurve->E_Fq);
	
	ecPoint S2_fq;
	ecurve->E_Fq->add(S2_fq, rH_Fq, secret_fq);//R=P+Q
	//let s2 = addon.addPoints(rH, secret, curve);

	ecurve->E_Fq->show(S2_fq);
	std::stringstream xx, yy;
	ecurve->E_Fq->field->writeElement(S2_fq.x, xx);
	ecurve->E_Fq->field->writeElement(S2_fq.y, yy);
	std::cout << xx.str() << std::endl;
	std::vector<std::string> elementsX, elementsY;
	std::string temp;
	while (!xx.eof()) {
		xx >> temp;
		if (temp == "+")
			continue;
		std::size_t start = temp.find("(");
		std::size_t end = temp.find(")");
		std::string str3 = temp.substr(start + 1, end - 1);
		elementsX.push_back(str3);
		//cout << str3 << endl;
		temp = "";
	}
	while (!yy.eof()) {
		yy >> temp;
		if (temp == "+")
			continue;
		std::size_t start = temp.find("(");
		std::size_t end = temp.find(")");
		std::string str3 = temp.substr(start + 1, end - 1);
		elementsY.push_back(str3);
		//cout << str3 << endl;
		temp = "";
	}

	BigNumber s1_x;
	BigNumber s1_y;
	if (!EC_POINT_get_affine_coordinates_GFp(curve->crv.curve, s1, s1_x.bn, s1_y.bn, NULL)) handleErrors();
	Napi::Object resR = Napi::Object::New(env);

    //Napi::Object obj = NodePT_Fq::NewInstance(Napi::External<ecPoint>::New(info.Env(), &S2_fq));
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

Napi::Object VerifyTate(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	
	Napi::Object sign = info[0].As<Napi::Object>();
	NodePT* s1 = Napi::ObjectWrap<NodePT>::Unwrap(info[1].As<Napi::Object>());
	NodePT* H = Napi::ObjectWrap<NodePT>::Unwrap(info[2].As<Napi::Object>());
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
	std::string strS2x("11"), strS2y("11");
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

	ExtensionField::Element SFq_x, SFq_y;
	std::string strSx_fq("11 7463519085493365693394121199190059745988511279031308831763630965575497714973 3213479576728694612873129502574939146123496605844840557128765993775758126890 13752713972998144419152467731240251127964542833056708196491325125872366091087 15553372254183114510739439757246731793806383848608048309939148850948110186888 12258644319749650037144571587825036927155675497479416759205860608047734134062 3532101921007362030794325086925105560686780697780826036915262551502427550164 4355439161355160537858443362077205542687166906735462310058911937938440403416 3802272190851511210630147460266578785161879155516601953628860280715370308621 1317637193017543797636378441570532755116450388205874274160281092658933403629 5473798230129279492859744018818548971274864114987064021826091310815335879733 8851270884955316309046441361625390056442140408234485954600071180679479553237 15132257993022606200919632833688451974231808385319226086684277348612204805425");
	std::string strSy_fq("11 5402906328658268916194312952865326995905954868409589434741588571597549983634 391870371242143485522013522668372963939928916979327764650555183010158927409 8388867712234558980469512863505598308802851258476952237041272427075151744109 7495609234823091861100624264137916927683614152015976278448757663646649941991 5658906808188150074100165944955289756224642167129582121658117411584296860873 13878886333438371495866267885612108469039825873739305210045820592485652966585 6963650853987273077816544478754914531960456246032726948867288748211675615415 8830885856243348641350531158188320567660408310040255566312664501804444943080 6106755154037675810581056224691089803765559530574180661442422341382742080274 5485664565367343310789520189176634801281297522570962649298265524017950485938 9805891012360959361564020302204274589333018098732386052032042786398105438146 15198026600655527523115482280007159805633263781115846119482141707705953976273");
	ecurve->E_Fq->field->readElement(strSx_fq, SFq_x);
	ecurve->E_Fq->field->readElement(strSy_fq, SFq_y);
	ecPoint S_fq(SFq_x, SFq_y);
	ecPoint G0_fq = mapToFq(G0->p, &curve->crv, *ecurve->E_Fq);

	ExtensionField::Element r1 = tatePairing(G0_fq, S2_fq, S_fq, *ecurve->E_Fq);
	std::cout << "\n r1: " << std::endl;
	ecurve->E_Fq->field->writeElement(r1);

	ecPoint MPK_fq = mapToFq(MPK->p, &curve->crv, *ecurve->E_Fq);
	ecPoint Q_Fq = mapToFq(Q->p, &curve->crv, *ecurve->E_Fq);
	ExtensionField::Element HFq_x, HFq_y;
	std::string strHx_fq("11 12795200616325864128301600439533238625836570855973778800724207558943117513205 12130884351496920759643149235993696557935956626275118316469563330330421956954 6283040097693372918304058385692122958293949981328347340387761786450421554751 4899237897159181067640145765574944595870830883586148580522329018622189949897 8846355464997914784016362869867541795213329883451326397888469916812886128424 14129079141784273041581234757445313819197902405344241350499624439513305758998 13352072847043331449752665150855039962671167605050063905273191449284813579560 6393127232507087088463231637082626347554201215151264493277478426389978548816 11055793717890002963273053758670586391406977509500385190412071151130460455343 6377301085569399016620998198775940872073165203580132449119152990662002772535 11447275512728877891081113656170964162742321407569066846904189083117611540998 5937852374468625217547352015728789862599812933954999569332681032009432549840");
	std::string strHy_fq("11 3218839677179312609018856855655013274124364702869282379100183166286769237998 15044457283554428446408480119819863018909478296385264354499712915354078803313 10836697868833735308741152896657122315989750961978455786725014823625823018718 6252900733548808857614294347803602330676526195807488170093773710720709936698 9072164750976273151044609579754931421152548695706084093022299941929293303770 1581051531791168413564622242556945736630412913604996002010381621887765731409 3095295538339275550800957843390877304418396432741414862000545449582323768268 11825058412863805355749018175020855939988339734006085226357534946021033274444 664800404171033514083577273183587063464881026711383308491155722761791501750 11913374880181628896317996635569558849448830303277386417285222453712730872323 11428928723220629553432075124518828787664163337109018680143588209208797492122 1104801707771267947748586133547196191205177949442762583878743413246435741153");
	ecurve->E_Fq->field->readElement(strHx_fq, HFq_x);
	ecurve->E_Fq->field->readElement(strHy_fq, HFq_y);
	ecPoint H_fq(HFq_x, HFq_y);

	//ecPoint H_fq = mapToFq(H->p, &curve->crv, *ecurve->E_Fq);
	ecPoint S1_fq = mapToFq(s1->p, &curve->crv, *ecurve->E_Fq);

	ExtensionField::Element b1 = tatePairing(MPK_fq, Q_Fq, S_fq, *ecurve->E_Fq);
	//cout << "\n b1: " << endl;
	//E_Fq.field->writeElement(b1);

	ExtensionField::Element c1 = tatePairing(S1_fq, H_fq, S_fq, *ecurve->E_Fq);
	//cout << "\n c1: " << endl;
	//E_Fq.field->writeElement(c1);

	ExtensionField::Element b1c1;
	ecurve->E_Fq->field->mul(b1c1, b1, c1);
	std::cout << "\n b1c1: " << std::endl;
	ecurve->E_Fq->field->writeElement(b1c1);

	BigNumber res(1);

  	return NodeBN::NewInstance(Napi::External<BigNumber>::New(info.Env(), &res));
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