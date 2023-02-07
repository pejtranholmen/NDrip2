#include "HBV_Functions.h"
 #include "../Atmosphere/AtmModel.h"
 #include "../PLANT/PlantModel.h"

HBV_Functions::HBV_Functions(void)
{


}
HBV_Functions::~HBV_Functions(void)
{
	
}


bool HBV_Functions::Def()
{

	Ps* pPs;
	Func *pFunc;
	string funcname;
	FunctorProp functorprop;
	vector<Ps*> ps_vec;
	vector<P*> p_vec;
	ps_vec.clear();
	ModuleNo=34;


	ModuleConditions="HBV Soil Module >0";
 	AtmModel* pAtm=(AtmModel*)m_pModelInfo->GetModelPtr("Atm Model");
	m_pMet=&pAtm->m_Irrigation;
	PlantModel* pPlant=(PlantModel*)m_pModelInfo->GetModelPtr("Plant Model");
	m_pPlant=&pPlant->m_WaterUptake;	

	pPs=new Ps	(&Initial_SoilStorage,SINGLE,WATER,INITF,NOBPROC,NORMAL);
	Define		(pPs,"Initial Soil Storage",30.,"mm","",0.,300.) ;
	pPs= new Ps (&Initial_PeakStorage,SINGLE, WATER, INITF,NOBPROC,NORMAL);
	Define		(pPs,"Initial Peak Storage",30.,"mm","",0.,300.);
	pPs= new Ps (&Initial_BaseStorage,SINGLE, WATER, INITF,NOBPROC,NORMAL);	
	Define		(pPs,"Initial Base Storage",30.,"mm","",0.,300.);


	pPs= new Ps (&FieldCapacity,SINGLE, WATER, STORAGE,NOBPROC,NORMAL);		
	ps_vec.push_back(pPs);
	Define		(pPs,"FieldCapacity",50.,"mm","",10.,400.);
	pPs= new Ps (&CriticalUptakeFrac,SINGLE, WATER, WATERUPTAKE,NOBPROC,NORMAL);	
	ps_vec.push_back(pPs);
	Define		(pPs,"Critical Uptake Frac",.45,"mm","",0.1,1.0);

	functorprop.FuncName = MyFunc::HBV_ACTUALEVAPORATION; functorprop.Arg = MyArgType::D2;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Actual Evaporation Function";
	pFunc = new Func(SINGLEPLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec); 
	ps_vec.erase(ps_vec.begin() + 1);



	pPs = new Ps(&BetaCoef, SINGLE, WATER, INFILTRATION, NOBPROC, NORMAL);	ps_vec.push_back(pPs);
	Define		(pPs,"BetaCoef",1.,"-","",1.,6.);		

	functorprop.FuncName = MyFunc::HBV_INFILTRATION; functorprop.Arg = MyArgType::D3;
	funcname = "Soil Infiltration Function";
	pFunc = new Func(SINGLEPLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec); ps_vec.clear();


	pPs= new Ps (&DisCoefk1,SINGLE, WATER, RUNOFF,NOBPROC,NORMAL);	ps_vec.push_back(pPs);
	Define		(pPs,"Discharge K1",.16,"1/day","",0.1,.3);
	pPs = new Ps(&DisCoefAlfa, SINGLE, WATER, RUNOFF, NOBPROC, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "Discharge Alfa", 0., "mm", "", 0., 1.);
	pPs = new Ps(&T_Step, SINGLE, WATER, RUNOFF, NOBPROC, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "HBV TStep", 1., "mm", "", 0.001, 1.);

	functorprop.FuncName = MyFunc::HBV_PEAK_RUNOFF; functorprop.Arg = MyArgType::D1;
	funcname = "Peak Discharge Function";
	pFunc = new Func(SINGLE, WATER, DRAINAGE, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec); 
	ps_vec.clear();

	pPs= new Ps (&DisCoefk2,SINGLE, WATER, RUNOFF,NOBPROC,NORMAL); ps_vec.push_back(pPs);
	Define		(pPs,"Discharge K2",0.016,"mm","",0.001,0.1);

	functorprop.FuncName = MyFunc::HBV_BASE_RUNOFF; functorprop.Arg = MyArgType::D1;
	funcname = "Base Discharge Function";
	pFunc = new Func(SINGLE, WATER, DRAINAGE, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec); ps_vec.clear();

	ps_vec.clear();
	pPs= new Ps (&MaxPerc,SINGLE, WATER, PERCOLATION,NOBPROC,NORMAL); ps_vec.push_back(pPs);
	Define		(pPs,"PMaxPerc",0.5,UNIT_TYPES::WATERFLOW_UNIT,"",0.1,0.6);
	functorprop.FuncName = MyFunc::HBV_GROUNDWATER_REDIST; functorprop.Arg = MyArgType::D1;
	functorprop.FunctorInitOption = string::npos;
	funcname = "HBV GroundWater Redistribution Function";
	pFunc = new Func(SINGLE, WATER, NOFPROC, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec); ps_vec.clear();



	pPs= new Ps (&Min_GWLev,SINGLE, WATER, DRAINAGE,NOBPROC,NORMAL); ps_vec.push_back(pPs);
	Define		(pPs,"Min_GwLev",-2.5,"m","",-100,-0.1);
	pPs= new Ps (&Max_GWLev,SINGLE, WATER, DRAINAGE,NOBPROC,NORMAL); ps_vec.push_back(pPs);
	Define		(pPs,"Max_GwLev",0.,"m","",0.,0.1);	

	pPs= new Ps (&Sens_GWLev,SINGLE, WATER, DRAINAGE,NOBPROC,NORMAL); ps_vec.push_back(pPs);
	Define		(pPs,"Sens_GwLev",2,"-","",1.,100.);
	pPs= new Ps (&PowerCoef_GWLev,SINGLE, WATER, DRAINAGE,NOBPROC,NORMAL); ps_vec.push_back(pPs);
	Define		(pPs,"PowerCoef_GwLev",1.,"-","",1.,100.);

	functorprop.FuncName = MyFunc::HBV_GROUNDWATERLEVEL; functorprop.Arg = MyArgType::D1;
	functorprop.FunctorInitOption = string::npos;
	funcname = "HBV GroundWater Level Function";
	pFunc = new Func(SINGLE, WATER, NOFPROC, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec); ps_vec.clear();





	Xs* pXs;
	pXs= new Xs(&SoilStorage, SINGLE, WATER, STORAGE,NOBPROC,NORMAL);
	Define		(pXs,"Soil Storage", 0.,"mm");
	pXs= new Xs(&PeakStorage, SINGLE, WATER, STORAGE,NOBPROC,NORMAL);
	Define(pXs,"Peak GW Storage", 0.,"mm");
	pXs= new Xs(&BaseStorage, SINGLE, WATER, STORAGE,NOBPROC,NORMAL);
	Define(pXs,"Base GW Storage"	, 0.	,"mm");

	Ts* pTs;
	pTs= new Ts(&HBV_Input_Soil, SINGLE, WATER, INFILTRATION,NOBPROC, NORMAL);
	Define		(pTs, "HBV Input SOIL", 0.0,UNIT_TYPES::WATERFLOW_UNIT);
	pTs= new Ts(&HBV_Input_GW, SINGLE, WATER, REDISTRIBUTION,NOBPROC, NORMAL);
	Define		(pTs, "HBV Input GW", 0.0,UNIT_TYPES::WATERFLOW_UNIT	)	;
	pTs= new Ts(&HBV_GW_Redist, SINGLE, WATER, REDISTRIBUTION ,NOBPROC, NORMAL);
	Define		(pTs, "HBV GW ReDist", 0.0,UNIT_TYPES::WATERFLOW_UNIT);	
	pTs= new Ts(&Q_Base, SINGLE, WATER, RUNOFF ,NOBPROC, NORMAL);		
	Define		(pTs,"Base Flow", 0.0,UNIT_TYPES::WATERFLOW_UNIT		);
	pTs= new Ts(&Q_Peak, SINGLE, WATER, RUNOFF ,NOBPROC, NORMAL);
	Define		(pTs,"Peak Flow", 0.0,UNIT_TYPES::WATERFLOW_UNIT		);
	Gs *pGs= new Gs(&HBV_GWLev, SINGLE, WATER, RUNOFF ,NOBPROC, NORMAL);
	Define		(pGs,"HBV GroundWaterLevel", 0.0,UNIT_TYPES::WATERFLOW_UNIT		);
	pTs= new Ts(&Q, SINGLE, WATER, RUNOFF,NOBPROC, NORMAL);
	Define		(pTs, "HBV RunOff", 0.0,UNIT_TYPES::WATERFLOW_UNIT		);
	pTs= new Ts(&HBV_Evaporation, SINGLE, WATER, EVAPOTRANSPIRATION ,NOBPROC, NORMAL);
	Define		(pTs, "HBV Evap", 0.0,UNIT_TYPES::WATERFLOW_UNIT		,"");

return true;
}

