#include "Profile_Functions.h"
#include "../PLANT/PlantModel.h"
#include "./Functors/Profile_Functors.h"

Profile_Functions::Profile_Functions(void)
{
}
Profile_Functions::~Profile_Functions(void)
{
}
bool Profile_Functions::Def()
{
	ModuleNo=4;
	NE *pNE;
	pNE= new NE(&NoLayerM, 1,1,100,1,50);
	Define(pNE,"No Measured Layers");
	
	NE* pNL;
	pNL=new NE(&NL, 10,2,100,2,100);
	Define(pNL, "Number of Layers");

	vector<double> initv;
	initv.resize(NoLayerM);
	P* pP;
	ModuleConditions="";
	for(auto i=initv.begin(); i!=initv.end();++i) *i=0.0;
	pP= new		P(&UpperDepth,pNE, MPROFILE, GENERAL, FYS_SHAPE, NOBPROC, SIMPLE);
	Define	(pP,pNE,"Upper Depth",initv,"m","",0.,1000.,0.,100.,1,2);
	mTab_MeasTexture.push_back(pP);


	for(auto i=initv.begin(); i!=initv.end();++i) *i=0.1;
	pP= new		P(&LowerDepth,pNE, MPROFILE, GENERAL, FYS_SHAPE, NOBPROC, SIMPLE);
	Define	(pP,pNE,"Lower Depth",initv,"m","",0.,1000.,0.,100.,1,2);
	mTab_MeasTexture.push_back(pP);

	for(auto i=initv.begin(); i!=initv.end();++i) *i=5.0;
	pP= new		P(&ClayContent,pNE, MPROFILE, GENERAL, FYS_SHAPE, NOBPROC, SIMPLE);
	Define	(pP,pNE,"Clay Content",initv,"Mass %","Thermal Conductivity Function>0",0.,100.,0.,100.,4);
	mTab_MeasTexture.push_back(pP);

	for(auto i=initv.begin(); i!=initv.end();++i) *i=50.0;
	pP= new		P(&SandContent,pNE, MPROFILE, GENERAL, FYS_SHAPE, NOBPROC, SIMPLE);
	Define	(pP,pNE,"Sand Content",initv,"Mass %","Thermal Conductivity Function>0",0.,100.,0.,100.,4);
	mTab_MeasTexture.push_back(pP);

	for(auto i=initv.begin(); i!=initv.end();++i) *i=0.0;
	pP= new		P(&GravelContent,pNE, MPROFILE, GENERAL, FYS_SHAPE, NOBPROC, SIMPLE);
	Define	(pP,pNE,"Gravel Content",initv,"Mass %","Thermal Conductivity Function>0",0.,100.,0.,100.,4);
	mTab_MeasTexture.push_back(pP);

	for(auto i=initv.begin(); i!=initv.end();++i) *i=2.0;
	pP= new		P(&OrganicContent,pNE, MPROFILE, GENERAL, FYS_SHAPE, NOBPROC, SIMPLE);
	Define	(pP,pNE,"Organic Content",initv,"Mass %","Thermal Conductivity Function>0",0.,100.,0.,100.,4);
	mTab_MeasTexture.push_back(pP);

	for(auto i=initv.begin(); i!=initv.end();++i) *i=10.0;
	pP= new		P(&QuartsFraction,pNE, MPROFILE, GENERAL, FYS_SHAPE, NOBPROC, SIMPLE);
	Define	(pP,pNE,"Quarts fraction of Sand",initv,"Mass %","Thermal Conductivity Function>0",0.,100.,0.,100.,4);
	mTab_MeasTexture.push_back(pP);



	initv.resize(NL);
	for(auto i=initv.begin(); i<initv.begin()+4;++i) *i=0.04;
	for(auto i=initv.begin()+4; i<initv.begin()+6;++i) *i=0.06;
 	for(auto i=initv.begin()+6; i<initv.begin()+8;++i) *i=0.1;
  	for(auto i=initv.begin()+8; i<initv.end();++i) *i=0.15;

	vector<double> initu;
	initu.resize(NL);
	for(size_t i=1; i<initu.size();i++) initu[i]=initu[i-1]+initv[i-1];


	pP= new		P(&ThicknessLayers,pNL, SIMPROFILE, GENERAL, FYS_SHAPE, NOBPROC, SIMPLE);	
		Define(pP,pNL,"Thickness Layers",initv,"m","",0.001,100.,0.01,10.,1) ;
		pP->SetReadOnly(false);
		mTab_Compartment.push_back(pP);
	
	pP= new		P(&AThicknessLayers,pNL, SIMPROFILE, GENERAL, FYS_SHAPE, NOBPROC, SIMPLE);	
		Define(pP,pNL, "AThicknessLayers",initv,"m","",0.001,100.,.01,1.,20);
	
	pP= new		P(&ModelUpperDepth,pNL, SIMPROFILE, GENERAL, FYS_SHAPE, NOBPROC, SIMPLE);	
	Define(pP,pNL, "mUpper Depth",initu,"m","",0.001,100.,0.01,10.,2) ;
	mTab_BC_Model.push_back(pP); pP->SetReadOnly(true); mTab_Gen_Model.push_back(pP);

	initu.erase(initu.begin());
	initu.push_back(*(initu.end()-1)+*(initv.end()-1));
	pP= new		P(&ModelLowerDepth,pNL, SIMPROFILE, GENERAL, FYS_SHAPE, NOBPROC, SIMPLE);
	Define(pP,pNL, "mLower Depth",initu,"m","",0.001,100.,0.01,10.,2) ;
	pP->SetReadOnly(true);
	mTab_BC_Model.push_back(pP);mTab_HC_Model.push_back(pP); mTab_Gen_Model.push_back(pP);

	vector<double> initm;
	initm.resize(NL);
	for(size_t i=0; i<initm.size();++i) {
	   initm[i]=initu[i]-initv[i]/2;
	}
	pP= new		P(&DepthMidpLayer,pNL, SIMPROFILE, GENERAL, FYS_SHAPE, NOBPROC, SIMPLE);
	Define(pP,pNL,"DepthMidpLayers",initu,"m","",0.001,100.)	;
	pP= new	P(&DepthLayers,pNL, SIMPROFILE, GENERAL, FYS_SHAPE, NOBPROC, SIMPLE);
	Define(pP,pNL,"DepthLayers",initu,"m","",0.001,100.,0.01,100.);
	
	vector<double> initd;
	initd.resize(NL);
	initd.front()=initm.front();
	for(size_t i=1; i<initd.size();++i) {
	   initd[i]=initm[i]-initm[i-1];
	}
	pP= new	P(&DistanceMidpLayers,pNL, SIMPROFILE, GENERAL, FYS_SHAPE, NOBPROC, SIMPLE);
    Define(pP,pNL, "DistanceMidpLayers",initd,"m");
	Tab *pTab;

	pTab= new Tab(&mTab_Compartment, SIMPROFILE, GENERAL, FYS_SHAPE, NOBPROC, SIMPLE);
	Define(pTab,"Compartment Sizes", "Thickness Layers","",1);


	pTab= new Tab(&mTab_MeasTexture, MPROFILE, GENERAL, FYS_SHAPE, NOBPROC, SIMPLE);
	Define(pTab,"Soil Texture", 3);


	//f_ReCalcProfile = ModelProfileLayers(NL);

	//f_ReCalcProfile(&OriginalThickness, &ThicknessLayers, &AThicknessLayers, &DistanceMidpLayers,
	//	&DepthLayers, &ModelUpperDepth, &DepthMidpLayer, &ModelLowerDepth);

	return FunctionDef();
}
bool Profile_Functions::FunctionDef() {
	FunctorProp functorprop;
	Func *pFunc;
	//bool (Profile_Functions::*fpoint)();
	//fpoint=&Profile_Functions::Profile_ReCalc;
		pFunc= new Func(SIMPROFILE,DISTANCE, NOFPROC,NOBPROC, NORMAL);
		functorprop.FuncName = MyFunc::PROFILE_RECALC;
		functorprop.Arg = MyArgType::BLANK;
		Define(pFunc, functorprop,this, "ReCalc Profile Function"); 
		bool res=pFunc->MakeOperation();

	return true;
}
bool Profile_Functions::Profile_ReCalc(){
auto ReCalcProfile = ModelProfileLayers(NL);

ReCalcProfile(OriginalThickness, ThicknessLayers, AThicknessLayers, DistanceMidpLayers,
		DepthLayers, ModelUpperDepth, DepthMidpLayer, ModelLowerDepth);
	return true;
}


DEPTH_type Profile_Functions::DepthDist(size_t choice, double totsum,  double s_depth, double s_exptail)
{  DEPTH_type out;
	
	out.NumLay=0;
	out.Dist.resize(NL);
	s_depth=min(-0.01, s_depth);
	if(choice==0) {	 // based on given table
		// Corrected bug July 14 2016
		PlantModel* p_Plant = (PlantModel*)m_pModelInfo->GetModelPtr("Plant Model");
		WaterUptake *pPlant = &p_Plant->m_WaterUptake;
		out.Dist = pPlant->RootF;
		out.NumLay = pPlant->RootF.size();
	}
	else if(choice==1) {	// linear
		double dec=1./s_depth;

		double rupper=1;
		double rlower;
		double thickn;
		//for(int i=0; i<NL.Elem;i++) {
		int i=0;
		while(DepthLayers[i]<-s_depth&&i<DepthLayers.size()-1) {
			 rlower=max(0.,1+dec*DepthLayers[i]);
			 thickn=max(0.,min(ThicknessLayers[i],-s_depth-DepthLayers[i]+ThicknessLayers[i]));
			 out.Dist[i]=-(rupper+rlower)*thickn*dec*totsum;
			 rupper=rlower;
			 i++;
		}
		out.NumLay=i;
	}
	else if(choice==2) { //uniform
		double rdens=-1/s_depth*totsum;
		size_t i=1;
		out.Dist[0]=min(-DepthLayers[0]+ThicknessLayers[0]-s_depth, ThicknessLayers[0] )*rdens;
		while(DepthLayers[i-1]<-s_depth&&i<out.Dist.size()) {
			 out.Dist[i]=min(-DepthLayers[i]+ThicknessLayers[i]-s_depth, ThicknessLayers[i] )*rdens;
			 i++;
		}
		out.NumLay=i;
	}
	else if(choice==3) {//Exponential
		double deck;
		deck=log(s_exptail)/s_depth;
		double rupper=1;
		double rlower;
		int i=0;
		rlower=max(s_exptail, exp(-deck*DepthLayers[0])-0.001*s_exptail);
		while(rlower>s_exptail&&i<DepthLayers.size()-1) {
			rlower=max(s_exptail, exp(-deck*DepthLayers[i])-0.001*s_exptail);
			out.Dist[i]=(rupper-rlower)/(1.-s_exptail)*totsum;
			rupper=rlower;
			i++;
		}
		if(i==0) {
			out.Dist[0]=totsum;
			out.NumLay=1;


		}
		else {
			out.NumLay=i;

		}
		double diff=sum(out.Dist)-totsum;
		if(diff!=0) {
			int i=0;


		}



	}

	return out;
}





