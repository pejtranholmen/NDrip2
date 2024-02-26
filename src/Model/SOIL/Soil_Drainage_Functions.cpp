#include "Soil_Drainage_Functions.h"
#include "../Atmosphere/Irrigation.h"
#include "../SOIL/Soil_HeatPump.h"
#include "./Functors/Drainage_Functor.h"
#include "../../Simulator/Simulator.h"

Soil_Drainage_Functions::Soil_Drainage_Functions(void)
{
	LayerOfSatLevel=1;
}
Soil_Drainage_Functions::~Soil_Drainage_Functions(void)
{
}
bool Soil_Drainage_Functions::Def()
{
	Hydraulic_Functions::Def();
	Sw* pSw;
	ModuleNo=21;
	ModuleConditions="WaterEq>0";

	pSw= new Sw(&DriveDrainLevel,  SINGLE, WATER, LOWERBOUNDARY, NOBPROC, NORMAL);
	Define(pSw,  "DriveDrainLevel",0,"Parameter|Driving File|Function of Surface Runoff","WaterEq = 1|GroundWaterFlow =1|PhysicalDrainEq > 0", 204)	;
	pSw= new Sw(&LBoundSaturated,  SINGLE, WATER, LOWERBOUNDARY, NOBPROC, NORMAL);
	Define(pSw,  "LBoundSaturated", 0,"No Flow|Unit Grad Flow|Seepage Flow|Static Grad Flow|Dynamic Grad Flow","WaterEq = 1|GroundWaterFlow = 1", 205);
	pSw= new Sw(&LBoundUnSaturated,  SINGLE, WATER, LOWERBOUNDARY, NOBPROC, NORMAL);
	Define(pSw,  "LBoundUnSaturated", 0,"Constant Psi|Maximum constant psi|Dynamic Psi|No Flow|Unit Grad Flow","WaterEq = 1|GroundWaterFlow =0") ;
	pSw= new Sw(&ReturnFlow,  SINGLE, WATER, LOWERBOUNDARY, NOBPROC, NORMAL);
	Define(pSw,  "ReturnFlow", 0,"off|on", "WaterEq = 1|GroundWaterFlow >= 1|PhysicalDrainEq > 0");
	pSw= new Sw(&EmpiricalDrainEq,  SINGLE, WATER, LOWERBOUNDARY, NOBPROC, NORMAL);
	Define(pSw,  "EmpiricalDrainEq", 0,"off|On", "WaterEq = 1|GroundWaterFlow >= 1",106);
	pSw= new Sw(&PhysicalDrainEq,  SINGLE, WATER, LOWERBOUNDARY, NOBPROC, NORMAL);
	Define(pSw,  "PhysicalDrainEq", 0,"off|Linear Model|Ernst Model|Hooghoudt Model", "WaterEq = 1|GroundWaterFlow >= 1",105);
	pSw= new Sw(&Pump,  SINGLE, WATER, LOWERBOUNDARY, NOBPROC, NORMAL);
	Define(pSw,  "Pump", 0,"off|On", "WaterEq = 1|GroundWaterFlow >= 1") ;
	pSw= new Sw(&m_pModelStructure->LateralGWSource,  SINGLE, WATER, LOWERBOUNDARY, NOBPROC, NORMAL);
	Define(pSw,  "Lateral GW Source", 0,"off|Later Input File", "WaterEq = 1|GroundWaterFlow >= 1|LateralInput=1");
	pSw= new Sw(&GWIndexFileName,  SINGLE, WATER, LOWERBOUNDARY, NOBPROC, NORMAL);
	Define(pSw,  "Dynamic indexed GW file",0,"off|on","WaterEq = 1|DriveDrainLevel >= 1");



	Ps *pPs;
	pPs= new Ps(&GWSoLayer,  SINGLE, WATER, LOWERBOUNDARY, NOBPROC, NORMAL);
	
   	Define(pPs, "GWSourceLayer",3.,"#","GroundWaterFlow >=1|WaterEq = 1",0.,100.);
	pPs= new Ps(&GWSoFlow ,  SINGLE, WATER, LOWERBOUNDARY, NOBPROC, NORMAL);
	Define(pPs, "GWSourceFlow" ,0.,UNIT_TYPES::WATERFLOW_UNIT,"GroundWaterFlow >= 1|WaterEq = 1",0., 100.);

	FunctorProp functorprop;
	string funcname;
	PLOT_PARAM plotpar;
	vector<Ps*> ps_vec;
	functorprop.FuncName = MyFunc::DRAINAGE_EMPIRICAL;
	functorprop.Arg = MyArgType::D1;
	functorprop.pSimModule = this;
	functorprop.pHyd = this;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Empirical Drainage Equation";
	Func* pFunc;
	plotpar.Num_Of_Curves = 1;
	plotpar.Num_Of_Points = 100;
	plotpar.Num_Of_X = 1;
	plotpar.X_Min = -3;
	plotpar.X_Max = 0;
	plotpar.LogXScale = false;
	plotpar.LogYScale = false;
	plotpar.X_Label = "Ground Water Level ";	 plotpar.X_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::LENGTH_UNIT, true);
	plotpar.Y_Label = "Ground Water Flow ";	 plotpar.Y_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::WATERFLOW_UNIT, true);
	plotpar.LegendsVec = { "" };

	
	pPs= new Ps(&GFLevBase,  SINGLE, WATER, LOWERBOUNDARY, NOBPROC, NORMAL);
	Define(pPs, "EmpGFLevBase", -3., "m", "GroundWaterFlow =1|WaterEq = 1|EmpiricalDrainEq = 1", -100., 0.); ps_vec.push_back(pPs);
	pPs= new Ps(&GFLevPeak,  SINGLE, WATER, LOWERBOUNDARY, NOBPROC, NORMAL);
		Define(pPs, "EmpGFLevPeak",-1.,"m", "GroundWaterFlow =1|WaterEq = 1|EmpiricalDrainEq = 1",-100., 0.);  ps_vec.push_back(pPs);
	pPs= new Ps(&GFlowBase,  SINGLE, WATER, LOWERBOUNDARY, NOBPROC, NORMAL);
		Define(pPs, "EmpGFLowbase",2.,UNIT_TYPES::WATERFLOW_UNIT, "GroundWaterFlow =1|WaterEq = 1|EmpiricalDrainEq = 1",0., 100.); ps_vec.push_back(pPs);
	pPs= new Ps(&GFlowPeak,  SINGLE, WATER, LOWERBOUNDARY, NOBPROC, NORMAL);
		Define(pPs, "EmpGFlowPeak",10.,UNIT_TYPES::WATERFLOW_UNIT, "GroundWaterFlow =1|WaterEq = 1|EmpiricalDrainEq = 1",0., 100.);	ps_vec.push_back(pPs);

	pFunc = new Func(SIMPROFILE, GENERAL, LOWERBOUNDARY, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar); ps_vec.clear();

	functorprop.FuncName = MyFunc::DRAINAGE_PHYSICAL;
	functorprop.Arg = MyArgType::D1;
	funcname = "Physical Based Drainage Equation";

	pPs= new Ps(&DLevel  ,  SINGLE, WATER, LOWERBOUNDARY, NOBPROC, NORMAL);
   	Define(pPs,  "DrainLevel"  ,-1.,"m","GroundWaterFlow =1|PhysicalDrainEq >= 1|WaterEq = 1",-100., 0.);  ps_vec.push_back(pPs);
	pPs= new Ps(&DDist,  SINGLE, WATER, LOWERBOUNDARY, NOBPROC, NORMAL);
   	Define(pPs,  "DrainSpacing"  ,10.,"m","GroundWaterFlow =1|PhysicalDrainEq >= 1|WaterEq = 1",0.,10000.);	   ps_vec.push_back(pPs);
	pPs = new Ps(&Dlayer, SINGLE, WATER, LOWERBOUNDARY, NOBPROC, NORMAL);
	Define(pPs, "DLayer", 4., "m", "PhysicalDrainEq > 1|WaterEq = 1", 0., 100.);	ps_vec.push_back(pPs);


	pPs= new Ps(&DLevelLB  ,  SINGLE, WATER, LOWERBOUNDARY, NOBPROC, NORMAL);
   	Define(pPs,  "DrainLevelLowerB"  ,-10.,"m","GroundWaterFlow = 1|LBoundSaturated = 2|WaterEq = 1",-100., 0.);
	pPs= new Ps(&DDistLB,  SINGLE, WATER, LOWERBOUNDARY, NOBPROC, NORMAL);
   	Define(pPs,  "DrainSpacingLowerB"  ,200.,"m","GroundWaterFlow =1|LBoundSaturated = 2|WaterEq = 1",0.,10000.);
	pPs= new Ps(&PsiBottom,  SINGLE, WATER, LOWERBOUNDARY, NOBPROC, NORMAL);
	Define(pPs, "PressureHeadBottom",100.,"cm","GroundWaterFlow = 0|LBoundUnSaturated <= 1|WaterEq = 1",0.,1.E5);
	pPs= new Ps(&RPipe,  SINGLE, WATER, LOWERBOUNDARY, NOBPROC, NORMAL);
	Define(pPs,     "RadiusPipe"   , 0.10,"m","PhysicalDrainEq > 1|WaterEq = 1",0.01,10., 0.05,0.5);

	pFunc = new Func(SIMPROFILE, GENERAL, LOWERBOUNDARY, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar); ps_vec.clear();

//! Klas new
	pPs= new Ps(&DrainLevelMin  ,  SINGLE, WATER, LOWERBOUNDARY, NOBPROC, NORMAL);
   	Define(pPs,  "DrainLevelMin"  ,-1.,"m","GroundWaterFlow =1|PhysicalDrainEq >= 1|DriveDrainLevel = 0|WaterEq = 1",-100., 0.);
//! New pumping regulation 20020302
	pPs= new Ps(&PumpHighLevel  ,  SINGLE, WATER, LOWERBOUNDARY, NOBPROC, NORMAL);
   	Define(pPs,  "PumpHighLevel"  ,-4.,"m","GroundWaterFlow =1|WaterEq = 1|Pump =1",-100., 0.);
	pPs= new Ps(&PumpLowLevel  ,  SINGLE, WATER, LOWERBOUNDARY, NOBPROC, NORMAL);
   	Define(pPs,  "PumpLowLevel"  ,-5.,"m","GroundWaterFlow =1|WaterEq = 1|Pump =1",-100., 0.);
	pPs= new Ps(&PumpFlowRate  ,  SINGLE, WATER, LOWERBOUNDARY, NOBPROC, NORMAL);
   	Define(pPs,  "PumpFlowRate"  ,5.,UNIT_TYPES::WATERFLOW_UNIT,"GroundWaterFlow =1|WaterEq = 1|Pump =1",-100., 1000.);

	pPs= new Ps(&HFlow_Input_Index  ,  SINGLE, WATER, LOWERBOUNDARY, NOBPROC, NORMAL);
   	Define(pPs,  "HFlow Input Index"  ,0.,"-","GroundWaterFlow =1|WaterEq = 1|LateralInput=2",0.,99999.);
	pPs= new Ps(&HFlow_Output_Index  ,  SINGLE, WATER, LOWERBOUNDARY, NOBPROC, NORMAL);
   	Define(pPs,  "HFlow Output Index"  ,1.,"-","GroundWaterFlow =1|WaterEq = 1|LateralInput=2",0.,99999.);
	pPs= new Ps(&GWFileName_Index,  SINGLE, WATER, LOWERBOUNDARY, NOBPROC, NORMAL);
    Define(pPs,  "GWFileNameIndex", 1.,"-","Dynamic indexed GW file>0|DriveDrainLevel >= 1", 1., 99000.);
	pPs= new Ps(&LowerB_GwGrad,  SINGLE, WATER, LOWERBOUNDARY, NOBPROC, NORMAL);
    Define(pPs,  "LowerB_GwGrad", 0.,"-","LBoundSaturated=3", -1.0, 1.);
   	//! Local definied variables
	T* pT;
 	
 	vector<double> initv;
	NE *pNL =m_pModelMap->GetNEPointer("Number of Layers");	
	pT= new T(&WaterDrainFlow,pNL,SIMPROFILE, WATER, DRAINAGE, NOBPROC, NORMAL);
	initv.resize(NL);
	initv.assign(NL, 0);
	Define(pT,pNL, "WaterDrainflow", initv,UNIT_TYPES::WATERFLOW_UNIT,"WaterEq = 1|GroundWaterFlow > 0");
	Ts *pTs;
 	pTs= new Ts(&DeepPerc	,   SINGLE, WATER, LOWERBOUNDARY, NOBPROC, NORMAL);
 
	Define(pTs, "DeepPerc", 0.,UNIT_TYPES::WATERFLOW_UNIT, "WaterEq = 1"); 
	G* pG;
 	pG= new G(&CorrHH,pNL,SIMPROFILE, WATER, DRAINAGE, NOBPROC, NORMAL);

	Define(pG, pNL, "CorrHeightFactor", initv,"-", "GroundWaterFlow = 2|WaterEq = 1");
 	Gs *pGs;
	pGs= new Gs(&SatLev,SINGLE, WATER, DRAINAGE, NOBPROC, NORMAL);

	Define(pGs, "SaturationLevel",0.,"m","GroundWaterFlow >= 1|WaterEq = 1");
 	pTs= new Ts(&NetEmpDrainage, SINGLE, WATER, DRAINAGE, NOBPROC, NORMAL);
 	Define(pTs,  "NetEmpDrainage", 0.,UNIT_TYPES::WATERFLOW_UNIT,"GroundWaterFlow = 1|EmpiricalDrainEq > 0");
 	pTs= new Ts(&NetPhysDrainage,   SINGLE, WATER, DRAINAGE, NOBPROC, NORMAL);
 	Define(pTs, "NetPhysDrainage", 0.,UNIT_TYPES::WATERFLOW_UNIT,"GroundWaterFlow = 1|PhysicalDrainEq >0");
 	pTs= new Ts(&TotalDrainage,   SINGLE, WATER, DRAINAGE, NOBPROC, NORMAL);
 	Define(pTs, "TotalDrainage", 0.,UNIT_TYPES::WATERFLOW_UNIT,"GroundWaterFlow >= 1");
 	pTs= new Ts(&TotalRunoff,   SINGLE, WATER, LOWERBOUNDARY, NOBPROC, NORMAL);
 	Define(pTs, "TotalRunoff",0.,UNIT_TYPES::WATERFLOW_UNIT, "WaterEq = 1");
 	pTs= new Ts(&vDriveDrainLevel,   SINGLE, WATER, LOWERBOUNDARY, NOBPROC, NORMAL);
 	Define(pTs, "vDriveDrainLevel",0.,"m","DrainFileUse=1") ;

	F *pF;
	pF = new F(&m_PG_Gw, ATMBOUNDARY,  CLIMATE, NOFPROC, NOBPROC, NORMAL);
  	Define(pF, "Ground water level","Drain.bin","DrainFileUse = 1");
	pF = new F(&m_PG_PressureHead, ATMBOUNDARY,  CLIMATE, NOFPROC, NOBPROC, NORMAL);
 	Define(pF, "Pressured head at Lower boundary","PsiBottom.bin","LBoundUnSaturated = 2|WaterEq > 0");

	return FunctionDef();
	
}
bool Soil_Drainage_Functions::FunctionDef() {

	SetDrainageFunctors();
	Func *pFunc;
	FunctorProp functorprop;
	string funcname;

		funcname = "ReCalc Drainage Function";
		pFunc = new Func(MPROFILE, WATER, WATERUPTAKE, NOBPROC, NORMAL);



		functorprop.FuncName = MyFunc::DRAINAGE_RECALC;
		functorprop.Arg = MyArgType::BLANK;
		Define(pFunc, functorprop, this, funcname);

		auto tt = pFunc->MakeOperation();



		//pFunc=FuncFactory(SIMPROFILE, GENERAL, NOFPROC, DECOMPOSITION, NORMAL);
		pFunc= new Func(SIMPROFILE,GENERAL, NOFPROC,DECOMPOSITION, NORMAL);
		functorprop.FuncName = MyFunc::DRAINAGE_EMPIRICAL;
		functorprop.Arg = MyArgType::D1;
		funcname = "Empirical Drainage Equation";
		Define(pFunc, functorprop,this,funcname); 
		double res=pFunc->GetValue(3.);


		pFunc= new Func(SIMPROFILE,GENERAL, NOFPROC,DECOMPOSITION, NORMAL);

		functorprop.FuncName = MyFunc::DRAINAGE_PHYSICAL;
		functorprop.Arg = MyArgType::D1;
		funcname = "Physical Based Drainage Equation";
		Define(pFunc, functorprop, this, funcname);
		res=pFunc->GetValue(3.);

		//fpoint=&Soil_Drainage_Functions::SeepageFunction;
		pFunc= new Func(SIMPROFILE,GENERAL, NOFPROC,DECOMPOSITION, NORMAL);
		functorprop.FuncName = MyFunc::DRAINAGE_SEEPAGE;
		functorprop.Arg = MyArgType::D1;
		funcname = "Bottom Boundary Seepage Equation";
		Define(pFunc, functorprop, this, funcname);
		//res=pFunc->GetValue(3.);

		//fpoint=&Soil_Drainage_Functions::DitchLevelFunction;
		pFunc= new Func(SIMPROFILE,GENERAL, NOFPROC,DECOMPOSITION, NORMAL);
		functorprop.FuncName = MyFunc::DRAINAGE_DITCHLEVEL;
		functorprop.Arg = MyArgType::D1;
		funcname = "Ditch Level Function";
		Define(pFunc, functorprop, this, funcname);


	//	Define(pFunc, m_fpointer0_1.size(),0, this, "Ditch Level Function");	m_fpointer0_1.push_back(fpoint);
		res=pFunc->GetValue(3.);

		return true;
}


bool Soil_Drainage_Functions::SetDrainageFunctors()
{
//	m_pModelStructure = ((Simulator*)m_pModelInfo)->GetSimStructurePointer();
	f_EmpiricalDrainage = EmpDrainage(GFLevBase, GFLevPeak, GFlowBase, GFlowPeak);
	f_PhysDrainageRate = DrainFlowRate(this);
	f_PhysicalDrainage = DrainFlowRate(this);
	f_SeepageFlow = SeepageFlow(this);
	f_DitchLevel = DitchLevel(DrainLevelMin);
	f_DeepPerc = DeepPercolation(m_pModelStructure,this,((Soil_HeatF*)pHeat), m_pModelInfo);
	if (f_EmpiricalDrainage != nullptr) return true;

	return false;
}

double Soil_Drainage_Functions::SumDrainage() {
//Use CommonStruc, Only	: SPoolRunoff

    return TotalDrainage+((Irrigation*)pAtm)->SpoolRunoff+DeepPerc;
}

double Soil_Drainage_Functions::GW_LowestLayer() {
//! Calculates groundwater layer when not found above
	LayerOfSatLevel=NL-1;
	return SatLev_F(LayerOfSatLevel, Psi.back());
}





double  Soil_Drainage_Functions::DrainFlow(size_t i) {
//Use Hydraulic, Only	: WcondSat

	 if(Below) {
		Psi[i]=Psi[i]-GWpressure(i,SatLev);
		Zero=false;
	 }
	 else {
		 bool BelowInit;
		 BelowInit=GWposition(i, Psi[i-1], Psi[i]);
		 if(BelowInit&&i>=LayerOfSatLevel) {
			SatLev=SatLev_F(i-1, Psi[i-1]);
			if(i-1<LayerOfSatLevel) Zero=true;
			LayerOfSatLevel=i-1;
			Psi[i]=Psi[i]-GWpressure(i, SatLev);
			Below=BelowInit;
		 }
		 else if(!BelowInit&&i+1>=Psi.size()) {
			 SatLev= -(DepthLayers.back()-0.5*ThicknessLayers.back()+Psi[i]/100);
		}
			return 0.;	 
	 }

	 if(PhysicalDrainEq==0||!Below) 
	   return 0;
	 else  	
       return f_PhysDrainageRate(i, SatLev);
}

bool  Soil_Drainage_Functions::GWposition(size_t i, double PsiUpper, double PsiLower) {

double Critic;	

   if(PsiLower>15.) 
	   return false;
   
   else {
      Critic=100*DistanceMidpLayers[i];
      if(PsiUpper<Critic&&PsiLower<=0.1||PsiLower<-5.&&PsiUpper<Critic+10)	
		 return true;
	  else
		 return false;
	  
   }
}
double Soil_Drainage_Functions::SatLev_F(size_t i, double PsiUpper) {
 if(i==2) 
      return -(DepthLayers[i]+max(PsiUpper,0.)/100);
 else if(i<DepthLayers.size())
      return -(DepthLayers[i]-0.5*ThicknessLayers[i]+max(PsiUpper,0.)/100.);
 else
	  return -(DepthLayers.back()-0.5*ThicknessLayers.back()+max(PsiUpper,0.)/100.);
 }
double Soil_Drainage_Functions:: GWpressure(size_t i, double SatLev) {
  return 100.*(DepthLayers[i]-0.5*ThicknessLayers[i]+SatLev);
}

void Soil_Drainage_Functions::Drain_Drive() {

      double ReadValue; 
	  ReadValue=GetEcoDat(&m_PGPOS_Gw, IGW_Pos);
	  /*if(pModelStructure->TimeResolution>=1) {
		  ReadValue=READECOScalar(T_Current+0.5d0, IDDRAIN)  
      else
		  ReadValue=READECOScalar(T_Current, IDDRAIN)  
      }*/
	  
	//  if(ReadValue<0.8*MISSING)  ReadValue=DLevel;
	  
	  vDriveDrainLevel=ReadValue;
     
      if(DriveDrainLevel==1) 
            DLevel=ReadValue ;     

}
//END SUBROUTINE DRAIN_Drive

//SUBROUTINE PSILOWER_Drive()

//	Use Timeinfo
//	Use Fileinfo
void Soil_Drainage_Functions::PsiLower_Drive() { 

	   PsiBottom=GetEcoDat(&m_PGPOS_PressureHead, IPressureHead_Pos );//READECOScalar(T_Current+0.5d0, IDDRAIN)



	
	 }

void Soil_Drainage_Functions::SetDrainLevel() {
    if(DriveDrainLevel==1||LBoundSaturated==4) 
	   Drain_Drive();
	else if(DriveDrainLevel==2) 
		DLevel=f_DitchLevel(((Irrigation*)pAtm)->SpoolRunoff);  			//	! Drainage level 
	else if(LBoundUnSaturated==2)
	  PsiLower_Drive();//			! bottom pressure head
	

}
//END Subroutine PsiLower_drive












