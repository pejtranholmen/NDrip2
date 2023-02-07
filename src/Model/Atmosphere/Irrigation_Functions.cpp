#include "./Irrigation_Functions.h"
#include "../ModelTypes/NewModelType.h"
Irrigation_Functions::Irrigation_Functions(void)
{
}
Irrigation_Functions::~Irrigation_Functions(void)
{
}



bool Irrigation_Functions::Def()
{

	Surface_Water::Def();
	ModuleNo=8;
//    ! Snow surface energy balance
	Sw* pSw;
	pSw= new Sw(&Dripper,SINGLE,WATER, INFILTRATION,NOBPROC, NORMAL);
	Define(pSw,    "Dripper",0,"Off|On","WaterEq > 0|Irrigation > 0",114) ; 
	pSw= new Sw(&UnitIrrig,SINGLE,WATER, INFILTRATION,NOBPROC, NORMAL);
	Define(pSw,  "UnitIrrig",0,"Rate|Amount","WaterEq > 0|IrrigationInput =0|Irrigation > 0")  ;
	pSw= new Sw(&IrrigationInput,SINGLE,WATER, INFILTRATION,NOBPROC, NORMAL);
	Define(pSw ,"IrrigationInput",0,"Driving variable|Automatic Generation|Auto but only with plant","WaterEq > 0|Irrigation > 0",200);

	Xs *pXs;
	pXs= new Xs(&DripContainer,IRRIGATIONCONTAINER,WATER, STORAGE,NOBPROC, NORMAL);

	Define(pXs,  "DripContainer"	, 0.	,"mm"		, "Dripper = 1");
	Ts *pTs;
	pTs= new Ts(&DripFill,IRRIGATIONCONTAINER,WATER, TECH_REGULATOR,NOBPROC, NORMAL);
	Define(pTs,   "DripFill"		, 0.	,"mm/day"	, "Dripper = 1");
	pTs= new Ts(&DripOutlet,IRRIGATIONCONTAINER,WATER, TECH_REGULATOR,NOBPROC, NORMAL);
	Define(pTs,  "DripOutlet"		, 0.	,"mm/day"	, "Dripper = 1");

	Ps *pPs;
	pPs= new Ps(&IStoreLayer,IRRIGATIONCONTAINER,WATER, TECH_REGULATOR,NOBPROC, NORMAL);
	Define(pPs,"IStoreLayer",4.,"#","IrrigationInput > 0",1.,100.,1.,10.);
	pPs= new Ps(&IStoreMin,IRRIGATIONCONTAINER,WATER, TECH_REGULATOR,NOBPROC, NORMAL);
	Define(pPs, "IStoreMin"  ,50.,"mm","IrrigationInput > 0",0., 400.,0.,100.);
	pPs= new Ps(&IrrigRate  ,IRRIGATIONCONTAINER,WATER, TECH_REGULATOR,NOBPROC, NORMAL);
	Define(pPs,  "IrrigRate"  ,50.,"mm/day","IrrigationInput >0",0., 2000.,0., 500.);
	pPs= new Ps(&IrrigAmount,IRRIGATIONCONTAINER,WATER, TECH_REGULATOR,NOBPROC, NORMAL);
	Define(pPs,  "IrrigAmount",20.,"mm","IrrigationInput > 0",0., 400., 0., 100.);
	
	pPs= new Ps(&IrrigV_Index  ,SINGLE,WATER,NOFPROC ,NOBPROC, NORMAL);
	Define(pPs,"Index in File",1.,"#","IrrigationInput =0|Irrigation > 0",1.,100.);
	pPs= new Ps(&SoilIrrigF,SINGLE,WATER, TECH_REGULATOR,NOBPROC, NORMAL);
 	Define(pPs,"SoilIrrigF", 0.,"-","Irrigation > 0",0.,1.);
	pPs= new Ps(&DripIrrigCover,SINGLE,WATER, TECH_REGULATOR,NOBPROC, NORMAL);
	Define(pPs,  "DripIrrigCover", 0.2,"-","Irrigation >0|Dripper>0",0.,1.);
	pPs= new Ps(&DripIrrigXCenter,SINGLE, WATER, TECH_REGULATOR,NOBPROC, NORMAL);
	Define(pPs, "DripIrrigXCentre", 0.5,"-","Irrigation >0|Dripper>0",0.,1.);
	pPs= new Ps(&DripIrrigRate,IRRIGATIONCONTAINER,WATER, TECH_REGULATOR,NOBPROC, NORMAL);
	Define(pPs,  "DripIrrigRate", 100.,"mm/day","Irrigation >0|Dripper>0",0.,10000.);


	P *pP;
	NE *pNL;
	pNL=m_pModelMap->GetNEPointer("Number of Layers");
	vector <double> initv;
	initv.assign(p_Soil_WaterF->NL,0);
	initv[0]=1.;

	pP=m_pModelMap->GetPPointer("mUpper Depth");
	mTab_Dripper.push_back(pP);
	pP=m_pModelMap->GetPPointer("mLower Depth");
	mTab_Dripper.push_back(pP);
	pP= new P(&DrippInfilDistF,pNL,IRRIGATIONCONTAINER,WATER, TECH_REGULATOR,NOBPROC, NORMAL);

	Define(pP, pNL, "InfilDistF"	,initv, "-","Dripper>0",0.,1.);

	mTab_Dripper.push_back(pP);
	Tab *pTab;
	pTab= new Tab(&mTab_Dripper,IRRIGATIONCONTAINER,WATER, TECH_REGULATOR,NOBPROC, NORMAL);
	Define(pTab,"Depth Distribution of Irrigation",3);


return true;
}



