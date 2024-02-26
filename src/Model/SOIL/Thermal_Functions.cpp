#include "Thermal_Functions.h"
#include "../PLANT/PlantModel.h"
#include "../SOIL/SoilModel.h"
#include "../SOIL/Functors/Profile_Functors.h"

Thermal_Functions::Thermal_Functions(void)
{
	SolidHeatCapDist=0;
}
Thermal_Functions::~Thermal_Functions(void)
{
}
bool Thermal_Functions::Def()
{

	PLOT_PARAM plotpar;
	plotpar.Num_Of_Curves = 1;
	plotpar.Num_Of_Points = 100;
	plotpar.Num_Of_X = 1;
	plotpar.X_Min = 0;
	plotpar.X_Max = 100;
	plotpar.X_Label = "X";
	plotpar.Y_Label = "Y";
	plotpar.LogXScale = false;
	plotpar.LogYScale = false;
	plotpar.Num_Of_Opt = 1;
	ModuleNo=6;
	SoilModel* pSoil = (SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
	p_Soil_WaterF = &pSoil->m_SoilFrost;
	Sw *pSw;
	pSw= new Sw(&ThCondFunction,SIMPROFILE,HEAT, HEAT_CONDUCTION,NOBPROC, NORMAL);

    Define(pSw  ,"Thermal Conductivity Function",0,"Kersten|Balland&Arp","HeatEq >= 1", 4 ); // 4 is ReCalc Function

	Func *pFunc;
	string funcname;

	FunctorProp functorprop;

	funcname = "ReCalc Soil Thermal Function";
	pFunc = new Func(MPROFILE, HEAT, HEAT_CONDUCTION, NOBPROC, NORMAL);


	functorprop.FuncName = MyFunc::BALLAND_ARP_RECALC;
	functorprop.Arg = MyArgType::BLANK;
	Define(pFunc, functorprop, this, funcname, plotpar);

	//auto tt = pFunc->MakeOperation();

	P* pP;
	vector<P*> p_vec;
	vector<Ps*> ps_vec;
	ps_vec.clear();



	Ps* pPs;
	pPs = new Ps(&Th_a, SIMPROFILE, HEAT, HEAT_CONDUCTION, NOBPROC, DETAILED); ps_vec.push_back(pPs);
	Define(pPs, "Balland_Arp a", 0.53, "-", "HeatEq >= 1|Thermal Conductivity Function=1", 0.4, 0.7);
	pPs=new Ps(&Th_Alfa, SIMPROFILE, HEAT, HEAT_CONDUCTION, NOBPROC, DETAILED); ps_vec.push_back(pPs);
	Define(pPs,"Balland_Arp Alpha",.24,"-","HeatEq >= 1|Thermal Conductivity Function=1", .1,.4);	
	pPs=new Ps(&Th_Beta, SIMPROFILE, HEAT, HEAT_CONDUCTION, NOBPROC, DETAILED); ps_vec.push_back(pPs);
	Define(pPs,"Balland_Arp Beta",18.3,"-","HeatEq >= 1|Thermal Conductivity Function=1", 10.,30.);	

	functorprop.FuncName = MyFunc::THERMALCOND_MEAS_BALLAND_ARP; functorprop.Arg = MyArgType::T_D2;
	funcname = "ThermalConduction Balland_Arp"; functorprop.FunctorInitOption = string::npos;
	functorprop.Set_PlotPar = true;
	
	p_vec.push_back(m_pModelMap->GetPPointer("Sand Content"));
	p_vec.push_back(m_pModelMap->GetPPointer("Gravel Content"));
	p_vec.push_back(m_pModelMap->GetPPointer("Organic Content"));
	p_vec.push_back(m_pModelMap->GetPPointer("Quarts fraction of Sand"));

	p_vec.push_back(m_pModelMap->GetPPointer("Saturation"));

	plotpar.Num_Of_Curves = 2;
	plotpar.Num_Of_Points = 100;
	plotpar.Num_Of_X = 2;
	plotpar.X_Min = 0;
	plotpar.X_Max = 10;
	plotpar.X_MaxPar = "mSaturation";
	plotpar.X_Label = "Water Content (vol %)";
	plotpar.Y_Label = "Thermal Conductivity (W/m/C)";
	plotpar.LogXScale = false;
	plotpar.LogYScale = false;
	plotpar.Num_Of_Opt = 2;
	plotpar.X2_Vec = { 0.,1. };
	plotpar.LegendsVec = { "unfrozen","frozen" };
	funcname = "ThermalConduction Orig Layers Balland_Arp";

	functorprop.FuncName = MyFunc::THERMALCOND_MEAS_BALLAND_ARP;
	pFunc = new Func(MPROFILE, HEAT, HEAT_CONDUCTION, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, p_vec, plotpar); p_vec.clear();
	
	
	
	
	ps_vec.clear();

	function<void(vector<double>, vector<double>&, vector<double>&, bool)> f_ModelLayers = FromMeasuredLayersToModelLayers((m_pModelMap->GetPPointer("Upper Depth"))->GetVector(),
		m_pModelMap->GetPPointer("Lower Depth")->GetVector(),
		m_pModelMap->GetPPointer("DepthLayers")->GetVector(),
		m_pModelMap->GetPPointer("Thickness Layers")->GetVector(),
		m_pModelMap->GetPPointer("DepthMidpLayers")->GetVector());

	vector<double> DummyFracMidPoint, initv;

	f_ModelLayers(m_pModelMap->GetPPointer("Quarts fraction of Sand")->GetVector(), DummyFracMidPoint, initv, true);

	NE* pNL;
	pNL = m_pModelMap->GetNEPointer("Number of Layers");

	pP = new P(&m_Quarts, pNL, SIMPROFILE, HEAT, HEAT_CONDUCTION, NOBPROC, DETAILED);
	mTab_ThScale.push_back(pP);

	Define(pP, pNL, "mQuarts", initv, "-", "HeatEq >= 1|Thermal Conductivity Function=1", 0., 100.);

	pP = new P(&m_SandFrac, pNL, SIMPROFILE, HEAT, HEAT_CONDUCTION, NOBPROC, DETAILED);
	mTab_ThScale.push_back(pP);
	f_ModelLayers(m_pModelMap->GetPPointer("Sand Content")->GetVector(), DummyFracMidPoint, initv, true);

	Define(pP, pNL, "mSandFrac", initv, "-", "HeatEq >= 1|Thermal Conductivity Function=1", 0., 100.);

	pP = new P(&m_GravelFrac, pNL, SIMPROFILE, HEAT, HEAT_CONDUCTION, NOBPROC, DETAILED);
	mTab_ThScale.push_back(pP);
	f_ModelLayers(m_pModelMap->GetPPointer("Gravel Content")->GetVector(), DummyFracMidPoint, initv, true);
	Define(pP, pNL, "mGravelFrac", initv, "-", "HeatEq >= 1|Thermal Conductivity Function=1", 0., 100.);

	pP = new P(&m_OrgFrac, pNL, SIMPROFILE, HEAT, HEAT_CONDUCTION, NOBPROC, DETAILED);
	mTab_ThScale.push_back(pP);


	f_ModelLayers(m_pModelMap->GetPPointer("Organic Content")->GetVector(), DummyFracMidPoint, initv, true);
	Define(pP, pNL, "mOrgFrac", initv, "-", "HeatEq >= 1|Thermal Conductivity Function=1", 0., 100.);
	Tab* pTab;
	pTab = new  Tab(&mTab_ThScale, SIMPROFILE, HEAT, STORAGE, NOBPROC, DETAILED);

	Define(pTab, "Thermal Conductivity, Model Layers", 1);


	funcname = "ThermalConduction Model Layers Balland_Arp";
	functorprop.FuncName = MyFunc::THERMALCOND_MODEL_BALLAND_ARP;
	ps_vec.clear(); p_vec.clear();

	ps_vec.push_back(m_pModelMap->GetPsPointer("Balland_Arp a"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("Balland_Arp Alpha"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("Balland_Arp Beta"));


	p_vec.push_back(m_pModelMap->GetPPointer("mSandFrac"));
	p_vec.push_back(m_pModelMap->GetPPointer("mGravelFrac"));
	p_vec.push_back(m_pModelMap->GetPPointer("mOrgFrac"));
	p_vec.push_back(m_pModelMap->GetPPointer("mQuarts"));
	p_vec.push_back(m_pModelMap->GetPPointer("mSaturation"));
	p_vec.push_back(m_pModelMap->GetPPointer("mUpper Depth"));
	p_vec.push_back(m_pModelMap->GetPPointer("mLower Depth"));
	p_vec.push_back(m_pModelMap->GetPPointer("DepthLayers"));
	p_vec.push_back(m_pModelMap->GetPPointer("Thickness Layers"));
	p_vec.push_back(m_pModelMap->GetPPointer("DepthMidpLayers"));

	pFunc = new Func(MPROFILE, HEAT, HEAT_CONDUCTION, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, p_vec, plotpar); ps_vec.clear(); p_vec.clear();






	p_vec.push_back(m_pModelMap->GetPPointer("Saturation"));
	p_vec.push_back(m_pModelMap->GetPPointer("Upper Depth"));
	p_vec.push_back(m_pModelMap->GetPPointer("Lower Depth"));
	p_vec.push_back(m_pModelMap->GetPPointer("DepthLayers"));
	p_vec.push_back(m_pModelMap->GetPPointer("Thickness Layers"));
	p_vec.push_back(m_pModelMap->GetPPointer("DepthMidpLayers"));

	pFunc = new Func(MPROFILE, HEAT, HEAT_CONDUCTION, NOBPROC, NORMAL);

	functorprop.FuncName = MyFunc::DRYBULKDENSITY; functorprop.Arg = MyArgType::BLANK;
	funcname = "Dry Bulk Density"; functorprop.FunctorInitOption = string::npos;
	Define(pFunc, functorprop, funcname, ps_vec, p_vec, plotpar); p_vec.clear(); ps_vec.clear();

	function<vector<double>(bool)> f_DryBulk = GetVector_boolBlank_Functor(MyFunc::DRYBULKDENSITY);
	vector<double> bd = f_DryBulk(true);

	
	pP = new P(&BulkDenS, pNL, SIMPROFILE, HEAT, HEAT_CONDUCTION, NOBPROC, DETAILED);
	Define(pP, pNL, "Bulk_DenS", bd, "-", "HeatEq >= 1", -3., 3., -1., 1.);

	p_vec.clear();
	p_vec.push_back(m_pModelMap->GetPPointer("mSaturation"));
	p_vec.push_back(m_pModelMap->GetPPointer("Thickness Layers"));

	pFunc = new Func(MPROFILE, HEAT, STORAGE , NOBPROC, NORMAL);

	functorprop.FuncName = MyFunc::HEATCAPACITY_SOLID; functorprop.Arg = MyArgType::BLANK;
	funcname = "Heat Capacity Solid Frac Soil"; functorprop.FunctorInitOption = string::npos;
	Define(pFunc, functorprop, funcname, ps_vec, p_vec, plotpar); p_vec.clear(); ps_vec.clear();
	function<vector<double>()> f_HeatCapSoil = GetVector_Blank_Functor(MyFunc::HEATCAPACITY_SOLID);
	auto ccc = f_HeatCapSoil();

	pPs = new Ps(&OrganicLayerThickness, SINGLE, HEAT, HEAT_CONDUCTION, NOBPROC, DETAILED); ps_vec.push_back(pPs);
	Define(pPs, "OrganicLayerThick", 0., "m", "HeatEq >= 1", 0., 100., 0.0, 10.); 
	

	pPs = new Ps(&CHumuS1, SINGLE, HEAT, HEAT_CONDUCTION, NOBPROC, DETAILED); ps_vec.push_back(pPs);
		Define(pPs, "OrganicC1", .06, "-", "HeatEq >= 1|Thermal Conductivity Function=0", 1.E-2, 1.E2, 0.01, 100.);
	pPs = new Ps(&CHumuS2, SINGLE, HEAT, HEAT_CONDUCTION, NOBPROC, DETAILED); ps_vec.push_back(pPs);
	Define(pPs, "OrganicC2", .005, "-", "HeatEq >= 1|Thermal Conductivity Function=0", 1.E-3, 1.E2, 0.001, 100.);
	pPs = new Ps(&CFrozenHumus, SINGLE, HEAT, HEAT_CONDUCTION, NOBPROC, DETAILED); ps_vec.push_back(pPs);
	Define(pPs, "OrganicFrozenC", 2., "-", "HeatEq >= 1|Thermal Conductivity Function=0", 0., 4., 1.5, 3.);

	pPs = new Ps(&Clay.UC1, SINGLE, HEAT, HEAT_CONDUCTION, NOBPROC, DETAILED); ps_vec.push_back(pPs);
	Define(pPs, "ClayUnFrozenC1", .13, "-", "HeatEq >= 1|Thermal Conductivity Function=0", 1.E-2, 1.E2, 0.01, 100.);
	pPs = new Ps(&Clay.UC2, SINGLE, HEAT, HEAT_CONDUCTION, NOBPROC, DETAILED); ps_vec.push_back(pPs);
	Define(pPs, "ClayUnFrozenC2", -0.029, "-", "HeatEq >= 1|Thermal Conductivity Function=0", -1., 1.E2, -1., 100.);
	pPs = new Ps(&Clay.UC3, SINGLE, HEAT, HEAT_CONDUCTION, NOBPROC, DETAILED); ps_vec.push_back(pPs);
	Define(pPs, "ClayUnFrozenC3", .6245, "-", "HeatEq >= 1|Thermal Conductivity Function=0", 1.E-2, 1.E2, 0.01, 100.);



	pPs=new Ps(&Clay.FC1, SINGLE, HEAT, HEAT_CONDUCTION, NOBPROC, DETAILED); ps_vec.push_back(pPs);
	Define(pPs,"ClayFrozenC1",.00144,"-","HeatEq >= 1|Thermal Conductivity Function=0", 1.E-3,1.E2,0.001,100.);	
	pPs=new Ps(&Clay.FC2, SINGLE, HEAT, HEAT_CONDUCTION, NOBPROC, DETAILED); ps_vec.push_back(pPs);
	Define(pPs,"ClayFrozenC2",1.32,"-","HeatEq >= 1|Thermal Conductivity Function=0", 1.E-2,1.E2,0.01,100.);
	pPs=new Ps(&Clay.FC3, SINGLE, HEAT, HEAT_CONDUCTION, NOBPROC, DETAILED); ps_vec.push_back(pPs);
	Define(pPs,"ClayFrozenC3",0.0036,"-","HeatEq >= 1|Thermal Conductivity Function=0", 1.E-3,1.E2,0.01,100.);	
	pPs=new Ps(&Clay.FC4, SINGLE, HEAT, HEAT_CONDUCTION, NOBPROC, DETAILED); ps_vec.push_back(pPs);
	Define(pPs,"ClayFrozenC4",0.8743,"-","HeatEq >= 1|Thermal Conductivity Function=0", 1.E-2,1.E2,0.01,100.);	

	pPs = new Ps(&Sand.UC1, SINGLE, HEAT, HEAT_CONDUCTION, NOBPROC, DETAILED); ps_vec.push_back(pPs);
	Define(pPs, "SandUnFrozenC1", .10, "-", "HeatEq >= 1|Thermal Conductivity Function=0", 1.E-2, 1.E2, 0.01, 100.);
	pPs = new Ps(&Sand.UC2, SINGLE, HEAT, HEAT_CONDUCTION, NOBPROC, DETAILED); ps_vec.push_back(pPs);
	Define(pPs, "SandUnFrozenC2", .058, "-", "HeatEq >= 1|Thermal Conductivity Function=0", 1.E-2, 1.E2, 0.01, 100.);
	pPs = new Ps(&Sand.UC3, SINGLE, HEAT, HEAT_CONDUCTION, NOBPROC, DETAILED); ps_vec.push_back(pPs);
	Define(pPs, "SandUnFrozenC3", .6245, "-", "HeatEq >= 1|Thermal Conductivity Function=0", 1.E-2, 1.E2, 0.01, 100.);

	pPs=new Ps(&Sand.FC1, SINGLE, HEAT, HEAT_CONDUCTION, NOBPROC, DETAILED); ps_vec.push_back(pPs);
	Define(pPs,"SandFrozenC1",.00158,"-","HeatEq >= 1|Thermal Conductivity Function=0", 1.E-4,1.E2,0.01,100.);	
	pPs=new Ps(&Sand.FC2, SINGLE, HEAT, HEAT_CONDUCTION, NOBPROC, DETAILED); ps_vec.push_back(pPs);
	Define(pPs,"SandFrozenC2",1.336,"-","HeatEq >= 1|Thermal Conductivity Function=0", 1.E-2,1.E2,0.01,100.);	
	pPs=new Ps(&Sand.FC3, SINGLE, HEAT, HEAT_CONDUCTION, NOBPROC, DETAILED); ps_vec.push_back(pPs);
	Define(pPs,"SandFrozenC3",.00375,"-","HeatEq >= 1|Thermal Conductivity Function=0", 1.E-3,1.E2,0.01,100.);	
	pPs=new Ps(&Sand.FC4, SINGLE, HEAT, HEAT_CONDUCTION, NOBPROC, DETAILED); ps_vec.push_back(pPs);
	Define(pPs,"SandFrozenC4",.9118,"-","HeatEq >= 1|Thermal Conductivity Function=0", 1.E-2,1.E2,0.01,100.);	




	p_vec.push_back(m_pModelMap->GetPPointer("Bulk_DenS"));

	
	initv.resize(p_Soil_WaterF->NL);
	for(auto i=initv.begin(); i!=initv.end();++i) *i=0.;

	pP=new P(&ThScaleLog,pNL, SIMPROFILE, HEAT, HEAT_CONDUCTION, NOBPROC, DETAILED); p_vec.push_back(pP);
	for (auto i = initv.begin(); i != initv.end(); ++i) *i = 0.;
	Define(pP, pNL,"ThScaleLog",initv,"-","HeatEq >= 1|Thermal Conductivity Function=0;", -3.,3.,-1.,1., 5);  // 5 is recalc to thermal properties functions

	p_vec.push_back(m_pModelMap->GetPPointer("DepthMidpLayers"));
	p_vec.push_back(m_pModelMap->GetPPointer("DistanceMidpLayers"));
	p_vec.push_back(m_pModelMap->GetPPointer("bLambda"));
	p_vec.push_back(m_pModelMap->GetPPointer("mWilting Point"));
	functorprop.Set_PlotPar = true;
	pFunc = new Func(MPROFILE, HEAT, HEAT_CONDUCTION, NOBPROC, NORMAL);
	functorprop.FuncName = MyFunc::THERMALCOND_KERSTEN; functorprop.Arg = MyArgType::T_D2;
	funcname = "ThermalConduction Kersten"; functorprop.FunctorInitOption = string::npos;

	Define(pFunc, functorprop, funcname, ps_vec, p_vec, plotpar); p_vec.clear(); ps_vec.clear();

	f_ThermalCond = Get_D2_TableFunctor(MyFunc::THERMALCOND_KERSTEN);
	auto kk = f_ThermalCond(0, 20., 0.);


	ps_vec.push_back(m_pModelMap->GetPsPointer("OrganicC1"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("OrganicC2"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("OrganicFrozenC"));

	pFunc = new Func(SINGLE, HEAT, HEAT_CONDUCTION, NOBPROC, NORMAL);
	functorprop.FuncName = MyFunc::THERMALCOND_ORGANIC; functorprop.Arg = MyArgType::D2;
	funcname = "Thermal Conductivity Organic Soil"; functorprop.FunctorInitOption = string::npos;
	functorprop.Set_PlotPar = true;
	plotpar.Num_Of_Curves = 2;
	plotpar.Num_Of_Points = 100;
	plotpar.Num_Of_X = 2;
	plotpar.X_Min = 0;
	plotpar.X_Max = 10;
	plotpar.X_MaxPar = "mSaturation";
	plotpar.X_Label = "Water Content (vol %)";
	plotpar.Y_Label = "Thermal Conductivity (W/m/C)";
	plotpar.LogXScale = false;
	plotpar.LogYScale = false;
	plotpar.Num_Of_Opt = 1;
	plotpar.X2_Vec = { 0.,1. };
	plotpar.LegendsVec = { "unfrozen","frozen" };




	Define(pFunc, functorprop, funcname, ps_vec, p_vec, plotpar); p_vec.clear(); ps_vec.clear();



	ps_vec.push_back(m_pModelMap->GetPsPointer("ClayUnFrozenC1"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("ClayUnFrozenC2"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("ClayUnFrozenC3"));


	ps_vec.push_back(m_pModelMap->GetPsPointer("ClayFrozenC1"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("ClayFrozenC2"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("ClayFrozenC3"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("ClayFrozenC4"));
	
	pFunc = new Func(SINGLE, HEAT, HEAT_CONDUCTION, NOBPROC, NORMAL);
	functorprop.FuncName = MyFunc::THERMALCOND_KERSTEN_CLAY; functorprop.Arg = MyArgType::D3;
	funcname = "Thermal Conductivity - Kersten Clay"; functorprop.FunctorInitOption = string::npos;
	functorprop.Set_PlotPar = true;
	plotpar.Num_Of_Curves = 4;
	plotpar.Num_Of_Points = 100;
	plotpar.Num_Of_X = 3;
	plotpar.X_Min = 0;
	plotpar.X_Max = 10;
	plotpar.X_MaxPar = "mSaturation";
	plotpar.X_Label = "Water Content (vol %)";
	plotpar.Y_Label = "Thermal Conductivity (W/m/C)";
	plotpar.LogXScale = false;
	plotpar.LogYScale = false;
	plotpar.Num_Of_Opt = 1;
	plotpar.X2_Vec = { 1.f,1.f,1.6f,1.6f };
	plotpar.X3_Vec = { 0.f,1.f, 0.f, 1.f };
	plotpar.LegendsVec = { "unfrozen- BD 1.0","frozen - BD 1.0" ,"unfrozen- BD 1.6","frozen - BD 1.6" };




	Define(pFunc, functorprop, funcname, ps_vec, p_vec, plotpar); p_vec.clear(); ps_vec.clear();

	ps_vec.push_back(m_pModelMap->GetPsPointer("SandUnFrozenC1"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("SandUnFrozenC2"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("SandUnFrozenC3"));


	ps_vec.push_back(m_pModelMap->GetPsPointer("SandFrozenC1"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("SandFrozenC2"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("SandFrozenC3"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("SandFrozenC4"));

	functorprop.FuncName = MyFunc::THERMALCOND_KERSTEN_SAND; functorprop.Arg = MyArgType::D3;
	funcname = "Thermal Conductivity - Kersten Sand"; functorprop.FunctorInitOption = string::npos;

	pFunc = new Func(SINGLE, HEAT, HEAT_CONDUCTION, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, p_vec, plotpar); p_vec.clear(); ps_vec.clear();



	mTab_ThScale.push_back(pP);


	functorprop.FuncName = MyFunc::THERMALCOND_KERSTEN; functorprop.Arg = MyArgType::T_D2;
	funcname = "Thermal Conductivity - Kersten"; functorprop.FunctorInitOption = string::npos;

	pFunc = new Func(SINGLE, HEAT, HEAT_CONDUCTION, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, p_vec, plotpar); p_vec.clear(); ps_vec.clear();


	vector<double> P1[5];
	auto koll = (m_pModelMap->GetPPointer("Upper Depth"))->GetVector();
	






	pP=new P(&K_IndType,pNL, SIMPROFILE, HEAT, HEAT_CONDUCTION, NOBPROC, DETAILED);
		for(auto i=initv.begin(); i!=initv.end();++i) *i=0;
		Define(pP, pNL,"K_IndType",initv,"-","HeatEq >= 1|Thermal Conductivity Function=0", -3.,3.,-1.,1.);


	pPs = new Ps(&CFrozenSurfCorr, SINGLE, HEAT, HEAT_CONDUCTION, NOBPROC, DETAILED); ps_vec.push_back(pPs);
		Define(pPs, "CFrozenSurfCorr", 0.2, "1/C", "HeatEq >= 1", 0.01, 1., 0.1, 0.5);
	pPs = new Ps(&CFrozenMaxDamp, SINGLE, HEAT, HEAT_CONDUCTION, NOBPROC, DETAILED); ps_vec.push_back(pPs);
		Define(pPs, "CFrozenMaxDamp", 0.9, "-", "HeatEq >= 1", 0., 1., 0., 0.9);

	pFunc = new Func(MPROFILE, HEAT, HEAT_CONDUCTION, NOBPROC, NORMAL);
	functorprop.FuncName = MyFunc::EMPIRICALSOILSURFACEDAMPING; functorprop.Arg = MyArgType::D1;
	funcname = "Empirical Reduction of Conduction because of freezing of uppermost layer"; functorprop.FunctorInitOption = string::npos;
	plotpar.Num_Of_Curves = 1;
	plotpar.Num_Of_Points = 100;
	plotpar.Num_Of_X = 1;
	plotpar.X_Min = -10;
	plotpar.X_Max = 0;
	plotpar.X_MaxPar = "";
	plotpar.X_Label = "Temperature (C)";
	plotpar.Y_Label = "Degree of Thermal Conductivity (-)";
	plotpar.LogXScale = false;
	plotpar.LogYScale = false;
	plotpar.Num_Of_Opt = 1;
	plotpar.X2_Vec = {};
	plotpar.X3_Vec = {};
	plotpar.LegendsVec = {};
	
	Define(pFunc, functorprop, funcname, ps_vec, p_vec,plotpar); p_vec.clear(); ps_vec.clear();


	pP=new P(&HeatCapSolid,pNL, SIMPROFILE, HEAT, STORAGE, NOBPROC, DETAILED);
		Define(pP, pNL,"HeatCapSolid",initv,"-","HeatEq >= 1", -3.,3.,-1.,1.);

	pP=new P(&HeatCapSolidDist,pNL, SIMPROFILE, HEAT, STORAGE, NOBPROC, DETAILED);
		for(auto i=initv.begin(); i!=initv.end();++i) *i=2.E6;
		Define(pP, pNL,"C bulk",initv,"J/m3","HeatEq >= 1|SolidHeatCapDist", 1.E5,4.E6);
	
	mTab_HeatCap.push_back(pP);


//

	return true;

}


bool Thermal_Functions::Thermal_ReCalc() noexcept 
{
	    HumusMixedBoundaryId=0;
		FractionOfOrgMixedBoundary=0.;
		if(OrganicLayerThickness>0.&&OrganicLayerThickness<p_Soil_WaterF->DepthMidpLayer.front()) {
			HumusMixedBoundaryId=0;
			FractionOfOrgMixedBoundary=OrganicLayerThickness/p_Soil_WaterF->DepthMidpLayer.front();
		}

	//	f_ReCalc = InitSoilThermalProperties(p_Soil_WaterF, (Soil_HeatF*)this, OrganicLayerThickness);

		ResIntegDepth=p_Soil_WaterF->DepthMidpLayer.front();

		for(size_t i=0; i<K_IndType.size(); i++) {
//!		Check for Soil type
		  if(OrganicLayerThickness>p_Soil_WaterF->DepthMidpLayer[i])
		    K_IndType[i]=0;		//	! Organic Soil
		  else if(p_Soil_WaterF->B_Lambda[i]<.1||p_Soil_WaterF->M_Wilting[i]>7.) 
//!   Check K_IndType condition to OR to make it more flexible for using also Genuchten equation
//! Lambda changed from 0.5 to 0.1 in connection with this
		     K_IndType[i]=2;	//		! Clay Soil type
		  else
             K_IndType[i]=1;//			! Sand Soil type
	
		            
		  if(i<K_IndType.size()-1) {
			if(OrganicLayerThickness>=p_Soil_WaterF->DepthMidpLayer[i]&&			
			   OrganicLayerThickness<p_Soil_WaterF->DepthMidpLayer[i+1] ) {
					HumusMixedBoundaryId=i+1;
					FractionOfOrgMixedBoundary=(OrganicLayerThickness-p_Soil_WaterF->DepthMidpLayer[i])/ 
					p_Soil_WaterF->DistanceMidpLayers[i+1];
			}
		  }

          BulkDenS[i]=(100-p_Soil_WaterF->B_Saturation[i])/100.*Fix::DensityofSolidSoil;
		  if(SolidHeatCapDist) 
	          HeatCapSolid[i]=(HeatCapSolidDist[i]*(100-p_Soil_WaterF->M_Saturation[i])/100.)*p_Soil_WaterF->ThicknessLayers[i];
		  else
	          HeatCapSolid[i]=(Fix::HeatCapSolidSoil*(100-p_Soil_WaterF->M_Saturation[i])/100.)*p_Soil_WaterF->ThicknessLayers[i];
		  
	

		  if(p_Soil_WaterF->DepthMidpLayer[i]<0.2) 
			ResIntegDepth=p_Soil_WaterF->DepthMidpLayer[i];
		  
		}

		function<void(vector<double>, vector<double>&, vector<double>&, bool)> f_ModelLayers = FromMeasuredLayersToModelLayers((m_pModelMap->GetPPointer("Upper Depth"))->GetVector(),
			m_pModelMap->GetPPointer("Lower Depth")->GetVector(),
			m_pModelMap->GetPPointer("DepthLayers")->GetVector(),
			m_pModelMap->GetPPointer("Thickness Layers")->GetVector(),
			m_pModelMap->GetPPointer("DepthMidpLayers")->GetVector());

		vector<double> DummyFracMidPoint, newvalues;

		f_ModelLayers(m_pModelMap->GetPPointer("Quarts fraction of Sand")->GetVector(), DummyFracMidPoint, newvalues, true);
		m_pModelMap->GetPPointer("mQuarts")->SetValue(newvalues);

		f_ModelLayers(m_pModelMap->GetPPointer("Sand Content")->GetVector(), DummyFracMidPoint, newvalues, true);
		m_pModelMap->GetPPointer("mSandFrac")->SetValue(newvalues);	
		
		f_ModelLayers(m_pModelMap->GetPPointer("Gravel Content")->GetVector(), DummyFracMidPoint, newvalues, true);
		m_pModelMap->GetPPointer("mGravelFrac")->SetValue(newvalues);

		f_ModelLayers(m_pModelMap->GetPPointer("Organic Content")->GetVector(), DummyFracMidPoint, newvalues, true);
		m_pModelMap->GetPPointer("mOrgFrac")->SetValue(newvalues);
		return true;
}





double Thermal_Functions::SurfFlowDamp( double SurfTemp)
{
  if(SurfTemp>=0)
	  return 1.;
  else
	  return exp(CFrozenSurfCorr*SurfTemp)*CFrozenMaxDamp+(1-CFrozenMaxDamp);

}
double Thermal_Functions::DampDepthYear(double Theta)
{

   	double Tcon, HeatCap, Diffu, CYC;

	size_t layer = 1;
	if (HeatCapSolid.size()<2) layer = 0;
     Tcon=f_ThermalCond(layer,Theta,0.);
	 HeatCap=HeatCapSolid[layer]/p_Soil_WaterF->ThicknessLayers[layer]+Theta*4.2E4;
     Diffu=Tcon/HeatCap ;
     CYC=2*Fix::PI/(365.25*86400.);
     return sqrt(2.*Diffu/CYC);
}
double Thermal_Functions::DampDepthDay(double Theta) 
{
    return DampDepthYear(Theta)/sqrt(365.25);
}
double Thermal_Functions::SoilHeatCap(size_t index, double Theta, double ThQual)
{
  return HeatCapSolid[index]/p_Soil_WaterF->ThicknessLayers[index]+
	ThQual*Theta*Fix::SPECI/100. + (1-ThQual)*Theta*Fix::SPECW/100.;

}
double Thermal_Functions::SoilHeat_TempF(size_t index, double Temp, double Wat)
{
	//Use PhysConst	
	///Use hydraulic, Only	: M_Wilting, M_Lambda 
//	Use Commonstruc, Only	: FpointF0,FpointF1,FpointFWi

	double   NonFrozenWat, MFWat, HeatCap, EFrost, Q, ELOWER,EUPPER, RatioLatent, SensibleHeat, TempLocal, EMiddle;
	double TempFrozen;
	if(Temp<=0.) {
		//!  NON-FREEZ WAT
		NonFrozenWat = min(10.*p_Soil_WaterF->M_Wilting[index]*p_Soil_WaterF->ThicknessLayers[index]*p_Soil_WaterF->FpointFWi, Wat);
//!C ******   FROZEN WATER <MM>
        MFWat = Wat - NonFrozenWat;
		HeatCap=HeatCapSolid[index]+MFWat*Fix::SPECI + NonFrozenWat*Fix::SPECW;
        if(Temp<=Fix::TempCompFrozenSoil) 
			TempFrozen=HeatCap*Temp-Fix::LATMELT*MFWat;
		else {
//!			david=0
//!			if(david.gt.0)then
//!			TempFrozen=-1.*(Tempv/TempCompFrozenSoil)**(1/(M_Lambda[index]*FpointF1)+FpointF0) &
//!	                     *LATMELT*MFWat+HeatCap*Tempv
//!			else
//			! iterative solution (simple but stable bisection method)
//			! 50 iterations takes approximately 0.1 second computation time
			EFrost=HeatCap*Fix::TempCompFrozenSoil-Fix::LATMELT*MFWat;
			ELOWER=EFrost;
			EUPPER=0.0;
			EMiddle=(EUPPER+ELOWER)*0.5;
			for(int j=0; j<50; j++) {
			//	! freezing point depression

				Q=(1-pow(EMiddle/EFrost,(p_Soil_WaterF->FpointF0+p_Soil_WaterF->FpointF1*p_Soil_WaterF->M_Lambda[index]))) * 
					min(1.,(EFrost-EMiddle) /(EFrost+Fix::LATMELT*MFWat));
      			//! ratio of latent heat to total heat at Tf
      			RatioLatent=-Fix::LATMELT*MFWat/EFrost;
      			//! sensible heat at any heat storage
      			SensibleHeat=EMiddle-RatioLatent*(1-Q)*EMiddle-Q*EMiddle;
				TempLocal=SensibleHeat/HeatCap;
				//! change estimate of heat storage if TempLocal too high or low
				if((TempLocal-Temp)>=0.0) {
					EUPPER=EMiddle;
         			EMiddle=(EUPPER+ELOWER)*0.5;
				}
				else {
         			ELOWER=EMiddle;
         			EMiddle=(EUPPER+ELOWER)*0.5;
				}
			}
			TempFrozen=EMiddle;
		}
	}
	else {
        HeatCap=HeatCapSolid[index]+Wat*Fix::SPECW;
        TempFrozen=Temp*HeatCap;
	}
	return TempFrozen;
}
FrozenCond Thermal_Functions::Temp_SoilHeatF(size_t index, double SoilHeat, double WatV)
{
	FrozenCond out;

	WatV=max(0.001,WatV);
	m_HeatCapSoilWater = HeatCapSolid[index] + WatV*Fix::SPECW;;



	if (p_Soil_WaterF->FreezingPointOptions == 0)
		m_FreezingStartTemperature = 0.;
	else if (p_Soil_WaterF->FreezingPointOptions == 1)
		m_FreezingStartTemperature = p_Soil_WaterF->FreezingStartTemp;
	else {
		PlantModel* p_Plant = (PlantModel*)m_pModelInfo->GetModelPtr("Plant Model");
		WaterUptake* pPlant = (WaterUptake*)&p_Plant->m_WaterUptake;
		//Salt_Functions* pSalt = (Salt_Functions*)Fix::GetModulePtr("Plant");
		m_FreezingStartTemperature = FreezingStartFunction(pPlant->OsmoticPressure[index]);
	}





	double HeatUnFrozen = m_FreezingStartTemperature*m_HeatCapSoilWater;

    if(SoilHeat<HeatUnFrozen) {//		!	Partly FROZEN SOIL 
		double NonFrozenWat = NonFrozenFuncL(index, WatV); ///(10.*p_Soil_WaterF->M_Wilting[index]*p_Soil_WaterF->ThicknessLayers[index]*p_Soil_WaterF->FpointFWi, WatV) ;
		double MaxIceAmount = max(0., WatV - NonFrozenWat );
		m_HeatCapSoilIceMixture=HeatCapSolid[index]+MaxIceAmount*Fix::SPECI + NonFrozenWat*Fix::SPECW;
     
		double HeatCompleteFrozen=-Fix::LATMELT*MaxIceAmount+(Fix::TempCompFrozenSoil- m_FreezingStartTemperature)*m_HeatCapSoilIceMixture+ m_FreezingStartTemperature*m_HeatCapSoilWater;
	  
      if(SoilHeat>HeatCompleteFrozen) {  //	! Partially frozen
		 double HeatDiff;
		 HeatDiff = SoilHeat - HeatUnFrozen;
		 out=SoilThermalQualityFuncL(index, HeatDiff, HeatCompleteFrozen, MaxIceAmount, WatV);  
	  }
	  else  {
	  // Completely Frozen
		out.Temp=(SoilHeat+Fix::LATMELT*MaxIceAmount)/m_HeatCapSoilIceMixture;
	    out.ThQual= min(1., MaxIceAmount/WatV);
	  }
	}
	else {
		// Unfrozen soil

		out.Temp=SoilHeat/m_HeatCapSoilWater;
		out.ThQual=0;
	}
	return out;
}
// from General functions

double Thermal_Functions::NonFrozenFuncL(size_t index, double MassWater) {
	
	return NonFrozenFunc(p_Soil_WaterF->M_Wilting[index], p_Soil_WaterF->ThicknessLayers[index], MassWater);

}

double Thermal_Functions::NonFrozenFunc(double WiltingPoint, double ThicknessLayer,  double MassWater) {
	// Return the maximal amount of water that can freeze (mm)
	return min(10.*WiltingPoint * ThicknessLayer * p_Soil_WaterF->FpointFWi, MassWater);
}



FrozenCond Thermal_Functions::SoilThermalQualityFuncL(size_t index, double HeatDiff, double HeatCompleteFrozen, double MaxFrozenMassWater, double MassWater) {


	double HeatMaxIce= -Fix::LATMELT*MaxFrozenMassWater ;
	double HeatAllIce = -Fix::LATMELT*MassWater ;



	return SoilThermalQualityFunc(HeatDiff, HeatCompleteFrozen, HeatMaxIce, HeatAllIce, p_Soil_WaterF->M_Lambda[index]);

}
FrozenCond Thermal_Functions::SoilThermalQualityFunc(double HeatDiff, double HeatCompleteFrozen, double HeatMaxIce, double HeatAllIce, double Lambda) {
	// The fraction of heat as Ice. 0 = Unfrozen, 1 = Complete frozen	FrozenCond out;
	FrozenCond out;

	double Q = (1 - pow(HeatDiff / HeatCompleteFrozen, p_Soil_WaterF->FpointF0 + p_Soil_WaterF->FpointF1*Lambda))
		*min(1., (HeatCompleteFrozen - HeatDiff) / (HeatCompleteFrozen - HeatMaxIce));
	double RatioLatent = HeatMaxIce / HeatCompleteFrozen;
	double SensibleHeat = HeatDiff - RatioLatent*HeatDiff - (1 - RatioLatent)*Q*HeatDiff;

	out.ThQual= min(1., (HeatDiff - SensibleHeat) / HeatAllIce);
	out.Temp= SensibleHeat/ m_HeatCapSoilIceMixture+m_FreezingStartTemperature;
	return out;
}
double Thermal_Functions::Soil_Temp_HeatDemoFunc(double SoilHeat) {
	
	size_t index = 0;
	SoilHeat *= 1.E6;
	double model_layer_thick = p_Soil_WaterF->ThicknessLayers[index];
	p_Soil_WaterF->ThicknessLayers[index] = 1;
	FrozenCond cond;
	
	double Water = p_Soil_WaterF->M_Saturation[index]*10;

	m_HeatCapSoilWater = HeatCapSolidDist[index] * p_Soil_WaterF->M_Saturation[index]/100. + Water*Fix::SPECW;;
	m_FreezingStartTemperature = p_Soil_WaterF->FreezingStartTemp;
	double HeatUnFrozen = m_FreezingStartTemperature*m_HeatCapSoilWater;
	if (SoilHeat < HeatUnFrozen) {//		!	Partly FROZEN SOIL 
		double NonFrozenWat = NonFrozenFuncL(index, Water); 
		double MaxIceAmount = max(0., Water - NonFrozenWat);
		m_HeatCapSoilIceMixture = HeatCapSolidDist[index] * p_Soil_WaterF->M_Saturation[index] / 100. + MaxIceAmount*Fix::SPECI + NonFrozenWat*Fix::SPECW;

		double HeatCompleteFrozen = -Fix::LATMELT*MaxIceAmount + (Fix::TempCompFrozenSoil - m_FreezingStartTemperature)*m_HeatCapSoilIceMixture + m_FreezingStartTemperature*m_HeatCapSoilWater;

		if (SoilHeat > HeatCompleteFrozen) {  //	! Partially frozen
			double HeatDiff;
			HeatDiff = SoilHeat - HeatUnFrozen;
			cond = SoilThermalQualityFuncL(index, HeatDiff, HeatCompleteFrozen, MaxIceAmount, Water);
		}
		else {
			// Completely Frozen
			cond.Temp = (SoilHeat + Fix::LATMELT*MaxIceAmount) / m_HeatCapSoilIceMixture;

		}
	}
	else {
		cond.Temp = SoilHeat / m_HeatCapSoilWater;
	}

	p_Soil_WaterF->ThicknessLayers[index] = model_layer_thick;
	return cond.Temp;
}
double Thermal_Functions::FreezingStartFunction(double OsmoticPotential) {
	return pow(10, p_Soil_WaterF->OsmoticScaleSens)*Fix::TempToOsmoticP*OsmoticPotential;
}

