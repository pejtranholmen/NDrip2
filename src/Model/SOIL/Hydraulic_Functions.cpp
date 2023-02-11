#include "Hydraulic_Functions.h"
#include "./Functors/HydFunctors.h"
#include "../PLANT/PlantModel.h"
#include "../Atmosphere/AtmModel.h"
#include "../SOIL/SoilModel.h"
#include "../../Util/FUtil.hpp"



Hydraulic_Functions::Hydraulic_Functions(void)
{
}
Hydraulic_Functions::~Hydraulic_Functions(void)
{
}
bool Hydraulic_Functions::Def()
{
	Profile::Def();
	SoilModel* pSoil = (SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
	pHeat = &pSoil->m_HeatPump;
	PlantModel* p_Plant=(PlantModel*)m_pModelInfo->GetModelPtr("Plant Model");
	pPlant=(SimModule*)&p_Plant->m_WaterUptake;
	AtmModel* p_Atm=(AtmModel*)m_pModelInfo->GetModelPtr("Atm Model");
	pAtm=(SimModule*)&p_Atm->m_Irrigation;

	Sw* pSw;
	ModuleNo=5;
	pSw= new Sw(&HydraulicFunc, SIMPROFILE, WATER, STORAGE, NOBPROC, NORMAL);
 	Define(pSw,"Hydraulic Functions",0,"Brooks & Corey|Genuchten");
 	pSw= new Sw(&MatricCondFunc, SINGLE, WATER, TRANSPORT, NOBPROC, NORMAL);
	Define(pSw,"Matric Conductivity",0,"Independent|Same as Total Conductivity|Function of total conductivity");
	pSw= new Sw(&ConductivityFunc, SINGLE, WATER, TRANSPORT, NOBPROC, NORMAL);
	Define(pSw,"Conductivity Function",0,"Mualem|Power of relative saturation|Power of effective saturation","",1);
	pSw= new Sw(&ScalingRetention, SINGLE, WATER, STORAGE, NOBPROC, NORMAL);
	Define(pSw,"Scaling retention",0,"No|Yes","",1);
	pSw= new Sw(&PedoFunction, SINGLE, WATER, NOFPROC, NOBPROC, NORMAL);
	Define(pSw,"Pedo Functions",0,"Not Used|Texture parameters|Texture (database)|pF- points|pF- points (fixed residual)","",1);
	pSw= new Sw(&Replace_MissingLowCond, SINGLE, WATER, TRANSPORT, NOBPROC, NORMAL);
	Define(pSw,"Replace K-values",0,"No|Yes","",1);		

	Ps* pPs;
	pPs= new Ps(&A0Temp,SINGLE, WATER, TRANSPORT, NOBPROC, NORMAL);
	
	Define(pPs,"TempFacAtZero",.54,"-","WaterEq = 1", 0.4,1.0,0.5,0.6,0,0,403);	//!Scalar parameter
	pPs= new Ps(&A1Temp,SINGLE, WATER, TRANSPORT, NOBPROC, NORMAL);

	Define(pPs, "TempFacLinIncrease",0.023,"-","WaterEq = 1", 0.,0.03,0.02,0.025,0,0,403);	//!Scalar parameter
	pPs= new Ps(&CMinUC,SINGLE, WATER, TRANSPORT, NOBPROC, NORMAL);
	Define(pPs,"MinimumCondValue",1.E-5,"mm/day","WaterEq = 1", 1.E-20,1.E-1,1.E-14,1.E-3,0,0,402);	//!Scalar parameter
	pPs= new Ps(&CommonConductivityValue,SINGLE, WATER, TRANSPORT, NOBPROC, NORMAL);
	Define(pPs,"Common value", 10.,UNIT_TYPES::WATERFLOW_UNIT,"WaterEq = 1|Matric Conductivity =2", 1.E-03,100.,1.E-4,1000.,1,0,422);
	pPs= new Ps(&SensAboveThreshold,SINGLE, WATER, TRANSPORT, NOBPROC, NORMAL);
	Define(pPs,"Sensivity", 0.5,"-","WaterEq = 1|Matric Conductivity =2", 0.,1.,1,0,422);
	
/*	pPs= new Ps(&ScaleCoefResidual,SINGLE, WATER, TRANSPORT, NOBPROC, NORMAL);
	Define(pPs,"Scale Coef Residual", 1.0,"-","WaterEq = 1|Scaling retention > 0", 0.,2.,1,0,423);
	pPs= new Ps(&SaturationDiff,SINGLE, WATER, TRANSPORT, NOBPROC, NORMAL);
	Define(pPs,"Saturation Diff", 0.,"-","WaterEq = 1|Scaling retention > 0", -1.,5.,1,0,424);
	pPs= new Ps(&FixedResidualPar,SINGLE, WATER, TRANSPORT, NOBPROC, NORMAL);
    Define(pPs,"Fixed Residual", 1.,"vol %","WaterEq = 1|Pedo Functions>3", 0.001,30., 0.01,30.);*/
	
	NE *pNoLayerM =m_pModelMap->GetNEPointer("No Measured Layers");
	NoLayerM=pNoLayerM->GetNEValue();
 	vector<double> initv;

	P* pP;
	pP=m_pModelMap->GetPPointer("Upper Depth");

		
	mTab_BC_Measured.push_back(pP);mTab_HC_Measured.push_back(pP); mTab_Pedo.push_back(pP);mTab_Gen_Measured.push_back(pP);



	pP=m_pModelMap->GetPPointer("Lower Depth");

		mTab_BC_Measured.push_back(pP);mTab_HC_Measured.push_back(pP); mTab_Pedo.push_back(pP);mTab_Gen_Measured.push_back(pP); 	
		initv.resize(NoLayerM);
		for(auto i=initv.begin(); i!=initv.end();++i) *i=1.5;
	pP= new		P(&GM	,pNoLayerM, MPROFILE, WATER, TRANSPORT, NOBPROC, SIMPLE);
		Define(pP,pNoLayerM, "m-value",initv,"-","Hydraulic Functions = 1", 0.1, 10.,0.1,10.);
		mTab_Gen_Measured.push_back(pP);
		for(auto i=initv.begin(); i!=initv.end();++i) *i=0.5;
	
	pP= new		P(&GN,pNoLayerM, MPROFILE, WATER, TRANSPORT, NOBPROC, SIMPLE);
		Define(pP,pNoLayerM, "n-value",initv,"-","Hydraulic Functions = 1", 0.01, 10.,.01,10.);
		mTab_Gen_Measured.push_back(pP);
		
		for(auto i=initv.begin(); i!=initv.end();++i) *i=0.5;
	pP= new		P(&Galpha,pNoLayerM, MPROFILE, WATER, TRANSPORT, NOBPROC, SIMPLE);
		Define(pP,pNoLayerM, "Alpha",initv,"1/cm","Hydraulic Functions = 1", 0.01, 10.,.01,10.);
		mTab_Gen_Measured.push_back(pP);

		for(auto i=initv.begin(); i!=initv.end();++i) *i=0.3;
	pP= new		P(&Lambda,pNoLayerM, MPROFILE, WATER, STORAGE, NOBPROC, SIMPLE);
  		Define(pP,pNoLayerM,"Lambda",initv,"-","Hydraulic Functions = 0",0.001,50.,0.01);
		mTab_BC_Measured.push_back(pP);

		for(auto i=initv.begin(); i!=initv.end();++i) *i=10.;
	pP= new		P(&PsiEntry,pNoLayerM, MPROFILE, WATER, STORAGE, NOBPROC, SIMPLE);	
		Define(pP,pNoLayerM,"Air Entry",initv,"cm","Hydraulic Functions = 0",0.001,1000.,0.01,100.);
		mTab_BC_Measured.push_back(pP);

		for(auto i=initv.begin(); i!=initv.end();++i) *i=45.;
	pP= new		P(&Saturation,pNoLayerM, MPROFILE, WATER, STORAGE, NOBPROC, SIMPLE);
		Define(pP,pNoLayerM, "Saturation",initv,"vol %","Kalle 0",1.,99.,4.,90.);
		mTab_BC_Measured.push_back(pP);mTab_Pedo.push_back(pP);mTab_Gen_Measured.push_back(pP);

		for(auto i=initv.begin(); i!=initv.end();++i) *i=4.0;
	pP= new		P(&Wilting,pNoLayerM, MPROFILE, WATER, STORAGE, NOBPROC, SIMPLE);
		Define(pP,pNoLayerM,"Wilting Point",initv,"vol %","Kalle 0",0.,50.,0.1,30.);
		mTab_BC_Measured.push_back(pP);mTab_Gen_Measured.push_back(pP);
		for(auto i=initv.begin(); i!=initv.end();++i) *i=1.0;
	pP= new		P(&ResWater,pNoLayerM, MPROFILE, WATER, STORAGE, NOBPROC, SIMPLE);
		Define(pP,pNoLayerM,"Residual Water",initv,"vol %","Hydraulic Functions != 2",0.0,50.,0.1,50.);
		mTab_BC_Measured.push_back(pP);mTab_Gen_Measured.push_back(pP);
		for(auto i=initv.begin(); i!=initv.end();++i) *i=4.0;
	pP= new		P(&MacroPore,pNoLayerM, MPROFILE, WATER, STORAGE, NOBPROC, SIMPLE);
		Define(pP,pNoLayerM,"Macro Pore", initv,"vol %","WaterEq = 1",0.1,10.,1.,6.);
		mTab_BC_Measured.push_back(pP);

		for(auto i=initv.begin(); i!=initv.end();++i) *i=8000.;
	pP= new		P(&UpperBoundary,pNoLayerM, MPROFILE, WATER, STORAGE, NOBPROC, SIMPLE);
		Define(pP,pNoLayerM,"Upper Boundary",initv,"cm","Hydraulic Functions != 2",50.,1.5E6,500.,10000.,1);
		mTab_BC_Measured.push_back(pP); mTab_Gen_Measured.push_back(pP);
		
		for(auto i=initv.begin(); i!=initv.end();++i) *i=1000.;
	pP= new		P(&UpperBoundaryTheta,pNoLayerM, MPROFILE, WATER, STORAGE, NOBPROC, SIMPLE);
		Define(pP,pNoLayerM,"Upper ThetaBoundary",initv,"vol %","Hydraulic Functions != 2",0.,100.);

		for(auto i=initv.begin(); i!=initv.end();++i) *i=5.0;
	pP= new		P(&ClayContent,pNoLayerM, MPROFILE, WATER, STORAGE, NOBPROC, SIMPLE);
		Define(pP,pNoLayerM,"ClayContent",initv,"mass %","Pedo Functions>0|Pedo Functions<3",0,100);
		mTab_BC_Measured.push_back(pP);mTab_Pedo.push_back(pP);


		for(auto i=initv.begin(); i!=initv.end();++i) *i=70;
	pP= new		P(&SandContent,pNoLayerM, MPROFILE, WATER, STORAGE, NOBPROC, SIMPLE);
		Define(pP,pNoLayerM,"SandContent",initv,"mass %","Pedo Functions>0|Pedo Functions<3",0,100);
		mTab_BC_Measured.push_back(pP);mTab_Pedo.push_back(pP);
		
	for(auto i=initv.begin(); i!=initv.end();++i) *i=800.;
	pP= new		P(&ProfileNo,pNoLayerM, MPROFILE, WATER, STORAGE, NOBPROC, SIMPLE);
		Define(pP,pNoLayerM,"ProfileNo",initv,"#","Hydraulic Functions>100",1,999);
		mTab_BC_Measured.push_back(pP);mTab_Gen_Measured.push_back(pP);

		for(auto i=initv.begin(); i!=initv.end();++i) *i=1.;
	pP= new		P(&ProfileRepNo,pNoLayerM, MPROFILE, WATER, STORAGE, NOBPROC, SIMPLE);
		Define(pP,pNoLayerM,"ProfileRepNo",initv,"#","Hydraulic Functions>100",1,99);
		mTab_BC_Measured.push_back(pP);mTab_Gen_Measured.push_back(pP);
		
		for(auto i=initv.begin(); i!=initv.end();++i) *i=1.;
	pP= new		P(&ProfileLayer,pNoLayerM, MPROFILE, WATER, STORAGE, NOBPROC, SIMPLE);
		Define(pP,pNoLayerM,"ProfileLayer",initv,"#","Hydraulic Functions>100",1,999) ;
		mTab_BC_Measured.push_back(pP);mTab_Gen_Measured.push_back(pP);

		for(auto i=initv.begin(); i!=initv.end();++i) *i=0.;
	pP= new		P(&ScalePsi,pNoLayerM, MPROFILE, WATER, STORAGE, NOBPROC, SIMPLE);
   	Define(pP,pNoLayerM,"Scaling pF",initv,"-","Hydraulic Functions = 0",-2.,2.,-1.,1.);

		for(auto i=initv.begin(); i!=initv.end();++i) *i=4000.;
	pP= new		P(&ValidRangeUpper,pNoLayerM, MPROFILE, WATER, STORAGE, NOBPROC, SIMPLE);
   	Define(pP,pNoLayerM,"Upper Valid",initv,"-","Pedo Functions>2",100.,50000.,100,50000,0,2);
		mTab_Pedo.push_back(pP);
   	
		for(auto i=initv.begin(); i!=initv.end();++i) *i=10.;
	pP= new		P(&ValidRangeLower,pNoLayerM, MPROFILE, WATER, STORAGE, NOBPROC, SIMPLE);
		Define(pP,pNoLayerM,"Lower Valid",initv,"-","Pedo Functions>2",.5,500.,.5,500.,0,2);
		mTab_Pedo.push_back(pP);

		for(auto i=initv.begin(); i!=initv.end();++i) *i=0.;
	pP= new		P(&RegLine_R2,pNoLayerM, MPROFILE, WATER, STORAGE, NOBPROC, SIMPLE);
		Define(pP,pNoLayerM,"Regline R2",initv,"-","Pedo Functions>2",0.,1.,0.,1.,0,2);
		mTab_Pedo.push_back(pP);


   		for(auto i=initv.begin(); i!=initv.end();++i) *i=0.;
	pP= new		P(&RegLine_N,pNoLayerM, MPROFILE, WATER, STORAGE, NOBPROC, SIMPLE);
		Define(pP,pNoLayerM,"Regline N",initv,"-","Pedo Functions>2",0.,50.,0.,50.,0,2) ;
		mTab_Pedo.push_back(pP);


		for(auto i=initv.begin(); i!=initv.end();++i) *i=100;
	pP= new		P(&SatMatCond,pNoLayerM, MPROFILE, WATER, TRANSPORT, NOBPROC, SIMPLE);
		Define(pP,pNoLayerM,"Matrix Conductivity",initv,"mm/day","Hydraulic Functions != 2",1.E-7,1.E7, 1.E-2, 1.E5);
		mTab_HC_Measured.push_back(pP);

		for(auto i=initv.begin(); i!=initv.end();++i) *i=1000;
	pP= new		P(&SatTotCond,pNoLayerM, MPROFILE, WATER, TRANSPORT, NOBPROC, SIMPLE);
		Define(pP,pNoLayerM,"Total Conductivity",initv,"mm/day","Hydraulic Functions != 2",1.E-7,1.E8, 1.E-2, 1.E6);
		mTab_HC_Measured.push_back(pP);

		for(auto i=initv.begin(); i!=initv.end();++i) *i=1.;
	pP= new		P(&NTortuosity,pNoLayerM, MPROFILE, WATER, TRANSPORT, NOBPROC, SIMPLE);
		Define(pP,pNoLayerM,"n Tortuosity",initv,"-","Conductivity Function = 0",-2., 4., -1., 2.);
		mTab_HC_Measured.push_back(pP);

		for(auto i=initv.begin(); i!=initv.end();++i) *i=3.;
	pP= new		P(&NSR,pNoLayerM, MPROFILE, WATER, TRANSPORT, NOBPROC, SIMPLE);
		Define(pP,pNoLayerM,"n Power (SatRel)",initv,"-","Conductivity Function = 1",0., 15., 0.5, 6.);
		mTab_HC_Measured.push_back(pP);

		for(auto i=initv.begin(); i!=initv.end();++i) *i=3.0;
	pP= new		P(&NSE,pNoLayerM, MPROFILE, WATER, TRANSPORT, NOBPROC, SIMPLE);
		Define(pP,pNoLayerM,"n Power (SatEffective)",initv,"-","Conductivity Function = 2",0., 15., 0.5, 6.);
		mTab_HC_Measured.push_back(pP);

	initv.resize(NL);
 	for(auto i=initv.begin(); i!=initv.end();++i) *i=0.3;

	NE *pNL =m_pModelMap->GetNEPointer("Number of Layers");		  
	NL=pNL->GetNEValue();

	 	for(auto i=initv.begin(); i!=initv.end();++i) *i=1.5;
	pP= new		P(&M_GM	,pNL, SIMPROFILE, WATER, TRANSPORT, NOBPROC, SIMPLE);
		Define(pP	,pNL,"m_m-value",initv,"-","Hydraulic Functions = 1", 0.001, 10.,.001, 10., 0,0,405);
		pP->SetReadOnly(true);
		mTab_Gen_Model.push_back(pP);

	 	for(auto i=initv.begin(); i!=initv.end();++i) *i=0.5;
	pP= new		P(&M_GN    , pNL, SIMPROFILE, WATER, TRANSPORT, NOBPROC, SIMPLE);
		Define(pP  ,pNL,"m_n-value",initv,"-","Hydraulic Functions = 1", 0.01, 10., .01, 10.,0,0,405);
		pP->SetReadOnly(true);
		mTab_Gen_Model.push_back(pP);

		for(auto i=initv.begin(); i!=initv.end();++i) *i=0.5;
	pP= new		P(&M_Galpha,pNL, SIMPROFILE, WATER, TRANSPORT, NOBPROC, SIMPLE);
		Define(pP,pNL, "mAlpha",initv,"1/cm","Hydraulic Functions = 1", 0.01, 10.,0.01, 10.,0,0,405);
		pP->SetReadOnly(true);
		mTab_Gen_Model.push_back(pP);
	pP= new		P(&M_Lambda,pNL, SIMPROFILE, WATER, STORAGE, NOBPROC, SIMPLE);
		Define(pP,pNL,"mLambda",initv,"-","Hydraulic Functions = 0",0.01,5.,0.001,50.);
		pP->SetReadOnly(true); mTab_BC_Model.push_back(pP);


		for(auto i=initv.begin(); i!=initv.end();++i) *i=0.1;
	pP= new		P(&M_PsiEntry,pNL, SIMPROFILE, WATER, STORAGE, NOBPROC, SIMPLE);
		Define(pP,pNL,"mAir Entry",initv,"cm","Hydraulic Functions = 0",0.01,1000.,0.1,100.);
		pP->SetReadOnly(true); mTab_BC_Model.push_back(pP);

		for(auto i=initv.begin(); i!=initv.end();++i) *i=45;
	pP= new		P(&M_Saturation,pNL, SIMPROFILE, WATER, STORAGE, NOBPROC, SIMPLE);
		Define(pP,pNL,"mSaturation",initv,"vol %","Kalle 0",1.,99.,4.,90.);
		pP->SetReadOnly(true); mTab_BC_Model.push_back(pP); mTab_Gen_Model.push_back(pP);
		
		for(auto i=initv.begin(); i!=initv.end();++i) *i=4;
	pP= new		P(&M_Wilting,pNL, SIMPROFILE, WATER, STORAGE, NOBPROC, SIMPLE);
		Define(pP,pNL,"mWilting Point", initv,"vol %","Kalle 0",0.,50.,0.1,30.);
		pP->SetReadOnly(true); mTab_BC_Model.push_back(pP); mTab_Gen_Model.push_back(pP);
		
		for(auto i=initv.begin(); i!=initv.end();++i) *i=1;
	pP= new		P(&M_ResWater,pNL, SIMPROFILE, WATER, STORAGE, NOBPROC, SIMPLE);
		Define(pP,pNL,"mResidual Water",initv,"vol %","Hydraulic Functions != 2",0.0,50.,0.1,50.);
		pP->SetReadOnly(true); mTab_BC_Model.push_back(pP); 	mTab_Gen_Model.push_back(pP);

	 	for(auto i=initv.begin(); i!=initv.end();++i) *i=4;
	pP= new		P(&M_MacroPore,pNL, SIMPROFILE, WATER, STORAGE, NOBPROC, SIMPLE);
		Define(pP,pNL,"mMacro Pore", initv,"vol %","Hydraulic Functions = 0",0.1,10.,1.,6.);
		pP->SetReadOnly(true); mTab_BC_Model.push_back(pP);

	 	for(auto i=initv.begin(); i!=initv.end();++i) *i=15.;
	pP= new		P(&M_UpperBPsi,pNL, SIMPROFILE, WATER, STORAGE, NOBPROC, SIMPLE);
		Define(pP,pNL,"mUpper Boundary",initv,"cm","Hydraulic Functions != 2",25.,1.E6,500.,2.E4);
		pP->SetReadOnly(true); mTab_BC_Model.push_back(pP); mTab_Gen_Model.push_back(pP);

	 	for(auto i=initv.begin(); i!=initv.end();++i) *i=15.;
	pP= new		P(&M_UpperBTheta,pNL, SIMPROFILE, WATER, STORAGE, NOBPROC, SIMPLE);
		Define(pP,pNL,"mUpper Boundary (Theta)",initv,"m","Hydraulic Functions != 2",0.5,100.,1.,50.);
		pP->SetReadOnly(true);
	 	for(auto i=initv.begin(); i!=initv.end();++i) *i=0.;
	pP= new		P(&M_ScalePsi,pNL, SIMPROFILE, WATER, STORAGE, NOBPROC, SIMPLE);
		Define(pP,pNL,"M_Scaling pF",initv,"-","Hydraulic Functions = 0",-2.,2.,-1.,1.);
		pP->SetReadOnly(true);
		for(auto i=initv.begin(); i!=initv.end();++i) *i=1.0;
	pP= new		P(&M_SatMatCond,pNL, SIMPROFILE, WATER, TRANSPORT, NOBPROC, SIMPLE);
		Define(pP,pNL,"mMatrix Conductivity",initv,"-","Hydraulic Functions < 2",1.E-7,1.E4, 1.E-2, 1.E2);
		pP->SetReadOnly(true);

	 	for(auto i=initv.begin(); i!=initv.end();++i) *i=10.;
	pP= new		P(&M_SatTotCond,pNL, SIMPROFILE, WATER, TRANSPORT, NOBPROC, SIMPLE);
		Define(pP,pNL,"mTotal Conductivity",initv,"-","Hydraulic Functions < 2",1.E-7,1.E6, 1.E-2, 1.E4);
		pP->SetReadOnly(true);

	 	for(auto i=initv.begin(); i!=initv.end();++i) *i=1.;
	pP= new		P(&M_NTortuosity,pNL, SIMPROFILE, WATER, TRANSPORT, NOBPROC, SIMPLE);
		Define(pP,pNL,"m_n Tortuosity",initv,"-","Conductivity Function =0",-2., 4., -1., 2.);
		pP->SetReadOnly(true);

 	 	for(auto i=initv.begin(); i!=initv.end();++i) *i=3.;
	pP= new		P(&M_NSR,pNL, SIMPROFILE, WATER, TRANSPORT, NOBPROC, SIMPLE);
		Define(pP,pNL,"m_n Power (SatRel)",initv,"-","Conductivity Function = 1",0., 15., 0.5, 6.);
		pP->SetReadOnly(true);

	 	for(auto i=initv.begin(); i!=initv.end();++i) *i=3.0;
	pP= new		P(&M_NSE,pNL, SIMPROFILE, WATER, TRANSPORT, NOBPROC, SIMPLE);
		Define(pP,pNL,"m_n Power (SatEffective)",initv,"-","Conductivity Function = 2",0., 15., 0.5, 6.);
		pP->SetReadOnly(true);

	 	for(auto i=initv.begin(); i!=initv.end();++i) *i=0.3;
	pP= new		P(&B_Lambda,pNL, SIMPROFILE, WATER, TRANSPORT, NOBPROC, SIMPLE);
		Define(pP,pNL,"bLambda",initv,"-","Hydraulic Functions = 0",0.01,5.,0.001,50.);
		pP->SetReadOnly(true);
	 	for(auto i=initv.begin(); i!=initv.end();++i) *i=0.1;
	pP= new		P(&B_PsiEntry,pNL, SIMPROFILE, WATER, TRANSPORT, NOBPROC, SIMPLE);
		Define(pP,pNL,"bAir Entry",initv,"cm","Hydraulic Functions = 0",0.01,1000.,0.1,100.);
		pP->SetReadOnly(true);
		for(auto i=initv.begin(); i!=initv.end();++i) *i=45.;
	pP= new		P(&B_Saturation,pNL, SIMPROFILE, WATER, TRANSPORT, NOBPROC, SIMPLE);
		Define(pP,pNL,"bSaturation",initv,"vol %","Kalle 0",1.,99.,4.,90.);
		pP->SetReadOnly(true);
	 	for(auto i=initv.begin(); i!=initv.end();++i) *i=4.;
	pP= new		P(&B_Wilting,pNL, SIMPROFILE, WATER, TRANSPORT, NOBPROC, SIMPLE);
		Define(pP,pNL,"bWilting Point", initv,"vol %","Kalle 0",0.,50.,0.1,30.);
		pP->SetReadOnly(true);
		for(auto i=initv.begin(); i!=initv.end();++i) *i=1.;
	pP= new		P(&B_ResWater,pNL, SIMPROFILE, WATER, TRANSPORT, NOBPROC, SIMPLE);
		Define(pP,pNL,"bResidual Water",initv,"vol %","Hydraulic Functions <2",0.0,50.,0.1,50.,0,0,405);
		pP->SetReadOnly(true);
		for(auto i=initv.begin(); i!=initv.end();++i) *i=4.;
	pP= new		P(&B_MacroPore,pNL, SIMPROFILE, WATER, TRANSPORT, NOBPROC, SIMPLE);
		Define(pP,pNL, "bMacro Pore", initv,"vol %","Hydraulic Functions = 0",0.1,10.,1.,6.);
		pP->SetReadOnly(true);
		for(auto i=initv.begin(); i!=initv.end();++i) *i=15;
	pP= new		P(&B_UpperBPsi,pNL, SIMPROFILE, WATER, TRANSPORT, NOBPROC, SIMPLE);
		Define(pP,pNL,"bUpper Boundary",initv,"m","Hydraulic Functions <2",50.,15000.,500.,10000.);
		pP->SetReadOnly(true);
	 	for(auto i=initv.begin(); i!=initv.end();++i) *i=15.;
	pP= new		P(&B_UpperBTheta,pNL, SIMPROFILE, WATER, TRANSPORT, NOBPROC, SIMPLE);
		Define(pP,pNL,"Upper Boundary (Theta)",initv,"cm","Hydraulic Functions < 2",0.5,100.,1.,50.);
		pP->SetReadOnly(true);
		for(auto i=initv.begin(); i!=initv.end();++i) *i=0.;
	pP= new		P(&B_ScalePsi,pNL, SIMPROFILE, WATER, TRANSPORT, NOBPROC, SIMPLE);
		Define(pP,pNL,"bScaling pF",initv,"-","Hydraulic Functions = 0",-2.,2.,-1.,1.);
		pP->SetReadOnly(true);
	 	for(auto i=initv.begin(); i!=initv.end();++i) *i=1.;
	pP= new		P(&B_SatMatCond,pNL, SIMPROFILE, WATER, TRANSPORT, NOBPROC, SIMPLE);
		Define(pP,pNL,"bMatrix Conductivity",initv,"-","Hydraulic Functions < 2",1.E-7,1.E4, 1.E-2, 1.E2);
		mTab_HC_Model.push_back(pP); 	
		pP->SetReadOnly(true);
		for(auto i=initv.begin(); i!=initv.end();++i) *i=10.;
	pP= new		P(&B_SatTotCond,pNL, SIMPROFILE, WATER, TRANSPORT, NOBPROC, SIMPLE);
		Define(pP,pNL,"bTotal Conductivity",initv,"-","Hydraulic Functions < 2",1.E-7,1.E6, 1.E-2, 1.E4);
		pP->SetReadOnly(true);
	 	mTab_HC_Model.push_back(pP);
		for(auto i=initv.begin(); i!=initv.end();++i) *i=1.0;
	pP= new		P(&B_NTortuosity,pNL, SIMPROFILE, WATER, TRANSPORT, NOBPROC, SIMPLE);
		Define(pP,pNL,"b_n Tortuosity",initv,"-","Conductivity Function = 0",-2., 4., -1., 2.);
		pP->SetReadOnly(true);
		mTab_HC_Model.push_back(pP);
 	 	for(auto i=initv.begin(); i!=initv.end();++i) *i=3.;
	pP= new		P(&B_NSR,pNL, SIMPROFILE, WATER, TRANSPORT, NOBPROC, SIMPLE);
		Define(pP,pNL,"b_n Power (SatRel)",initv,"-","Conductivity Function = 1",0., 15., 0.5, 6.);
		pP->SetReadOnly(true); mTab_HC_Model.push_back(pP);
	 	for(auto i=initv.begin(); i!=initv.end();++i) *i=3.0;
	pP= new		P(&B_NSE,pNL, SIMPROFILE, WATER, TRANSPORT, NOBPROC, SIMPLE);
		Define(pP,pNL,"b_n Power (SatEffective)",initv,"-","Conductivity Function = 2",0., 15., 0.5, 6.);
		pP->SetReadOnly(true); mTab_HC_Model.push_back(pP);
	 	for(auto i=initv.begin(); i!=initv.end();++i) *i=0.5;
	pP= new		P(&B_Galpha,pNL, SIMPROFILE, WATER, TRANSPORT, NOBPROC, SIMPLE);
	    pP->SetReadOnly(true); Define(pP,pNL, "bAlpha",initv,"1/cm","Hydraulic Functions = 1", 0.01, 10.,.01,10.,0,0,405);
	
		for(auto i=initv.begin(); i!=initv.end();++i) *i=0.5;
	pP= new		P(&B_GN   ,pNL, SIMPROFILE, WATER, TRANSPORT, NOBPROC, SIMPLE);
	    pP->SetReadOnly(true); Define(pP  ,pNL, "b_n-value",initv,"-","Hydraulic Functions = 1", 0.01, 10.,0.01, 10., 0,0,405);

	 	for(auto i=initv.begin(); i!=initv.end();++i) *i=1.5;
	pP= new		P(&B_GM	, pNL, SIMPROFILE, WATER, TRANSPORT, NOBPROC, SIMPLE);
	pP->SetReadOnly(true); Define(pP,pNL,"b_m-value",initv,"-","Hydraulic Functions = 1", 0.1, 10.,.1,10.,0,0,405);

	Tab *pTab;

	pTab= new Tab(&mTab_BC_Measured , MPROFILE, WATER, STORAGE, NOBPROC, SIMPLE);

	Define(pTab,  "Brooks-Corey, water retention, measured horizons",9);

//!	CALL Tab("Hidden", "measured horizons",TINDEXCHECK, NolayerM, &
//!			"ClayContent|SandContent")

	pTab= new Tab(&mTab_BC_Model , SIMPROFILE, WATER, STORAGE, NOBPROC, SIMPLE);
		Define(pTab,  "Brooks-Corey, water retention, model layers",9);
	
	pTab= new Tab(&mTab_Pedo , MPROFILE, WATER, STORAGE, NOBPROC, SIMPLE);	
		Define(pTab,  "Pedo Functions Inputs, measured horizons",4);

	pP=(P*)m_pModelMap->GetP_Pointer("Macro Pore"); if(pP!=nullptr) mTab_HC_Measured.push_back(pP);

	pTab= new Tab(&mTab_HC_Measured , MPROFILE, WATER, TRANSPORT, NOBPROC, SIMPLE);
 		Define(pTab,  "Hydraulic conductivity, measured horizons",6);
	pP=(P*)m_pModelMap->GetP_Pointer("bMacro Pore"); if(pP!=nullptr) mTab_HC_Model.push_back(pP);
	pTab= new Tab(&mTab_HC_Model , SIMPROFILE, WATER, TRANSPORT, NOBPROC, SIMPLE);
		Define(pTab,  "Hydraulic conductivity, model boundaries",5);
	
	pTab= new Tab(&mTab_Gen_Measured , MPROFILE, WATER, STORAGE, NOBPROC, SIMPLE);
		Define(pTab, "Genuchten, water retention, measured horizons",8);

	pTab= new Tab(&mTab_Gen_Model , SIMPROFILE, WATER, STORAGE, NOBPROC, SIMPLE); 	
		Define(pTab, "Genuchten, water retention, model layers",6);

	return FunctionDef();
}

bool Hydraulic_Functions::FunctionDef() {
	ReCalc();

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
	string funcname;

	FunctorProp functorprop;
	vector<P*> p_vec;
	vector<Ps*> ps_vec;
	ps_vec.clear();
	Func *pFunc;


	funcname = "ReCalc Soil Function";
	pFunc= new Func(MPROFILE,WATER, WATERUPTAKE,NOBPROC, NORMAL);


	functorprop.FuncName = MyFunc::HYDRAULIC_RECALC;
	functorprop.Arg = MyArgType::BLANK;
	Define(pFunc,functorprop,this,funcname, plotpar ); 

	auto tt=pFunc->MakeOperation();
	
	
	functorprop.pHyd = ((Hydraulic*)this);
		
	functorprop.FuncName = MyFunc::HYDRAULIC_CONDTEMPFACTOR;
	functorprop.Arg = MyArgType::D1;
	funcname = "Temperature Dependence Hydraulic Conductivity Function";
	pFunc= new Func(ALL_SOIL_LAYERS,WATER, WATERUPTAKE,NOBPROC, NORMAL);
	Define(pFunc, functorprop, this, funcname, plotpar); 

	auto vv=f_CondTempFactor(12.);
	pFunc->SetFunctor(f_CondTempFactor);

	vv=pFunc->GetValue(12.);





		functorprop.FuncName = MyFunc::PSI_THETA_MEASURED;
		functorprop.Arg = MyArgType::T_D1;
		plotpar.LogYScale = true;
		plotpar.X_Label = "Water Content";
		plotpar.Y_Label = "Pressure head (cm water)";


		funcname = "Measured Water Retention Curve";
		pFunc = new Func(MPROFILE, WATER, STORAGE, NOBPROC, NORMAL);
		p_vec.clear();
		p_vec.push_back(m_pModelMap->GetPPointer("Air Entry"));
		p_vec.push_back(m_pModelMap->GetPPointer("Lambda"));
		p_vec.push_back(m_pModelMap->GetPPointer("Saturation"));
		p_vec.push_back(m_pModelMap->GetPPointer("Wilting Point"));
		p_vec.push_back(m_pModelMap->GetPPointer("Residual Water"));
		p_vec.push_back(m_pModelMap->GetPPointer("Macro Pore"));
		p_vec.push_back(m_pModelMap->GetPPointer("Upper Boundary"));
		Define(pFunc, functorprop,funcname,p_vec, plotpar);
		
		pFunc= new Func(MPROFILE,WATER, STORAGE,NOBPROC, NORMAL);
		functorprop.FuncName = MyFunc::PSI_THETA;
		funcname = "Water Retention Curve";
		p_vec.clear();

		p_vec.push_back(m_pModelMap->GetPPointer("mAir Entry"));
		p_vec.push_back(m_pModelMap->GetPPointer("mLambda"));
		p_vec.push_back(m_pModelMap->GetPPointer("mSaturation"));
		p_vec.push_back(m_pModelMap->GetPPointer("mWilting Point"));
		p_vec.push_back(m_pModelMap->GetPPointer("mResidual Water"));
		p_vec.push_back(m_pModelMap->GetPPointer("mMacro Pore"));
		p_vec.push_back(m_pModelMap->GetPPointer("mUpper Boundary"));


		Define(pFunc, functorprop, funcname,p_vec, plotpar);

		p_vec.clear();
		pFunc = new Func(MPROFILE, WATER, STORAGE, NOBPROC, NORMAL);
		functorprop.FuncName = MyFunc::HYDRAULIC_COND_THETA;
		functorprop.Arg = MyArgType::T_D2;
		plotpar.Y_Label = "Hydraulic Conductivity (mm/day)";
		funcname = "Unsaturated Conductivity Theta Function";
		plotpar.X_Max = 100;
		plotpar.X_Label = "Water Content";
		p_vec.push_back(m_pModelMap->GetPPointer("bMatrix Conductivity"));
		p_vec.push_back(m_pModelMap->GetPPointer("bTotal Conductivity"));
		p_vec.push_back(m_pModelMap->GetPPointer("b_n Tortuosity"));
		Define(pFunc, functorprop, funcname, p_vec, plotpar);

		pFunc= new Func(MPROFILE,WATER, STORAGE,NOBPROC, NORMAL);
		functorprop.FuncName = MyFunc::HYDRAULIC_COND_PSI;
		funcname = "Unsaturated Conductivity Psi Function";
		plotpar.X_Max = 400;
		plotpar.X_Label = "Pressure head (cm water)";

		Define(pFunc, functorprop, this, funcname, plotpar);


		p_vec.clear();
		pFunc= new Func(MPROFILE,WATER, STORAGE,NOBPROC, NORMAL);
		functorprop.FuncName = MyFunc::HYDRAULIC_COND_THETA_MEASURED;
		funcname = "Measured Unsaturated Conductivity Theta Function";
		p_vec.push_back(m_pModelMap->GetPPointer("Matrix Conductivity"));
		p_vec.push_back(m_pModelMap->GetPPointer("Total Conductivity"));
		p_vec.push_back(m_pModelMap->GetPPointer("n Tortuosity"));
		plotpar.X_Label = "Water Content";
		plotpar.X_Max = 100;

		Define(pFunc, functorprop, funcname,p_vec,  plotpar);
		

		pFunc= new Func(MPROFILE,WATER, STORAGE,NOBPROC, NORMAL);
		functorprop.FuncName = MyFunc::HYDRAULIC_COND_PSI_MEASURED;
		funcname = "Measured Unsaturated Conductivity Psi Function";
		plotpar.X_Max = 400;
		plotpar.X_Label = "Pressure head (cm water)";

		Define(pFunc, functorprop,  funcname, p_vec, plotpar);

		return true;
}



bool Hydraulic_Functions::ReCalc() {

	function<void(vector<double>&, vector<double>&, vector<double>&, bool)> f_conv;

	f_conv = FromMeasuredLayersToModelLayers(UpperDepth, LowerDepth, DepthLayers, ThicknessLayers, DepthMidpLayer);

	f_conv(Lambda, M_Lambda, B_Lambda, true);
	f_conv(PsiEntry, M_PsiEntry, B_PsiEntry, true);
	f_conv(Saturation, M_Saturation, B_Saturation, true);
	f_conv(Wilting, M_Wilting, B_Wilting, true);
	f_conv(ResWater, M_ResWater, B_ResWater, true);
	f_conv(UpperBoundary, M_UpperBPsi, B_UpperBPsi, true);
	f_conv(MacroPore, M_MacroPore, B_MacroPore, true);
	f_conv(NTortuosity, M_NTortuosity, B_NTortuosity, true);
	f_conv(ScalePsi, M_ScalePsi, B_ScalePsi, true);
	f_conv(PsiEntry, M_PsiEntry, B_PsiEntry, true);
	f_conv(NSE, M_NSE, B_NSE, true);
	f_conv(NSR, M_NSR, B_NSR, true);
	f_conv(GM, M_GM, B_GM, true);
	f_conv(GN, M_GN, B_GN, true);

	f_conv(Galpha, M_Galpha, B_Galpha, true);
	f_conv(SatTotCond, M_SatTotCond, B_SatTotCond, false);

	f_conv(SatMatCond, M_SatMatCond, B_SatMatCond, false);

	if (MatricCondFunc > 0) AdjustMatricConductivity();
	HYDRAULIC_COEF c, b, meas;

	c.ret.resize(M_Saturation.size());
	c.cond.resize(M_Saturation.size());
	b.cond.resize(B_Saturation.size());
	b.ret.resize(B_Saturation.size());
	meas.ret.resize(Saturation.size());
	meas.cond.resize(Saturation.size());
	CONDUCTIVITY_COMMON_PAR cond_com;
	cond_com.A0Temp = A0Temp;
	cond_com.A1Temp = A1Temp;
	cond_com.CMinUC = CMinUC;

	vector<double> UpperBPrior_Meas, UpperBPrior_Model, UpperBPrior_Boundary;
	for (size_t i = 0; i < Saturation.size(); i++)
		UpperBPrior_Meas.push_back(UpperBoundary[i]);
	for (size_t i = 0; i < M_Saturation.size(); i++) {
		UpperBPrior_Model.push_back(M_UpperBPsi[i]);
		UpperBPrior_Boundary.push_back(B_UpperBPsi[i]);
	}
	for (size_t i = 0; i < M_Saturation.size(); i++) {
		c.ret[i].Saturation = M_Saturation[i];
		c.ret[i].Lambda = M_Lambda[i];
		c.ret[i].PsiEntry = M_PsiEntry[i];
		c.ret[i].Wilting = M_Wilting[i];
		c.ret[i].ResWater = M_ResWater[i];
		c.ret[i].MacroPore = M_MacroPore[i];
		c.ret[i].UpperBPsi = 1.E8;
		c.ret[i].UpperBTheta = M_UpperBTheta[i];
		c.ret[i].ScalePsi = M_ScalePsi[i];
		c.ret[i].Galpha = M_Galpha[i];
		c.ret[i].GN = M_GN[i];
		c.ret[i].GM = M_GM[i];
		c.cond[i].NTortuosity = M_NTortuosity[i];
		c.cond[i].NSE = M_NSE[i];
		c.cond[i].NSR= M_NSR[i];
		c.cond[i].SatTotCond = M_SatTotCond[i];
		c.cond[i].SatMatCond = M_SatMatCond[i];

	};

	for (size_t i = 0; i < B_Saturation.size(); i++) {
		b.ret[i].Saturation = B_Saturation[i];
		b.ret[i].Lambda = B_Lambda[i];
		b.ret[i].PsiEntry = B_PsiEntry[i];
		b.ret[i].Wilting = B_Wilting[i];
		b.ret[i].ResWater = B_ResWater[i];
		b.ret[i].MacroPore = B_MacroPore[i];
		b.ret[i].UpperBPsi = 1.E8;
		b.ret[i].UpperBTheta = B_UpperBTheta[i];
		b.ret[i].ScalePsi = B_ScalePsi[i];
		b.ret[i].Galpha = B_Galpha[i];
		b.ret[i].GN = B_GN[i];
		b.ret[i].GM = B_GM[i];
		b.cond[i].NTortuosity = B_NTortuosity[i];
		b.cond[i].NSE = B_NSE[i];
		b.cond[i].NSR = B_NSR[i];
		b.cond[i].SatTotCond = B_SatTotCond[i];
		b.cond[i].SatMatCond = B_SatMatCond[i];

	};

	for (size_t i = 0; i < Saturation.size(); i++) {
		meas.ret[i].Saturation = Saturation[i];
		meas.ret[i].Lambda = Lambda[i];
		meas.ret[i].PsiEntry = PsiEntry[i];
		meas.ret[i].Wilting = Wilting[i];
		meas.ret[i].ResWater = ResWater[i];
		meas.ret[i].MacroPore = MacroPore[i];
		meas.ret[i].UpperBPsi = 1.E8;
		meas.ret[i].UpperBTheta = UpperBoundaryTheta[i];
		meas.ret[i].ScalePsi = ScalePsi[i];
		meas.ret[i].Galpha = Galpha[i];
		meas.ret[i].GN = GN[i];
		meas.ret[i].GM = GM[i];
		meas.cond[i].NTortuosity = NTortuosity[i];
		meas.cond[i].NSE = NSE[i];
		meas.cond[i].NSR = NSR[i];
		meas.cond[i].SatTotCond = SatTotCond[i];
		meas.cond[i].SatMatCond = SatMatCond[i];
	}
	f_ThetaPsi_M = Theta_fromPsi(c.ret, HydraulicFunc);
	f_ThetaPsi_B = Theta_fromPsi(b.ret, HydraulicFunc);
	f_ThetaPsi_Measured = Theta_fromPsi(meas.ret, HydraulicFunc);

	f_CondTheta = Conductivity_fromTheta(b, cond_com, HydraulicFunc, ConductivityFunc);
	f_CondTempFactor= Conductivity_fromTheta(b, cond_com,  HydraulicFunc,  ConductivityFunc);
	f_CondSaturated = Conductivity_fromTheta(b, cond_com,  HydraulicFunc,  ConductivityFunc);

	f_CondThetaMatric= Conductivity_fromTheta(b, cond_com,  HydraulicFunc,  ConductivityFunc);
	f_CondTheta_Measured = Conductivity_fromTheta(meas, cond_com,  HydraulicFunc,  ConductivityFunc);
	
	f_CondPsi =Conductivity_fromPsi(b, cond_com,  HydraulicFunc,  ConductivityFunc);
	f_CondPsi_Measured = Conductivity_fromPsi(meas, cond_com,  HydraulicFunc,  ConductivityFunc);
	
	f_CondThetaFrozen = Conductivity_fromPartlyFrozen(b, cond_com,  HydraulicFunc,  ConductivityFunc);
	f_CondThetaFrozen_Measured = Conductivity_fromPartlyFrozen(meas, cond_com,  HydraulicFunc,  ConductivityFunc);
	f_CondHighFlowDampFactor = Conductivity_HighFlowDamp(HighFlowDampC);
	f_MatricCondEstimate = MatricConductivityFunctor(CommonConductivityValue, SensAboveThreshold);
	f_ThetafromWaterFlow = WaterContent_FlowRate(b);

	for(size_t i=0; i<NL;i++) {
        double  XXPSI=pow(10.,M_ScalePsi[i])*M_UpperBPsi[i];
		c.ret[i].UpperBTheta = f_ThetaPsi_M(i, XXPSI);
		c.ret[i].UpperBPsi = UpperBPrior_Model[i];

		XXPSI = pow(10., B_ScalePsi[i])*B_UpperBPsi[i];
		b.ret[i].UpperBTheta=f_ThetaPsi_B(i, XXPSI);
		b.ret[i].UpperBPsi = UpperBPrior_Boundary[i];
	}
	for(size_t i=0;i<NoLayerM;i++) {
		double  XXPSI=pow(10.,ScalePsi[i])*UpperBoundary[i];
		meas.ret[i].UpperBTheta=f_ThetaPsi_Measured(i, XXPSI);
		meas.ret[i].UpperBPsi = XXPSI;
	}
	// New instance to set new value for UpperBoundary of Theta
	f_ThetaPsi_M = Theta_fromPsi(c.ret, HydraulicFunc);
	f_ThetaPsi_B = Theta_fromPsi(b.ret, HydraulicFunc);
	f_ThetaPsi_Measured = Theta_fromPsi(meas.ret, HydraulicFunc);

	f_PsiTheta_M = Psi_fromTheta(c.ret, HydraulicFunc);
	f_PsiTheta_B = Psi_fromTheta(b.ret, HydraulicFunc);

	f_PsiTheta_Measured = Psi_fromTheta(meas.ret, HydraulicFunc);

	auto y1=f_CondHighFlowDampFactor(0.7);
	auto y2 = f_MatricCondEstimate(0.7);
	y1 = f_ThetafromWaterFlow(0 ,0.7,.1);


	auto kval = f_CondTheta_Measured(0, 12., 5.);
	auto kval1 = f_CondTheta(0, 12., 5.);
	auto kval2 = f_CondPsi_Measured(0, 60., 5.);
	auto kval3 = f_CondPsi(0, 60., 5.);
	auto kval4 = f_CondThetaFrozen(0, 12, -1., 0.6);
	auto kval5 = f_CondThetaFrozen_Measured(0, 12, -1., 0.6);
	function<double(double, double, double)> f_AirEntry;

	function<double(size_t index)> f_AirEntryIndex, f_LambdaIndex;

	f_AirEntryIndex = PedoFunc_fromIndex(PedoType::AIRENTRY, SandContent, ClayContent, Saturation);
	f_LambdaIndex= PedoFunc_fromIndex(PedoType::LAMBDA, SandContent, ClayContent, Saturation);

	auto val_Air=f_AirEntryIndex(0);
	auto val_l = f_LambdaIndex(0);

	f_AirEntry = AirEntry_fromTexture();
	auto koll = f_AirEntry(.3, .12, 40.);
	auto koll2 = FUtil::PedoFunc_AirEntry(.3, .12, 40);
	SOILPROPEVAPORATION pevap;
	pevap.ret_ulayer = c.ret[0];
	pevap.cond_ulayer = c.cond[0];
	pevap.Thickness_ulayer = ThicknessLayers[0];

	f_MaxEvaporationRate_Psi = MaxSoilEvaporationRate_PsiUpperlayer(pevap,  HydraulicFunc);
	auto koll9 = f_MaxEvaporationRate_Psi(100.);
	return true;


}


double Hydraulic_Functions::WCondB_Derivate(size_t ipar, double Theta, double Temp)
{
	 return (f_CondTheta(ipar,Theta, Temp)-f_CondTheta(ipar, Theta-1.,Temp))/0.01;
}
double Hydraulic_Functions::PsiFunc_Derivate(size_t ipar, double Theta)
{
	return (f_PsiTheta_M(ipar, Theta)-f_PsiTheta_M(ipar,Theta-1.))/0.01;
}
double Hydraulic_Functions::EvaRateMax(double PsiUpperMidLayer)
{  double b, PsiNullMax;
	if(HydraulicFunc==0)
		b=-((M_NTortuosity[0]+2)*M_Lambda[0]+2);
	else
		b=-((M_NTortuosity[0]+2)*(M_GN[0]-1)+2);

	PsiNullMax=((((ThicknessLayers[0]*100)*b)/2)-(PsiUpperMidLayer*(1-b)))/(1+b);
	if(HydraulicFunc==0) 
		return M_SatMatCond[0]*pow((PsiNullMax+PsiUpperMidLayer)/(2*M_PsiEntry[0]),b);
	else {
		double a=pow((PsiNullMax+PsiUpperMidLayer)/2,M_GN[0]-1);
		double b=pow(1+(1/M_PsiEntry[0])*((PsiNullMax+PsiUpperMidLayer)/2),M_GN[0]);
		 return M_SatMatCond[0]* pow(1-((1/M_PsiEntry[0])*a*pow(b,-M_GM[0] ) ),2)
		/pow(b,M_GM[0]*M_NTortuosity[0]); 
	}

}
double Hydraulic_Functions::ThetaFlow(size_t i, double FLOW, double Thickness) {

	double PSM, PSB, PSI, SE;
      if(HydraulicFunc==1) 
        SE=0.5;
      else {
        PSM=M_PsiEntry[i]*pow(FLOW/M_SatMatCond[i],1/(-2-M_Lambda[i]*(2+M_NTortuosity[i])));
        PSB=B_PsiEntry[i]*pow(FLOW/B_SatMatCond[i],1/(-2-M_Lambda[i]*(2+B_NTortuosity[i])));
        PSI=min(PSM,PSB+Thickness*25);
        SE=pow(PSI/M_PsiEntry[i],-M_Lambda[i]);
      }

      return SE*(M_Saturation[i]-M_ResWater[i])+M_ResWater[i];
}
 
void Hydraulic_Functions::AdjustMatricConductivity() {
		for(size_t i=0; i<NL;i++) {
			if(MatricCondFunc==1) B_SatMatCond[i]=B_SatTotCond[i];
			else B_SatMatCond[i]=f_MatricCondEstimate(B_SatTotCond[i]);

			if(MatricCondFunc==1) M_SatMatCond[i]=M_SatTotCond[i];
			else M_SatMatCond[i] = f_MatricCondEstimate(M_SatTotCond[i]);
		}

}
