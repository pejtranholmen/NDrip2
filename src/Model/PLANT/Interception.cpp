#include "Interception.h"
#include "../Atmosphere/Irrigation.h"
#include "../../Simulator/Simulator.h"
#include "Functors/InterCeption_Functors.h"

	

Interception::Interception(void)
{
	ModuleName="Interception";
	ModuleNo=10;
	ModuleConditions="PrecInterception>0";
}
bool Interception::Def()
{
	Potential_Transpiration::Def();
	ModuleNo = 10;
	ModuleConditions = "PlantType > 0";

	Sw* pSw;

	pSw = new Sw(&PrecInterception, ATMBOUNDARY, PRECIPITATION, INTERCEPTION, NOBPROC, NORMAL);
	Define(pSw, "PrecInterception", 0, "off|on", "WaterEq > 0", 107);
	pSw = new Sw(&SnowInterception, ATMBOUNDARY, PRECIPITATION, INTERCEPTION, NOBPROC, NORMAL);
	Define(pSw, "SnowInterception", 0, "off|on", "PlantType > 1|WaterEq > 0|PrecInterception>0");
	pSw = new Sw(&SnowIntUnload, ATMBOUNDARY, PRECIPITATION, INTERCEPTION, NOBPROC, NORMAL);
	Define(pSw, "SnowIntUnload", 1, "Thermal Quality|Air Temperature", "PlantType > 1|WaterEq > 0|SnowInterception>0");
	pSw = new Sw(&InterceptionModel, ATMBOUNDARY, PRECIPITATION, INTERCEPTION, NOBPROC, NORMAL);
	Define(pSw, "InterceptionModel", 0, "Threshhold|Exponential", "PlantType == 3|WaterEq > 0|SnowInterception>0");

	Ps* pPs;
	Func *pFunc;
	string funcname;
	FunctorProp functorprop;
	vector<Ps*> ps_vec;
	vector<P*> p_vec;
	ps_vec.clear();

	PLOT_PARAM plotpar;
	plotpar.Num_Of_Curves = 3;
	plotpar.Num_Of_Points = 100;
	plotpar.Num_Of_X = 100;
	plotpar.X_Min = 0;
	plotpar.X_Max = 10;
	plotpar.X_Label = "Potential Transpiration (mm/day)";
	plotpar.Y_Label = "Potential WaterUptake (mm/day)";
	plotpar.LogXScale = false;
	plotpar.LogYScale = false;
	plotpar.Num_Of_Opt = 1;
	plotpar.LegendsVec = { "Interception 0", "Interception 3","Interception 6" };
	plotpar.X2_Vec = { 0.,3.,6. };



	pPs = new Ps(&EPRatio, ATMBOUNDARY, PRECIPITATION, INTERCEPTION, NOBPROC, NORMAL);
	Define(pPs, "Ratio_Eva-Transp", 3., "-", "PlantType >= 1|PrecInterception > 0|WaterEq > 0", 0., 10., 1., 5.);
	ps_vec.push_back(pPs);

	functorprop.FunctorInitOption = string::npos;
	functorprop.FuncName = MyFunc::POTENTIAL_WATERUPTAKE_INTERCEPTION_IMPLICIT; functorprop.Arg = MyArgType::D2;
	funcname = "Potential Water Uptake - InterCeption Evaporation";
	pFunc = new Func(SINGLEPLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar);  ps_vec.clear();

	
	
	
	functorprop.FuncName = MyFunc::POTENTIAL_WATERUPTAKE_INTERCEPTION_EXPLICIT; functorprop.Arg = MyArgType::D3;
	funcname = "Potential Water Uptake - InterCeption Evaporation and Ratio Evaporation/Transpiration";
	pFunc = new Func(SINGLEPLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec); ps_vec.clear();

	pPs = new Ps(&IntLAI, ATMBOUNDARY, PRECIPITATION, INTERCEPTION, NOBPROC, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "WaterCapacityPerLAI", 0.2, "mm/m2", "PlantType > 1|PrecInterception > 0|WaterEq > 0", 0.05, 1., 0.1, 0.5);
	pPs = new Ps(&IntBase, ATMBOUNDARY, PRECIPITATION, INTERCEPTION, NOBPROC, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "WaterCapacityBase", 0., "mm", "PlantType > 1|PrecInterception > 0|WaterEq > 0", 0.0, 10., 0., 6.);


	functorprop.FuncName = MyFunc::INTERCEPTION_RAIN_CAPACITY; functorprop.Arg = MyArgType::D1;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Water Interception Function";
	pFunc = new Func(SINGLEPLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec); //ps_vec.clear();


	pPs = new Ps(&IntLAIFrozen, ATMBOUNDARY, PRECIPITATION, INTERCEPTION, NOBPROC, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "SnowCapacityPerLAI", 1.0, "mm/m2", "PlantType > 1|SnowInterception > 0|WaterEq > 0", .05, 10., 0.1, 4.);

	functorprop.FuncName = MyFunc::INTERCEPTION_SNOW_TA; functorprop.Arg = MyArgType::D2;
	funcname = "Water and Snow Interception Function from Temp";
	pFunc = new Func(SINGLEPLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec); ps_vec.clear();

	functorprop.FuncName = MyFunc::INTERCEPTION_SNOW_THQUAL; functorprop.Arg = MyArgType::D2;
	funcname = "Water and Snow Interception Function from Thermal Quality";
	pFunc = new Func(SINGLEPLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec); ps_vec.clear();




	pPs = new Ps(&IntEvapFracMin, ATMBOUNDARY, PRECIPITATION, INTERCEPTION, NOBPROC, NORMAL);
	Define(pPs, "IntEvapFracMin", 1., "-", "PlantType > 2|PrecInterception > 0|WaterEq > 0", 0.0, 1., 0., 1.);
	ps_vec.push_back(pPs);
	ps_vec.push_back(m_pModelMap->GetPsPointer("Ratio_Eva-Transp"));
	pPs = new Ps(&TiStep, ATMBOUNDARY, TIME, NOFPROC, NOBPROC, DETAILED);
	Define(pPs, "Time Step Interception", .03125, UNIT_TYPES::TIME_UNIT, "WaterEq >100", 0.1E-9, 1., .125, 1.); ps_vec.push_back(pPs);
	plotpar.Num_Of_Curves = 2;
	plotpar.X_Max = 6;
	plotpar.X_Label = "Potential Transpiration (mm/day)";
	plotpar.Y_Label = "Interception Evaporation (mm/day)";
	plotpar.LogXScale = false;
	plotpar.LogYScale = false;
	plotpar.Num_Of_Opt = 1;
	plotpar.LegendsVec = { "Half Storage (1 mm)", "Full Storage (2 mm)" };
	plotpar.X2_Vec = { 1.,2.};
	plotpar.X3_Vec = { 2.,2.};


	functorprop.FuncName = MyFunc::INTERCEPTION_EVAPORATION_IMPLICIT_PLANT; functorprop.Arg = MyArgType::D3;
	funcname = "Interception Evaporation Rate - Storage and Pot Transpiration";
	pFunc = new Func(SINGLEPLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar);
	ps_vec.erase(ps_vec.begin() + 1);
	
	
	
	functorprop.FuncName = MyFunc::INTERCEPTION_EVAPORATION_EXPLICIT_PLANT; functorprop.Arg = MyArgType::D3;
	funcname = "Interception Evaporation Rate - Storage and ratio Pot Evaporation/Transpiration";
	pFunc = new Func(SINGLEPLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec); ps_vec.clear();


	pPs = new Ps(&IntSnowMeltScale, ATMBOUNDARY, PRECIPITATION, INTERCEPTION, NOBPROC, NORMAL);
	Define(pPs, "IntSnowMeltScale", 1., "-", "PlantType > 1|SnowInterception > 0|WaterEq > 0|SnowIntUnload<=0", 0., 5., 0., 5.);

	pPs = new Ps(&IntRS, ATMBOUNDARY, PRECIPITATION, INTERCEPTION, NOBPROC, NORMAL);
	Define(pPs, "WithinCanopyRes", 5.0, "s/m", "PlantType >= 2|PrecInterception > 0|WaterEq > 0", 0., 100., 1., 50.);
	pPs = new Ps(&IntMaxCondensation, ATMBOUNDARY, PRECIPITATION, INTERCEPTION, NOBPROC, NORMAL);
	Define(pPs, "Max Int Condens Rate", 1.0, "mm/day", "PlantType >= 2|PrecInterception > 0|WaterEq > 0", 0., 10., 0., 2.);
	pPs = new Ps(&DirectThroughfall, ATMBOUNDARY, PRECIPITATION, INTERCEPTION, NOBPROC, NORMAL);
	Define(pPs, "DirectThroughfall", 0., "-", "PrecInterception > 0|WaterEq > 0|PlantType=1", 0., 1., 0., .5);
	P* pP;
	vector<double> initv;
	NE *pNumPlants;
	pNumPlants = m_pModelMap->GetNEPointer("Number of Plants");
	pP = new P(&MaxCover, pNumPlants, PLANT, PRECIPITATION, INTERCEPTION, NOBPROC, NORMAL);
	SurfaceCoverTab.push_back(pP);
	initv.assign(NumPlants, 0.6);
	Define(pP, pNumPlants, "Maximal Cover", initv, "-", "PlantType = 3|PrecInterception >0|WaterEq > 0", 0., 10.);

	pP = new P(&LAICoverSens, pNumPlants, PLANT, PLANT_SHAPE, INTERCEPTION, NOBPROC, NORMAL);
	SurfaceCoverTab.push_back(pP);
	Define(pP, pNumPlants, "LAI Cover Sensitivity", initv, "-", "PlantType = 3|PrecInterception >0|WaterEq > 0", 0., 1.);





	Tab *pTab = new Tab(&SurfaceCoverTab, PLANT, PLANT_SHAPE, INTERCEPTION, NOBPROC, NORMAL);
	Define(pTab, "Surface cover function for different plants",
		"Maximal Cover|LAI Cover Sensitivity", "PlantType = 3|PrecInterception >0|WaterEq >0");

	Gs* pGs = new Gs(&IntCapacity, SINGLEPLANT, WATER, INTERCEPTION, NOBPROC, NORMAL);
	Define(pGs, "InterceptionCapacity", 0., "mm");
	pGs = new Gs(&IntRate, SINGLEPLANT, WATER, INTERCEPTION, NOBPROC, NORMAL);
	Define(pGs, "InterceptionRate", 0.,UNIT_TYPES::WATERFLOW_UNIT);
	pGs = new Gs(&IntStorage, SINGLEPLANT, WATER, INTERCEPTION, NOBPROC, NORMAL);
	Define(pGs, "InterceptionStorage", 0., "mm");
	pGs = new Gs(&ThroughFall, SINGLEPLANT, WATER, INTERCEPTION, NOBPROC, NORMAL);
	Define(pGs, "Throughfall", 0.,UNIT_TYPES::WATERFLOW_UNIT);
	pGs = new Gs(&EvaPotInt, SINGLEPLANT, WATER, INTERCEPTION, NOBPROC, NORMAL);
	Define(pGs, "InterceptionPotEva", 0.,UNIT_TYPES::WATERFLOW_UNIT);
	pGs = new Gs(&EvaActInt, SINGLEPLANT, WATER, INTERCEPTION, NOBPROC, NORMAL);
	Define(pGs, "InterceptionActEva", 0.,UNIT_TYPES::WATERFLOW_UNIT);
	pGs = new Gs(&IntThQual, SINGLEPLANT, WATER, INTERCEPTION, NOBPROC, NORMAL);
	Define(pGs, "InterceptedWaterThQ", 0., "-");

	X *pX; G *pG;
	initv.assign(NumPlants, 0.);
	pX = new X(&MC_IntStorage, pNumPlants, PLANT, PRECIPITATION, INTERCEPTION, NOBPROC, NORMAL);
	Define(pX, pNumPlants, "Canopy IntercStorage", initv, "mm", "PrecInterception > 0|PlantType = 3|WaterEq > 0");

	pG = new G(&MC_IntCapacity, pNumPlants, PLANT, PRECIPITATION, INTERCEPTION, NOBPROC, NORMAL);
	Define(pG, pNumPlants, "Canopy Interc Capac", initv, "mm", "PrecInterception > 0|PlantType = 3|WaterEq > 0");


	T* pT = new T(&MC_EvaActInt, pNumPlants, PLANT, PRECIPITATION, INTERCEPTION, NOBPROC, NORMAL);
	Define(pT, pNumPlants, "Canopy Interc ActEva", initv,UNIT_TYPES::WATERFLOW_UNIT, "PrecInterception > 0|PlantType = 3|WaterEq > 0");

	pG = new G(&MC_EvaPotInt, pNumPlants, PLANT, PRECIPITATION, INTERCEPTION, NOBPROC, NORMAL);
	Define(pG, pNumPlants, "Canopy Interc PotEva", initv,UNIT_TYPES::WATERFLOW_UNIT, "PrecInterception > 0|PlantType = 3|WaterEq > 0");


	return true;

}


bool Interception::Ini()	{
	//m_pModelStructure = ((Simulator*)m_pModelInfo)->GetSimStructurePointer();
	Irrigation *pMet = (Irrigation*)p_Atm;
	Potential_Transpiration::Ini();
	if(PlantType_Sw(m_pModelStructure->PlantType)==PlantType_Sw::Explicit_Multi_Leaves) {
		IndexList.resize(NumPlants*2);
		Int_Filling.resize(NumPlants);
	}

	if(!m_pModelInfo->StartStateValue) 
		MC_IntStorage.assign(NumPlants,0.);
	
	for (size_t i = 0; i < IndexList.size(); i++) {
		IndexList[i] = i ;

	}
	
	
	IntStorage=0.0;
	TiStep = m_pModelInfo->T_StepSnow;

	return true;
}
bool Interception::IniFunctors() {
	//m_pModelStructure = ((Simulator*)m_pModelInfo)->GetSimStructurePointer();
	Irrigation *pMet = (Irrigation*)p_Atm;
	Potential_Transpiration::IniFunctors();


	if (PlantType_Sw(m_pModelStructure->PlantType) == PlantType_Sw::Explicit_Multi_Leaves) {
		if (SnowInterception_Sw(SnowInterception) == SnowInterception_Sw::OFF) f_IntCapacity_Base_MultiLayer = InterceptionCapacity(IntLAI, IntBase);
		else if (SnowIntUnload_Sw(SnowIntUnload) == SnowIntUnload_Sw::THERMAL_QUALITY) f_IntCapacity_Snow_MultiLayer = InterceptionCapacity_THQUAL(pMet, IntLAI, IntBase, IntLAIFrozen);
		else  f_IntCapacity_Snow_MultiLayer = InterceptionCapacity_TA(pMet, IntLAI, IntBase, IntLAIFrozen);
	}
	else {
		if (SnowInterception_Sw(SnowInterception) == SnowInterception_Sw::OFF) f_IntCapacity_Base = Get_D1_Functor(MyFunc::INTERCEPTION_RAIN_CAPACITY);
		else if (SnowIntUnload_Sw(SnowIntUnload) == SnowIntUnload_Sw::THERMAL_QUALITY) f_IntCapacity_Snow = Get_D2_Functor(MyFunc::INTERCEPTION_SNOW_THQUAL);
		else f_IntCapacity_Snow = Get_D2_Functor(MyFunc::INTERCEPTION_SNOW_TA);
	}
	if (SnowInterception_Sw(SnowInterception) == SnowInterception_Sw::ON && SnowIntUnload_Sw(SnowIntUnload) == SnowIntUnload_Sw::THERMAL_QUALITY) {
		f_ThermalQualityInterCept = ThermalQualityIntceptedWater(pMet, IntSnowMeltScale);
		f_ThermalQualityThroughFall = ThermalQualityThroughFall(pMet);
	}


	f_ThroughFall = ThroughFallRateWithoutPlants(pMet);
	if (PlantType_Sw(m_pModelStructure->PlantType) == PlantType_Sw::NoVegetation || PrecInterception_Sw(PrecInterception) <= PrecInterception_Sw::OFF) {

		if (PrecInterception_Sw(PrecInterception) <= PrecInterception_Sw::ON) {
			f_EvapImplicitPlant = Get_D3_Functor(MyFunc::INTERCEPTION_EVAPORATION_IMPLICIT_PLANT);
			f_PotentialWaterUptakeImplicitPlant = Get_D2_Functor(MyFunc::POTENTIAL_WATERUPTAKE_INTERCEPTION_IMPLICIT);
		}
	}
	else if (PlantType_Sw(m_pModelStructure->PlantType) == PlantType_Sw::Implicit_Big_Leaf) {
		f_ThroughFallSinglePlant = ThroughFallRateSinglePlant(DirectThroughfall, m_pModelInfo->T_StepSnow, pMet);
		f_IntStorage = InterceptionStorageSinglePlant(DirectThroughfall, m_pModelInfo->T_StepSnow, pMet);
		f_EvapImplicitPlant = Get_D3_Functor(MyFunc::INTERCEPTION_EVAPORATION_IMPLICIT_PLANT);
		f_PotentialWaterUptakeImplicitPlant = Get_D2_Functor(MyFunc::POTENTIAL_WATERUPTAKE_INTERCEPTION_IMPLICIT);
	}
	else if (PlantType_Sw(m_pModelStructure->PlantType) == PlantType_Sw::Explicit_Single_Leaf) {
		f_ThroughFallSinglePlant = ThroughFallRateSinglePlant(DirectThroughfall, m_pModelInfo->T_StepSnow, pMet);
		f_IntStorage = InterceptionStorageSinglePlant(DirectThroughfall, m_pModelInfo->T_StepSnow, pMet);
		f_EvapExplicitPlant = Get_D4_Functor(MyFunc::INTERCEPTION_EVAPORATION_EXPLICIT_PLANT);
		f_PotentialWaterUptakeExplicitPlant = Get_D3_Functor(MyFunc::POTENTIAL_WATERUPTAKE_INTERCEPTION_EXPLICIT);
	}
	else if (PlantType_Sw(m_pModelStructure->PlantType) == PlantType_Sw::Explicit_Multi_Leaves) {
		f_PM_EquationPotentialIntercept_MultiLayer = PotentialEvaporation_Interception(pMet, IntRS);
		bool ModelOption = false; if (InterceptionModel_Sw(InterceptionModel) == InterceptionModel_Sw::EXPONENTIAL) ModelOption = true;
		f_MultiFill = MultiFillByPass(ModelOption, pMet->PRainLim, m_pModelInfo->T_StepSnow, MaxCover, LAICoverSens);
		f_EvapMultiPlant = InterceptionEvaporationMultiPlant(IntEvapFracMin, IntMaxCondensation, TiStep);
		f_PotentialWaterUptakeMultiPlant = PotentialWaterUptake();
	}


	return true;
}
bool Interception::Dyn()
{
	Potential_Transpiration::Dyn();
	if(PlantType_Sw(m_pModelStructure->PlantType)== PlantType_Sw::NoVegetation) {
		Irrigation *pMet=(Irrigation*)p_Atm;
		ThroughFall=pMet->PrecCorr+pMet->IrrigationRate*(1-pMet->SoilIrrigF);
		ThroughFall = f_ThroughFall();
		return true;
	}
	Flux();

	return true;
}

void Interception::Flux()
{
  Irrigation *pMet = (Irrigation*)p_Atm;
  double ZRatio, IntFracNew,PotInt,IntEvapFrac=1.;


  //!=====================================================
  //! Case 0 - No interception represented              ==
  //!=====================================================
  if(PrecInterception_Sw(PrecInterception)<= PrecInterception_Sw::OFF|| Evaporation_Sw(m_pModelStructure->Evaporation)<= Evaporation_Sw::off) {
	 ThroughFall = f_ThroughFall();
     PotWaterUptake=pMet->PotTransp;
     if(PlantType_Sw(m_pModelStructure->PlantType)== PlantType_Sw::Explicit_Multi_Leaves)
        for(size_t i=0; i<MC_PotWaterUptake.size(); i++)  MC_PotWaterUptake[i]=MC_PotTransp[i];
     else
        MC_PotWaterUptake.front()=pMet->PotTransp;    
     return;
  }

 // =====================================================================================
//  ! 2.1 Multiple plant interception model-liquid or frozen interception treated equally     
//  !======================================================================================
  if(PlantType_Sw(m_pModelStructure->PlantType) == PlantType_Sw::Explicit_Multi_Leaves) {
	  MC_EvaPotInt = f_PM_EquationPotentialIntercept_MultiLayer(MC_ResAir, LAI);
	  if (SnowInterception_Sw(SnowInterception) == SnowInterception_Sw::OFF)
		  MC_IntCapacity = f_IntCapacity_Base_MultiLayer(LAI);
	  else if(SnowIntUnload_Sw(SnowIntUnload) == SnowIntUnload_Sw::AIR_TEMPERATURE)
		  MC_IntCapacity = f_IntCapacity_Snow_MultiLayer(LAI, pMet->TAir);
	  else {
		  IntFracNew = 0.;
		  PotInt = IntStorage + f_ThroughFall()*TiStep;
		  if (PotInt > 0.0) IntFracNew = (pMet->PrecCorr + pMet->IrrigationRate*(1 - pMet->SoilIrrigF))*TiStep / PotInt;
		  IntThQual = max(0., IntFracNew*pMet->ThQualP + (1. - IntFracNew)*IntThQual);
		  MC_IntCapacity = f_IntCapacity_Snow_MultiLayer(LAI, IntThQual);
	  }
	  auto IndexPlantsByHeight = [](const vector<double>& h) {
		  vector<size_t> out;
		  vector<pair<double, size_t>> tosort;
		  tosort.resize(h.size() * 2);
		  out.resize(tosort.size());
		  for (size_t i = 0; i < h.size(); i++) {
			  tosort[i].first = h[i];
			  tosort[i].second = i;
			  tosort[h.size() + i].first = h[i] / 2;
			  tosort[h.size() + i].second = i;
		  }
		  sort(tosort.begin(), tosort.end(), [](pair<double, size_t> a, pair<double, size_t> b) {
			  return a.first > b.first;
			  });
		  for (size_t i = 0; i < tosort.size(); i++) {
			  out[i] = tosort[i].second;
		  }
		  return out;
	  };



	  IndexList = IndexPlantsByHeight(PlantHeight);
	  ThroughFall = f_MultiFill(&MC_IntStorage, &IndexList, &MC_IntCapacity, LAI, pMet->TAir, f_ThroughFall());
	  MC_EvaActInt = f_EvapMultiPlant(MC_IntStorage, MC_IntCapacity, MC_EvaPotInt);
	  OutFlow(MC_IntStorage, MC_EvaActInt);

	  MC_PotWaterUptake= f_PotentialWaterUptakeMultiPlant(MC_PotTransp, MC_EvaActInt, MC_EvaPotInt);

     if(sum(MC_IntStorage)<100) {   
         EvaPotInt=sum(MC_EvaPotInt);
		 IntCapacity = sum(MC_IntCapacity);
		 IntStorage = sum(MC_IntStorage);
         EvaActInt=sum(MC_EvaActInt);
		 PotWaterUptake = sum(MC_PotWaterUptake);
		 IntRate = f_ThroughFall()- ThroughFall;
	 }
     else
         MC_IntStorage.assign(NumPlants,0.);


  }

  else {   

    // ! 2.1 Interception model for Single big leaf or implicit vegetation-liquid or frozen
	  if (PlantType_Sw(m_pModelStructure->PlantType) == PlantType_Sw::Explicit_Single_Leaf) {
		  if (SnowInterception_Sw(SnowInterception) == SnowInterception_Sw::ON) {
			  // ! Thermal Quality of precipitation
			  pMet->ThQualP = pMet->ThermQualPrecFunction(pMet->TAir);
			  // ! Thermal Quality of Interception reservoir (mix of old int. and prec.)
			  if (SnowIntUnload == 0) {
				  IntFracNew = 0.;
				  PotInt = IntStorage + (pMet->PrecCorr + pMet->IrrigationRate*(1 - pMet->SoilIrrigF))*TiStep;
				  if (PotInt > 0.0)IntFracNew = (pMet->PrecCorr + pMet->IrrigationRate*(1 - pMet->SoilIrrigF))*TiStep / PotInt;
				  IntThQual = max(0., IntFracNew*pMet->ThQualP + (1. - IntFracNew)*IntThQual);
				  IntCapacity = f_IntCapacity_Snow(LAI_Total, IntThQual);
			  }
			  else
				  IntCapacity = f_IntCapacity_Snow(LAI_Total, pMet->TAir);
		  }
		  else
			  IntCapacity = f_IntCapacity_Base(LAI_Total);
		  if (pMet->SnowReduceLAIFactor.front() >= pMet->SnowReduceLAIThreshold) {
			  EvaPotInt = PMEquation(pMet->TAir, pMet->RadNetTot - pMet->RadNetGround, pMet->VapourPDef, ResAir, IntRS, LAI_Total);
			  ZRatio = max(1., EvaPotInt / pMet->PotTransp);

		  }
		  else {
			  EvaPotInt = 0.;
			  ZRatio = 1.;
			  IntCapacity = 0.;
		  }
	  }
	  else if (PlantType_Sw(m_pModelStructure->PlantType) == PlantType_Sw::Implicit_Big_Leaf) {
		  //!Implicit representation of vegetation
		  ZRatio = EPRatio;
		  EvaPotInt = pMet->PotTransp*ZRatio;
	  }

     //*************** DRIP AND ThroughFall *********************************

	 ThroughFall=f_ThroughFallSinglePlant(IntCapacity, IntStorage);
    // *************** INTERCEPTION STORAGE **********************************

	 IntStorage = f_IntStorage(IntCapacity, IntStorage);
     MC_IntStorage.front()=IntStorage;
    
	 if(pMet->PotTransp>0.&&IntCapacity>0.) 
        IntEvapFrac=min(1.0,max(IntEvapFracMin,IntStorage/IntCapacity));
     else
        IntEvapFrac=1.;

	 // Interception Evaporation
	 if(PlantType_Sw(m_pModelStructure->PlantType) == PlantType_Sw::Implicit_Big_Leaf)
		EvaActInt = f_EvapImplicitPlant( pMet->PotTransp, IntStorage, IntCapacity);
	 else
		EvaActInt = f_EvapExplicitPlant( pMet->PotTransp, IntStorage, IntCapacity, ZRatio);

     MC_EvaActInt.front()=EvaActInt;

     
	 //*************** POTENTIAL WATER UPTAKE ********************************
	      pMet->PotTransp=max(0.001,pMet->PotTransp);
     PotWaterUptake=max(0.001,min(pMet->PotTransp,pMet->PotTransp-abs(EvaActInt)/ZRatio));
	 if (PlantType_Sw(m_pModelStructure->PlantType) == PlantType_Sw::Implicit_Big_Leaf)
		PotWaterUptake = f_PotentialWaterUptakeImplicitPlant(pMet->PotTransp, EvaActInt);
	 else
		 PotWaterUptake = f_PotentialWaterUptakeExplicitPlant(pMet->PotTransp, EvaActInt, ZRatio); 
	 MC_PotWaterUptake.front()=PotWaterUptake;

     IntStorage=IntStorage-EvaActInt*TiStep;     // REMAINING INTERCEPTION STORAGE 
     IntRate=f_ThroughFall()-ThroughFall; // Interception Rate 
  } 
  // === Snow Interception model  to Estimate Thermal Quality ===================================
  if(SnowInterception_Sw(SnowInterception) == SnowInterception_Sw::ON&&SnowIntUnload_Sw(SnowIntUnload) == SnowIntUnload_Sw::THERMAL_QUALITY){
		IntThQual=f_ThermalQualityInterCept(IntThQual, IntStorage, LAI_Total);
        if(ThroughFall>0.)	f_ThermalQualityThroughFall(IntThQual, ThroughFall, DirectThroughfall);      
  }

}


bool Interception::End()
{
return true;
}
Interception::~Interception(void)
{
}
