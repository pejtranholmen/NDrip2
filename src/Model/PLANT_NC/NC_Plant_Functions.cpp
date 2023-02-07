#include "NC_Plant_Functions.h"
#include "./Functors/NC_Plant_Functors.h"
#include "../SOIL/SoilModel.h"
#include "../PLANT/PlantModel.h"
#include "../SOIL_NC/SoilOrgModel.h"
#include "../SOIL_NC/NC_Organic.h"
#include "../Structure/Structure.h"
#include "../Atmosphere/AtmModel.h"

NC_Plant_Functions::NC_Plant_Functions(void)
{
    
	P_StateCompName[0]="NewMobile";
	P_StateCompName[1] = "Leaf";
	P_StateCompName[2] = "Stem";
	P_StateCompName[3] = "Grain";
	P_StateCompName[4] = "Mobile";
	P_StateCompName[5] = "OldLeaf";
	P_StateCompName[6] = "OldStem";
	P_StateCompName[7] = "Root";
	P_StateCompName[8] = "CoarseRoot";
	P_StateCompName[9] = "OldRoot";
	P_StateCompName[10] = "OldCoarseRoot";
	P_StateCompName[11] = "Fungi";


	P_BioElementName[0] = "C ";
	P_BioElementName[1] = "N ";
	P_BioElementName[2] = "P ";
	P_BioElementName[3] = "TE ";


	
	P_ReLink[0] = pair < size_t, size_t>(_LEAF, _GRAIN);
	P_ReLink[1] = pair < size_t, size_t>(_STEM, _GRAIN);
	P_ReLink[2] = pair < size_t, size_t>(_ROOT, _GRAIN);
	P_ReLink[3] = pair < size_t, size_t>(_COARSE_ROOT, _GRAIN);
	P_ReLink[4] = pair <size_t, size_t>(_LEAF, _STEM);
	P_ReLink[5] = pair <size_t, size_t>(_ROOT, _LEAF);
	P_ReLink[6] = pair <size_t, size_t>(_LEAF, _OLD_LEAF);
	P_ReLink[7] = pair <size_t, size_t>(_STEM, _OLD_STEM);
	P_ReLink[8] = pair <size_t, size_t>(_ROOT, _OLD_ROOT);
	P_ReLink[9] = pair <size_t, size_t>(_COARSE_ROOT, _OLD_COARSE_ROOT);
	P_ReLink[10] = pair <size_t, size_t>(_LEAF, _MOBILE);
	P_ReLink[11] = pair <size_t, size_t>(_OLD_LEAF, _MOBILE);
	P_ReLink[12] = pair <size_t, size_t>(_NEWMOBILE, _FUNGI);
	P_ReLink[13] = pair <size_t, size_t>(_OLD_LEAF, _MOBILE);
	P_ReLink[14] = pair <size_t, size_t>(_OLD_STEM, _MOBILE);
	P_ReLink[15] = pair <size_t, size_t>(_OLD_ROOT, _MOBILE);
	P_ReLink[16] = pair <size_t, size_t>(_OLD_COARSE_ROOT, _MOBILE);
	P_ReLink[17] = pair <size_t, size_t>(_LEAF, _MOBILE);
	P_ReLink[18] = pair <size_t, size_t>(_STEM, _MOBILE);
	P_ReLink[19] = pair <size_t, size_t>(_ROOT, _MOBILE);
	P_ReLink[20] = pair <size_t, size_t>(_COARSE_ROOT, _MOBILE);
	P_ReLink[21] = pair <size_t, size_t>( _MOBILE, _LEAF);
	P_ReLink[22] = pair <size_t, size_t>(_MOBILE, _NEWMOBILE);

}


NC_Plant_Functions::~NC_Plant_Functions(void)
{
}
bool NC_Plant_Functions::Def()
{
	NC_Drivings::Def();
	ModuleNo = 24;
	ModuleConditions = "";

	SoilModel* pSoil = (SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
	p_Soil_WaterF = &pSoil->m_SoilFrost;
	PlantModel* pPlant = (PlantModel*)m_pModelInfo->GetModelPtr("Plant Model");
	WaterUptake* p_Plant = &pPlant->m_WaterUptake;
	NE *pNL = m_pModelMap->GetNEPointer("Number of Layers");
	NE *pNumPlants = m_pModelMap->GetNEPointer("Number of Plants");


	Sw* pSw; Ps *pPs; P *pP; X *pX; T *pT; G *pG;   Gs *pGs;
	pSw = new Sw(&Growth, PLANT, GENERAL, TRANSPORT, PHOTOSYNTHESIS, NORMAL);
	Define(pSw, "Growth", 0, "off|Logistic function|Water use efficiency|Radiation use efficiency|Farquhar", "Nitrogen and Carbon > 0", 101);
	pSw = new Sw(&PhotoInput, PLANT, GENERAL, TRANSPORT, PHOTOSYNTHESIS, NORMAL);
	Define(pSw, "PhotoInput", 0, "Simulated|PG-file", "Nitrogen and Carbon > 0");
	pSw = new Sw(&Winter_Regulation, PLANT, GENERAL, TRANSPORT, PHOTOSYNTHESIS, NORMAL);
	Define(pSw, "Winter regulation", 0, "Off|On", "Nitrogen and Carbon > 0|Growth >0");
	pSw = new Sw(&N_Fixation, PLANT, GENERAL, TRANSPORT, NFIXATION, NORMAL);
	Define(pSw, "N fixation", 0, "off|C demand driven|Root biomass driven|Stem and Leaf biomass driven", "Nitrogen and Carbon > 0|Growth >0");
	pSw = new Sw(&N_Fixed_Supply, PLANT, GENERAL, TRANSPORT, PHOTOSYNTHESIS, NORMAL);
	Define(pSw, "N Fixed Supply", 0, "off|on", "Nitrogen and Carbon > 0|Growth >0");
	pSw = new Sw(&Root_Density_Influence, PLANT, GENERAL, TRANSPORT, PLANT_NUPTAKE, NORMAL);
	Define(pSw, "Root Density Influence", 0, "no|on nitrogen uptake", "Nitrogen and Carbon > 0");

	pSw = new Sw(&Root_Allocation_Combination, PLANT, GENERAL, TRANSPORT, REALLOCATION, NORMAL);
	Define(pSw, "Root Alloc Combination", 0, "Maximum value|Multiplicative respons|Average respons", "Nitrogen and Carbon > 0|Growth > 0");
	pSw = new Sw(&Root_Alloc_TotMass, PLANT, GENERAL, TRANSPORT, REALLOCATION, NORMAL);
	Define(pSw, "Root Allocation mass", 0, "Independent|Linear function|Exponential function", "Nitrogen and Carbon > 0|Growth > 0");
	pSw = new Sw(&Root_Alloc_N_Leaf, PLANT, GENERAL, TRANSPORT, REALLOCATION, NORMAL);
	Define(pSw, "Root Allocation N Leaf", 0, "Independent|Linear function|Exponential function", "Nitrogen and Carbon > 0|Growth > 0");
	pSw = new Sw(&Root_Alloc_P_Leaf, PLANT, GENERAL, TRANSPORT, REALLOCATION, NORMAL);
	Define(pSw, "Root Allocation P Leaf", 0, "Independent|Linear function|Exponential function", "Nitrogen and Carbon > 0|Growth > 0|Phosphorus>0");

	pSw = new Sw(&Root_Alloc_WaterStress, PLANT, GENERAL, TRANSPORT, REALLOCATION, NORMAL);
	Define(pSw, "Root Allocation water", 0, "Independent|Linear function|Exponential function", "Nitrogen and Carbon > 0|Growth > 0");
	pSw = new Sw(&Leaf_Alloc_TotMass, PLANT, GENERAL, TRANSPORT, REALLOCATION, NORMAL);
	Define(pSw, "Leaf Allocation shoot", 0, "Independent|Linear function|Exponential function", "Nitrogen and Carbon > 0|Growth > 0");
	pSw = new Sw(&N_ReAllocation, PLANT, GENERAL, TRANSPORT, REALLOCATION, NORMAL);
	Define(pSw, "N ReAllocation", 0, "Off|On", "Nitrogen and Carbon > 0|Growth > 0");
	pSw = new Sw(&Phos_ReAllocation, PLANT, GENERAL, TRANSPORT, REALLOCATION, NORMAL);
	Define(pSw, "Phosph ReAllocation", 0, "As Carbon|As Nitrogen|Indepedent", "Nitrogen and Carbon > 0|Growth > 0|Phosphorus>0");
	pSw = new Sw(&Resp_Temp_Response, PLANT, GENERAL, TRANSPORT, RESPIRATION, NORMAL);
	Define(pSw, "Resp Temp Response", 0, "Common|Q10 whole range|Q10 above threshold", "Nitrogen and Carbon > 0|Growth >0");

	pSw = new Sw(&GrainFillAllocation, PLANT, GENERAL, TRANSPORT, REALLOCATION, NORMAL);
	Define(pSw, "Grain Fill Allocation", 0, "Constant_Rate|Reduced_Rate", "Nitrogen and Carbon > 0|Growth > 0");


	pSw = new Sw(&LitterFall_Dynamics, PLANT, GENERAL, TRANSPORT, LITTERFALL, NORMAL);
	Define(pSw, "Litter fall dynamics", 0, "Static|f(GrowthTempSum)|f(DormingTempSum)", "Nitrogen and Carbon > 0|Growth > 0");
	pSw = new Sw(&Salinity_Stress, PLANT, GENERAL, TRANSPORT, PHOTOSYNTHESIS, NORMAL);
	Define(pSw, "Salinity stress", 0, "off|on photosynthesis|on respiration", "Nitrogen and Carbon > 0|Growth > 0|SaltTracer >0", 104);
	pSw = new Sw(&PlantStress, PLANT, GENERAL, TRANSPORT, PHOTOSYNTHESIS, NORMAL);
	Define(pSw, "Plant Stress", 0, "All multiplicative|Combined Water and Nitrogen function", "Nitrogen and Carbon > 0|Growth > 0");
	pSw = new Sw(&PhoSaturation, PLANT, GENERAL, TRANSPORT, PHOTOSYNTHESIS, NORMAL);
	Define(pSw, "PhoSaturation", 0, "off|on", "Nitrogen and Carbon > 0|Growth > 0|SaltTracer >0");
	;
	pSw = new Sw(&PlantRespiration, PLANT, GENERAL, TRANSPORT, RESPIRATION, NORMAL);
	Define(pSw, "PlantRespiration", 0, "Lumped and Maintenance Only|Growth and Maintenance|Growth and Maintainance and NitrogenUptake", "Nitrogen and Carbon > 0|Growth>0");
	;
	pSw = new Sw(&GrowthResponseVar, PLANT, GENERAL, TRANSPORT, PHOTOSYNTHESIS, NORMAL);
	Define(pSw, "Growth Response Var", 0, "Actual|Mean and Delayed", "Nitrogen and Carbon > 0|Growth>0");
	pSw = new Sw(&RadInterception, PLANT, GENERAL, TRANSPORT, PHOTOSYNTHESIS, NORMAL);
	Define(pSw, "Rad Interception", 0, "Simulated Input|Assumed Input", "Nitrogen and Carbon > 0|Growth>2");
	;
	pSw = new Sw(&InitialPlantConditions, PLANT, GENERAL, TRANSPORT, INITB, NORMAL);
	Define(pSw, "Initial Plant Conditions", 0, "As Nitrogen|As Nitogen and Carbon", "Nitrogen and Carbon > 0|Growth>2");
	pSw = new Sw(&InitialPlantPConditions, PLANT, GENERAL, TRANSPORT, INITB, NORMAL);
	Define(pSw, "Initial Plant P Conditions", 0, "As Phosphorus|As Phosphorus and Carbon", "Nitrogen and Carbon > 0|Growth>2|Phosphorus>0");
	;

	pSw = new Sw(&TerminatingHarvest, PLANT, GENERAL, TRANSPORT, HARVEST, NORMAL);
	Define(pSw, "Terminating Harvest", 0, "off|On", "Nitrogen and Carbon > 0|Growth>0");
	pSw = new Sw(&FungalGrowth, PLANT, GENERAL, TRANSPORT, LIVING, NORMAL);
	Define(pSw, "Mycorrhiza Fungal Growth", 0, "off|Simple|Detailed", "Nitrogen and Carbon > 0|Growth>0");
	;
	pSw = new Sw(&ReAllocationToLeaf, PLANT, GENERAL, TRANSPORT, REALLOCATION, NORMAL);
	Define(pSw, "ReAllocationToLeaf", 0, "off|On", "Nitrogen and Carbon > 0|Growth>0"); //Christine;
	pSw = new Sw(&ResetGSI_AtHarvest, PLANT, GENERAL, TRANSPORT, PHENOLOGY, NORMAL);
	Define(pSw, "Reset GSI at Harvest", 0, "off|On|On at complete grain harvest", "Nitrogen and Carbon > 0|Growth>0"); //Christine;
	// New Harvest variables;
	pSw = new Sw(&Harvest_Day, PLANT, GENERAL, TRANSPORT, REALLOCATION, NORMAL);
	Define(pSw, "Harvest Day", 0, "Fixed Single|Fixed Multiple Days|Estimated GSI|Estimated LAI|PG File specified|In Year Table", "Nitrogen and Carbon > 0|Growth >0");
	pSw = new Sw(&PlantsToHarvest, PLANT, GENERAL, TRANSPORT, REALLOCATION, NORMAL);
	Define(pSw, "Harvest Range", 0, "All Plants|Only One Plant", "Nitrogen and Carbon > 0|Harvest Day >0");
	pSw = new Sw(&Cleaning, PLANT, GENERAL, TRANSPORT, REALLOCATION, NORMAL);
	Define(pSw, "Cleaning operation", 0, "off|All Years|Single Years", "Nitrogen and Carbon > 0|Growth >0");

	pSw = new Sw(&Sowing_Switch, PLANT, GENERAL, TRANSPORT, INITB, NORMAL);
	Define(pSw, "Sowing", 0, "off|Fixed dayNo|Temperature Sum|Multiple DayNo", "Nitrogen and Carbon > 0|Growth >0", 701);
	pSw = new Sw(&Emergence_Switch, PLANT, GENERAL, TRANSPORT, REALLOCATION, NORMAL);
	Define(pSw, "Emergence", 0, "off|Fixed dayNo|Temperature Sum|Multiple DayNo", "Nitrogen and Carbon > 0|Growth >0", 701);

	pSw = new Sw(&Seedling_Management, PLANT, GENERAL, TRANSPORT, INITB, NORMAL);
	Define(pSw, "Seedling Management", 0, "off|After Final Harvest|Specific Years", "Nitrogen and Carbon > 0");


	pSw = new Sw(&StartSeasonTableExist, PLANT, GENERAL, TRANSPORT, REALLOCATION, NORMAL);
	Define(pSw, "StartSeasonTableExist", 0, "off|On", "Nitrogen and Carbon > 0|Growth >0|Growth=0");  //Hidden Switch
	pSw = new Sw(&GreenIndexForcing, PLANT, GENERAL, TRANSPORT, PHOTOSYNTHESIS, NORMAL);
	Define(pSw, "Green Index Forcing", 0, "off|On", "Nitrogen and Carbon > 0|Growth >2");  //Hidden Switch

	pSw = new Sw(&ZadokScale, PLANT, GENERAL, NOFPROC, PHENOLOGY, NORMAL);
	Define(pSw, "ZadokScale", 0, "off|On|WithOutFile_Australia|WithOutFile_France", "Nitrogen and Carbon > 0|Growth >2");  //Hidden Switch

	pSw = new Sw(&ZadokForcingFromFileInput, PLANT, GENERAL, NOFPROC, PHENOLOGY, NORMAL);
	Define(pSw, "ZadokFocingFromFileInput", 0, "off|On", "Nitrogen and Carbon > 0|Growth >2|ZadokScale>0");  //Hidden Switch


	pSw = new Sw(&CanopyCO2_Storage, PLANT, GENERAL, NOFPROC, PHOTOSYNTHESIS, NORMAL);
	Define(pSw, "CanopyCO2_Storage", 0, "Static|Dynamic", "Nitrogen and Carbon > 0|Growth >3");  //Hidden Switch

	F* pF;
	pF = new F(&m_PG_Zadok, PLANT, WATER, FYS_SHAPE, PHENOLOGY, NORMAL);
	Define(pF, "Zadok Forcing", "ZadokInput.bin", "ZadokFocingFromFileInput >= 1");


	vector<double> initv;	initv.assign(p_Plant->NumPlants, 0.);


	for (size_t i = 0; i < NUM_BIOELEM - 1; i++) {
		string conditions = "Nitrogen and Carbon >= 1";
		if (i == _P_) conditions += "|Phosphorus>0";
		for (size_t j = 0; j < NUM_PLANT_COMP; j++) {
			pX = new X(&P_State[j][i], pNumPlants, PLANT, elements(CARBON + i), STORAGE, LIVING, NORMAL);
			Define(pX, pNumPlants, P_BioElementName[i] + P_StateCompName[j], initv, UNIT_TYPES::MASS_UNIT, conditions);
		}
		conditions += "| Mycorrhiza Fungal Growth > 0";
		for (size_t j = NUM_PLANT_COMP; j < NUM_PLANT_FUNGI_COMP; j++) {
			pX = new X(&P_State[j][i], pNumPlants, MYCON, elements(i + 2), STORAGE, LIVING, NORMAL);
			Define(pX, pNumPlants, P_BioElementName[i] + P_StateCompName[j], initv, UNIT_TYPES::MASS_UNIT, conditions);
		}
	}

	SimPlantAge.assign(p_Plant->NumPlants, 0.);
	pX = new X(&SimPlantAge, pNumPlants, PLANT, TIME, STORAGE, PHENOLOGY, NORMAL);
	Define(pX, pNumPlants, "SimPlantAge", initv, "Days", "Nitrogen and Carbon >= 1|Growth >0");
	pX = new X(&GrowthStageIndex, pNumPlants, PLANT, TIME, STORAGE, PHENOLOGY, NORMAL);
	Define(pX, pNumPlants, "GrowthStageIndex", initv, "-", "Nitrogen and Carbon >= 1|Growth >0");
	;
	// Flows;
	pT = new T(&NFixationPlant, pNumPlants, PLANT, NITROGEN, TRANSPORT, NFIXATION, NORMAL);
	Define(pT, pNumPlants, "N FixationPlant", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|N Fixation >0");
	pT = new T(&NPlantAddSupply, pNumPlants, PLANT, NITROGEN, TRANSPORT, NFIXATION, NORMAL);
	Define(pT, pNumPlants, "N PlandAddSupply", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|N Fixed Supply >0");
	pT = new T(&P_Uptake[PLANT_TOT_N][_N_ - 1], pNumPlants, PLANT, NITROGEN, TRANSPORT, PLANT_NUPTAKE, NORMAL);
	Define(pT, pNumPlants, "N SoilNewMobile", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0");
	pT = new T(&P_Uptake[PLANT_TOT_N][_P_ - 1], pNumPlants, PLANT, PHOSPHORUS, TRANSPORT, PLANT_PUPTAKE, NORMAL);
	Define(pT, pNumPlants, "P SoilNewMobile", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|Phosphorus>0");
	;
	pT = new T(&AtmNewMobile, pNumPlants, PLANT, CARBON, TRANSPORT, PHOTOSYNTHESIS, NORMAL);
	Define(pT, pNumPlants, "C AtmNewMobile", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0");

	
		string conditions = "Nitrogen and Carbon >= 1|Growth >0";
		pT = new T(&P_CarbonAlloNewMobile.Leaf, pNumPlants, PLANT, elements(CARBON ), TRANSPORT, PHOTOSYNTHESIS, NORMAL);
		Define(pT, pNumPlants, P_BioElementName[0] + P_StateCompName[0] + P_StateCompName[1], initv,UNIT_TYPES::MASSFLOW_UNIT, conditions);
		pT = new T(&P_CarbonAlloNewMobile.Stem, pNumPlants, PLANT, elements(CARBON), TRANSPORT, PHOTOSYNTHESIS, NORMAL);
		Define(pT, pNumPlants, P_BioElementName[0] + P_StateCompName[0] + P_StateCompName[2], initv,UNIT_TYPES::MASSFLOW_UNIT, conditions);
		pT = new T(&P_CarbonAlloNewMobile.Roots, pNumPlants, PLANT, elements(CARBON), TRANSPORT, PHOTOSYNTHESIS, NORMAL);
		Define(pT, pNumPlants, P_BioElementName[0] + P_StateCompName[0] + P_StateCompName[7], initv,UNIT_TYPES::MASSFLOW_UNIT, conditions);
		pT = new T(&P_CarbonAlloNewMobile.CoarseRoots, pNumPlants, PLANT, elements(CARBON), TRANSPORT, PHOTOSYNTHESIS, NORMAL);
		Define(pT, pNumPlants, P_BioElementName[0] + P_StateCompName[0] + P_StateCompName[8], initv,UNIT_TYPES::MASSFLOW_UNIT, conditions);


		for (size_t i = 0; i < NUM_BIOELEM - 1; i++) {
			string conditions = "Nitrogen and Carbon >= 1|Growth >0";
			if (i == _P_-1) conditions += "|Phosphorus>0";
			pT = new T(&P_UptakeNewMobile[i].Leaf, pNumPlants, PLANT, elements(NITROGEN+i), TRANSPORT, PHOTOSYNTHESIS, NORMAL);
			Define(pT, pNumPlants, P_BioElementName[i+1] + P_StateCompName[0] + P_StateCompName[1], initv,UNIT_TYPES::MASSFLOW_UNIT, conditions);
			pT = new T(&P_UptakeNewMobile[i].Stem, pNumPlants, PLANT, elements(NITROGEN+i), TRANSPORT, PHOTOSYNTHESIS, NORMAL);
			Define(pT, pNumPlants, P_BioElementName[i+1] + P_StateCompName[0] + P_StateCompName[2], initv,UNIT_TYPES::MASSFLOW_UNIT, conditions);
			pT = new T(&P_UptakeNewMobile[i].Roots, pNumPlants, PLANT, elements(NITROGEN + i), TRANSPORT, PHOTOSYNTHESIS, NORMAL);
			Define(pT, pNumPlants, P_BioElementName[i+1] + P_StateCompName[0] + P_StateCompName[7], initv,UNIT_TYPES::MASSFLOW_UNIT, conditions);
			pT = new T(&P_UptakeNewMobile[i].CoarseRoots, pNumPlants, PLANT, elements(NITROGEN + i), TRANSPORT, PHOTOSYNTHESIS, NORMAL);
			Define(pT, pNumPlants, P_BioElementName[i+1] + P_StateCompName[0] + P_StateCompName[8], initv,UNIT_TYPES::MASSFLOW_UNIT, conditions);
		}






	pT = new T(&P_ReAllocation[NEWMOBILE_FUNGI][_C_], pNumPlants, PLANT, CARBON, TRANSPORT, PHOTOSYNTHESIS, NORMAL);
	Define(pT, pNumPlants, "C NewMobileFungi", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|Mycorrhiza Fungal Growth>0");
	;

	for (size_t i = 0; i < NUM_BIOELEM - 1; i++) {
		string conditions = "Nitrogen and Carbon >= 1|Growth >0";
		if (i == _P_) conditions += "|Phosphorus>0";
		for (size_t j = LEAF_GRAIN; j <= COARSE_ROOT_GRAIN; j++) {
			pT = new T(&P_ReAllocation[j][i], pNumPlants, PLANT, elements(CARBON + i), TRANSPORT, PHOTOSYNTHESIS, NORMAL);
			Define(pT, pNumPlants, P_BioElementName[i] + P_StateCompName[P_ReLink[j].first] + P_StateCompName[P_ReLink[j].second], initv,UNIT_TYPES::MASSFLOW_UNIT, conditions);
		}
	}
	/*	pT= new T(&P_Flux.Stem_Grain[_C_]		,  pNumPlants,  PLANT,CARBON, TRANSPORT,REALLOCATION, NORMAL);
			Define(pT, pNumPlants,  "C StemGrain"		,initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|Grain Development > 0");
		pT= new T(&P_Flux.Leaf_Grain[_C_]		,  pNumPlants,  PLANT,CARBON, TRANSPORT,REALLOCATION, NORMAL);
			Define(pT, pNumPlants,  "C LeafGrain"		,initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|Grain Development > 0");
		pT= new T(&P_Flux.Root_Grain[_C_]		,  pNumPlants,  PLANT,CARBON, TRANSPORT,REALLOCATION, NORMAL);
			Define(pT, pNumPlants,  "C RootsGrain"		,initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|Grain Development > 0");
		pT= new T(&CCoarseRootsGrain	,  pNumPlants,  PLANT,CARBON, TRANSPORT,REALLOCATION, NORMAL);
			Define(pT, pNumPlants,  "C CoarseRootsGrain" 	,initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0");*/

	for (size_t i = 0; i < NUM_BIOELEM - 1; i++) {
		string conditions = "Nitrogen and Carbon >= 1|Growth >0";
		if (i == _P_) conditions += "|Phosphorus>0";
		for (size_t j = 0; j < NUM_PLANT_COMP; j++) {
			pT = new T(&P_Harvest[j][i], pNumPlants, PLANT, elements(CARBON + i), TRANSPORT, HARVEST, NORMAL);
			Define(pT, pNumPlants, P_BioElementName[i] + P_StateCompName[j] + "Harvest", initv,UNIT_TYPES::MASSFLOW_UNIT, conditions);
		}
	}

	for (size_t j = 0; j < NUM_PLANT_COMP; j++) {
		pT = new T(&P_Respiration[j], pNumPlants, PLANT, CARBON, TRANSPORT, RESPIRATION, NORMAL);
		Define(pT, pNumPlants, P_BioElementName[0] + P_StateCompName[j] + "Atm", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0");
	}


	/*	pT= new T(&P_Respiration[_ROOT]			,  pNumPlants,  PLANT,CARBON, TRANSPORT,RESPIRATION, NORMAL);
			Define(pT, pNumPlants,  "C RootsAtm"	 	,initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0");
		pT= new T(&P_Respiration[_OLD_ROOT]		,  pNumPlants,  PLANT,CARBON, TRANSPORT,RESPIRATION, NORMAL);
			Define(pT, pNumPlants,  "C OldRootsAtm"   	,initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0");*/
	;
	pT = new T(&P_Respiration[_FUNGI], pNumPlants, PLANT, CARBON, TRANSPORT, RESPIRATION, NORMAL);
	Define(pT, pNumPlants, P_BioElementName[0] + P_StateCompName[11] + "Atm", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|Mycorrhiza Fungal Growth>0");
	;
	pT = new T(&P_Uptake[FUNGI_TOT_N][_N_ - 1], pNumPlants, PLANT, NITROGEN, TRANSPORT, MYCON_NUPTAKE, NORMAL);
	Define(pT, pNumPlants, "N FungiUptake", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|Mycorrhiza Fungal Growth>0");
	pT = new T(&P_Uptake[FUNGI_TOT_N][_P_ - 1], pNumPlants, PLANT, PHOSPHORUS, TRANSPORT, MYCON_NUPTAKE, NORMAL);
	Define(pT, pNumPlants, "P FungiUptake", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|Mycorrhiza Fungal Growth>0|Phosphorus>0");


	pT = new T(&NFungiPlant, pNumPlants, PLANT, NITROGEN, TRANSPORT, MYCON_NUPTAKE, NORMAL);
	Define(pT, pNumPlants, "N FungiPlant", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|Mycorrhiza Fungal Growth>0");
	;
	pT = new T(&PFungiPlant, pNumPlants, PLANT, PHOSPHORUS, TRANSPORT, MYCON_NUPTAKE, NORMAL);
	Define(pT, pNumPlants, "P FungiPlant", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|Mycorrhiza Fungal Growth>0|Phosphorus>0");
	;
	pT = new T(&P_Uptake[FUNGI_ORG_N][_N_ - 1], pNumPlants, PLANT, NITROGEN, TRANSPORT, MYCON_NUPTAKE, NORMAL);
	Define(pT, pNumPlants, "N OrgFungiUptake", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|Mycorrhiza Fungal Growth>0");
	pT = new T(&P_Uptake[FUNGI_ORG_N][_P_ - 1], pNumPlants, PLANT, PHOSPHORUS, TRANSPORT, MYCON_NUPTAKE, NORMAL);
	Define(pT, pNumPlants, "P OrgFungiUptake", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|Mycorrhiza Fungal Growth>0|Phosphorus>0");


	;
	;
	for (size_t i = 0; i < NUM_BIOELEM - 1; i++) {
		string conditions = "Nitrogen and Carbon >= 1|Growth >0";
		if (i == _P_) conditions += "|Phosphorus>0";
		for (size_t j = 0; j <= NUM_PLANT_COMP; j++) {
			pT = new T(&P_LitterFall[j][i], pNumPlants, PLANT, elements(i + CARBON), TRANSPORT, LITTERFALL, NORMAL);
			Define(pT, pNumPlants, P_BioElementName[i] + P_StateCompName[j] + "Litter", initv,UNIT_TYPES::MASSFLOW_UNIT, conditions);

		}
	}

	;
	for (size_t i = 0; i < NUM_BIOELEM - 1; i++) {
		string conditions = "Nitrogen and Carbon >= 1|Growth >0";
		if (i == _P_) conditions += "|Phosphorus>0";
		for (size_t j = LEAF_OLD_LEAF; j <= COARSE_ROOT_OLD_COARSE_ROOT; j++) {
			pT = new T(&P_ReAllocation[j][i], pNumPlants, PLANT, elements(i + 2), TRANSPORT, REALLOCATION, NORMAL);
			Define(pT, pNumPlants, P_BioElementName[i] + P_StateCompName[P_ReLink[j].first] + P_StateCompName[P_ReLink[j].second], initv,UNIT_TYPES::MASSFLOW_UNIT, conditions);
		}
	};

	;
	for (size_t i = 0; i < NUM_BIOELEM - 1; i++) {
		string conditions = "Nitrogen and Carbon >= 1|Growth >0";
		if (i == _P_) conditions += "|Phosphorus>0";
		for (size_t j = LEAF_MOBILE_RETURN; j <= OLD_LEAF_MOBILE_RETURN; j++) {
			pT = new T(&P_ReAllocation[j][i], pNumPlants, PLANT, elements(i + 2), TRANSPORT, REALLOCATION, NORMAL);
			Define(pT, pNumPlants, P_BioElementName[i] + P_StateCompName[P_ReLink[j].first] + P_StateCompName[P_ReLink[j].second] + " ret", initv,UNIT_TYPES::MASSFLOW_UNIT, conditions);
		}
	};
	for (size_t i = 0; i < NUM_BIOELEM - 1; i++) {
		string conditions = "Nitrogen and Carbon >= 1|Growth >0|Winter regulation>0";
		if (i == _P_) conditions += "|Phosphorus>0";

		for (size_t j = OLD_LEAF_MOBILE; j <= MOBILE_NEWMOBILE; j++) {
			pT = new T(&P_ReAllocation[j][i], pNumPlants, PLANT, elements(i + 2), TRANSPORT, REALLOCATION, NORMAL);
			Define(pT, pNumPlants, P_BioElementName[i] + P_StateCompName[P_ReLink[j].first] + P_StateCompName[P_ReLink[j].second], initv,UNIT_TYPES::MASSFLOW_UNIT, conditions);
		}
	};

	//	Farquhar ;
	pT = new T(&Cflux, pNumPlants, PLANT, CARBON, TRANSPORT, PHOTOSYNTHESIS, NORMAL);
	Define(pT, pNumPlants, "CO2 flux", initv, "mol/m2/s", "Nitrogen and Carbon >= 1|Growth = 4");
	;

	initv.assign(p_Plant->NumPlants, 0.);
	for (size_t jj = 1; jj < NUM_BIOELEM - 1; jj++) {
		string conditions = "";
		if (jj == _P_) conditions += "|Phosphorus>0";
		pG = new G(&P_State_CarbonRatio[_LEAF_][jj - 1], pNumPlants, PLANT, CARBON, TRANSPORT, RATIO, NORMAL);
		Define(pG, pNumPlants, "C" + P_BioElementName[jj] + "RatioLeaf", initv, "-", "Nitrogen and Carbon >= 1|Growth >0" + conditions);
		pG = new G(&P_State_CarbonRatio[_STEM_][jj - 1], pNumPlants, PLANT, CARBON, TRANSPORT, RATIO, NORMAL);
		Define(pG, pNumPlants, "C" + P_BioElementName[jj] + "RatioStem", initv, "-", "Nitrogen and Carbon >= 1|Growth >0" + conditions);
		pG = new G(&P_State_CarbonRatio[_GRAIN_][jj - 1], pNumPlants, PLANT, CARBON, TRANSPORT, RATIO, NORMAL);
		Define(pG, pNumPlants, "C" + P_BioElementName[jj] + "RatioGrain", initv, "-", "Nitrogen and Carbon >= 1|Growth >0" + conditions);
		pG = new G(&P_State_CarbonRatio[_ROOT_][jj - 1], pNumPlants, PLANT, CARBON, TRANSPORT, RATIO, NORMAL);
		Define(pG, pNumPlants, "C" + P_BioElementName[jj] + "RatioRoots", initv, "-", "Nitrogen and Carbon >= 1|Growth >0" + conditions);
		pG = new G(&P_State_CarbonRatio[_COARSE_ROOT_][jj - 1], pNumPlants, PLANT, CARBON, TRANSPORT, RATIO, NORMAL);
		Define(pG, pNumPlants, "C" + P_BioElementName[jj] + "RatioCoarseRoots", initv, "-", "Nitrogen and Carbon >= 1|Growth >0" + conditions);
		pG = new G(&P_State_CarbonRatio[_FUNGI_][jj - 1], pNumPlants, PLANT, CARBON, TRANSPORT, RATIO, NORMAL);
		Define(pG, pNumPlants, "C" + P_BioElementName[jj] + "RatioFungi", initv, "-", "Nitrogen and Carbon >= 1|Growth >0|Mycorrhiza Fungal Growth>0" + conditions);
		pG = new G(&DemandUptake[jj - 1].Demand, pNumPlants, PLANT, NITROGEN, TRANSPORT, bioprocess(PLANT_NUPTAKE+jj-1), NORMAL);
		Define(pG, pNumPlants, P_BioElementName[jj] + "PlantDemand", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0" + conditions);
	}
	pG = new G(&SpringDevDayNum, pNumPlants, PLANT, TIME, STORAGE, PHENOLOGY, NORMAL);
	Define(pG, pNumPlants, "SpringDev DayNum", initv, "-", "Nitrogen and Carbon >= 1|Growth >0|Emergence>0");
	pG = new G(&DormingStartDayNum, pNumPlants, PLANT, TIME, STORAGE, PHENOLOGY, NORMAL);
	Define(pG, pNumPlants, "DormingStart DayNum", initv, "-", "Nitrogen and Carbon >= 1|Growth >0|Emergence>0");
	pG = new G(&GrowingStage, pNumPlants, PLANT, TIME, STORAGE, PHENOLOGY, NORMAL);
	Define(pG, pNumPlants, "GrowingStage", initv, "-", "Nitrogen and Carbon >= 1|Growth >0|Emergence>0");


	pG = new G(&SimPlantYearAge, pNumPlants, PLANT, TIME, TRANSPORT, PHENOLOGY, NORMAL);
	Define(pG, pNumPlants, "Sim Plant Year Age", initv, "Years", "Nitrogen and Carbon >= 1|Growth>0");
	pG = new G(&DormingTempSum, pNumPlants, PLANT, TIME, TRANSPORT, PHENOLOGY, NORMAL);
	Define(pG, pNumPlants, "DormingTempSum", initv, UNIT_TYPES::TEMPSUM_UNIT, "Nitrogen and Carbon >= 1|Growth >0");
	pG = new G(&GrowthTempSum, pNumPlants, PLANT, TIME, TRANSPORT, PHENOLOGY, NORMAL);
	Define(pG, pNumPlants, "GrowthTempSum", initv, UNIT_TYPES::TEMPSUM_UNIT, "Nitrogen and Carbon >= 1|Growth >0");
	pG = new G(&ZadokIndex, pNumPlants, PLANT, TIME, STORAGE, PHENOLOGY, NORMAL);
	Define(pG, pNumPlants, "ZadokIndex", initv, "-", "Nitrogen and Carbon >= 1|Growth >0|ZadokScale>0");
	pG = new G(&ZadokDayNumShift, pNumPlants, PLANT, TIME, STORAGE, PHENOLOGY, NORMAL);
	Define(pG, pNumPlants, "Zadok DayNum Shift", initv, "-", "Nitrogen and Carbon >= 1|Growth >0|ZadokScale>0");
	pG = new G(&NFungiAvail, pNumPlants, PLANT, NITROGEN, TRANSPORT, MYCON_NUPTAKE, NORMAL);//HH
	Define(pG, pNumPlants, "N FungiAvailable", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|Mycorrhiza Fungal Growth>0");
	pG = new G(&NFungiDemand, pNumPlants, PLANT, NITROGEN, TRANSPORT, MYCON_NUPTAKE, NORMAL);
	Define(pG, pNumPlants, "N FungiDemand", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|Mycorrhiza Fungal Growth>0");
	pG = new G(&PFungiAvail, pNumPlants, PLANT, PHOSPHORUS, TRANSPORT, MYCON_NUPTAKE, NORMAL);
	Define(pG, pNumPlants, "P FungiAvailable", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|Mycorrhiza Fungal Growth>0|Phosphorus>0");
	pG = new G(&PFungiDemand, pNumPlants, PLANT, PHOSPHORUS, TRANSPORT, MYCON_NUPTAKE, NORMAL);
	Define(pG, pNumPlants, "P FungiDemand", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|Mycorrhiza Fungal Growth>0|Phosphorus>0");
	;
	pG = new G(&FungiNorgUptpot, pNumPlants, PLANT, NITROGEN, TRANSPORT, MYCON_NUPTAKE, NORMAL);
	Define(pG, pNumPlants, "Fungi Organic N upt pot", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|Mycorrhiza Fungal Growth>0");
	pG = new G(&FungiNitrateUptpot, pNumPlants, PLANT, NITROGEN, TRANSPORT, MYCON_NUPTAKE, NORMAL);
	Define(pG, pNumPlants, "Fungi Nitrate upt pot", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|Mycorrhiza Fungal Growth>0");
	pG = new G(&FungiAmmoniumUptpot, pNumPlants, PLANT, NITROGEN, TRANSPORT, MYCON_NUPTAKE, NORMAL);
	Define(pG, pNumPlants, "Fungi Ammonia upt pot", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|Mycorrhiza Fungal Growth>0");
	;
	pG = new G(&FungiPorgUptpot, pNumPlants, PLANT, PHOSPHORUS, TRANSPORT, MYCON_NUPTAKE, NORMAL);
	Define(pG, pNumPlants, "Fungi Organic P upt pot", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|Mycorrhiza Fungal Growth>0|Phosphorus>0");
	pG = new G(&FungiPhosphateUptpot, pNumPlants, PLANT, PHOSPHORUS, TRANSPORT, MYCON_NUPTAKE, NORMAL);
	Define(pG, pNumPlants, "Fungi Phosphate upt pot", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|Mycorrhiza Fungal Growth>0|Phosphorus>0");
	
	pG = new G(&FungiRootCover, pNumPlants, PLANT, AREA, TRANSPORT, MYCON_NUPTAKE, NORMAL);
	Define(pG, pNumPlants, "Fungi Root Cover", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|Mycorrhiza Fungal Growth>0");
	pG = new G(&factCNFungi, pNumPlants, PLANT, MYCORRHIZA, TRANSPORT, MYCON_NUPTAKE, NORMAL);
	Define(pG, pNumPlants, "fact CN Fungi", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|Mycorrhiza Fungal Growth>0");
	pG = new G(&factNSupplyFungi, pNumPlants, PLANT, NITROGEN, TRANSPORT, MYCON_NUPTAKE, NORMAL);
	Define(pG, pNumPlants, "fact Nsupply Fungi", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|Mycorrhiza Fungal Growth>0");
	pG = new G(&factNdemandFungi, pNumPlants, PLANT, NITROGEN, TRANSPORT, MYCON_NUPTAKE, NORMAL);
	Define(pG, pNumPlants, "fact Ndemand Fungi", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|Mycorrhiza Fungal Growth>0");
	pG = new G(&factNavailFungi, pNumPlants, PLANT, NITROGEN, TRANSPORT, MYCON_NUPTAKE, NORMAL);
	Define(pG, pNumPlants, "fact Navail Fungi", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|Mycorrhiza Fungal Growth>0");
	;
	pG = new G(&factCPFungi, pNumPlants, PLANT, MYCORRHIZA, TRANSPORT, MYCON_NUPTAKE, NORMAL);//HH
	Define(pG, pNumPlants, "fact CP Fungi", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|Mycorrhiza Fungal Growth>0|Phosphorus>0");
	pG = new G(&factPSupplyFungi, pNumPlants, PLANT, PHOSPHORUS, TRANSPORT, MYCON_NUPTAKE, NORMAL);
	Define(pG, pNumPlants, "fact Psupply Fungi", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|Mycorrhiza Fungal Growth>0|Phosphorus>0");
	pG = new G(&factPdemandFungi, pNumPlants, PLANT, PHOSPHORUS, TRANSPORT, MYCON_NUPTAKE, NORMAL);
	Define(pG, pNumPlants, "fact Pdemand Fungi", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|Mycorrhiza Fungal Growth>0|Phosphorus>0");
	pG = new G(&factPavailFungi, pNumPlants, PLANT, PHOSPHORUS, TRANSPORT, MYCON_NUPTAKE, NORMAL);
	Define(pG, pNumPlants, "fact Pavail Fungi", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|Mycorrhiza Fungal Growth>0|Phosphorus>0");
	;
	pG = new G(&RadPlantAdsorbed, pNumPlants, PLANT, RADIATION, TRANSPORT, PHOTOSYNTHESIS, NORMAL);
	Define(pG, pNumPlants, "Radiation adsorbed", initv, UNIT_TYPES::HEATFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >2");
	;

	pG = new G(&GPP_Driver.Nitrogen, pNumPlants, PLANT, PLANT_STRESS, TRANSPORT, PHOTOSYNTHESIS, NORMAL); initv.assign(p_Plant->NumPlants, 1.);
	Define(pG, pNumPlants, "Response N", initv, "-", "Nitrogen and Carbon >= 1|Growth >2");
	pG = new G(&GPP_Driver.Nutrient_Water, pNumPlants, PLANT, PLANT_STRESS, TRANSPORT, PHOTOSYNTHESIS, NORMAL);
	Define(pG, pNumPlants, "Response N-Water", initv, "-", "Nitrogen and Carbon >= 1|Growth >2|Plant Stress>0");
	pG = new G(&GPP_Driver.Temp, pNumPlants, PLANT, PLANT_STRESS, TRANSPORT, PHOTOSYNTHESIS, NORMAL);
	Define(pG, pNumPlants, "Response Temp", initv, "-", "Nitrogen and Carbon >= 1|Growth >2");
	pG = new G(&GPP_Driver.Water, pNumPlants, PLANT, PLANT_STRESS, TRANSPORT, PHOTOSYNTHESIS, NORMAL);
	Define(pG, pNumPlants, "Response Water", initv, "-", "Nitrogen and Carbon >= 1|Growth >2");
	pG = new G(&GPP_Driver.Phosphorus, pNumPlants, PLANT, PLANT_STRESS, TRANSPORT, PHOTOSYNTHESIS, NORMAL);
	Define(pG, pNumPlants, "Response Phosphorus", initv, "-", "Nitrogen and Carbon >= 1|Growth >2|Phosphorus>0");
	pG = new G(&GPP_Driver.Light, pNumPlants, PLANT, PLANT_STRESS, TRANSPORT, PHOTOSYNTHESIS, NORMAL);
	Define(pG, pNumPlants, "Response Light", initv, "-", "Nitrogen and Carbon >= 1|Growth >2");
	pG = new G(&GPP_Driver.Salt, pNumPlants, PLANT, PLANT_STRESS, TRANSPORT, PHOTOSYNTHESIS, NORMAL); initv.assign(p_Plant->NumPlants, 1.);
	Define(pG, pNumPlants, "Response Salt", initv, "-", "Nitrogen and Carbon >= 1|Growth >2|Salinity stress>0");
	;



	for (size_t jj = 0; jj < NUM_BIOELEM - 1; jj++) {
		string conditions = "";
		if (jj == _P_) conditions += "|Phosphorus>0";
		pG = new G(&P_SumPlant[_ROOT_ALL][jj], pNumPlants, PLANT, elements(jj + CARBON), TRANSPORT, LIVING, NORMAL); initv.assign(p_Plant->NumPlants, 0.);//HH
		Define(pG, pNumPlants, P_BioElementName[jj] + "Roots", initv, UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Growth >0" + conditions);
		pG = new G(&P_SumPlant[_PLANT_WHOLE][jj], pNumPlants, PLANT, elements(jj + CARBON), TRANSPORT, LIVING, NORMAL);
		Define(pG, pNumPlants, P_BioElementName[jj] + "Plant", initv, UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Growth >0" + conditions);
		pG = new G(&P_SumPlant[_LEAF_ALL][jj], pNumPlants, PLANT, elements(jj + CARBON), TRANSPORT, LIVING, NORMAL);
		Define(pG, pNumPlants, P_BioElementName[jj] + "Total Leaf", initv, UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Growth >0" + conditions);
		pG = new G(&P_SumPlant[_ABOVE_GROUND][jj], pNumPlants, PLANT, elements(jj + CARBON), TRANSPORT, LIVING, NORMAL);
		Define(pG, pNumPlants, P_BioElementName[jj] + "Plant AboveG", initv, UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Growth >0" + conditions);
		pG = new G(&TotalPlantLitterFall[jj], pNumPlants, PLANT, elements(jj + CARBON), TRANSPORT, LIVING, NORMAL);
		Define(pG, pNumPlants, P_BioElementName[jj] + "Plant LitterFall", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0" + conditions);

		pGs = new Gs(&P_AllPlants[_ROOT_ALL][jj], PLANT, elements(jj + CARBON), TRANSPORT, LIVING, NORMAL);
		Define(pGs, P_BioElementName[jj] + "Total Roots", 0.0, UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Growth >0" + conditions);
		pGs = new Gs(&P_AllPlants[_PLANT_WHOLE][jj], PLANT, elements(jj + CARBON), TRANSPORT, LIVING, NORMAL);
		Define(pGs, P_BioElementName[jj] + "Total Plant", 0.0, UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Growth >0" + conditions);
		pGs = new Gs(&P_AllPlants[_ABOVE_GROUND][jj], PLANT, elements(jj + CARBON), TRANSPORT, LIVING, NORMAL);
		Define(pGs, P_BioElementName[jj] + "Total Plant AboveG", 0.0, UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Growth >0" + conditions);
		pG = new G(&Mobile_Plant_Flux[jj], pNumPlants, PLANT, NITROGEN, TRANSPORT, GROWTH, NORMAL);
		Define(pG, pNumPlants, P_BioElementName[jj] + "Mobile Plant Flux", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|N ReAllocation >0" + conditions);
		pGs = new Gs(&Mobile_TotPlant_Flux[jj], PLANT, elements(jj + CARBON), TRANSPORT, GROWTH, NORMAL);
		Define(pGs, P_BioElementName[jj] + "Mobile TotPlant Flux", 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|N ReAllocation >0" + conditions);
		if (jj == 0) continue;
		pG = new G(&UptDeficit_1[jj - 1], pNumPlants, PLANT, elements(jj + CARBON), TRANSPORT, bioprocess(PLANT_NUPTAKE+jj-1), NORMAL);
		Define(pG, pNumPlants, P_BioElementName[jj] + "Uptake Deficit 1", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0" + conditions);
		pG = new G(&UptDeficit_2[jj - 1], pNumPlants, PLANT, elements(jj + CARBON), TRANSPORT, bioprocess(PLANT_NUPTAKE+jj-1), NORMAL);
		Define(pG, pNumPlants, P_BioElementName[jj] + "Uptake Deficit 2", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|Organic Uptake >0" + conditions);
		pG = new G(&UptFungiDeficit_1[jj - 1], pNumPlants, PLANT, elements(jj + CARBON), TRANSPORT, MYCON_NUPTAKE, NORMAL);
		Define(pG, pNumPlants, P_BioElementName[jj] + "UptFungi Deficit 1", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|Mycorrhiza Fungal Growth>0" + conditions);
		pG = new G(&UptFungiDeficit_2[jj - 1], pNumPlants, PLANT, elements(jj + CARBON), TRANSPORT, MYCON_NUPTAKE, NORMAL);
		Define(pG, pNumPlants, P_BioElementName[jj] + "UptFungi Deficit 2", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|Organic Uptake >0|Mycorrhiza Fungal Growth>0" + conditions);
		pG = new G(&DemandUptake[jj - 1].DemandSoil, pNumPlants, PLANT, elements(jj + CARBON), TRANSPORT,bioprocess( PLANT_NUPTAKE+jj-1), NORMAL);
		Define(pG, pNumPlants, P_BioElementName[jj] + "PlantSoilDemand", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0" + conditions);
		pG = new G(&P_Uptake[PLANT_ORG_N][jj - 1], pNumPlants, PLANT, elements(jj + CARBON), TRANSPORT, bioprocess(PLANT_NUPTAKE+jj-1), NORMAL);
		Define(pG, pNumPlants, P_BioElementName[jj] + "PlantOrgUptake", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|OrganicUptake>0" + conditions);
		pGs = new Gs(&C_RatioTotRoots[jj], PLANT, elements(jj + CARBON), TRANSPORT, RATIO, NORMAL);
		Define(pGs, "C" + P_BioElementName[jj] + "Ratio Total Roots", 0.0, "-", "Nitrogen and Carbon >= 1|Growth >0" + conditions);

		pGs = new Gs(&C_RatioTotPlant[jj - 1], PLANT,  elements(jj + CARBON), TRANSPORT, RATIO, NORMAL);
		Define(pGs, "C" + P_BioElementName[jj] + "Ratio Total Plant", 0.0, "-", "Nitrogen and Carbon >= 1|Growth >0" + conditions);




	}



	pG = new G(&CPlantAtm, pNumPlants, PLANT, CARBON, TRANSPORT, RESPIRATION, NORMAL);
	Define(pG, pNumPlants, "C Plant Resp", initv, UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >=1|Growth >0");
	pG = new G(&CPlantAtm_AboveG, pNumPlants, PLANT, CARBON, TRANSPORT, RESPIRATION, NORMAL);
	Define(pG, pNumPlants, "C Plant Resp AboveG", initv, UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >=1|Growth >0");
	initv.assign(p_Soil_WaterF->NL, 0.);
	pG = new G(&CRootsAtmLayer, pNL, TOTPROFILE, CARBON, TRANSPORT, RESPIRATION, NORMAL);
	Define(pG, pNL, "C RootsAtm Layer", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0");
	pG = new G(&CFungiAtmLayer, pNL, TOTPROFILE, CARBON, TRANSPORT, RESPIRATION, NORMAL);
	Define(pG, pNL, "C FungiAtm layer", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|Mycorrhiza Fungal Growth>0");
	pG = new G(&CCoarseRootsAtmLayer, pNL, TOTPROFILE, CARBON, TRANSPORT, RESPIRATION, NORMAL);
	Define(pG, pNL, "C CoarseRootsAtm Layer", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0");



	initv.assign(p_Plant->NumPlants, 0.);
	pG = new G(&NPP_Leaf, pNumPlants, PLANT, CARBON, TRANSPORT, GROWTH, NORMAL);
	Define(pG, pNumPlants, "C NPP Leaf", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >=1|Growth >0");
	pG = new G(&NPP_Stem, pNumPlants, PLANT, CARBON, TRANSPORT, GROWTH, NORMAL);
	Define(pG, pNumPlants, "C NPP Stem", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >=1|Growth >0");
	pG = new G(&NPP_CoarseRoots, pNumPlants, PLANT, CARBON, TRANSPORT, GROWTH, NORMAL);
	Define(pG, pNumPlants, "C NPP CoarseRoot", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >=1|Growth >0");
	pG = new G(&NPP_Roots, pNumPlants, PLANT, CARBON, TRANSPORT, GROWTH, NORMAL);
	Define(pG, pNumPlants, "C NPP Root", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >=1|Growth >0");
	pG = new G(&NPP_Plant, pNumPlants, PLANT, CARBON, TRANSPORT, GROWTH, NORMAL);
	Define(pG, pNumPlants, "C NPP Plant", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >=1|Growth >0");
	pG = new G(&AccNPP_Leaf, pNumPlants, PLANT, CARBON, TRANSPORT, GROWTH, NORMAL);
	Define(pG, pNumPlants, "C AccNPP Leaf", initv, UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >=1|Growth >0");
	pG = new G(&AccNPP_Stem, pNumPlants, PLANT, CARBON, TRANSPORT, GROWTH, NORMAL);
	Define(pG, pNumPlants, "C AccNPP Stem", initv, UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >=1|Growth >0");
	pG = new G(&AccNPP_CoarseRoots, pNumPlants, PLANT, CARBON, TRANSPORT, GROWTH, NORMAL);
	Define(pG, pNumPlants, "C AccNPP CoarseRoot", initv, UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >=1|Growth >0");
	pG = new G(&AccNPP_Roots, pNumPlants, PLANT, CARBON, TRANSPORT, GROWTH, NORMAL);
	Define(pG, pNumPlants, "C AccNPP Root", initv, UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >=1|Growth >0");
	pG = new G(&AccNPP_Plant, pNumPlants, PLANT, CARBON, TRANSPORT, GROWTH, NORMAL);
	Define(pG, pNumPlants, "C AccNPP Plant", initv, UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >=1|Growth >0");
	
	
	
	
	
	
	
	
	;
	pG = new G(&Annual_D_CPlant, pNumPlants, PLANT, CARBON, TRANSPORT, GROWTH, NORMAL);
	Define(pG, pNumPlants, "Annual Change CPlant", initv, UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >=1|Growth >0");
	pGs = new Gs(&Tot_Response_N, PLANT, PLANT_STRESS, TRANSPORT, PHOTOSYNTHESIS, NORMAL);
	Define(pGs, "Total Response N", 0.0, "-", "Nitrogen and Carbon >= 1|Growth >2");
	pGs = new Gs(&Tot_Response_Temp, PLANT, PLANT_STRESS, TRANSPORT, PHOTOSYNTHESIS, NORMAL);
	Define(pGs, "Total Response Temp", 0.0, "-", "Nitrogen and Carbon >= 1|Growth >2");
	pGs = new Gs(&Tot_Response_Water, PLANT, PLANT_STRESS, TRANSPORT, PHOTOSYNTHESIS, NORMAL);
	Define(pGs, "Total Response Water", 0.0, "-", "Nitrogen and Carbon >= 1|Growth >2");
	pGs = new Gs(&Tot_Response_N_Water, PLANT, PLANT_STRESS, TRANSPORT, PHOTOSYNTHESIS, NORMAL);
	Define(pGs, "Total Response N Water", 0.0, "-", "Nitrogen and Carbon >= 1|Growth >2|Plant Stress>0");
	;
	for (size_t jj = 0; jj < NUM_BIOELEM - 1; jj++) {
		string conditions = "";
		if (jj == _P_) conditions += "|Phosphorus>0";
		pGs = new Gs(&TotHarvest[jj], PLANT, elements(jj + CARBON), TRANSPORT, HARVEST, NORMAL);
		Define(pGs, P_BioElementName[jj] + "Total Harvest", 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0" + conditions);
		pGs = new Gs(&TotPlantLitter[jj], PLANT, elements(jj + CARBON), TRANSPORT, LITTERFALL, NORMAL);
		Define(pGs, P_BioElementName[jj] + "Total PlantLitter", 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0" + conditions);
		pGs = new Gs(&TotAbovePlantLitter[jj], PLANT, elements(jj + CARBON), TRANSPORT, LITTERFALL, NORMAL);
		Define(pGs, P_BioElementName[jj] + "Total AbovePLitter", 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0" + conditions);
		pGs = new Gs(&TotFungiLitter[jj], PLANT, elements(jj + CARBON), TRANSPORT, LITTERFALL, NORMAL);
		Define(pGs, P_BioElementName[jj] + "Total FungiLitter", 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Growth >0|Mycorrhiza Fungal Growth>0" + conditions);
		pGs = new Gs(&YearGrainHarvest[jj], PLANT,  elements(jj + CARBON), TRANSPORT, HARVEST, NORMAL);
		Define(pGs, P_BioElementName[jj] + "Grain Latest Harvest", 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1" + conditions);
		pGs = new Gs(&YearTotalHarvest[jj], PLANT, elements(jj + CARBON), TRANSPORT, HARVEST, NORMAL);
		Define(pGs, P_BioElementName[jj] + "Total Latest Harvest", 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1" + conditions);
	}
	;
	pGs = new Gs(&C_TotHarvest_1, PLANT, CARBON, TRANSPORT, HARVEST, NORMAL);
	Define(pGs, "C Total Harvest_1", 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Terminating Harvest>0|Nitrogen and Carbon >= 1|Growth >0");
	pGs = new Gs(&N_TotHarvest_1, PLANT, NITROGEN, TRANSPORT, HARVEST, NORMAL);
	Define(pGs, "N Total Harvest_1", 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Terminating Harvest>0|Nitrogen and Carbon >= 1|Growth >0");
	pGs = new Gs(&C_TotHarvest_2, PLANT, CARBON, TRANSPORT, HARVEST, NORMAL);
	Define(pGs, "C Total Harvest_2", 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Terminating Harvest>0|Nitrogen and Carbon >= 1|Growth >0");
	pGs = new Gs(&N_TotHarvest_2, PLANT, NITROGEN, TRANSPORT, HARVEST, NORMAL);
	Define(pGs, "N Total Harvest_2", 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Terminating Harvest>0|Nitrogen and Carbon >= 1|Growth >0");
	;
	pGs = new Gs(&C_Thinning_1, PLANT, CARBON, TRANSPORT, HARVEST, NORMAL);
	Define(pGs, "C Total Thinning_1", 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Terminating Harvest>0|Nitrogen and Carbon >= 1|Growth >0");
	pGs = new Gs(&N_Thinning_1, PLANT, NITROGEN, TRANSPORT, HARVEST, NORMAL);
	Define(pGs, "N Total Thinning_1", 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Terminating Harvest>0|Nitrogen and Carbon >= 1|Growth >0");
	pGs = new Gs(&C_Thinning_2, PLANT, CARBON, TRANSPORT, HARVEST, NORMAL);
	Define(pGs, "C Total Thinning_2", 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Terminating Harvest>0|Nitrogen and Carbon >= 1|Growth >0");
	pGs = new Gs(&N_Thinning_2, PLANT, NITROGEN, TRANSPORT, HARVEST, NORMAL);
	Define(pGs, "N Total Thinning_2", 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Terminating Harvest>0|Nitrogen and Carbon >= 1|Growth >0");

	for (size_t jj = 1; jj < NUM_BIOELEM - 1; jj++) {
		string conditions = "";
		if (jj == _P_) conditions += "|Phosphorus>0";
		pGs = new Gs(&DemandFungiUptake[jj - 1].TotalDemand, PLANT, elements(jj + CARBON), TRANSPORT, MYCON_NUPTAKE, NORMAL);
		Define(pGs, P_BioElementName[jj] + " Total FungiDemand", 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Mycorrhiza Fungal Growth>0" + conditions);
		pGs = new Gs(&Tot_FungiUptake[jj - 1], PLANT, elements(jj + CARBON), TRANSPORT, MYCON_NUPTAKE, NORMAL);
		Define(pGs, P_BioElementName[jj] + " Total FungiUptake", 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Mycorrhiza Fungal Growth>0" + conditions);
		pGs = new Gs(&Tot_PlantUptake[jj - 1], PLANT, elements(jj + CARBON), TRANSPORT, bioprocess(PLANT_NUPTAKE+jj-1), NORMAL);
		Define(pGs, P_BioElementName[jj] + " Total PlantUptake", 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1" + conditions);
		pGs = new Gs(&DemandUptake[jj - 1].TotalDemand, PLANT,  elements(jj + CARBON), TRANSPORT, bioprocess(PLANT_NUPTAKE+jj-1), NORMAL);
		Define(pGs, P_BioElementName[jj] + " Total PlantDemand", 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1" + conditions);
		pGs = new Gs(&DemandUptake[jj - 1].TotalDemandSoil, PLANT, elements(jj + CARBON), TRANSPORT,  bioprocess(PLANT_NUPTAKE+jj-1), NORMAL);
		Define(pGs, P_BioElementName[jj] + " Total PlantSoilDem", 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1" + conditions);


	}
	;
	pG = new G(&PCO2_Internal, pNumPlants, PLANT, CARBON, TRANSPORT, PHOTOSYNTHESIS, NORMAL);
	Define(pG, pNumPlants, "PCO2 Stomata", initv, "ppm", "Nitrogen and Carbon >= 1|Growth=4");
	pG = new G(&PCO2_Surface, pNumPlants, PLANT, CARBON, TRANSPORT, PHOTOSYNTHESIS, NORMAL);
	Define(pG, pNumPlants, "PCO2 Surface", initv, "ppm", "Nitrogen and Carbon >= 1|Growth=4");
	pGs = new Gs(&PCO2_Boundary, PLANT, CARBON, TRANSPORT, PHOTOSYNTHESIS, NORMAL);
	Define(pGs, "PCO2 Canopy", 0.0, "ppm", "Nitrogen and Carbon >= 1|Growth>=3");
	pG = new G(&PRubOut, pNumPlants, PLANT, CARBON, TRANSPORT, PHOTOSYNTHESIS, NORMAL);
	Define(pG, pNumPlants, "P Rubisco", initv, UNIT_TYPES::MIKRO_MOL_SEC_RATE, "Nitrogen and Carbon >= 1|Growth=4");
	pG = new G(&PLightOut, pNumPlants, PLANT, CARBON, TRANSPORT, PHOTOSYNTHESIS, NORMAL);
	Define(pG, pNumPlants, "P Light", initv, UNIT_TYPES::MIKRO_MOL_SEC_RATE, "Nitrogen and Carbon >= 1|Growth=4");
	pG = new G(&PSinkOut, pNumPlants, PLANT, CARBON, TRANSPORT, PHOTOSYNTHESIS, NORMAL);
	Define(pG, pNumPlants, "P Sink", initv, UNIT_TYPES::MIKRO_MOL_SEC_RATE, "Nitrogen and Carbon >= 1|Growth=4");
	;
	;


	;
	// Parameters;
	Func *pFunc;
	string funcname;
	FunctorProp functorprop;
	vector<Ps*> ps_vec;
	vector<P*> p_vec;
	ps_vec.clear();


	Tab *pTab;



	;
	pP = new P(&HarvestDayNo, pNumPlants, PLANT, ORGANIC, TRANSPORT, HARVEST, NORMAL); initv.assign(p_Plant->NumPlants, 280.);
	Define(pP, pNumPlants, "Harvest Dayno", initv, "#", "Nitrogen and Carbon >= 1|Harvest Day = 0", 0., 365.);
	mTab_Harvest.push_back(pP);
	pP = new P(&HarvFracGrain, pNumPlants, PLANT, ORGANIC, TRANSPORT, HARVEST, NORMAL); initv.assign(p_Plant->NumPlants, 1.);
	Define(pP, pNumPlants, "FHarvest Grain", initv, "-", "Nitrogen and Carbon >= 1", 0., 1.);
	mTab_Harvest.push_back(pP);
	pP = new P(&HarvFracLeaf, pNumPlants, PLANT, ORGANIC, TRANSPORT, HARVEST, NORMAL); initv.assign(p_Plant->NumPlants, 0.1);
	Define(pP, pNumPlants, "FHarvest Leaf", initv, "-", "Nitrogen and Carbon >= 1", 0., 1.);
	mTab_Harvest.push_back(pP);
	pP = new P(&LitterFracLeaf, pNumPlants, PLANT, ORGANIC, TRANSPORT, HARVEST, NORMAL); initv.assign(p_Plant->NumPlants, 0.1);
	Define(pP, pNumPlants, "FLitter Leaf", initv, "-", "Nitrogen and Carbon >= 1", 0., 1.);
	mTab_Harvest.push_back(pP);
	pP = new P(&HarvFracStem, pNumPlants, PLANT, ORGANIC, TRANSPORT, HARVEST, NORMAL); initv.assign(p_Plant->NumPlants, 0.1);
	Define(pP, pNumPlants, "FHarvest Stem", initv, "-", "Nitrogen and Carbon >= 1", 0., 1.);
	mTab_Harvest.push_back(pP);
	pP = new P(&LitterFracStem, pNumPlants, PLANT, ORGANIC, TRANSPORT, HARVEST, NORMAL); initv.assign(p_Plant->NumPlants, 0.1);
	Define(pP, pNumPlants, "FLitter Stem", initv, "-", "Nitrogen and Carbon >= 1", 0., 1.);
	mTab_Harvest.push_back(pP);
	pP = new P(&HarvFracRoots, pNumPlants, PLANT, ORGANIC, TRANSPORT, HARVEST, NORMAL); initv.assign(p_Plant->NumPlants, 0.);
	Define(pP, pNumPlants, "FHarvest Roots", initv, "-", "Nitrogen and Carbon >= 1", 0., 1.);
	mTab_Harvest.push_back(pP);
	pP = new P(&LitterFracRoots, pNumPlants, PLANT, ORGANIC, TRANSPORT, HARVEST, NORMAL); initv.assign(p_Plant->NumPlants, 0.);
	Define(pP, pNumPlants, "FLitter Roots", initv, "-", "Nitrogen and Carbon >= 1", 0., 1.);
	mTab_Harvest.push_back(pP);
	pP = new P(&LitterFracGrain, pNumPlants, PLANT, ORGANIC, TRANSPORT, HARVEST, NORMAL); initv.assign(p_Plant->NumPlants, 0.);
	Define(pP, pNumPlants, "FLitter Grain", initv, "-", "Nitrogen and Carbon >= 1", 0., 1.);
	mTab_Harvest.push_back(pP);
	pP = new P(&HarvFracCoarseRoots, pNumPlants, PLANT, ORGANIC, TRANSPORT, HARVEST, NORMAL); initv.assign(p_Plant->NumPlants, 0.);
	Define(pP, pNumPlants, "FHarvest CRoots", initv, "-", "Nitrogen and Carbon >= 1", 0., 1.);
	mTab_Harvest.push_back(pP);
	pP = new P(&LitterFracCoarseRoots, pNumPlants, PLANT, ORGANIC, TRANSPORT, HARVEST, NORMAL); initv.assign(p_Plant->NumPlants, 0.);
	Define(pP, pNumPlants, "FLitter CRoots", initv, "-", "Nitrogen and Carbon >= 1", 0., 1.);
	mTab_Harvest.push_back(pP);
	pP = new P(&HarvestLAIValue, pNumPlants, PLANT, ORGANIC, TRANSPORT, HARVEST, NORMAL); initv.assign(p_Plant->NumPlants, 7.);
	Define(pP, pNumPlants, "Harvest LAI", initv, "-", "Nitrogen and Carbon >= 1|Growth>0|Harvest Day>2", 1., 500.);
	mTab_Harvest.push_back(pP);

	pTab = new Tab(&mTab_Harvest, PLANT, ORGANIC, TRANSPORT, HARVEST, NORMAL);
	Define(pTab, "Harvest of Plants", "Harvest Dayno|FHarvest Grain|FHarvest Leaf|FLitter Leaf|FHarvest Stem|FLitter Stem|FHarvest Roots|FLitter Roots|FLitter Grain|FHarvest CRoots|FLitter CRoots|Harvest LAI", "Nitrogen and Carbon > 0");
	;
	pP = new P(&P_HarvFracGrain, pNumPlants, PLANT, ORGANIC, TRANSPORT, HARVEST, NORMAL); initv.assign(p_Plant->NumPlants, 1.);
	Define(pP, pNumPlants, "pFHarvest Grain", initv, "-", "Nitrogen and Carbon >= 1", 0., 1.);
	mTab_HarvestPrior.push_back(pP);
	pP = new P(&P_HarvFracLeaf, pNumPlants, PLANT, ORGANIC, TRANSPORT, HARVEST, NORMAL); initv.assign(p_Plant->NumPlants, 0.1);
	Define(pP, pNumPlants, "pFHarvest Leaf", initv, "-", "Nitrogen and Carbon >= 1", 0., 1.);
	mTab_HarvestPrior.push_back(pP);
	pP = new P(&P_LitterFracLeaf, pNumPlants, PLANT, ORGANIC, TRANSPORT, HARVEST, NORMAL); initv.assign(p_Plant->NumPlants, 0.1);
	Define(pP, pNumPlants, "pFLitter Leaf", initv, "-", "Nitrogen and Carbon >= 1", 0., 1.);
	mTab_HarvestPrior.push_back(pP);
	pP = new P(&P_HarvFracStem, pNumPlants, PLANT, ORGANIC, TRANSPORT, HARVEST, NORMAL); initv.assign(p_Plant->NumPlants, 0.1);
	Define(pP, pNumPlants, "pFHarvest Stem", initv, "-", "Nitrogen and Carbon >= 1", 0., 1.);
	mTab_HarvestPrior.push_back(pP);
	pP = new P(&P_LitterFracStem, pNumPlants, PLANT, ORGANIC, TRANSPORT, HARVEST, NORMAL); initv.assign(p_Plant->NumPlants, 0.1);
	Define(pP, pNumPlants, "pFLitter Stem", initv, "-", "Nitrogen and Carbon >= 1", 0., 1.);
	mTab_HarvestPrior.push_back(pP);
	pP = new P(&P_HarvFracRoots, pNumPlants, PLANT, ORGANIC, TRANSPORT, HARVEST, NORMAL); initv.assign(p_Plant->NumPlants, 0.);
	Define(pP, pNumPlants, "pFHarvest Roots", initv, "-", "Nitrogen and Carbon >= 1", 0., 1.);
	mTab_HarvestPrior.push_back(pP);
	pP = new P(&P_LitterFracRoots, pNumPlants, PLANT, ORGANIC, TRANSPORT, HARVEST, NORMAL); initv.assign(p_Plant->NumPlants, 0.);
	Define(pP, pNumPlants, "pFLitter Roots", initv, "-", "Nitrogen and Carbon >= 1", 0., 1.);
	mTab_HarvestPrior.push_back(pP);
	pP = new P(&P_LitterFracGrain, pNumPlants, PLANT, ORGANIC, TRANSPORT, HARVEST, NORMAL); initv.assign(p_Plant->NumPlants, 0.);
	Define(pP, pNumPlants, "pFLitter Grain", initv, "-", "Nitrogen and Carbon >= 1", 0., 1.);
	mTab_HarvestPrior.push_back(pP);
	pP = new P(&P_HarvFracCoarseRoots, pNumPlants, PLANT, ORGANIC, TRANSPORT, HARVEST, NORMAL); initv.assign(p_Plant->NumPlants, 0.);
	Define(pP, pNumPlants, "pFHarvest CRoots", initv, "-", "Nitrogen and Carbon >= 1", 0., 1.);
	mTab_HarvestPrior.push_back(pP);
	pP = new P(&P_LitterFracCoarseRoots, pNumPlants, PLANT, ORGANIC, TRANSPORT, HARVEST, NORMAL); initv.assign(p_Plant->NumPlants, 0.);
	Define(pP, pNumPlants, "pFLitter CRoots", initv, "-", "Nitrogen and Carbon >= 1", 0., 1.);
	mTab_HarvestPrior.push_back(pP);

	pTab = new Tab(&mTab_HarvestPrior, PLANT, ORGANIC, TRANSPORT, HARVEST, NORMAL);
	Define(pTab, "Harvest of Plants Prior final Harvest",
		"pFHarvest Grain|pFHarvest Leaf|pFLitter Leaf|pFHarvest Stem|pFLitter Stem|pFHarvest Roots|pFLitter Roots|pFLitter Grain|pFHarvest CRoots|pFLitter CRoots", "Nitrogen and Carbon > 0|Harvest Day=1");
	;

	;
	pP = new P(&InitPlantAge, pNumPlants, PLANT, TIME, TRANSPORT, INITB, NORMAL); initv.assign(p_Plant->NumPlants, 0.);
	Define(pP, pNumPlants, "I Plant Age", initv, "days", "Nitrogen and Carbon >= 1", 0., 50000.);
	mTab_PlantInit[0].push_back(pP);
	string name; double max_value[3]{ 200., 10., 1. }; double default_value; string condition;
	for (size_t jj = 0; jj < NUM_BIOELEM - 1; jj++) {
		condition = "";
		if (jj == _P_) condition = "|Phosphorus >0";
		for (size_t j = 1; j <= _OLD_COARSE_ROOT; j++) {
			//if (j == _MOBILE) continue;
			pP = new P(&P_State_IniValue[j][jj], pNumPlants, PLANT, elements(CARBON + jj), TRANSPORT, INITB, NORMAL);
			name = "I " + P_BioElementName[jj] +  P_StateCompName[j];
			double m_value = max_value[jj]; default_value = 30.; 
			if (j == _MOBILE && jj== _C_) default_value = 5.;
			if (j == _MOBILE && jj == _N_) default_value = .2;
			initv.assign(p_Plant->NumPlants, default_value);
			if (j == _OLD_STEM) m_value *= 20;
			Define(pP, pNumPlants, name, initv, UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1" + condition, 0., m_value);
			mTab_PlantInit[jj].push_back(pP);
		}
		pTab = new Tab(&mTab_PlantInit[jj], PLANT, elements(jj + CARBON), TRANSPORT, INITB, NORMAL);
		name = "Initial C conditions of plants";
		if (jj == _N_) name = "Initial conditions of plants";
		if (jj == _P_) name = "Initial P conditions of plants";
		Define(pTab, name, "", "Nitrogen and Carbon > 0" + condition);
		if (jj == 0) continue;
		double ratio_max[2] = { 100., 600. }; double ratio_def[2] = { 20., 100. };
		for (size_t j = 1; j <= _OLD_COARSE_ROOT; j++) {
			//if (j != _MOBILE) {
				condition = "|Initial Plant Conditions = 0";
				pP = new P(&P_State_IniCarbonRatio[j][jj - 1], pNumPlants, PLANT, elements(CARBON + jj), TRANSPORT, INITB, NORMAL);
				name = "I C" + P_BioElementName[jj] + P_StateCompName[j];
				double m_value = max_value[jj - 1]; double ratio = ratio_def[jj - 1];
				if (j == _OLD_STEM) {
					ratio *= 10.;
					m_value *= 10.;
				}
				initv.assign(p_Plant->NumPlants, ratio);
				Define(pP, pNumPlants, name, initv, UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1" + condition, 0., m_value);
				mTab_PlantInitRatio[jj - 1].push_back(pP);
			//}
		}
		pTab = new Tab(&mTab_PlantInitRatio[jj - 1], PLANT, ORGANIC, TRANSPORT, INITB, NORMAL);
		if (jj == 1) name = "Initial CN ratios of plants";
		else if (jj == 2) name = "Initial CP ratios of plants";
		Define(pTab, name, "", "Nitrogen and Carbon > 0" + condition);

	}

	/*
	;
		pP= new P(&P_State_IniCarbonRatio[_GRAIN][_N_-1]   	,  pNumPlants,  PLANT,ORGANIC, TRANSPORT,INITB, NORMAL);initv.assign(p_Plant->NumPlants, 10.);
			Define(pP, pNumPlants,  "I CN Grain"	,initv,"-","Nitrogen and Carbon >= 1",5.,50.);
			mTab_PlantInitCN.push_back(pP);
		pP= new P(&P_State_IniCarbonRatio[_LEAF][_N_-1],  pNumPlants,  PLANT,ORGANIC, TRANSPORT,INITB, NORMAL);initv.assign(p_Plant->NumPlants, 20.);
			Define(pP, pNumPlants,  "I CN Leaf"	,initv,"-", "Nitrogen and Carbon >= 1",5.,40.);
			mTab_PlantInitCN.push_back(pP);
		pP= new P(&P_State_IniCarbonRatio[_STEM][_N_-1] 		,  pNumPlants,  PLANT,ORGANIC, TRANSPORT,INITB, NORMAL);initv.assign(p_Plant->NumPlants, 50.);
			Define(pP, pNumPlants,  "I CN Stem"	,initv,"-", "Nitrogen and Carbon >= 1",10.,200.);
			mTab_PlantInitCN.push_back(pP);
		pP= new P(&P_State_IniCarbonRatio[_ROOT][_N_-1]		,  pNumPlants,  PLANT,ORGANIC, TRANSPORT,INITB, NORMAL);initv.assign(p_Plant->NumPlants, 20.);
			Define(pP, pNumPlants,  "I CN Roots"	,initv,"-", "Nitrogen and Carbon >= 1",5.,30.);
			mTab_PlantInitCN.push_back(pP);
		pP= new P(&P_State_IniCarbonRatio[_OLD_LEAF][_N_-1]  	,  pNumPlants,  PLANT,ORGANIC, TRANSPORT,INITB, NORMAL);initv.assign(p_Plant->NumPlants, 20.);
			Define(pP, pNumPlants,  "I CN OldLeaf",initv,"-", "Nitrogen and Carbon >= 1",5.,100.);
			mTab_PlantInitCN.push_back(pP);
		pP= new P(&P_State_IniCarbonRatio[_OLD_STEM][_N_-1] 	,  pNumPlants,  PLANT,ORGANIC, TRANSPORT,INITB, NORMAL);initv.assign(p_Plant->NumPlants, 50.);
			Define(pP, pNumPlants,  "I CN OldStem",initv,"-", "Nitrogen and Carbon >= 1",5.,200.);
			mTab_PlantInitCN.push_back(pP);
		pP= new P(&P_State_IniCarbonRatio[_OLD_ROOT][_N_-1]	,  pNumPlants,  PLANT,ORGANIC, TRANSPORT,INITB, NORMAL);initv.assign(p_Plant->NumPlants, 30.);
			Define(pP, pNumPlants,  "I CN OldRoots",initv,"-", "Nitrogen and Carbon >= 1",5.,30.);
			mTab_PlantInitCN.push_back(pP);
		pTab= new Tab(&mTab_PlantInitCN, PLANT,ORGANIC, TRANSPORT,INITB, NORMAL);
		Define(pTab,"Initial CN ratios of plants","i CN Grain|i CN Leaf|i CN Stem|i CN Roots|i CN OldLeaf|i CN OldStem|i CN OldRoots","Nitrogen and Carbon > 0|Initial Plant Conditions=0");




			pP = new P(&P_State_IniValue[_GRAIN][_P_], pNumPlants, PLANT, PHOSPHORUS, TRANSPORT, INITB, NORMAL); initv.assign(p_Plant->NumPlants, 0.);
			Define(pP, pNumPlants, "I P Grain", initv, UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Phosphorus>0", 0., 50.);
			mTab_PlantInitP.push_back(pP);
			pP = new P(&P_State_IniValue[_LEAF][_P_], pNumPlants, PLANT, PHOSPHORUS, TRANSPORT, INITB, NORMAL); initv.assign(p_Plant->NumPlants, 0.);
			Define(pP, pNumPlants, "I P Leaf", initv, UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Phosphorus>0", 0., 20.);
			mTab_PlantInitP.push_back(pP);
			pP = new P(&P_State_IniValue[_STEM][_P_], pNumPlants, PLANT, PHOSPHORUS, TRANSPORT, INITB, NORMAL); initv.assign(p_Plant->NumPlants, 0.);
			Define(pP, pNumPlants, "I P Stem", initv, UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Phosphorus>0", 0., 30.);
			mTab_PlantInitP.push_back(pP);
			pP = new P(&P_State_IniValue[_ROOT][_P_], pNumPlants, PLANT, PHOSPHORUS, TRANSPORT, INITB, NORMAL); initv.assign(p_Plant->NumPlants, 0.);
			Define(pP, pNumPlants, "I P Roots", initv, UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Phosphorus>0", 0., 30.);
			mTab_PlantInitP.push_back(pP);
			pP = new P(&P_State_IniValue[_OLD_LEAF][_P_], pNumPlants, PLANT, PHOSPHORUS, TRANSPORT, INITB, NORMAL); initv.assign(p_Plant->NumPlants, 0.);
			Define(pP, pNumPlants, "I P OldLeaf", initv, UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Perennial Plant >0|Phosphorus>0", 0., 100.);
			mTab_PlantInitP.push_back(pP);
			pP = new P(&P_State_IniValue[_OLD_STEM][_P_], pNumPlants, PLANT, PHOSPHORUS, TRANSPORT, INITB, NORMAL); initv.assign(p_Plant->NumPlants, 0.);
			Define(pP, pNumPlants, "I P OldStem", initv, UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Perennial Plant >0|Phosphorus>0", 0., 500.);
			mTab_PlantInitP.push_back(pP);
			pP = new P(&P_State_IniValue[_OLD_ROOT][_P_], pNumPlants, PLANT, PHOSPHORUS, TRANSPORT, INITB, NORMAL); initv.assign(p_Plant->NumPlants, 0.);
			Define(pP, pNumPlants, "I P OldRoots", initv, UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Perennial Plant >0|Phosphorus>0", 0., 100.);
			mTab_PlantInitP.push_back(pP);
			pP = new P(&P_State_IniValue[_COARSE_ROOT][_P_], pNumPlants, PLANT, PHOSPHORUS, TRANSPORT, INITB, NORMAL); initv.assign(p_Plant->NumPlants, 0.);
			Define(pP, pNumPlants, "I P CRoots", initv, UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Phosphorus>0", 0., 30.);
			mTab_PlantInitP.push_back(pP);
			pP = new P(&P_State_IniValue[_OLD_COARSE_ROOT][_P_], pNumPlants, PLANT, PHOSPHORUS, TRANSPORT, INITB, NORMAL); initv.assign(p_Plant->NumPlants, 0.);
			Define(pP, pNumPlants, "I P OldCRoots", initv, UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Perennial Plant >0|Phosphorus>0", 0., 100.);
			mTab_PlantInitP.push_back(pP);

		pTab = new Tab(&mTab_PlantInitP, PLANT, CARBON, TRANSPORT, INITB, NORMAL);

			Define(pTab, "Initial P conditions of plants", "i P Grain|i P Leaf|i P Stem|i P Roots|i P OldLeaf|i P OldStem|i P OldRoots|i P CRoots|i P OldCRoots", "Nitrogen and Carbon > 0|Phosphorus>0|Initial Plant P Conditions=0");
			;
			pP = new P(&P_State_IniCarbonRatio[_GRAIN][_P_-1], pNumPlants, PLANT, ORGANIC, TRANSPORT, INITB, NORMAL); initv.assign(p_Plant->NumPlants, 100.);
			Define(pP, pNumPlants, "I CP Grain", initv, "-", "Nitrogen and Carbon >= 1|Phosphorus>0", 5., 500.);
			mTab_PlantInitCP.push_back(pP);
			pP = new P(&P_State_IniCarbonRatio[_LEAF][_P_-1], pNumPlants, PLANT, ORGANIC, TRANSPORT, INITB, NORMAL); initv.assign(p_Plant->NumPlants, 200.);
			Define(pP, pNumPlants, "I CP Leaf", initv, "-", "Nitrogen and Carbon >= 1|Phosphorus>0", 5., 1000.);
			mTab_PlantInitCP.push_back(pP);
			pP = new P(&P_State_IniCarbonRatio[_STEM][_P_-1], pNumPlants, PLANT, ORGANIC, TRANSPORT, INITB, NORMAL); initv.assign(p_Plant->NumPlants, 300.);
			Define(pP, pNumPlants, "I CP Stem", initv, "-", "Nitrogen and Carbon >= 1|Phosphorus>0", 10., 2000.);
			mTab_PlantInitCP.push_back(pP);
			pP = new P(&P_State_IniCarbonRatio[_ROOT][_P_-1], pNumPlants, PLANT, ORGANIC, TRANSPORT, INITB, NORMAL); initv.assign(p_Plant->NumPlants, 200.);
			Define(pP, pNumPlants, "I CP Roots", initv, "-", "Nitrogen and Carbon >= 1|Phosphorus>0", 5., 1000.);
			mTab_PlantInitCP.push_back(pP);
			pP = new P(&P_State_IniCarbonRatio[_OLD_LEAF][_P_-1], pNumPlants, PLANT, ORGANIC, TRANSPORT, INITB, NORMAL); initv.assign(p_Plant->NumPlants, 200.);
			Define(pP, pNumPlants, "I CP OldLeaf", initv, "-", "Nitrogen and Carbon >= 1|Perennial Plant >0|Phosphorus>0", 5., 1000.);
			mTab_PlantInitCP.push_back(pP);
			pP = new P(&P_State_IniCarbonRatio[_OLD_STEM][_P_-1], pNumPlants, PLANT, ORGANIC, TRANSPORT, INITB, NORMAL); initv.assign(p_Plant->NumPlants, 500.);
			Define(pP, pNumPlants, "I CP OldStem", initv, "-", "Nitrogen and Carbon >= 1|Perennial Plant >0|Phosphorus>0", 5., 2000.);
			mTab_PlantInitCP.push_back(pP);
			pP = new P(&P_State_IniCarbonRatio[_OLD_ROOT][_P_-1], pNumPlants, PLANT, ORGANIC, TRANSPORT, INITB, NORMAL); initv.assign(p_Plant->NumPlants, 200.);
			Define(pP, pNumPlants, "I CP OldRoots", initv, "-", "Nitrogen and Carbon >= 1|Perennial Plant >0|Phosphorus>0", 5., 1000.);
			mTab_PlantInitCP.push_back(pP);
			pTab = new Tab(&mTab_PlantInitCP, PLANT, ORGANIC, TRANSPORT, INITB, NORMAL);
			Define(pTab, "Initial CP ratios of plants", "i CP Grain|i CP Leaf|i CP Stem|i CP Roots|i CP OldLeaf|i CP OldStem|i CP OldRoots", "Nitrogen and Carbon > 0|Phosphorus>0|Initial Plant P Conditions>0");

			*/

	pP = new P(&CN_RatioMinRoots, pNumPlants, PLANT, ORGANIC, TRANSPORT, PLANT_NUPTAKE, NORMAL); initv.assign(p_Plant->NumPlants, 25.);
	Define(pP, pNumPlants, "CN Ratio Min Roots", initv, "-", "Nitrogen and Carbon >= 1", 5., 60.);
	mTab_PlantCN.push_back(pP);
	pP = new P(&CN_RatioMinStem, pNumPlants, PLANT, ORGANIC, TRANSPORT, PLANT_NUPTAKE, NORMAL); initv.assign(p_Plant->NumPlants, 25.);
	Define(pP, pNumPlants, "CN Ratio Min Stem", initv, "-", "Nitrogen and Carbon >= 1", 10., 800.);
	mTab_PlantCN.push_back(pP);
	pP = new P(&CN_RatioMinLeaf, pNumPlants, PLANT, ORGANIC, TRANSPORT, PLANT_NUPTAKE, NORMAL); initv.assign(p_Plant->NumPlants, 25.);
	Define(pP, pNumPlants, "CN Ratio Min Leaf", initv, "-", "Nitrogen and Carbon >= 1", 5., 40.);
	mTab_PlantCN.push_back(pP);
	pP = new P(&CN_RatioMinCoarseRoots, pNumPlants, PLANT, ORGANIC, TRANSPORT, PLANT_NUPTAKE, NORMAL); initv.assign(p_Plant->NumPlants, 25.);
	Define(pP, pNumPlants, "CN Ratio Min CRoots", initv, "-", "Nitrogen and Carbon >= 1", 5., 600.);
	mTab_PlantCN.push_back(pP);
	//	Define(pP, pNumPlants,  CN_RatioMinFungi   	,"CN Ratio Min Fungi"	,16.	,p_Plant->NumPlants,"-","Nitrogen and Carbon >= 1|Mycorrhiza Fungal Growth>0",15.,25.);
	pTab = new Tab(&mTab_PlantCN, PLANT, ORGANIC, TRANSPORT, PLANT_NUPTAKE, NORMAL);
	Define(pTab, "Minimum CN Ratios of plants", "CN Ratio Min Roots|CN Ratio Min Stem|CN Ratio Min Leaf|CN Ratio Min CRoots", "Nitrogen and Carbon > 0");


	pP = new P(&CP_RatioMinRoots, pNumPlants, PLANT, ORGANIC, TRANSPORT, PLANT_PUPTAKE, NORMAL); initv.assign(p_Plant->NumPlants, 800.);
	Define(pP, pNumPlants, "CP Ratio Min Roots", initv, "-", "Nitrogen and Carbon >= 1|Phosphorus>0", 5., 2000.);
	mTab_PlantCP.push_back(pP);
	pP = new P(&CP_RatioMinStem, pNumPlants, PLANT, ORGANIC, TRANSPORT, PLANT_PUPTAKE, NORMAL); initv.assign(p_Plant->NumPlants, 1000.);
	Define(pP, pNumPlants, "CP Ratio Min Stem", initv, "-", "Nitrogen and Carbon >= 1|Phosphorus>0", 10., 4000.);
	mTab_PlantCP.push_back(pP);
	pP = new P(&CP_RatioMinLeaf, pNumPlants, PLANT, ORGANIC, TRANSPORT, PLANT_PUPTAKE, NORMAL); initv.assign(p_Plant->NumPlants, 400.);
	Define(pP, pNumPlants, "CP Ratio Min Leaf", initv, "-", "Nitrogen and Carbon >= 1|Phosphorus>0", 5., 1000.);
	mTab_PlantCP.push_back(pP);
	pP = new P(&CP_RatioMinCoarseRoots, pNumPlants, PLANT, ORGANIC, TRANSPORT, PLANT_PUPTAKE, NORMAL); initv.assign(p_Plant->NumPlants, 1000.);//HH
	Define(pP, pNumPlants, "CP Ratio Min CRoots", initv, "-", "Nitrogen and Carbon >= 1|Phosphorus>0", 5., 3000.);
	mTab_PlantCP.push_back(pP);
	pTab = new Tab(&mTab_PlantCP, PLANT, ORGANIC, TRANSPORT, PLANT_PUPTAKE, NORMAL);
	Define(pTab, "Minimum CP Ratios of plants", "CP Ratio Min Roots|CP Ratio Min Stem|CP Ratio Min Leaf|CP Ratio Min CRoots", "Nitrogen and Carbon > 0|Phosphorus>0");

	;
	pP = new P(&CN_RatioMaxLeaf, pNumPlants, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL); initv.assign(p_Plant->NumPlants, 100.);
	Define(pP, pNumPlants, "CN Ratio Max Litter", initv, "-", "Nitrogen and Carbon >= 1", 50., 200.);
	mTab_CNLitter.push_back(pP);
	pTab = new Tab(&mTab_CNLitter, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL);
	Define(pTab, "Maximum CN Ratios of Leaf Litter", "CN Ratio Max Litter", "Nitrogen and Carbon > 0");

	pP = new P(&CP_RatioMaxLeaf, pNumPlants, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL); initv.assign(p_Plant->NumPlants, 1000.);//HH
	Define(pP, pNumPlants, "CP Ratio Max Litter", initv, "-", "Nitrogen and Carbon >= 1|Phosphorus>0", 5., 4000.);
	mTab_CPLitter.push_back(pP);
	pTab = new Tab(&mTab_CPLitter, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL);
	Define(pTab, "Maximum CP Ratios of Leaf Litter", "CP Ratio Max Litter", "Nitrogen and Carbon > 0|Phosphorus>0");
	;


	;

	;
	;
	;
	;
	;
	pP = new P(&Allo_C_LeafStem, pNumPlants, PLANT, ORGANIC, TRANSPORT, REALLOCATION, NORMAL); initv.assign(p_Plant->NumPlants, 0.);
	Define(pP, pNumPlants, "C Leaf to Stem", initv, "-", "Nitrogen and Carbon >= 1", 0., 1.);
	mTab_AlloStem.push_back(pP);
	pTab = new Tab(&mTab_AlloStem, PLANT, ORGANIC, TRANSPORT, REALLOCATION, NORMAL);
	Define(pTab, "Allocation to Stem from Leaf", "C Leaf to Stem", "Nitrogen and Carbon > 0|Growth > 0|Grain Development > 0");
	;
	pP = new P(&AlloFromRootLeafCoef, pNumPlants, PLANT, ORGANIC, TRANSPORT, REALLOCATION, NORMAL); initv.assign(p_Plant->NumPlants, 0.02);
	Define(pP, pNumPlants, "RateCoef_fRoot", initv, "/day", "Nitrogen and Carbon >= 1", 0., .1);
	mTab_AlloLeaf.push_back(pP);
	pP = new P(&AlloFromRootLeafRatio, pNumPlants, PLANT, ORGANIC, TRANSPORT, REALLOCATION, NORMAL); initv.assign(p_Plant->NumPlants, 1.);
	Define(pP, pNumPlants, "Root Leaf Ratio", initv, "/day", "Nitrogen and Carbon >= 1", 0.5, 25.);
	mTab_AlloLeaf.push_back(pP);

	pTab = new Tab(&mTab_AlloLeaf, PLANT, ORGANIC, TRANSPORT, REALLOCATION, NORMAL);
	Define(pTab, "Allocation From Roots to Leaf", "RateCoef_fRoot|Root Leaf Ratio", "Nitrogen and Carbon > 0|Growth > 0|Grain Development > 0");
	;
	;
	pP = new P(&AlloCoarseRoots, pNumPlants, PLANT, ORGANIC, TRANSPORT, REALLOCATION, NORMAL); initv.assign(p_Plant->NumPlants, 0.);
	Define(pP, pNumPlants, "Fraction R", initv, "-", "Nitrogen and Carbon >= 1", 0., 1.);
	mTab_AlloCoarseRoots.push_back(pP);

	pTab = new Tab(&mTab_AlloCoarseRoots, PLANT, ORGANIC, TRANSPORT, REALLOCATION, NORMAL);
	Define(pTab, "Coarse Roots versus remaining C", "Fraction R", "Nitrogen and Carbon > 0|Growth > 0");
	;
	;
	pP = new P(&Allo_C_LeafGrain, pNumPlants, PLANT, CARBON, TRANSPORT, REALLOCATION, NORMAL); initv.assign(p_Plant->NumPlants, 0.01);
	Define(pP, pNumPlants, "C Leaf to Grain", initv, "-", "Nitrogen and Carbon >= 1|Grain Development > 0", 0., 1.);
	mTab_AlloGrain.push_back(pP);
	pP = new P(&Allo_C_StemGrain, pNumPlants, PLANT, CARBON, TRANSPORT, REALLOCATION, NORMAL); initv.assign(p_Plant->NumPlants, 0.02);
	Define(pP, pNumPlants, "C Stem to Grain", initv, "-", "Nitrogen and Carbon >= 1|Grain Development > 0", 0., 1.);
	mTab_AlloGrain.push_back(pP);
	pP = new P(&Allo_C_RootsGrain, pNumPlants, PLANT, CARBON, TRANSPORT, REALLOCATION, NORMAL); initv.assign(p_Plant->NumPlants, 0.01);
	Define(pP, pNumPlants, "C Roots to Grain", initv, "-", "Nitrogen and Carbon >= 1|Grain Development > 0", 0., 1.);
	mTab_AlloGrain.push_back(pP);
	;
	pP = new P(&Allo_N_LeafGrain, pNumPlants, PLANT, NITROGEN, TRANSPORT, REALLOCATION, NORMAL); initv.assign(p_Plant->NumPlants, 0.01);
	Define(pP, pNumPlants, "N Leaf to Grain", initv, "-", "Nitrogen and Carbon >= 1|Grain Development > 0", 0., 1.);
	mTab_AlloGrain.push_back(pP);
	pP = new P(&Allo_N_StemGrain, pNumPlants, PLANT, NITROGEN, TRANSPORT, REALLOCATION, NORMAL); initv.assign(p_Plant->NumPlants, 0.02);
	Define(pP, pNumPlants, "N Stem to Grain", initv, "-", "Nitrogen and Carbon >= 1|Grain Development > 0", 0., 1.);
	mTab_AlloGrain.push_back(pP);
	pP = new P(&Allo_N_RootsGrain, pNumPlants, PLANT, NITROGEN, TRANSPORT, REALLOCATION, NORMAL); initv.assign(p_Plant->NumPlants, 0.01);
	Define(pP, pNumPlants, "N Roots to Grain", initv, "-", "Nitrogen and Carbon >= 1|Grain Development > 0", 0., 1.);
	mTab_AlloGrain.push_back(pP);

	pP = new P(&Allo_P_LeafGrain, pNumPlants, PLANT, PHOSPHORUS, TRANSPORT, REALLOCATION, NORMAL); initv.assign(p_Plant->NumPlants, 0.01);
	Define(pP, pNumPlants, "P Leaf to Grain", initv, "-", "Nitrogen and Carbon >= 1|Grain Development > 0|Phosphorus>0", 0., 1.);
	mTab_AlloGrain.push_back(pP);
	pP = new P(&Allo_P_StemGrain, pNumPlants, PLANT, PHOSPHORUS, TRANSPORT, REALLOCATION, NORMAL); initv.assign(p_Plant->NumPlants, 0.02);
	Define(pP, pNumPlants, "P Stem to Grain", initv, "-", "Nitrogen and Carbon >= 1|Grain Development > 0|Phosphorus>0", 0., 1.);
	mTab_AlloGrain.push_back(pP);
	pP = new P(&Allo_P_RootsGrain, pNumPlants, PLANT, PHOSPHORUS, TRANSPORT, REALLOCATION, NORMAL); initv.assign(p_Plant->NumPlants, 0.01);
	Define(pP, pNumPlants, "P Roots to Grain", initv, "-", "Nitrogen and Carbon >= 1|Grain Development > 0|Phosphorus>0", 0., 1.);
	mTab_AlloGrain.push_back(pP);
	pP = new P(&Allo_GrainReductionRange, pNumPlants, PLANT, PHOSPHORUS, TRANSPORT, REALLOCATION, NORMAL); initv.assign(p_Plant->NumPlants, 0.4);
	Define(pP, pNumPlants, "Allocation Grain Range", initv, "-", "Nitrogen and Carbon >= 1|Grain Development > 0|Grain Fill Allocation>0", 0., 1.);
	mTab_AlloGrain.push_back(pP);

	;
	pTab = new Tab(&mTab_AlloGrain, PLANT, ORGANIC, TRANSPORT, REALLOCATION, NORMAL);
	Define(pTab, "Allocation to Grain",1);
	;


	pP = new P(&Mulch, pNumPlants, PLANT, ORGANIC, SOILEVAPORATION, PLANT_MANAGEMENT, NORMAL); initv.assign(p_Plant->NumPlants, 0.);
	Define(pP, pNumPlants, "Mulch", initv, "-", "Nitrogen and Carbon >= 1", 0.5, 20.);
	mTab_PlantMan.push_back(pP);
	pP = new P(&Drip, pNumPlants, PLANT, ORGANIC, IRRIGATION, PLANT_MANAGEMENT, NORMAL); initv.assign(p_Plant->NumPlants, 0.);
	Define(pP, pNumPlants, "Drip", initv, "-", "WaterEq = 1|Evaporation Method > 0", 0.5, 20.);
	mTab_PlantMan.push_back(pP);
	pTab = new Tab(&mTab_PlantMan, PLANT, NITROGEN, TRANSPORT, PLANT_MANAGEMENT, NORMAL);
	Define(pTab, "Plant management", "Mulch|Drip", "WaterEq = 1|Evaporation Method > 0");
	;
	pF = new F(&m_PG_PlantBiomass, PLANT, WATER, FYS_SHAPE, LIVING, NORMAL);
	Define(pF, "Plant Biomass data", "NDrivePhoto.bin", "PhotoInput > 0");
	//CALL F(Group_24, FileDriv12a,"Plant biomass data","BIN",true,"NDrivPhoto","PhotoInput > 0");

	NE *pNum_CleaningDays, *pNum_CleaningYears;
	pNum_CleaningDays = new NE(&NUM_CleaningDays, 3, 1, 100, 1, 40);
	Define(pNum_CleaningDays, "Cleaning Days");
	pNum_CleaningYears = new NE(&NUM_CleaningYears, 3, 1, 100, 1, 40);
	Define(pNum_CleaningYears, "Cleaning Years");
	//call PnE(,3,1,100,1,100)				//Element is Table;
	//call PnE(NUM_CleaningYears,3,1,100,1,100)				//Element is Table;
	pP = new P(&CleaningYears, pNum_CleaningYears, PLANT, CARBON, TRANSPORT, HARVEST, NORMAL); initv.assign(NUM_CleaningYears, 40.);
	Define(pP, pNum_CleaningYears, "Cleaning Years", initv, "YearNo", "Nitrogen and Carbon >= 1|Growth>0|Cleaning operation=2", 1., 500.);
	mTab_CleanD.push_back(pP);
	pTab = new Tab(&mTab_CleanD, PLANT, ORGANIC, TRANSPORT, HARVEST, NORMAL);
	Define(pTab, "Cleaning During Years", "Cleaning Years", "Nitrogen and Carbon > 0|Growth>0|Cleaning operation=2");
	;
	pP = new P(&CleaningDays, pNum_CleaningDays, PLANT, TIME, TRANSPORT, HARVEST, NORMAL); initv.assign(NUM_CleaningDays, 300.);
	Define(pP, pNum_CleaningDays, "Cleaning DayNo", initv, "DayNo", "Nitrogen and Carbon >= 1|Growth>0|Cleaning operation>0", 1., 500.);
	mTab_CleanY.push_back(pP);
	pP = new P(&CleaningPlantIndex, pNum_CleaningDays, PLANT, TIME, TRANSPORT, HARVEST, NORMAL); ; initv.assign(NUM_CleaningDays, 1.);
	Define(pP, pNum_CleaningDays, "Plant Cleaning Index", initv, "-", "Nitrogen and Carbon >= 1|Growth>0|Harvest Day=1|Cleaning operation>0", 1., 10.);
	mTab_CleanY.push_back(pP);
	pP = new P(&CleaningDegree, pNum_CleaningDays, PLANT, TIME, TRANSPORT, HARVEST, NORMAL);	initv.assign(NUM_CleaningDays, 0.5);
	Define(pP, pNum_CleaningDays, "Degree of Cleaning", initv, "-", "Nitrogen and Carbon >= 1|Growth>0|Harvest Day=1|Cleaning operation>0", 0., 1.);
	mTab_CleanY.push_back(pP);
	pTab = new Tab(&mTab_CleanY, PLANT, ORGANIC, TRANSPORT, HARVEST, NORMAL);
	Define(pTab, "Cleaning Within Year", "Cleaning DayNo|Plant Cleaning index|Degree of Cleaning", "Nitrogen and Carbon > 0|Growth>0|Cleaning operation>0");

	pF = new F(&m_PG_HarvestDate, PLANT, WATER, FYS_SHAPE, HARVEST, NORMAL);
	Define(pF, "Harvest date data", "HarvestDates.bin", "Harvest Day =4");
	//	CALL F(Group_24, FileDriv13a,"Harvest date data","BIN",true,"HarvestDates","Harvest Day = 4");
	;
	NE *pNUM_HarvestDays, *pNUM_HarvestYears;
	pNUM_HarvestDays = new NE(&NUM_HarvestDays, 3, 1, 100, 1, 100);				//Element is Table;
	pNUM_HarvestYears = new NE(&NUM_HarvestYears, 3, 1, 100, 1, 100);				//Element is Table;
	;
	Define(pNUM_HarvestDays, "Num of Harvest Days");
	Define(pNUM_HarvestYears, "Num of Harvest Years");
	;
	pP = new P(&HarvestYears, pNUM_HarvestYears, PLANT, TIME, TRANSPORT, HARVEST, NORMAL); initv.assign(NUM_HarvestYears, 40.);
	Define(pP, pNUM_HarvestYears, "Harvest Years", initv, "YearNo", "Nitrogen and Carbon >= 1|Growth>0|Harvest Day=1", 1., 500.); initv.assign(p_Plant->NumPlants, 0.);
	mTab_HarvD.push_back(pP);
	pP = new P(&HarvestDays, pNUM_HarvestDays, PLANT, TIME, TRANSPORT, HARVEST, NORMAL); initv.assign(NUM_HarvestDays, 300.);
	Define(pP, pNUM_HarvestDays, "Harvest DayNo", initv, "DayNo", "Nitrogen and Carbon >= 1|Growth>0|Harvest Day=1", 1., 500.); initv.assign(p_Plant->NumPlants, 0.);
	mTab_HarvY.push_back(pP);
	pP = new P(&HarvestYearPlantIndex, pNUM_HarvestYears, PLANT, TIME, TRANSPORT, HARVEST, NORMAL); initv.assign(NUM_HarvestYears, 1.);
	Define(pP, pNUM_HarvestYears, "Plant Year Index", initv, "-", "Nitrogen and Carbon >= 1|Growth>0|Harvest Day=1|Harvest Range>0", 1., 500.);
	mTab_HarvD.push_back(pP);
	pP = new P(&HarvestDayPlantIndex, pNUM_HarvestDays, PLANT, TIME, TRANSPORT, HARVEST, NORMAL); initv.assign(NUM_HarvestDays, 1.);
	Define(pP, pNUM_HarvestDays, "Plant Day Index", initv, "-", "Nitrogen and Carbon >= 1|Growth>0|Harvest Day=1|Harvest Range>0", 1., 500.);
	mTab_HarvY.push_back(pP);
	pP = new P(&Harvest_Type_Years, pNUM_HarvestYears, PLANT, TIME, TRANSPORT, HARVEST, NORMAL); initv.assign(NUM_HarvestYears, 0.);
	Define(pP, pNUM_HarvestYears, "Final Type Year", initv, "-", "Nitrogen and Carbon >= 1|Growth>0|Harvest Day=1", 0., 1.);
	mTab_HarvD.push_back(pP);
	pP = new P(&Harvest_Type_Days, pNUM_HarvestDays, PLANT, TIME, TRANSPORT, HARVEST, NORMAL); initv.assign(NUM_HarvestDays, 0.);
	Define(pP, pNUM_HarvestDays, "Final Type Day", initv, "-", "Nitrogen and Carbon >= 1|Growth>0|Harvest Day=1", 0., 1.);
	mTab_HarvY.push_back(pP);

	pTab = new Tab(&mTab_HarvD, PLANT, TIME, TRANSPORT, HARVEST, NORMAL);
	Define(pTab, "Harvest During Years", "Harvest Years|Plant Year index|Final Type Year", "Nitrogen and Carbon > 0|Growth>0|Harvest Day=1");

	pTab = new Tab(&mTab_HarvY, PLANT, TIME, TRANSPORT, HARVEST, NORMAL);
	Define(pTab, "Harvest Within Year", "Harvest DayNo|Plant Day index|Final Type Day", "Nitrogen and Carbon > 0|Growth>0|Harvest Day=1");


	NE *pNUM_StartYears;
	pNUM_StartYears = new NE(&NUM_DynamicYearStart, 1, 1, 100, 1, 100);

	Define(pNUM_StartYears, "Number of Dynamic StartYears");
	pP = new P(&m_StartYears, pNUM_StartYears, PLANT, TIME, TRANSPORT, INITB, NORMAL); initv.assign(NUM_DynamicYearStart, 2010.);
	Define(pP, pNUM_StartYears, "Start Years", initv, "Year", "Nitrogen and Carbon >= 1|Growth>0|StartSeasonTableExist=1", 1., 2500.);
	mTab_StartYear.push_back(pP);
	pP = new P(&m_SowingDays, pNUM_StartYears, PLANT, TIME, TRANSPORT, INITB, NORMAL); initv.assign(NUM_DynamicYearStart, 120.);
	Define(pP, pNUM_StartYears, "Sowing Days", initv, "DayNo", "Nitrogen and Carbon >= 1|Growth>0|Sowing=3", 1., 500.);
	mTab_StartYear.push_back(pP);
	pP = new P(&m_EmergenceDays, pNUM_StartYears, PLANT, TIME, TRANSPORT, EMERGENCE, NORMAL); initv.assign(NUM_DynamicYearStart, 140.);
	Define(pP, pNUM_StartYears, "Emergence Days", initv, "DayNo", "Nitrogen and Carbon >= 1|Growth>0|Emergence=3", 1., 500.);
	mTab_StartYear.push_back(pP);
	pP = new P(&m_HarvestDays, pNUM_StartYears, PLANT, TIME, TRANSPORT, EMERGENCE, NORMAL); initv.assign(NUM_DynamicYearStart, 140.);
	Define(pP, pNUM_StartYears, "Harvest Days", initv, "DayNo", "Nitrogen and Carbon >= 1|Growth>0|Harvest Day=5", 1., 500.);
	mTab_StartYear.push_back(pP);
	pTab = new Tab(&mTab_StartYear, PLANT, TIME, TRANSPORT, HARVEST, NORMAL);
	Define(pTab, "Start and end of Crops for Different Years", "Start Years|Sowing Days|Emergence Days", "Nitrogen and Carbon > 0|Growth>0");
	Def_GrowthStages();
	Def_PhotoSynthesis();
	Def_Allocation();
	Def_LitterFall();
	Def_Respiration();
	Def_Fungi();
	Def_Grazing();
	Def_WoodEating();
	Def_NFixation();
	return FunctionDef();


}


bool NC_Plant_Functions::Def_GrowthStages()
{
	Func *pFunc;
	SoilModel* pSoil = (SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
	p_Soil_WaterF = &pSoil->m_SoilFrost;
	PlantModel* pPlant = (PlantModel*)m_pModelInfo->GetModelPtr("Plant Model");
	WaterUptake* p_Plant = &pPlant->m_WaterUptake;
	NE *pNL = m_pModelMap->GetNEPointer("Number of Layers");
	NE *pNumPlants = m_pModelMap->GetNEPointer("Number of Plants");

	// Parameters;
	string funcname;
	FunctorProp functorprop;
	vector<Ps*> ps_vec;
	vector<P*> p_vec;
	ps_vec.clear();
	PLOT_PARAM plotpar;
	plotpar.Num_Of_Curves = 1;
	plotpar.Num_Of_Points = 300;
	plotpar.Num_Of_X = 1;
	plotpar.X_Min = 0;
	plotpar.X_Max = 100;
	plotpar.X_Label = "Water content (vol %)";
	plotpar.Y_Label = "Adjust of TempSum  (-)";
	plotpar.LogXScale = false;
	plotpar.LogYScale = false;
	plotpar.Num_Of_Opt = 1;



	vector<double> initv;	initv.assign(p_Plant->NumPlants, 0.);
	Ps *pPs; P *pP;
	Tab *pTab;

	functorprop.FuncName = MyFunc::MOISTURE_RESPONSE_EMERGENCE; functorprop.Arg = MyArgType::D1;
	functorprop.FunctorInitOption = string::npos;
	functorprop.pModelInfo = m_pModelInfo;

	funcname = "Uppermost layer Moisture Control";


	pPs = new Ps(&MoistureRangeEmergence, PLANT, TIME, TRANSPORT, EMERGENCE, NORMAL);
	Define(pPs, "MoistureRangeEmergence", 10., "-", "Nitrogen and Carbon >= 1|Growth>0", 0.1, 100.,1.,50.);
	ps_vec.push_back(pPs);

	pFunc = new Func(PLANT, TIME, NOFPROC, PHENOLOGY, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar);


	functorprop.FuncName = MyFunc::ZADOK_GSI; functorprop.Arg = MyArgType::D1;
	ps_vec.clear();
	plotpar.X_Label = "Growth Stage Index (-)";
	plotpar.Y_Label = "Zadok Value  (-)";
	plotpar.X_Max = 4.;
	funcname = "Zadok Phenology Function";

	pPs = new Ps(&ZadokRangeGSI2, PLANT, TIME, TRANSPORT, PHENOLOGY, NORMAL);
	Define(pPs, "ZadokRange-GSI1to2", 50., "-", "Nitrogen and Carbon >= 1|Growth>0|ZadokScale>0", 10., 70.);
	ps_vec.push_back(pPs);

	pPs = new Ps(&GSI_DeltaZadok30, PLANT, TIME, TRANSPORT, PHENOLOGY, NORMAL);
	Define(pPs, "GSI_DeltaZadok30", 0.4, "-", "Nitrogen and Carbon >= 1|Growth>0|ZadokScale>0", .1, .7);
	ps_vec.push_back(pPs);
	
	pPs = new Ps(&GSI_DeltaZadok55, PLANT, TIME, TRANSPORT, PHENOLOGY, NORMAL);
	Define(pPs, "GSI_DeltaZadok55", 0.95, "-", "Nitrogen and Carbon >= 1|Growth>0|ZadokScale>0", 0.8, 1.);
	ps_vec.push_back(pPs);



	pFunc = new Func(PLANT, TIME, NOFPROC, PHENOLOGY, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar);





	functorprop.FuncName = MyFunc::TEMP_SUM_SOWING; functorprop.Arg = MyArgType::T_D1;
	functorprop.FunctorInitOption = string::npos;
	functorprop.pModelInfo = m_pModelInfo;
	ps_vec.clear();

	funcname = "Logistic Growth Response Function";

	pP = new P(&SowingDayNo, pNumPlants, PLANT, TIME, TRANSPORT, EMERGENCE, NORMAL); initv.assign(p_Plant->NumPlants, 120.);
	Define(pP, pNumPlants, "Sowing Dayno", initv, "#", "Nitrogen and Carbon >= 1|Sowing >0", 0., 365.);
	mTab_Start.push_back(pP);
	pP = new P(&EmergenceDayNo, pNumPlants, PLANT, TIME, TRANSPORT, EMERGENCE, NORMAL); initv.assign(p_Plant->NumPlants, 135.);
	Define(pP, pNumPlants, "Emergence Dayno", initv, "#", "Nitrogen and Carbon >= 1|Sowing >0", 0., 365.);
	mTab_Start.push_back(pP);
	pP = new P(&C_Seed, pNumPlants, PLANT, CARBON, TRANSPORT, EMERGENCE, NORMAL); initv.assign(p_Plant->NumPlants, 1.);
	Define(pP, pNumPlants, "C Seed", initv, UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Sowing>0", 0.01, 1000.);
	mTab_Start.push_back(pP);

	pTab = new Tab(&mTab_Start, PLANT, CARBON, TRANSPORT, EMERGENCE, NORMAL);
	Define(pTab, "Start of growth", "Sowing Dayno|Emergence Dayno|C Seed", "Nitrogen and Carbon > 0");

	mTab_Seedlings.clear();
	pP = new P(&Seedling_Year, pNumPlants, PLANT, TIME, TRANSPORT, EMERGENCE, NORMAL); initv.assign(p_Plant->NumPlants, 2012.);
	Define(pP, pNumPlants, "Seedling Year", initv, "#", "Nitrogen and Carbon >= 1|Sowing=0|Seedling Management=2", 0., 365.);
	mTab_Seedlings.push_back(pP);

	pP = new P(&Seedling_YearAfterFinalHarvest, pNumPlants, PLANT, TIME, TRANSPORT, EMERGENCE, NORMAL); initv.assign(p_Plant->NumPlants, 2.);
	Define(pP, pNumPlants, "Seedling Year after harvest", initv, "#", "Nitrogen and Carbon >= 1|Sowing=0|Seedling Management=1", 0., 5.);
	mTab_Seedlings.push_back(pP);

	pP = new P(&Seedling_DayNo, pNumPlants, PLANT, TIME, TRANSPORT, EMERGENCE, NORMAL); initv.assign(p_Plant->NumPlants, 210.);
	Define(pP, pNumPlants, "Seedling DayNo", initv, "#", "Nitrogen and Carbon >= 1|Sowing=0|Seedling Management>0", 0., 366.);
	mTab_Seedlings.push_back(pP);


	pP = new P(&Seedling_CLeaf, pNumPlants, PLANT, TIME, TRANSPORT, EMERGENCE, NORMAL); initv.assign(p_Plant->NumPlants, 1.);
	Define(pP, pNumPlants, "Seedling CLeaf", initv, "#", "Nitrogen and Carbon >= 1|Sowing=0|Seedling Management>0", 0., 5.);
	mTab_Seedlings.push_back(pP);

	pP = new P(&Seedling_CStem, pNumPlants, PLANT, TIME, TRANSPORT, EMERGENCE, NORMAL); initv.assign(p_Plant->NumPlants, 1.);
	Define(pP, pNumPlants, "Seedling CStem", initv, "#", "Nitrogen and Carbon >= 1|Sowing=0|Seedling Management>0", 0., 5.);
	mTab_Seedlings.push_back(pP);

	pP = new P(&Seedling_CRoot, pNumPlants, PLANT, TIME, TRANSPORT, EMERGENCE, NORMAL); initv.assign(p_Plant->NumPlants, 1.);
	Define(pP, pNumPlants, "Seedling CRoot", initv, "#", "Nitrogen and Carbon >= 1|Sowing=0|Seedling Management>0", 0., 5.);
	mTab_Seedlings.push_back(pP);

	pP = new P(&Seedling_Age, pNumPlants, PLANT, TIME, TRANSPORT, EMERGENCE, NORMAL); initv.assign(p_Plant->NumPlants, 2.);
	Define(pP, pNumPlants, "Seedling Age", initv, "#", "Nitrogen and Carbon >= 1|Sowing=0|Seedling Management>0", 0., 5.);
	mTab_Seedlings.push_back(pP);


	pTab = new Tab(&mTab_Seedlings, PLANT, CARBON, TRANSPORT, EMERGENCE, NORMAL);
	Define(pTab, "Seedling Management", "Seedling Year|Seedling Year after harvest|Seedling CLeaf|Seedling CStem|Seedling CRoot|Seedling Age", "Nitrogen and Carbon > 0|Seedling Management > 0");


	;
	pP = new P(&Min_GSI, pNumPlants, PLANT, CARBON, TRANSPORT, PHENOLOGY, NORMAL); initv.assign(p_Plant->NumPlants, 0.);
	Define(pP, pNumPlants, "Min GSI", initv, "#", "Nitrogen and Carbon >= 1|Growth > 1", -1., 2.);
	mTab_GSI.push_back(pP);
	pP = new P(&Max_GSI, pNumPlants, PLANT, CARBON, TRANSPORT, PHENOLOGY, NORMAL); initv.assign(p_Plant->NumPlants, 4.);
	Define(pP, pNumPlants, "Max GSI", initv, "#", "Nitrogen and Carbon >= 1|Growth > 1", 2., 4.);
	mTab_GSI.push_back(pP);

	pP = new P(&Initial_GSI, pNumPlants, PLANT, CARBON, TRANSPORT, PHENOLOGY, NORMAL); initv.assign(p_Plant->NumPlants, 0.);
	Define(pP, pNumPlants, "Initial GSI", initv, "#", "Nitrogen and Carbon >= 1|Growth > 1", -1., 4.);
	mTab_GSI.push_back(pP);
	pP = new P(&Max_Leaf_Lifetime, pNumPlants, PLANT, CARBON, TRANSPORT, PHENOLOGY, NORMAL); initv.assign(p_Plant->NumPlants, 1.);
	Define(pP, pNumPlants, "Max Leaf Lifetime", initv, "yr", "Nitrogen and Carbon >= 1|Growth > 1", 1., 20.);
	mTab_GSI.push_back(pP);
	pP = new P(&Max_Plant_LifeTime, pNumPlants, PLANT, CARBON, TRANSPORT, PHENOLOGY, NORMAL); initv.assign(p_Plant->NumPlants, 2.);
	Define(pP, pNumPlants, "Max Plant Lifetime", initv, "yr", "Nitrogen and Carbon >= 1|Growth > 1", 1., 200.);
	mTab_GSI.push_back(pP);
	pP = new P(&ShootCoef, pNumPlants, PLANT, CARBON, TRANSPORT, PHENOLOGY, NORMAL); initv.assign(p_Plant->NumPlants, .2);
	Define(pP, pNumPlants, "Shoot Coef", initv, "-", "Nitrogen and Carbon >= 1|Growth > 1", 0., 0.5);
	mTab_GSI.push_back(pP);
	pP = new P(&MobileAlloCoef, pNumPlants, PLANT, CARBON, TRANSPORT, PHENOLOGY, NORMAL); initv.assign(p_Plant->NumPlants, .5);//!Mobile coef for N and P can be different HH
	Define(pP, pNumPlants, "Mobile Allo Coef", initv, "-", "Nitrogen and Carbon >= 1|Growth > 1", 0., 2.);
	mTab_GSI.push_back(pP);
	
	pP = new P(&GSI_PostHarvest, pNumPlants, PLANT, CARBON, TRANSPORT, PHENOLOGY, NORMAL); initv.assign(p_Plant->NumPlants, 1.);
	Define(pP, pNumPlants, "GSI Post Harvest", initv, "#", "Nitrogen and Carbon >= 1|Growth > 1|Reset GSI at Harvest>0", 0., 4.);
	mTab_GSI.push_back(pP);
	pTab = new Tab(&mTab_GSI, PLANT, CARBON, TRANSPORT, PHENOLOGY, NORMAL);
	Define(pTab, "Plant Behaviour", "Min GSI|Max GSI|Initial GSI|Max Leaf Lifetime|Max Plant Lifetime|Shoot Coef|Mobile Allo Coef|GSI Post Harvest", "Nitrogen and Carbon > 0|Growth > 1");//HH
	;





	//REMAIN INCLUDE two pointer to Salt response
	plotpar.X_MaxPar = "Sow Tsum";
	functorprop.FuncName = MyFunc::TEMP_SUM_SOWING; functorprop.Arg = MyArgType::T_D1;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Sowing GSI function of adjusted TempSum";
	plotpar.X_Label = "TempSum (Cday)";
	plotpar.Y_Label = "Relative stage prior Sowing (-)";
	p_vec.clear();

	pP = new P(&T_Thres_Sowing, pNumPlants, PLANT, TIME, TRANSPORT, PHENOLOGY, NORMAL); initv.assign(p_Plant->NumPlants, 3.);
	Define(pP, pNumPlants, "Sow Tth", initv, UNIT_TYPES::TEMP_UNIT, "Nitrogen and Carbon >= 1|Growth >= 2", 0.1, 15.);
	mTab_GSI2.push_back(pP); p_vec.push_back(pP);
	pP = new P(&T_Sum_Sowing, pNumPlants, PLANT, TIME, TRANSPORT, PHENOLOGY, NORMAL); initv.assign(p_Plant->NumPlants, 30.);
	Define(pP, pNumPlants, "Sow Tsum", initv, UNIT_TYPES::TEMPSUM_UNIT, "Nitrogen and Carbon >= 1|Growth >= 2", 0.1, 500.);
	mTab_GSI2.push_back(pP); p_vec.push_back(pP);
	
	pFunc = new Func(PLANT, TIME, NOFPROC, PHENOLOGY, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec, plotpar);

	plotpar.X_MaxPar = "Emerge Tsum";
	functorprop.FuncName = MyFunc::TEMP_SUM_EMERGENCE; functorprop.Arg = MyArgType::T_D1;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Emergence GSI function of adjusted TempSum";
	plotpar.X_Label = "Adjusted TempSum (Cday)";
	plotpar.Y_Label = "Relative stage of Emergence (-)";
	p_vec.clear();


	
	pP = new P(&T_Thres_Emerg, pNumPlants, PLANT, TIME, TRANSPORT, PHENOLOGY, NORMAL); initv.assign(p_Plant->NumPlants, 5.);
	Define(pP, pNumPlants, "Emerge Tth", initv, UNIT_TYPES::TEMP_UNIT, "Nitrogen and Carbon >= 1|Growth >= 2", 0.1, 15.);
	mTab_GSI2.push_back(pP); p_vec.push_back(pP);
	pP = new P(&T_Sum_Emerg, pNumPlants, PLANT, TIME, TRANSPORT, PHENOLOGY, NORMAL); initv.assign(p_Plant->NumPlants, 40.);
	Define(pP, pNumPlants, "Emerge Tsum", initv, UNIT_TYPES::TEMPSUM_UNIT, "Nitrogen and Carbon >= 1|Growth >= 2", 0.1, 500.);
	mTab_GSI2.push_back(pP); p_vec.push_back(pP);

	pP = new P(&T_Sum_Initial, pNumPlants, PLANT, TIME, TRANSPORT, PHENOLOGY, NORMAL); initv.assign(p_Plant->NumPlants, 0.);
	Define(pP, pNumPlants, "Initial Tsum", initv, UNIT_TYPES::TEMPSUM_UNIT, "Nitrogen and Carbon >= 1|Growth >= 2", 0., 1500.);
	mTab_GSI2.push_back(pP); 


	pFunc = new Func(PLANT, TIME, NOFPROC, PHENOLOGY, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec, plotpar);

	plotpar.X_MaxPar = "";
	plotpar.X_Max = 1440.;
	functorprop.FuncName = MyFunc::PRIOR_GRAIN_DAYLENGTH; functorprop.Arg = MyArgType::T_D1;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Grain Start as function of Daylength";
	plotpar.Y_Label = "Relative Response (-)";
	plotpar.X_Label = "DayLength (minutes)";
	p_vec.clear();





	pP = new P(&GrainSI_Step, pNumPlants, PLANT, TIME, TRANSPORT, PHENOLOGY, NORMAL); initv.assign(p_Plant->NumPlants, 0.06);
	Define(pP, pNumPlants, "Grain Step", initv, "-", "Nitrogen and Carbon >= 1|Growth >= 2|Grain Development >0", 0.01, 5.);
	mTab_GSI2.push_back(pP); p_vec.push_back(pP);

	pP = new P(&GrainSI_StepDay, pNumPlants, PLANT, TIME, TRANSPORT, PHENOLOGY, NORMAL); initv.assign(p_Plant->NumPlants, 0.5);
	Define(pP, pNumPlants, "Grain StepDay", initv, "/hr", "Nitrogen and Carbon >= 1|Growth >= 2|Grain Development >0", 0.01, 10.);
	mTab_GSI2.push_back(pP); p_vec.push_back(pP);
	pP = new P(&GrainSI_ThresDay, pNumPlants, PLANT, TIME, TRANSPORT, PHENOLOGY, NORMAL); initv.assign(p_Plant->NumPlants, 5.);
	Define(pP, pNumPlants, "Grain ThDay", initv, "hr", "Nitrogen and Carbon >= 1|Growth >= 2|Grain Development >0", 0.01, 10.);
	mTab_GSI2.push_back(pP); p_vec.push_back(pP);
	
	pP = new P(&GrainSI_StepTemp, pNumPlants, PLANT, TIME, TRANSPORT, PHENOLOGY, NORMAL); initv.assign(p_Plant->NumPlants, 0.2);
	Define(pP, pNumPlants, "Grain StepTemp", initv, UNIT_TYPES::INVERSE_TEMP_UNIT, "Nitrogen and Carbon >= 1|Growth >= 2|Grain Development >0", 0.01, 10.);
	mTab_GSI2.push_back(pP); p_vec.push_back(pP);
	pP = new P(&GrainSI_ThresTemp, pNumPlants, PLANT, TIME, TRANSPORT, PHENOLOGY, NORMAL); initv.assign(p_Plant->NumPlants, 10.);
	Define(pP, pNumPlants, "Grain ThTemp", initv, UNIT_TYPES::TEMP_UNIT, "Nitrogen and Carbon >= 1|Growth >= 2|Grain Development >0", 0.1, 10.);
	mTab_GSI2.push_back(pP); p_vec.push_back(pP);

	pFunc = new Func(PLANT, TIME, NOFPROC, PHENOLOGY, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec, plotpar);
	functorprop.FuncName = MyFunc::PRIOR_GRAIN_TEMP; functorprop.Arg = MyArgType::T_D2;
	funcname = "Grain Start as function of Temperature";
	plotpar.Y_Label = "Relative Response (-)";
	plotpar.X_Label = "Temperature (C)";
	plotpar.X_Max = 30.;
	plotpar.X2_Vec = { 1.,1. };
	pFunc = new Func(PLANT, TIME, NOFPROC, PHENOLOGY, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec, plotpar);
	p_vec.clear();

	functorprop.FuncName = MyFunc::TEMP_SUM_GRAINFILLING; functorprop.Arg = MyArgType::T_D1;
	funcname = "Grain Filling function of Temperature";
	plotpar.Y_Label = "Relative Response (-)";
	plotpar.X_Label = "Temperature Sum (Cday)";
	plotpar.X_MaxPar = "Mature Tsum";

	plotpar.X2_Vec = { };
	pP = new P(&T_Thres_GrainFill, pNumPlants, PLANT, TIME, TRANSPORT, PHENOLOGY, NORMAL); initv.assign(p_Plant->NumPlants, 5.);
	Define(pP, pNumPlants, "Mature Tth", initv, UNIT_TYPES::TEMP_UNIT, "Nitrogen and Carbon >= 1|Growth >= 2", 0.1, 15.);
	mTab_GSI2.push_back(pP); p_vec.push_back(pP);
	pP = new P(&T_Sum_GrainFill, pNumPlants, PLANT, TIME, TRANSPORT, PHENOLOGY, NORMAL); initv.assign(p_Plant->NumPlants, 450.);
	Define(pP, pNumPlants, "Mature Tsum", initv, UNIT_TYPES::TEMPSUM_UNIT, "Nitrogen and Carbon >= 1|Growth >= 2", 0.1, 5000.);
	mTab_GSI2.push_back(pP); p_vec.push_back(pP);

	pFunc = new Func(PLANT, TIME, NOFPROC, PHENOLOGY, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec, plotpar);

	functorprop.FuncName = MyFunc::TEMP_SUM_RIPENING; functorprop.Arg = MyArgType::T_D1;
	funcname = "From grain filled to dormant seed";
	plotpar.Y_Label = "Relative Response (-)";
	plotpar.X_Label = "Temperature Sum (Cday)";
	plotpar.X_MaxPar = "Ripening Tsum";

	plotpar.X2_Vec = { };
	pP = new P(&T_Thres_Ripening, pNumPlants, PLANT, TIME, TRANSPORT, PHENOLOGY, NORMAL); initv.assign(p_Plant->NumPlants, 5.);
	Define(pP, pNumPlants, "Ripening Tth", initv, UNIT_TYPES::TEMP_UNIT, "Nitrogen and Carbon >= 1|Growth >= 2", 0.1, 15.);
	mTab_GSI2.push_back(pP); p_vec.push_back(pP);
	pP = new P(&T_Sum_Ripening, pNumPlants, PLANT, TIME, TRANSPORT, PHENOLOGY, NORMAL); initv.assign(p_Plant->NumPlants, 450.);
	Define(pP, pNumPlants, "Ripening Tsum", initv, UNIT_TYPES::TEMPSUM_UNIT, "Nitrogen and Carbon >= 1|Growth >= 2", 0.1, 5000.);
	mTab_GSI2.push_back(pP); p_vec.push_back(pP);

	pFunc = new Func(PLANT, TIME, NOFPROC, PHENOLOGY, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec, plotpar);





	pP = new P(&T_Thres_Dormancy, pNumPlants, PLANT, TIME, TRANSPORT, PHENOLOGY, NORMAL); initv.assign(p_Plant->NumPlants, 5.);
	Define(pP, pNumPlants, "Dormancy Tth", initv, UNIT_TYPES::TEMP_UNIT, "Nitrogen and Carbon >= 1|Growth >= 2", 0.1, 15.);
	mTab_GSI2.push_back(pP);
	pTab = new Tab(&mTab_GSI2, PLANT, TIME, TRANSPORT, PHENOLOGY, NORMAL);
	Define(pTab, "Growth stage", "Sow Tsum|Sow Tth|Emerge Tsum|Emerge Tth|Grain Step|Grain StepDay|Grain ThDay|Grain StepTemp|Grain ThTemp|Mature Tsum|Mature Tth|Ripe Tsum|Ripe Tth|Dormancy Tth", "Nitrogen and Carbon > 0|Growth > 1");
	;




	return true;
}
bool NC_Plant_Functions::Def_PhotoSynthesis()
{
	Func *pFunc;
	SoilModel* pSoil = (SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
	p_Soil_WaterF = &pSoil->m_SoilFrost;
	PlantModel* pPlant = (PlantModel*)m_pModelInfo->GetModelPtr("Plant Model");
	WaterUptake* p_Plant = &pPlant->m_WaterUptake;
	NE *pNL = m_pModelMap->GetNEPointer("Number of Layers");
	NE *pNumPlants = m_pModelMap->GetNEPointer("Number of Plants");

	// Parameters;
	string funcname;
	FunctorProp functorprop;
	vector<Ps*> ps_vec;
	vector<P*> p_vec;
	ps_vec.clear();
	vector<double> initv;	initv.assign(p_Plant->NumPlants, 0.);
	Ps *pPs; P *pP;
	Tab *pTab;

	PLOT_PARAM plotpar;
	plotpar.Num_Of_Curves = 1;
	plotpar.Num_Of_Points = 300;
	plotpar.Num_Of_X = 2;
	plotpar.X_Min = 0;
	plotpar.X_Max = 1440;
	plotpar.X_Label = "Day Number";
	plotpar.Y_Label = "Potential Transpiration (mm/day)";
	plotpar.LogXScale = false;
	plotpar.LogYScale = false;
	plotpar.Num_Of_Opt = 1;

	functorprop.FuncName = MyFunc::LOGISTIC_GROWTH; functorprop.Arg = MyArgType::T_D1;
	functorprop.FunctorInitOption = string::npos;
	functorprop.pModelInfo=m_pModelInfo;

	funcname = "Logistic Growth Response Function";

	p_vec.clear();
	initv.assign(p_Plant->NumPlants, 120.);
	pP = new P(&UptakeStart, pNumPlants, PLANT, ORGANIC, TRANSPORT, PLANT_NUPTAKE, NORMAL); p_vec.push_back(pP);
	Define(pP, pNumPlants, "Up Start", initv, "#", "Nitrogen and Carbon >= 1|Growth =1", 0., 366.);
	mTab_Nup.push_back(pP);
	pP = new P(&UptakeEnd, pNumPlants, PLANT, ORGANIC, TRANSPORT, PLANT_NUPTAKE, NORMAL); p_vec.push_back(pP); initv.assign(p_Plant->NumPlants, 240.);
	Define(pP, pNumPlants, "Up End", initv, "#", "Nitrogen and Carbon >= 1|Growth =1");
	mTab_Nup.push_back(pP);
	pP = new P(&UptakeACoef, pNumPlants, PLANT, ORGANIC, TRANSPORT, PLANT_NUPTAKE, NORMAL); p_vec.push_back(pP); initv.assign(p_Plant->NumPlants, 20.);
	Define(pP, pNumPlants, "UpA Coef", initv, UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Growth =1");
	mTab_Nup.push_back(pP);
	pP = new P(&UptakeBCoef, pNumPlants, PLANT, ORGANIC, TRANSPORT, PLANT_NUPTAKE, NORMAL);  p_vec.push_back(pP); initv.assign(p_Plant->NumPlants, 1.);
	Define(pP, pNumPlants, "UpB Coef", initv, "-", "Nitrogen and Carbon >= 1|Growth =1");
	mTab_Nup.push_back(pP);
	pP = new P(&UptakeCCoef, pNumPlants, PLANT, ORGANIC, TRANSPORT, PLANT_NUPTAKE, NORMAL);  p_vec.push_back(pP); initv.assign(p_Plant->NumPlants, .12);
	Define(pP, pNumPlants, "UpC Coef", initv, "/day", "Nitrogen and Carbon >= 1|Growth =1");
	mTab_Nup.push_back(pP);
	pP = new P(&UptakeCNRatio, pNumPlants, PLANT, ORGANIC, TRANSPORT, PLANT_NUPTAKE, NORMAL); p_vec.push_back(pP); initv.assign(p_Plant->NumPlants, 25);
	Define(pP, pNumPlants, "Up CN Ratio", initv, "-", "Nitrogen and Carbon >= 1|Growth =1");
	mTab_Nup.push_back(pP);

	pTab = new Tab(&mTab_Nup, PLANT, ORGANIC, TRANSPORT, PLANT_NUPTAKE, NORMAL);
	Define(pTab, "N and C Uptake (plants)", "Up Start|Up End|UpA Coef|UpB Coef|UpC Coef|Up CN Ratio", "Nitrogen and Carbon > 0|Growth = 1");
	pFunc = new Func(PLANT, CARBON, NOFPROC, GROWTH, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec);



	functorprop.FuncName = MyFunc::PHOTO_NLEAF; functorprop.Arg = MyArgType::T_D1;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Photosynthesis Nitrogen Leaf Response Function";
	p_vec.clear();

	plotpar.Num_Of_Points = 300;
	plotpar.Num_Of_X = 1;
	plotpar.X_Min = 0;
	plotpar.X_Max = 100;
	plotpar.X_Label = "C/N ratio Leaf";
	plotpar.Y_Label = "Response (-)";
	plotpar.Num_Of_Opt = 1;



	pP = new P(&CNLeafOpt, pNumPlants, PLANT, CARBON, TRANSPORT, PHOTOSYNTHESIS, NORMAL); initv.assign(p_Plant->NumPlants, 25.);
	Define(pP, pNumPlants, "CN LOpt", initv, "-", "Nitrogen and Carbon >= 1|Growth >= 3", 10., 40.);
	mTab_Photo.push_back(pP);
	p_vec.push_back(pP);
	pP = new P(&CNLeafThres, pNumPlants, PLANT, CARBON, TRANSPORT, PHOTOSYNTHESIS, NORMAL); initv.assign(p_Plant->NumPlants, 80.);
	Define(pP, pNumPlants, "CN LTh", initv, "-", "Nitrogen and Carbon >= 1|Growth >= 3", 10., 300.);
	mTab_Photo.push_back(pP);
	p_vec.push_back(pP);

	pFunc = new Func(PLANT, CARBON, NOFPROC, GROWTH, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec, plotpar);



	functorprop.FuncName = MyFunc::PHOTO_PLEAF; functorprop.Arg = MyArgType::T_D1;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Photosynthesis Phosphorus Leaf Response Function";
	p_vec.clear();
	plotpar.X_Min = 0;
	plotpar.X_Max = 200;
	plotpar.X_Label = "C/P ratio Leaf";
	plotpar.Y_Label = "Response (-)";

	pP = new P(&CPLeafOpt, pNumPlants, PLANT, CARBON, TRANSPORT, PHOTOSYNTHESIS, NORMAL); initv.assign(p_Plant->NumPlants, 200.);//HH
	Define(pP, pNumPlants, "CP LOpt", initv, "-", "Nitrogen and Carbon >= 1|Phosphorus>0|Growth >= 3", 10., 400.);
	mTab_Photo.push_back(pP);
	p_vec.push_back(pP);
	pP = new P(&CPLeafThres, pNumPlants, PLANT, CARBON, TRANSPORT, PHOTOSYNTHESIS, NORMAL); initv.assign(p_Plant->NumPlants, 500.);
	Define(pP, pNumPlants, "CP LTh", initv, "-", "Nitrogen and Carbon >= 1|Phosphorus>0|Growth >= 3", 10., 1000.);
	mTab_Photo.push_back(pP);
	p_vec.push_back(pP);

	pFunc = new Func(PLANT, CARBON, NOFPROC, PHOTOSYNTHESIS, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec, plotpar);

	functorprop.FuncName = MyFunc::PHOTO_DIRECT_TEMP; functorprop.Arg = MyArgType::T_D1;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Photosynthesis Air Temp Response Function";
	p_vec.clear();

	plotpar.X_Min = -5;
	plotpar.X_Max = 40;
	plotpar.X_Label = "Leaf Temperature";
	plotpar.Y_Label = "Response (-)";


	pP = new P(&TempResMin, pNumPlants, PLANT, CARBON, TRANSPORT, PHOTOSYNTHESIS, NORMAL); initv.assign(p_Plant->NumPlants, 5.);
	p_vec.push_back(pP);
	Define(pP, pNumPlants, "T LMin", initv, UNIT_TYPES::TEMP_UNIT, "Nitrogen and Carbon >= 1|Growth >= 3", -10., 10.);
	mTab_Photo.push_back(pP);
	pP = new P(&TempResOpt1, pNumPlants, PLANT, CARBON, TRANSPORT, PHOTOSYNTHESIS, NORMAL); initv.assign(p_Plant->NumPlants, 15.);
	p_vec.push_back(pP);
	Define(pP, pNumPlants, "T LOpt1", initv, UNIT_TYPES::TEMP_UNIT, "Nitrogen and Carbon >= 1|Growth >= 3", 3., 30.);
	mTab_Photo.push_back(pP);
	pP = new P(&TempResOpt2, pNumPlants, PLANT, CARBON, TRANSPORT, PHOTOSYNTHESIS, NORMAL); initv.assign(p_Plant->NumPlants, 25.);
	p_vec.push_back(pP);
	Define(pP, pNumPlants, "T LOpt2", initv, UNIT_TYPES::TEMP_UNIT, "Nitrogen and Carbon >= 1|Growth >= 3", 5., 50.);
	mTab_Photo.push_back(pP);
	pP = new P(&TempResMax, pNumPlants, PLANT, CARBON, TRANSPORT, PHOTOSYNTHESIS, NORMAL); initv.assign(p_Plant->NumPlants, 35.);
	p_vec.push_back(pP);
	Define(pP, pNumPlants, "T LMax", initv, UNIT_TYPES::TEMP_UNIT, "Nitrogen and Carbon >= 1|Growth >= 3", 20., 60.);
	mTab_Photo.push_back(pP);

	pFunc = new Func(PLANT, CARBON, NOFPROC, PHOTOSYNTHESIS, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec, plotpar);

	pP = m_pModelMap->GetPPointer("Salt p");
	mTab_Photo.push_back(pP);
	pP = m_pModelMap->GetPPointer("Salt c50");
	mTab_Photo.push_back(pP);

	functorprop.FuncName = MyFunc::PHOTO_COMBINED_N_WATER_RESPONSE; functorprop.Arg = MyArgType::T_D2;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Photosynthesis Combined Respone of Nitrogen and Water";
	p_vec.clear();
	pP = new P(&RelWeightWater, pNumPlants, PLANT, CARBON, TRANSPORT, PHOTOSYNTHESIS, NORMAL); initv.assign(p_Plant->NumPlants, 0.35); p_vec.push_back(pP);
	Define(pP, pNumPlants, "RelW Water", initv, "-", "Nitrogen and Carbon >= 1|Growth >= 3|Plant Stress>0", 0., 1.);
	mTab_Photo.push_back(pP);
	pP = new P(&MinimumP_Response, pNumPlants, PLANT, CARBON, TRANSPORT, PHOTOSYNTHESIS, NORMAL); initv.assign(p_Plant->NumPlants, 0.); p_vec.push_back(pP);
	Define(pP, pNumPlants, "Minimum Response", initv, "-", "Nitrogen and Carbon >= 1|Growth >= 3|Plant Stress>0", 0., 1.);
	mTab_Photo.push_back(pP);
	
	pFunc = new Func(PLANT, CARBON, NOFPROC, PHOTOSYNTHESIS, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec);


	pP = new P(&TempResponseCurrent, pNumPlants, PLANT, CARBON, TRANSPORT, PHOTOSYNTHESIS, NORMAL); initv.assign(p_Plant->NumPlants, 1.);
	Define(pP, pNumPlants, "T RCurrent", initv, "-", "Nitrogen and Carbon >= 1|Growth >= 3|Green Index Forcing=0", 0.01, 1.);
	mTab_Photo.push_back(pP);
	;
	functorprop.FuncName = MyFunc::PHOTO_TEMPSUM; functorprop.Arg = MyArgType::T_D1;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Photosynthesis Air Temp Sum Response Function";
	p_vec.clear();
	plotpar.X_Min = 0;
	plotpar.X_Max = 500;
	plotpar.X_Label = "Air Temperature Sum ";  plotpar.X_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::TEMPSUM_UNIT, true);
	plotpar.Y_Label = "Response (-)";


	pP = new P(&TempSumResponseStart, pNumPlants, PLANT, CARBON, TRANSPORT, PHOTOSYNTHESIS, NORMAL); initv.assign(p_Plant->NumPlants, 1.);
	Define(pP, pNumPlants, "TF Sum Start", initv, "-", "Nitrogen and Carbon >= 1|Growth >= 3|Green Index Forcing=0", 0.01, 1.);
	mTab_Photo.push_back(pP);
	p_vec.push_back(pP);
	pP = new P(&TempSumResponseOpt, pNumPlants, PLANT, CARBON, TRANSPORT, PHOTOSYNTHESIS, NORMAL); initv.assign(p_Plant->NumPlants, 200.);
	Define(pP, pNumPlants, "T Sum Opt", initv, UNIT_TYPES::TEMPSUM_UNIT, "Nitrogen and Carbon >= 1|Growth >= 3|Green Index Forcing=0", 0., 500.);
	mTab_Photo.push_back(pP);
	p_vec.push_back(pP);
	
	pFunc = new Func(PLANT, CARBON, NOFPROC, PHOTOSYNTHESIS, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec, plotpar);


	pP = new P(&MinFracGreenResponse, pNumPlants, PLANT, CARBON, TRANSPORT, PHOTOSYNTHESIS, NORMAL); initv.assign(p_Plant->NumPlants, .3);
	Define(pP, pNumPlants, "Min Frac GreenResponse", initv, "-", "Nitrogen and Carbon >= 1|Growth >= 3|Green Index Forcing=1", 0., 1.);
	mTab_Photo.push_back(pP);
	pP = new P(&OptGreenIndex, pNumPlants, PLANT, CARBON, TRANSPORT, PHOTOSYNTHESIS, NORMAL); initv.assign(p_Plant->NumPlants, .1);
	Define(pP, pNumPlants, "Opt Green Index", initv, "-", "Nitrogen and Carbon >= 1|Growth >= 3|Green Index Forcing=1", 0., .2);
	mTab_Photo.push_back(pP);

	pTab = new Tab(&mTab_Photo, PLANT, PLANT_STRESS, TRANSPORT, PHOTOSYNTHESIS, NORMAL);
	Define(pTab, "Photosynthesis Response", "CN LOpt|CN LTh|T LMin|T LOpt1|T LOpt2|T LMax|Salt p|Salt c50|RelW Water|Minimum Response|T RCurrent|TF Sum Start|T Sum Opt", "Nitrogen and Carbon > 0|Growth > 1");


	pP = new P(&PhoWaterEfficiency, pNumPlants, PLANT, CARBON, TRANSPORT, PHOTOSYNTHESIS, NORMAL); initv.assign(p_Plant->NumPlants, 3.);
	Define(pP, pNumPlants, "WaterEfficiency", initv, "gDw/mm", "Nitrogen and Carbon >= 1|Growth = 2", 0., 20., 1., 5.);
	mTab_PhotoW.push_back(pP);
	pTab = new Tab(&mTab_PhotoW, PLANT, PLANT_STRESS, TRANSPORT, PHOTOSYNTHESIS, NORMAL);
	Define(pTab, "Water use efficiency", "WaterEfficiency", "Nitrogen and Carbon > 0|Growth = 2");


	functorprop.FuncName = MyFunc::PHOTO_EFF_LIGHT; functorprop.Arg = MyArgType::T_D1;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Photosynthesis Light efficiency";
	p_vec.clear();
	plotpar.X_Min = 0;
	plotpar.X_Max = 1.E7;
	plotpar.X_ScalingCoef = -6;
	plotpar.Y_Label = "Photosynthesis Rate ";   plotpar.Y_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::CARBON_MASSFLOW_UNIT);
	plotpar.X_Label = "Adsorbed Global Radiation "; plotpar.X_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::MEGA_HEATFLOW_UNIT);




	pP = new P(&PhoRadEfficiency, pNumPlants, PLANT, PLANT_STRESS, TRANSPORT, PHOTOSYNTHESIS, NORMAL); initv.assign(p_Plant->NumPlants, 2.);
	Define(pP, pNumPlants, "RadEfficiency", initv, "gDw/MJ", "Nitrogen and Carbon >= 1|Growth = 3", 0., 10.);
	mTab_PhotoRad.push_back(pP);
	p_vec.push_back(pP);

	pFunc = new Func(PLANT, CARBON, NOFPROC, PHOTOSYNTHESIS, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec, plotpar);

	functorprop.FuncName = MyFunc::PHOTO_GRAINFILL; functorprop.Arg = MyArgType::T_D1;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Photosynthesis Scaling during Grainfilling";
	p_vec.clear();
	plotpar.X_Min = 0;
	plotpar.X_ScalingCoef = -0;
	plotpar.X_Max = 1.;
	plotpar.X_Label = "Grain Filling Degree (-)";
	plotpar.Y_Label = "Reduction Factor (-)";


	pP = new P(&PhoRadEff_Reduc, pNumPlants, PLANT, PLANT_STRESS, TRANSPORT, PHOTOSYNTHESIS, NORMAL); initv.assign(p_Plant->NumPlants, 50.);
	Define(pP, pNumPlants, "RadEffRed", initv, "%", "Nitrogen and Carbon >= 1|Growth >= 3", -100., 100.);
	mTab_PhotoRad.push_back(pP);

	p_vec.push_back(pP);

	pFunc = new Func(PLANT, CARBON, NOFPROC, PHOTOSYNTHESIS, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec, plotpar);




	pP = new P(&PhoMax, pNumPlants, PLANT, PLANT_STRESS, TRANSPORT, PHOTOSYNTHESIS, NORMAL); initv.assign(p_Plant->NumPlants, 40.);
	Define(pP, pNumPlants, "Pmax", initv, "gC/m2/day", "Nitrogen and Carbon >= 1|Growth = 3|PhoSaturation >0", 0., 100.);
	mTab_PhotoRad.push_back(pP);
	functorprop.FuncName = MyFunc::PHOTO_SATURATION; functorprop.Arg = MyArgType::T_D1;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Photosynthesis Reduction during Light Saturation";
	p_vec.clear();

	p_vec.push_back(pP);
	pFunc = new Func(PLANT, CARBON, NOFPROC, PHOTOSYNTHESIS, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec);



	pP = new P(&N_MinDegreeSupply, pNumPlants, PLANT, PLANT_STRESS, TRANSPORT, PHOTOSYNTHESIS, NORMAL); initv.assign(p_Plant->NumPlants, 0.6);
	Define(pP, pNumPlants, "FixN supply", initv, "-", "Nitrogen and Carbon >= 1|Growth >= 3|N Fixed Supply > 0", .01, 1.);
	mTab_PhotoRad.push_back(pP);
	pTab = new Tab(&mTab_PhotoRad, PLANT, PLANT_STRESS, TRANSPORT, PHOTOSYNTHESIS, NORMAL);
	Define(pTab, "Radiation use efficiency", "RadEfficiency|RadEffRed|Pmax|FixN supply", "Nitrogen and Carbon > 0|Growth = 3");
	;

	// New Farquhar model parameters;
	;
	pPs = new Ps(&P_ATheta, PLANT, ORGANIC, TRANSPORT, PHOTOSYNTHESIS, NORMAL);
	Define(pPs, "P_ATheta", 0.877, "-", "Nitrogen and Carbon >= 1|Growth>3", 0.1, 1.5);
	pPs = new Ps(&P_BTheta, PLANT, ORGANIC, TRANSPORT, PHOTOSYNTHESIS, NORMAL);
	Define(pPs, "P_BTheta", 0.99, "-", "Nitrogen and Carbon >= 1|Growth>3", 0.1, 1.5);
	pPs = new Ps(&P_Surface, PLANT, ORGANIC, TRANSPORT, PHOTOSYNTHESIS, NORMAL);
	Define(pPs, "P_Surface", 1.E5, "Pa", "Nitrogen and Carbon >=1 |Growth>3", 0.9E5, 1.2E5);
	pPs = new Ps(&CO2_A, PLANT, ORGANIC, TRANSPORT, PHOTOSYNTHESIS, NORMAL);
	Define(pPs, "CO2_A", 354., "ppm", "Nitrogen and Carbon >= 1|Growth>=3", 0., 1000.);
	;
	pP = new P(&C3Type, pNumPlants, PLANT, ORGANIC, TRANSPORT, PHOTOSYNTHESIS, NORMAL); initv.assign(p_Plant->NumPlants, 1.);
	Define(pP, pNumPlants, "C3Type", initv, "-", "Nitrogen and Carbon >= 1|Growth>3", 0., 1.);
	mTab_Farq.push_back(pP);
	pP = new P(&Vcmax, pNumPlants, PLANT, ORGANIC, TRANSPORT, PHOTOSYNTHESIS, NORMAL); initv.assign(p_Plant->NumPlants, 60.);
	Define(pP, pNumPlants, "Vcmax", initv, "micro mol/m2/s", "Nitrogen and Carbon >= 1|Growth>3", 10., 200.);
	mTab_Farq.push_back(pP);
	pP = new P(&QEff, pNumPlants, PLANT, ORGANIC, TRANSPORT, PHOTOSYNTHESIS, NORMAL); initv.assign(p_Plant->NumPlants, 8.);
	Define(pP, pNumPlants, "QuanEff", initv, "gDw/MJ", "Nitrogen and Carbon >= 1|Growth>3", 0.5, 20.); mTab_Farq.push_back(pP);

	functorprop.FuncName = MyFunc::PHOTO_EFF_FARQUAR; functorprop.Arg = MyArgType::T_D1;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Photosynthesis Light efficiency Farquar";
	p_vec.clear();
	p_vec.push_back(pP);

	pFunc = new Func(PLANT, CARBON, NOFPROC, PHOTOSYNTHESIS, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec);





	pP = new P(&RBoundary, pNumPlants, PLANT, ORGANIC, TRANSPORT, PHOTOSYNTHESIS, NORMAL); initv.assign(p_Plant->NumPlants, 10.);
	Define(pP, pNumPlants, "RBoundary", initv, "s/m", "Nitrogen and Carbon >= 1|Growth>3", 0., 100.);
	mTab_Farq.push_back(pP);
	pP = m_pModelMap->GetPPointer("Conduct Max"); mTab_Farq.push_back(pP);

	pP = m_pModelMap->GetPPointer("RadEffRed"); mTab_Farq.push_back(pP);

	pP = m_pModelMap->GetPPointer("FixN supply"); mTab_Farq.push_back(pP);

	//Remain add ConductMax, RadEff and FixN
	pTab = new Tab(&mTab_Farq, PLANT, NITROGEN, TRANSPORT, PHOTOSYNTHESIS, NORMAL);

	Define(pTab, "Farquhar parameters", "C3Type|Vcmax|QuanEff|RBoundary|Conduct Max|RadEffRed|FixN supply", "Nitrogen and Carbon > 0|Growth > 3");
	;


	return true;
}
bool NC_Plant_Functions::Def_Allocation() {
	Func *pFunc;
	SoilModel* pSoil = (SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
	p_Soil_WaterF = &pSoil->m_SoilFrost;
	PlantModel* pPlant = (PlantModel*)m_pModelInfo->GetModelPtr("Plant Model");
	WaterUptake* p_Plant = &pPlant->m_WaterUptake;
	NE *pNL = m_pModelMap->GetNEPointer("Number of Layers");
	NE *pNumPlants = m_pModelMap->GetNEPointer("Number of Plants");

	// Parameters;
	string funcname;
	FunctorProp functorprop;
	vector<Ps*> ps_vec;
	vector<P*> p_vec;
	ps_vec.clear();
	vector<double> initv;	initv.assign(p_Plant->NumPlants, 0.);
	Ps *pPs; P *pP;
	Tab *pTab;

	PLOT_PARAM plotpar;
	plotpar.Num_Of_Curves = 1;
	plotpar.Num_Of_Points = 300;
	plotpar.Num_Of_X = 1;
	plotpar.X_Min = 0;
	plotpar.X_Max = 2;
	plotpar.LogXScale = false;
	plotpar.LogYScale = false;
	plotpar.Num_Of_Opt = 1;


	functorprop.FunctorInitOption = string::npos;
	functorprop.pModelInfo = m_pModelInfo;

	functorprop.FuncName = MyFunc::ALLO_LEAF_MASS_FIXED;
	funcname = "Allocation to Leaf fixed Function";
	// Allocation parameters;
	pP = new P(&AlloLeaf1, pNumPlants, PLANT, ORGANIC, TRANSPORT, REALLOCATION, NORMAL); initv.assign(p_Plant->NumPlants, 0.2);
	Define(pP, pNumPlants, "Leaf c1", initv, "-", "Nitrogen and Carbon >= 1", 0., 1.);
	mTab_Allo.push_back(pP);
	p_vec.push_back(pP);
	pFunc = new Func(PLANT, CARBON, NOFPROC, REALLOCATION, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec);

	functorprop.FuncName = MyFunc::ALLO_LEAF_MASS_LINEAR;
	funcname = "Allocation to Leaf linear Function";

	plotpar.X_Min = 0;
	plotpar.X_Max = 300;
	plotpar.X_Label = "Leaf ";	plotpar.X_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::CARBON_MASS_UNIT, true);
	plotpar.Y_Label = "Degree of Allocation ";	plotpar.Y_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::NO_UNIT, true);


	pP = new P(&AlloLeaf2, pNumPlants, PLANT, ORGANIC, TRANSPORT, REALLOCATION, NORMAL); initv.assign(p_Plant->NumPlants, -0.0001);
	Define(pP, pNumPlants, "Leaf c2", initv, UNIT_TYPES::INVERSE_MASS_UNIT, "Nitrogen and Carbon >= 1|Leaf Allocation shoot = 1", -.005, .1);
	functorprop.Arg = MyArgType::T_D1;
	mTab_Allo.push_back(pP);
	p_vec.push_back(pP);
	pFunc = new Func(PLANT, CARBON, NOFPROC, REALLOCATION, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec, plotpar);

	functorprop.FuncName = MyFunc::ALLO_LEAF_MASS_EXP;
	funcname = "Allocation to Leaf exp Function";
	pP = new P(&AlloLeaf3, pNumPlants, PLANT, ORGANIC, TRANSPORT, REALLOCATION, NORMAL); initv.assign(p_Plant->NumPlants, -.002);
	Define(pP, pNumPlants, "Leaf c3", initv, "-", "Nitrogen and Carbon >= 1|Leaf Allocation shoot > 1", -0.1, 0.);
	mTab_Allo.push_back(pP);
	p_vec.push_back(pP);
	pFunc = new Func(PLANT, CARBON, NOFPROC, REALLOCATION, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec, plotpar);



	functorprop.FuncName = MyFunc::ALLO_ROOT_WATER_FIXED; functorprop.Arg = MyArgType::T_D1;
	funcname = "Allocation to Root Water fixed Function";
	p_vec.clear();
	pP = new P(&AlloRootWater1, pNumPlants, PLANT, ORGANIC, TRANSPORT, REALLOCATION, NORMAL); initv.assign(p_Plant->NumPlants, 0.2);
	Define(pP, pNumPlants, "Root Water c1", initv, "-", "Nitrogen and Carbon >= 1", 0., 1.);
	mTab_Allo.push_back(pP);
	p_vec.push_back(pP);
	pFunc = new Func(PLANT, CARBON, NOFPROC, REALLOCATION, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec, plotpar);


	functorprop.FuncName = MyFunc::ALLO_ROOT_WATER_LINEAR;
	funcname = "Allocation to Root Water Linear Function";
	plotpar.X_Max = 1;
	plotpar.X_Label = "Degree of Potential Water Uptake ";	plotpar.X_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::NO_UNIT, true);
	pP = new P(&AlloRootWater2, pNumPlants, PLANT, ORGANIC, TRANSPORT, REALLOCATION, NORMAL); initv.assign(p_Plant->NumPlants, -.1);
	Define(pP, pNumPlants, "Root Water c2", initv, "-", "Nitrogen and Carbon >= 1|Root Allocation water > 0", -.5, .5);
	mTab_Allo.push_back(pP);

	p_vec.push_back(pP);
	pFunc = new Func(PLANT, CARBON, NOFPROC, REALLOCATION, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec, plotpar);

	functorprop.FuncName = MyFunc::ALLO_ROOT_WATER_EXP;
	funcname = "Allocation to Root Water Exp Function";
	pP = new P(&AlloRootWater3, pNumPlants, PLANT, ORGANIC, TRANSPORT, REALLOCATION, NORMAL); initv.assign(p_Plant->NumPlants, -0.2);
	Define(pP, pNumPlants, "Root Water c3", initv, "-", "Nitrogen and Carbon >= 1|Root Allocation water > 1", -1., 0.);
	mTab_Allo.push_back(pP);

	p_vec.push_back(pP);
	pFunc = new Func(PLANT, CARBON, NOFPROC, REALLOCATION, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec, plotpar);



	functorprop.FuncName = MyFunc::ALLO_ROOT_NITROGEN_FIXED; functorprop.Arg = MyArgType::T_D1;
	funcname = "Allocation to Root CN Leaf fixed Function";
	p_vec.clear();
	plotpar.X_Label = "Degree of Potential Nitrogen Response ";	plotpar.X_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::NO_UNIT, true);


	pP = new P(&AlloRoot_CN1, pNumPlants, PLANT, ORGANIC, TRANSPORT, REALLOCATION, NORMAL); initv.assign(p_Plant->NumPlants, 0.2);
	Define(pP, pNumPlants, "Root CN c1", initv, "-", "Nitrogen and Carbon >= 1", 0., 1.);
	mTab_Allo.push_back(pP);
	p_vec.push_back(pP);
	pFunc = new Func(PLANT, CARBON, NOFPROC, REALLOCATION, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec, plotpar);

	funcname = "Allocation to Root CN Leaf linear Function";
	functorprop.FuncName = MyFunc::ALLO_ROOT_NITROGEN_LINEAR;

	pP = new P(&AlloRoot_CN2, pNumPlants, PLANT, ORGANIC, TRANSPORT, REALLOCATION, NORMAL); initv.assign(p_Plant->NumPlants, -.1);
	Define(pP, pNumPlants, "Root CN c2", initv, "-", "Nitrogen and Carbon >= 1|Root Allocation N Leaf >0", -.5, 0.5);
	mTab_Allo.push_back(pP);
	p_vec.push_back(pP);

	pFunc = new Func(PLANT, CARBON, NOFPROC, REALLOCATION, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec, plotpar);

	funcname = "Allocation to Root CN Leaf exp Function";
	functorprop.FuncName = MyFunc::ALLO_ROOT_NITROGEN_EXP;
	pP = new P(&AlloRoot_CN3, pNumPlants, PLANT, ORGANIC, TRANSPORT, REALLOCATION, NORMAL); initv.assign(p_Plant->NumPlants, -0.2);
	Define(pP, pNumPlants, "Root CN c3", initv, "-", "Nitrogen and Carbon >= 1|Root Allocation N Leaf >1", -1., 0.);
	mTab_Allo.push_back(pP);
	p_vec.push_back(pP);

	pFunc = new Func(PLANT, CARBON, NOFPROC, REALLOCATION, NORMAL);

	Define(pFunc, functorprop, funcname, p_vec, plotpar);

	functorprop.FuncName = MyFunc::ALLO_ROOT_PHOSPHORUS_FIXED; functorprop.Arg = MyArgType::T_D1;
	funcname = "Allocation to Root CP Leaf fixed Function";
	p_vec.clear();
	functorprop.FuncName = MyFunc::ALLO_ROOT_NITROGEN_FIXED; functorprop.Arg = MyArgType::T_D1;
	p_vec.clear();
	plotpar.X_Label = "Degree of Potential Phosphorus Response ";	plotpar.X_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::NO_UNIT, true);


	pP = new P(&AlloRoot_CP1, pNumPlants, PLANT, ORGANIC, TRANSPORT, REALLOCATION, NORMAL); initv.assign(p_Plant->NumPlants, 0.2);
	Define(pP, pNumPlants, "Root CP c1", initv, "-", "Nitrogen and Carbon >= 1|Phosphorus>0", 0., 1.);
	mTab_Allo.push_back(pP);
	p_vec.push_back(pP);
	pFunc = new Func(PLANT, CARBON, NOFPROC, REALLOCATION, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec);

	funcname = "Allocation to Root CP Leaf linear Function";
	functorprop.FuncName = MyFunc::ALLO_ROOT_PHOSPHORUS_LINEAR;
	pP = new P(&AlloRoot_CP2, pNumPlants, PLANT, ORGANIC, TRANSPORT, REALLOCATION, NORMAL); initv.assign(p_Plant->NumPlants, -.1);
	Define(pP, pNumPlants, "Root CP c2", initv, "-", "Nitrogen and Carbon >= 1|Root Allocation P Leaf >0|Phosphorus>0", -.5, 0.5);
	mTab_Allo.push_back(pP);
	p_vec.push_back(pP);

	pFunc = new Func(PLANT, CARBON, NOFPROC, REALLOCATION, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec, plotpar);

	funcname = "Allocation to Root CP Leaf exp Function";
	functorprop.FuncName = MyFunc::ALLO_ROOT_PHOSPHORUS_EXP;
	pP = new P(&AlloRoot_CP3, pNumPlants, PLANT, ORGANIC, TRANSPORT, REALLOCATION, NORMAL); initv.assign(p_Plant->NumPlants, -0.2);
	Define(pP, pNumPlants, "Root CP c3", initv, "-", "Nitrogen and Carbon >= 1|Root Allocation P Leaf >1|Phosphorus>0", -1., 0.);
	mTab_Allo.push_back(pP);
	p_vec.push_back(pP);
	pFunc = new Func(PLANT, CARBON, NOFPROC, REALLOCATION, NORMAL);

	Define(pFunc, functorprop, funcname, p_vec, plotpar);





	functorprop.FuncName = MyFunc::ALLO_ROOT_MASS_FIXED; functorprop.Arg = MyArgType::T_D1;
	funcname = "Allocation to Root Above Ground Mass Function";
	p_vec.clear();

	plotpar.X_Label = "Above Ground Biomass ";	plotpar.X_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::CARBON_MASS_UNIT, true);
	plotpar.X_Max = 300;



	pP = new P(&AlloRootTotMass1, pNumPlants, PLANT, ORGANIC, TRANSPORT, REALLOCATION, NORMAL); initv.assign(p_Plant->NumPlants, 0.2);
	Define(pP, pNumPlants, "Root Mass c1", initv, "-", "Nitrogen and Carbon >= 1", 0., 1.);
	mTab_Allo.push_back(pP);
	p_vec.push_back(pP);
	pFunc = new Func(PLANT, CARBON, NOFPROC, REALLOCATION, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec, plotpar);

	functorprop.FuncName = MyFunc::ALLO_ROOT_MASS_LINEAR;
	funcname = "Allocation to Root Above Ground Mass linear Function";
	pP = new P(&AlloRootTotMass2, pNumPlants, PLANT, ORGANIC, TRANSPORT, REALLOCATION, NORMAL); initv.assign(p_Plant->NumPlants, 0.002);
	Define(pP, pNumPlants, "Root Mass c2", initv, UNIT_TYPES::INVERSE_MASS_UNIT, "Nitrogen and Carbon >= 1|Root Allocation mass >0", -.5, 1.);
	mTab_Allo.push_back(pP);
	p_vec.push_back(pP);
	
	pPs = new Ps(&AlloFracMinimumRoots, PLANT, ORGANIC, TRANSPORT, REALLOCATION, NORMAL);
	Define(pPs, "Allo Frac Miniumum Roots", 0.05, UNIT_TYPES::NO_UNIT, "Nitrogen and Carbon >= 1|Root Allocation mass >0", .0, .2);
	ps_vec.clear();
	ps_vec.push_back(pPs);
	pFunc = new Func(PLANT, CARBON, NOFPROC, REALLOCATION, NORMAL);
	Define(pFunc, functorprop, funcname,ps_vec, p_vec, plotpar);




	functorprop.FuncName = MyFunc::ALLO_ROOT_MASS_EXP;
	funcname = "Allocation to Root Above Ground Exp Function";
	pP = new P(&AlloRootTotMass3, pNumPlants, PLANT, ORGANIC, TRANSPORT, REALLOCATION, NORMAL); initv.assign(p_Plant->NumPlants, -0.002);
	Define(pP, pNumPlants, "Root Mass c3", initv, UNIT_TYPES::INVERSE_MASS_UNIT, "Nitrogen and Carbon >= 1|Root Allocation mass >1", -10., 0.);
	mTab_Allo.push_back(pP);
	p_vec.push_back(pP);
	pFunc = new Func(PLANT, CARBON, NOFPROC, REALLOCATION, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec, plotpar); p_vec.clear();

	functorprop.FuncName = MyFunc::ALLO_RATECOEF_NEWMOBILE; functorprop.Arg = MyArgType::T_D1;
	funcname = "Allocation from NewMobile to Root, Leaf and Stem";
	

	plotpar.X_Min = 0;
	plotpar.X_Max = 3;
	plotpar.X_Label = "NewMobile ";	plotpar.X_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::MASS_UNIT, true);
	plotpar.Y_Label = "Total Allocation to Growth ";	plotpar.Y_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::MASSFLOW_UNIT, true);

	;
	pP = new P(&AlloRateCoef, pNumPlants, PLANT, ORGANIC, TRANSPORT, REALLOCATION, NORMAL); initv.assign(p_Plant->NumPlants, 1.);
	Define(pP, pNumPlants, "AlloRateCoef", initv, "-", "Nitrogen and Carbon >= 1", 0., 1.);	p_vec.push_back(pP);

	pFunc = new Func(PLANT, CARBON, NOFPROC, REALLOCATION, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec, plotpar);

	mTab_Allo.push_back(pP);
	;
	pTab = new Tab(&mTab_Allo, PLANT, ORGANIC, TRANSPORT, REALLOCATION, NORMAL);
	Define(pTab, "Allocation parameters", "Leaf c1|Leaf c2|Leaf c3|Root Water c1|Root Water c2|Root Water c3|Root CN c1|Root CN c2|Root CN c3|Root CP c1|Root CP c2|Root CP c3|Root Mass c1|Root Mass c2|Root Mass c3|AlloRateCoef",
		"Nitrogen and Carbon > 0|Growth > 0");


	return true;
}
bool NC_Plant_Functions::Def_LitterFall() {
	Func *pFunc;
	SoilModel* pSoil = (SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
	p_Soil_WaterF = &pSoil->m_SoilFrost;
	PlantModel* pPlant = (PlantModel*)m_pModelInfo->GetModelPtr("Plant Model");
	WaterUptake* p_Plant = &pPlant->m_WaterUptake;
	NE *pNL = m_pModelMap->GetNEPointer("Number of Layers");
	NE *pNumPlants = m_pModelMap->GetNEPointer("Number of Plants");

	// Parameters;
	string funcname;
	FunctorProp functorprop;
	vector<Ps*> ps_vec;
	vector<P*> p_vec;
	ps_vec.clear();
	vector<double> initv;	initv.assign(p_Plant->NumPlants, 0.);
	Ps *pPs; P *pP;
	Tab *pTab;

	functorprop.FunctorInitOption = string::npos;
	functorprop.pModelInfo = m_pModelInfo;


	functorprop.FuncName = MyFunc::LEAF_LITTERFALL_TSUM; functorprop.Arg = MyArgType::T_D1;
	funcname = "Leaf LitterFall TempSum Respons";
	p_vec.clear();

	pP = new P(&Leaf_LitterRateC1, pNumPlants, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL); initv.assign(p_Plant->NumPlants, 0.001);
	Define(pP, pNumPlants, "LeafRate1", initv, "/day", "Nitrogen and Carbon >= 1", 0., 0.1); p_vec.push_back(pP);
	mTab_LitterA.push_back(pP);
	pP = new P(&Leaf_LitterRateC2, pNumPlants, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL); initv.assign(p_Plant->NumPlants, 0.05);
	Define(pP, pNumPlants, "LeafRate2", initv, "/day", "Nitrogen and Carbon >= 1|Litter fall dynamics > 0", 0., 0.7); p_vec.push_back(pP);
	mTab_LitterA.push_back(pP);
	pP = new P(&Leaf_LitterTsum1, pNumPlants, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL); initv.assign(p_Plant->NumPlants, 1200.);
	Define(pP, pNumPlants, "LeafTsum1", initv, UNIT_TYPES::TEMPSUM_UNIT, "Nitrogen and Carbon >= 1|Litter fall dynamics > 0", 0., 3000.); p_vec.push_back(pP);
	mTab_LitterA.push_back(pP);
	pP = new P(&Leaf_LitterTsum2, pNumPlants, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL); initv.assign(p_Plant->NumPlants, 1400.);
	Define(pP, pNumPlants, "LeafTsum2", initv, UNIT_TYPES::TEMPSUM_UNIT, "Nitrogen and Carbon >= 1|Litter fall dynamics > 0", 10., 3000.); p_vec.push_back(pP);
	mTab_LitterA.push_back(pP);
	pP = new P(&Dorm_FinalDayNo, pNumPlants, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL); initv.assign(p_Plant->NumPlants, 320.);
	Define(pP, pNumPlants, "Dorm_FinalDayNo", initv,UNIT_TYPES::DAYOFYEAR_UNIT , "Nitrogen and Carbon >= 1|Litter fall dynamics > 0", 100., 366.); p_vec.push_back(pP);
	mTab_LitterA.push_back(pP);

	pFunc = new Func(PLANT, CARBON, NOFPROC, LITTERFALL, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec);

	functorprop.FuncName = MyFunc::STEM_LITTERFALL_TSUM; functorprop.Arg = MyArgType::T_D1;
	funcname = "Stem LitterFall TempSum Respons";
	p_vec.clear();


	pP = new P(&Stem_LitterRateC1, pNumPlants, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL); initv.assign(p_Plant->NumPlants, 0.00001);
	Define(pP, pNumPlants, "StemRate1", initv, "/day", "Nitrogen and Carbon >= 1", 0., 0.1); p_vec.push_back(pP);
	mTab_LitterA.push_back(pP);
	pP = new P(&Stem_LitterRateC2, pNumPlants, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL); initv.assign(p_Plant->NumPlants, 0.00002);
	Define(pP, pNumPlants, "StemRate2", initv, "/day", "Nitrogen and Carbon >= 1|Litter fall dynamics > 0", 0., 0.7); p_vec.push_back(pP);
	mTab_LitterA.push_back(pP);
	pP = new P(&Stem_LitterTsum1, pNumPlants, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL); initv.assign(p_Plant->NumPlants, 1200.);
	Define(pP, pNumPlants, "StemTsum1", initv, UNIT_TYPES::TEMPSUM_UNIT, "Nitrogen and Carbon >= 1|Litter fall dynamics > 0", 0., 10000.); p_vec.push_back(pP);
	mTab_LitterA.push_back(pP);
	pP = new P(&Stem_LitterTsum2, pNumPlants, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL); initv.assign(p_Plant->NumPlants, 1400.);
	Define(pP, pNumPlants, "StemTsum2", initv, UNIT_TYPES::TEMPSUM_UNIT, "Nitrogen and Carbon >= 1|Litter fall dynamics > 0", 0., 10000.); p_vec.push_back(pP);
	mTab_LitterA.push_back(pP);

	pFunc = new Func(PLANT, CARBON, NOFPROC, LITTERFALL, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec);



	functorprop.FuncName = MyFunc::GRAIN_LITTERFALL_TSUM; functorprop.Arg = MyArgType::T_D3;
	funcname = "Grain LitterFall TempSum Respons";
	p_vec.clear();


	pP = new P(&Grain_LitterRateC1, pNumPlants, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL); initv.assign(p_Plant->NumPlants, 0.001);
	Define(pP, pNumPlants, "GrainRate1", initv, "/day", "Nitrogen and Carbon >= 1|Grain Development >0", 0., 0.1); p_vec.push_back(pP);
	mTab_LitterA.push_back(pP);
	pP = new P(&Grain_LitterRateC2, pNumPlants, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL); initv.assign(p_Plant->NumPlants, 0.05);
	Define(pP, pNumPlants, "GrainRate2", initv, "/day", "Nitrogen and Carbon >= 1|Litter fall dynamics > 0|Grain Development >0", 0., 0.7); p_vec.push_back(pP);
	mTab_LitterA.push_back(pP);
	pP = new P(&Grain_LitterTsum1, pNumPlants, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL); initv.assign(p_Plant->NumPlants, 1200.);
	Define(pP, pNumPlants, "GrainTsum1", initv, UNIT_TYPES::TEMPSUM_UNIT, "Nitrogen and Carbon >= 1|Litter fall dynamics > 0|Grain Development >0", 0., 3000.); p_vec.push_back(pP);
	mTab_LitterA.push_back(pP);
	pP = new P(&Grain_LitterTsum2, pNumPlants, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL); initv.assign(p_Plant->NumPlants, 1400.);
	Define(pP, pNumPlants, "GrainTsum2", initv, UNIT_TYPES::TEMPSUM_UNIT, "Nitrogen and Carbon >= 1|Litter fall dynamics > 0|Grain Development >0", 10., 3000.); p_vec.push_back(pP);
	mTab_LitterA.push_back(pP);

	pP = new P(&AboveGroundLitterFall_Position4Target, pNumPlants, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL); initv.assign(p_Plant->NumPlants, 0.);
	Define(pP, pNumPlants, "Position4Target", initv, UNIT_TYPES::LENGTH_UNIT, "Nitrogen and Carbon >= 1", -10, 0.01);
	mTab_LitterA.push_back(pP);



	pFunc = new Func(PLANT, CARBON, NOFPROC, LITTERFALL, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec);







	pP = new P(&Leaf_FracLitter2, pNumPlants, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL); initv.assign(p_Plant->NumPlants, 0.1);
	Define(pP, pNumPlants, "Leaf_LF2", initv, "-", "Nitrogen and Carbon >= 1|LitterPools >0", 0., 1.0);
	mTab_LitterBC.push_back(pP);
	pP = new P(&Stem_FracLitter2, pNumPlants, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL); initv.assign(p_Plant->NumPlants, 0.1);
	Define(pP, pNumPlants, "Stem_LF2", initv, "-", "Nitrogen and Carbon >= 1|LitterPools >0", 0., 1.0);
	mTab_LitterBC.push_back(pP);
	pP = new P(&Grain_FracLitter2, pNumPlants, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL); initv.assign(p_Plant->NumPlants, 0.1);
	Define(pP, pNumPlants, "Grain_LF2", initv, "-", "Nitrogen and Carbon >= 1|LitterPools >0", 0., 1.0);
	mTab_LitterBC.push_back(pP);
	;



	functorprop.FuncName = MyFunc::ROOT_LITTERFALL_TSUM; functorprop.Arg = MyArgType::T_D3;
	funcname = "Root LitterFall TempSum Respons";
	p_vec.clear();

	pP = new P(&Root_LitterRateC1, pNumPlants, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL); initv.assign(p_Plant->NumPlants, 0.01);
	Define(pP, pNumPlants, "RootRate1", initv, "/day", "Nitrogen and Carbon >= 1", 0., 0.1); p_vec.push_back(pP);
	mTab_LitterB.push_back(pP);
	pP = new P(&Root_LitterRateC2, pNumPlants, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL); initv.assign(p_Plant->NumPlants, 0.05);
	Define(pP, pNumPlants, "RootRate2", initv, "/day", "Nitrogen and Carbon >= 1|Litter fall dynamics > 0", 0., 0.5); p_vec.push_back(pP);
	mTab_LitterB.push_back(pP);
	pP = new P(&Root_LitterTsum1, pNumPlants, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL); initv.assign(p_Plant->NumPlants, 1200.);
	Define(pP, pNumPlants, "RootTsum1", initv, UNIT_TYPES::TEMPSUM_UNIT, "Nitrogen and Carbon >= 1|Litter fall dynamics > 0", 0., 3000.); p_vec.push_back(pP);
	mTab_LitterB.push_back(pP);
	pP = new P(&Root_LitterTsum2, pNumPlants, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL); initv.assign(p_Plant->NumPlants, 1400.);
	Define(pP, pNumPlants, "RootTsum2", initv, UNIT_TYPES::TEMPSUM_UNIT, "Nitrogen and Carbon >= 1|Litter fall dynamics > 0", 10., 3000.); p_vec.push_back(pP);
	mTab_LitterB.push_back(pP);
	pFunc = new Func(PLANT, CARBON, NOFPROC, LITTERFALL, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec);

	
	
	pP = new P(&Root_FracLitter2, pNumPlants, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL); initv.assign(p_Plant->NumPlants, 0.1);
	Define(pP, pNumPlants, "Root_LF2", initv, "-", "Nitrogen and Carbon >= 1|LitterPools >0", 0., 1.0);
	mTab_LitterBC.push_back(pP);
	;
	functorprop.FuncName = MyFunc::COARSEROOT_LITTERFALL_TSUM; functorprop.Arg = MyArgType::T_D3;
	funcname = "Coarse Root LitterFall TempSum Respons";
	p_vec.clear();


	pP = new P(&CoarseRoot_LitterRateC1, pNumPlants, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL); initv.assign(p_Plant->NumPlants, 0.0001);
	Define(pP, pNumPlants, "CoarseRootRate1", initv, "/day", "Nitrogen and Carbon >= 1", 0., 0.1); p_vec.push_back(pP);
	mTab_LitterB.push_back(pP);
	pP = new P(&CoarseRoot_LitterRateC2, pNumPlants, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL); initv.assign(p_Plant->NumPlants, 0.0001);
	Define(pP, pNumPlants, "CoarseRootRate2", initv, "/day", "Nitrogen and Carbon >= 1|Litter fall dynamics > 0", 0., 0.5); p_vec.push_back(pP);
	mTab_LitterB.push_back(pP);
	pP = new P(&CoarseRoot_LitterTsum1, pNumPlants, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL); initv.assign(p_Plant->NumPlants, 1200.);
	Define(pP, pNumPlants, "CoarseRootTsum1", initv, UNIT_TYPES::TEMPSUM_UNIT, "Nitrogen and Carbon >= 1|Litter fall dynamics > 0", 0., 3000.); p_vec.push_back(pP);
	mTab_LitterB.push_back(pP);
	pP = new P(&CoarseRoot_LitterTsum2, pNumPlants, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL); initv.assign(p_Plant->NumPlants, 1400.);
	Define(pP, pNumPlants, "CoarseRootTsum2", initv, UNIT_TYPES::TEMPSUM_UNIT, "Nitrogen and Carbon >= 1|Litter fall dynamics > 0", 10., 3000.); p_vec.push_back(pP);
	mTab_LitterB.push_back(pP);
	pP = new P(&CoarseRoot_FracLitter2, pNumPlants, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL); initv.assign(p_Plant->NumPlants, 0.1);
	Define(pP, pNumPlants, "CoarseRoot_LF2", initv, "-", "Nitrogen and Carbon >= 1|LitterPools >0", 0., 1.0);
	mTab_LitterBC.push_back(pP);

	pFunc = new Func(PLANT, CARBON, NOFPROC, LITTERFALL, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec);


	pTab = new Tab(&mTab_LitterA, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL);
	Define(pTab, "Litter fall Above Ground", "LeafRate1|LeafRate2|LeafTsum1|LeafTsum2|StemRate1|StemRate2|StemTsum1|StemTsum2|GrainRate1|GrainRate2|GrainTsum1|GrainTsum2|Dorm_FinalDayNo", "Nitrogen and Carbon > 0");
	pTab = new Tab(&mTab_LitterB, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL);
	Define(pTab, "Litter fall Below Ground", "RootRate1|RootRate2|RootTsum1|RootTsum2|CoarseRootRate1|CoarseRootRate2|CoarseRootTsum1|CoarseRootTsum2", "Nitrogen and Carbon > 0");

	pTab = new Tab(&mTab_LitterBC, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL);
	Define(pTab, "Fractions of Litter2", "Leaf_LF2|Stem_LF2|Grain_LF2|Root_LF2|CoarseRoot_LF2", "Nitrogen and Carbon > 0|LitterPools >0");

	pP = new P(&ScaleOldLeaf, pNumPlants, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL); initv.assign(p_Plant->NumPlants, 1.);
	Define(pP, pNumPlants, "Old Leaf", initv, "-", "Nitrogen and Carbon >= 1", 0., 1.);
	mTab_LitterScale.push_back(pP);
	pP = new P(&ScaleNewLeaf, pNumPlants, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL); initv.assign(p_Plant->NumPlants, 0.);
	Define(pP, pNumPlants, "New Leaf", initv, "-", "Nitrogen and Carbon >= 1", 0., 1.);
	mTab_LitterScale.push_back(pP);
	pP = new P(&ScaleOldRoots, pNumPlants, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL); initv.assign(p_Plant->NumPlants, 1.);
	Define(pP, pNumPlants, "Old Roots", initv, "-", "Nitrogen and Carbon >= 1", 0., 1.);
	mTab_LitterScale.push_back(pP);
	pP = new P(&ScaleNewRoots, pNumPlants, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL); initv.assign(p_Plant->NumPlants, 1.);
	Define(pP, pNumPlants, "New Roots", initv, "-", "Nitrogen and Carbon >= 1", 0., 1.);
	mTab_LitterScale.push_back(pP);
	pP = new P(&ScaleOldStem, pNumPlants, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL); initv.assign(p_Plant->NumPlants, 1.);
	Define(pP, pNumPlants, "Old Stem", initv, "-", "Nitrogen and Carbon >= 1", 0., 1.);
	mTab_LitterScale.push_back(pP);
	pP = new P(&ScaleNewStem, pNumPlants, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL); initv.assign(p_Plant->NumPlants, 0.);
	Define(pP, pNumPlants, "New Stem", initv, "-", "Nitrogen and Carbon >= 1", 0., 1.);
	mTab_LitterScale.push_back(pP);
	pP = new P(&ScaleOldCoarseRoots, pNumPlants, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL); initv.assign(p_Plant->NumPlants, 1.);
	Define(pP, pNumPlants, "Old CoarseRoots", initv, "-", "Nitrogen and Carbon >= 1", 0., 1.);
	mTab_LitterScale.push_back(pP);
	pP = new P(&ScaleNewCoarseRoots, pNumPlants, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL); initv.assign(p_Plant->NumPlants, 1.);
	Define(pP, pNumPlants, "New CoarseRoots", initv, "-", "Nitrogen and Carbon >= 1", 0., 1.);
	mTab_LitterScale.push_back(pP);

	functorprop.FuncName = MyFunc::LEAF_LITTERFALL_LAI; functorprop.Arg = MyArgType::T_D1;
	funcname = "Leaf LitterFall LAI Response";
	p_vec.clear();
	pP = new P(&LAI_Enh_Coef, pNumPlants, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL); initv.assign(p_Plant->NumPlants, 0.);
	Define(pP, pNumPlants, "LAI Enh Coef", initv, "-", "Nitrogen and Carbon >= 1", 0., 1.5);
	mTab_LitterScale.push_back(pP); p_vec.push_back(pP);

	pFunc = new Func(PLANT, CARBON, NOFPROC, LITTERFALL, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec);


	functorprop.FuncName = MyFunc::LEAF_LITTERFALL_STATIC; functorprop.Arg = MyArgType::T_D4;
	funcname = "Leaf LitterFall Static";
	p_vec.clear();
	
	p_vec.push_back(m_pModelMap->GetPPointer("LeafRate1"));
	p_vec.push_back(m_pModelMap->GetPPointer("LAI Enh Coef"));
	p_vec.push_back(m_pModelMap->GetPPointer("New Leaf"));
	p_vec.push_back(m_pModelMap->GetPPointer("Old Leaf"));


	pFunc = new Func(PLANT, CARBON, NOFPROC, LITTERFALL, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec);

	functorprop.FuncName = MyFunc::LEAF_LITTERFALL_DYNAMIC; functorprop.Arg = MyArgType::T_D4;
	funcname = "Leaf LitterFall Dynamic";
	p_vec.clear();

	p_vec.push_back(m_pModelMap->GetPPointer("LeafRate1"));
	p_vec.push_back(m_pModelMap->GetPPointer("LeafRate2"));
	p_vec.push_back(m_pModelMap->GetPPointer("LeafTsum1"));
	p_vec.push_back(m_pModelMap->GetPPointer("LeafTsum2"));
	p_vec.push_back(m_pModelMap->GetPPointer("Dorm_FinalDayNo"));
	p_vec.push_back(m_pModelMap->GetPPointer("LAI Enh Coef"));
	p_vec.push_back(m_pModelMap->GetPPointer("New Leaf"));
	p_vec.push_back(m_pModelMap->GetPPointer("Old Leaf"));

	pFunc = new Func(PLANT, CARBON, NOFPROC, LITTERFALL, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec);

	functorprop.FuncName = MyFunc::STEM_LITTERFALL_STATIC; functorprop.Arg = MyArgType::T_D4;
	funcname = "Stem LitterFall Static";
	p_vec.clear();

	p_vec.push_back(m_pModelMap->GetPPointer("StemRate1"));
	p_vec.push_back(m_pModelMap->GetPPointer("New Stem"));
	p_vec.push_back(m_pModelMap->GetPPointer("Old Stem"));


	pFunc = new Func(PLANT, CARBON, NOFPROC, LITTERFALL, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec);

	functorprop.FuncName = MyFunc::STEM_LITTERFALL_DYNAMIC; functorprop.Arg = MyArgType::T_D4;
	funcname = "Stem LitterFall Dynamic";
	p_vec.clear();

	p_vec.push_back(m_pModelMap->GetPPointer("StemRate1"));
	p_vec.push_back(m_pModelMap->GetPPointer("StemRate2"));
	p_vec.push_back(m_pModelMap->GetPPointer("StemTsum1"));
	p_vec.push_back(m_pModelMap->GetPPointer("StemTsum2"));
	p_vec.push_back(m_pModelMap->GetPPointer("Dorm_FinalDayNo"));
	p_vec.push_back(m_pModelMap->GetPPointer("New Stem"));//HH
	p_vec.push_back(m_pModelMap->GetPPointer("Old Stem"));//HH

	pFunc = new Func(PLANT, CARBON, NOFPROC, LITTERFALL, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec);

	functorprop.FuncName = MyFunc::GRAIN_LITTERFALL_STATIC; functorprop.Arg = MyArgType::T_D4;
	funcname = "Grain LitterFall Static";
	p_vec.clear();

	p_vec.push_back(m_pModelMap->GetPPointer("GrainRate1"));



	pFunc = new Func(PLANT, CARBON, NOFPROC, LITTERFALL, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec);

	functorprop.FuncName = MyFunc::GRAIN_LITTERFALL_DYNAMIC; functorprop.Arg = MyArgType::T_D3;
	funcname = "Grain LitterFall Dynamic";
	p_vec.clear();

	p_vec.push_back(m_pModelMap->GetPPointer("GrainRate1"));
	p_vec.push_back(m_pModelMap->GetPPointer("GrainRate2"));
	p_vec.push_back(m_pModelMap->GetPPointer("GrainTsum1"));
	p_vec.push_back(m_pModelMap->GetPPointer("GrainTsum2"));
	p_vec.push_back(m_pModelMap->GetPPointer("Dorm_FinalDayNo"));


	pFunc = new Func(PLANT, CARBON, NOFPROC, LITTERFALL, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec);

	functorprop.FuncName = MyFunc::ROOT_LITTERFALL_STATIC; functorprop.Arg = MyArgType::T_D4;
	funcname = "Root LitterFall Static";
	p_vec.clear();

	p_vec.push_back(m_pModelMap->GetPPointer("RootRate1"));
	p_vec.push_back(m_pModelMap->GetPPointer("New Roots"));
	p_vec.push_back(m_pModelMap->GetPPointer("Old Roots"));


	pFunc = new Func(PLANT, CARBON, NOFPROC, LITTERFALL, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec);

	functorprop.FuncName = MyFunc::ROOT_LITTERFALL_DYNAMIC; functorprop.Arg = MyArgType::T_D4;
	funcname = "Root LitterFall Dynamic";
	p_vec.clear();

	p_vec.push_back(m_pModelMap->GetPPointer("RootRate1"));
	p_vec.push_back(m_pModelMap->GetPPointer("RootRate2"));
	p_vec.push_back(m_pModelMap->GetPPointer("RootTsum1"));
	p_vec.push_back(m_pModelMap->GetPPointer("RootTsum2"));
	p_vec.push_back(m_pModelMap->GetPPointer("Dorm_FinalDayNo"));
	p_vec.push_back(m_pModelMap->GetPPointer("New Roots"));
	p_vec.push_back(m_pModelMap->GetPPointer("Old Roots"));

	pFunc = new Func(PLANT, CARBON, NOFPROC, LITTERFALL, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec);

	functorprop.FuncName = MyFunc::COARSEROOT_LITTERFALL_STATIC; functorprop.Arg = MyArgType::T_D4;
	funcname = "Coarse Root LitterFall Static";
	p_vec.clear();

	p_vec.push_back(m_pModelMap->GetPPointer("CoarseRootRate1"));
	p_vec.push_back(m_pModelMap->GetPPointer("New CoarseRoots"));
	p_vec.push_back(m_pModelMap->GetPPointer("Old CoarseRoots"));


	pFunc = new Func(PLANT, CARBON, NOFPROC, LITTERFALL, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec);

	functorprop.FuncName = MyFunc::COARSEROOT_LITTERFALL_DYNAMIC; functorprop.Arg = MyArgType::T_D4;
	funcname = "Coarse Root LitterFall Dynamic";
	p_vec.clear();

	p_vec.push_back(m_pModelMap->GetPPointer("CoarseRootRate1"));
	p_vec.push_back(m_pModelMap->GetPPointer("CoarseRootRate2"));
	p_vec.push_back(m_pModelMap->GetPPointer("CoarseRootTsum1"));
	p_vec.push_back(m_pModelMap->GetPPointer("CoarseRootTsum2"));
	p_vec.push_back(m_pModelMap->GetPPointer("Dorm_FinalDayNo"));
	p_vec.push_back(m_pModelMap->GetPPointer("New CoarseRoots"));
	p_vec.push_back(m_pModelMap->GetPPointer("Old CoarseRoots"));

	pFunc = new Func(PLANT, CARBON, NOFPROC, LITTERFALL, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec);

	
	pTab = new Tab(&mTab_LitterScale, PLANT, ORGANIC, TRANSPORT, LITTERFALL, NORMAL);
	Define(pTab, "Scaling of Litter fall", "Old Leaf|New Leaf|Old Roots|New Roots|Old Stem|New Stem|Old CoarseRoots|New CoarseRoots|LAI Enh Coef", "Nitrogen and Carbon > 0|Growth > 0|Perennial Plant > 0");
	return true;
}
bool NC_Plant_Functions::Def_Respiration() {
	Func *pFunc;
	SoilModel* pSoil = (SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
	p_Soil_WaterF = &pSoil->m_SoilFrost;
	PlantModel* pPlant = (PlantModel*)m_pModelInfo->GetModelPtr("Plant Model");
	WaterUptake* p_Plant = &pPlant->m_WaterUptake;
	NE *pNL = m_pModelMap->GetNEPointer("Number of Layers");
	NE *pNumPlants = m_pModelMap->GetNEPointer("Number of Plants");

	// Parameters;
	string funcname;
	FunctorProp functorprop;
	vector<Ps*> ps_vec;
	vector<P*> p_vec;
	ps_vec.clear();
	vector<double> initv;	initv.assign(p_Plant->NumPlants, 0.);
	Ps *pPs; P *pP;
	Tab *pTab;

	functorprop.FunctorInitOption = string::npos;
	functorprop.pModelInfo = m_pModelInfo;


	functorprop.FuncName = MyFunc::GROWTH_RESPIRATION; functorprop.Arg = MyArgType::T_D1;
	funcname = "Growth Respiration";
	p_vec.clear();
	pP = new P(&RespGCoef, pNumPlants, PLANT, CARBON, TRANSPORT, RESPIRATION, NORMAL); initv.assign(p_Plant->NumPlants, 0.21); p_vec.push_back(pP);
	Define(pP, pNumPlants, "GrowthCoef", initv, "1/day", "Nitrogen and Carbon >= 1|Growth >0|PlantRespiration>0", 0.0001, 0.6);


	pFunc = new Func(PLANT, CARBON, NOFPROC, RESPIRATION, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec);






	mTab_PResp.push_back(pP);

	pP = new P(&RespMCoefGrain, pNumPlants, PLANT, CARBON, TRANSPORT, RESPIRATION, NORMAL); initv.assign(p_Plant->NumPlants, 0.011);
	Define(pP, pNumPlants, "MCoefGrain", initv, "1/day", "Nitrogen and Carbon >= 1|Growth >0", 0.0001, 0.1);
	mTab_PResp.push_back(pP);
	pP = new P(&RespMCoefLeaf, pNumPlants, PLANT, CARBON, TRANSPORT, RESPIRATION, NORMAL); initv.assign(p_Plant->NumPlants, 0.034);
	Define(pP, pNumPlants, "MCoefLeaf", initv, "1/day", "Nitrogen and Carbon >= 1|Growth >0", 0.0001, 0.1);
	mTab_PResp.push_back(pP);
	pP = new P(&RespMCoefStem, pNumPlants, PLANT, CARBON, TRANSPORT, RESPIRATION, NORMAL); initv.assign(p_Plant->NumPlants, 0.017);
	Define(pP, pNumPlants, "MCoefStem", initv, "1/day", "Nitrogen and Carbon >= 1|Growth >0", 1.E-5, 0.1);
	mTab_PResp.push_back(pP);
	pP = new P(&RespMCoefRoot, pNumPlants, PLANT, CARBON, TRANSPORT, RESPIRATION, NORMAL); initv.assign(p_Plant->NumPlants, 0.011);
	Define(pP, pNumPlants, "MCoefRoot", initv, "1/day", "Nitrogen and Carbon >= 1|Growth >0", 0.0001, 0.1);
	mTab_PResp.push_back(pP);
	pP = new P(&RespMCoefCoarseRoot, pNumPlants, PLANT, CARBON, TRANSPORT, RESPIRATION, NORMAL); initv.assign(p_Plant->NumPlants, 0.011);
	Define(pP, pNumPlants, "MCoefCoarseRoot", initv, "1/day", "Nitrogen and Carbon >= 1|Growth >0", 1.E-5, 0.1);
	mTab_PResp.push_back(pP);

	pTab = new Tab(&mTab_PResp, PLANT, CARBON, TRANSPORT, RESPIRATION, NORMAL);
	Define(pTab, "Respiration of plants", "GrowthCoef|MCoefGrain|MCoefLeaf|MCoefStem|MCoefRoot|MCoefCoarseRoot", "Nitrogen and Carbon > 0");
	;
	pP = new P(&RespNO3Coef, pNumPlants, PLANT, NITROGEN, TRANSPORT, RESPIRATION, NORMAL); initv.assign(p_Plant->NumPlants, 2.61);
	Define(pP, pNumPlants, "NO3Coef", initv, "gC/gN", "Nitrogen and Carbon >= 1|PlantRespiration >1", 0.0, 10.0);
	mTab_PRespN.push_back(pP);
	pP = new P(&RespNH4Coef, pNumPlants, PLANT, NITROGEN, TRANSPORT, RESPIRATION, NORMAL); initv.assign(p_Plant->NumPlants, 0.79);
	Define(pP, pNumPlants, "NH4Coef", initv, "gC/gN", "Nitrogen and Carbon >= 1|PlantRespiration >1", 0.0, 10.0);
	mTab_PRespN.push_back(pP);
	pP = new P(&RespNOrgCoef, pNumPlants, PLANT, NITROGEN, TRANSPORT, RESPIRATION, NORMAL); initv.assign(p_Plant->NumPlants, 0.);
	Define(pP, pNumPlants, "NOrgCoef", initv, "gC/gN", "Nitrogen and Carbon >= 1|PlantRespiration >1", 0.0, 100.0);
	mTab_PRespN.push_back(pP);
	pTab = new Tab(&mTab_PRespN, PLANT, NITROGEN, TRANSPORT, RESPIRATION, NORMAL);
	Define(pTab, "Root NitrogenUptake Respiration", "NO3Coef|NH4Coef|NOrgCoef", "Nitrogen and Carbon > 0|PlantRespiration>1");
	;


	pPs = new Ps(&RespRateCoef, PLANT, CARBON, TRANSPORT, RESPIRATION, NORMAL);
	Define(pPs, "RespRateCoef", .0001, "1/day", "Nitrogen and Carbon >= 1|Growth >0|PlantRespiration=0", 0.0, 0.1);


	functorprop.FuncName = MyFunc::PLANTRESP_Q10_RESPONSE; functorprop.Arg = MyArgType::T_D1;
	funcname = "Plant Respiration Temp Response";
	ps_vec.clear();

	pPs = new Ps(&RespTemQ10, PLANT, CARBON, TRANSPORT, RESPIRATION, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "RespTemQ10", 2., "-", "Nitrogen and Carbon > 0|Resp Temp Response < 3|Resp Temp Response > 0", 0.0, 5., 0.5, 4.);
	pPs = new Ps(&RespTemQ10Bas, PLANT, CARBON, TRANSPORT, RESPIRATION, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "RespTemQ10Bas", 20., UNIT_TYPES::TEMP_UNIT, "Nitrogen and Carbon > 0|Resp Temp Response < 3|Resp Temp Response > 0", 0.0001, 100., 10., 30.);
	pFunc = new Func(PLANT, CARBON, NOFPROC, RESPIRATION, NORMAL);

	Define(pFunc, functorprop, funcname, ps_vec);

	functorprop.FuncName = MyFunc::PLANTRESP_Q10THRESHOLD_RESPONSE; functorprop.Arg = MyArgType::T_D1;

	funcname = "Plant Respiration Temp Response - with threshold";
	pPs = new Ps(&RespTemQ10Threshold, PLANT, CARBON, TRANSPORT, RESPIRATION, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "RespTemQ10Threshold", 5., UNIT_TYPES::TEMP_UNIT, "Nitrogen and Carbon > 0|Resp Temp Response = 2", 0.0001, 20., 2., 10.);
	
	pFunc = new Func(PLANT, CARBON, NOFPROC, RESPIRATION, NORMAL);

	Define(pFunc, functorprop, funcname, ps_vec);
	





	return true;
}

bool NC_Plant_Functions::Def_Fungi() {
	Func *pFunc;
	SoilModel* pSoil = (SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
	p_Soil_WaterF = &pSoil->m_SoilFrost;
	PlantModel* pPlant = (PlantModel*)m_pModelInfo->GetModelPtr("Plant Model");
	WaterUptake* p_Plant = &pPlant->m_WaterUptake;
	NE *pNL = m_pModelMap->GetNEPointer("Number of Layers");
	NE *pNumPlants = m_pModelMap->GetNEPointer("Number of Plants");

	// Parameters;
	string funcname;
	FunctorProp functorprop;
	vector<Ps*> ps_vec;
	vector<P*> p_vec;
	ps_vec.clear();
	vector<double> initv;	initv.assign(p_Plant->NumPlants, 0.);
	Ps *pPs; P *pP;
	Tab *pTab;

	functorprop.FunctorInitOption = string::npos;
	functorprop.pModelInfo = m_pModelInfo;


	p_vec.clear();
	pP = new P(&AlloFungiC1, pNumPlants, MYCON, ORGANIC, TRANSPORT, MYCON_NUPTAKE, NORMAL); initv.assign(p_Plant->NumPlants, 0.3);
	Define(pP, pNumPlants, "Allo Fungi c1", initv, "-", "Nitrogen and Carbon >= 1|Mycorrhiza Fungal Growth>0", 0., 1.);
	mTab_FungiGrowth.push_back(pP);
	pP = new P(&AlloMaxC1, pNumPlants, MYCON, ORGANIC, TRANSPORT, MYCON_NUPTAKE, NORMAL); initv.assign(p_Plant->NumPlants, 0.5);
	Define(pP, pNumPlants, "Allo Max Fungi c1", initv, "-", "Nitrogen and Carbon >= 1|Mycorrhiza Fungal Growth>0", 0., 1.);
	mTab_FungiGrowth.push_back(pP);
	pP = new P(&FungiCarbonDemandCoef, pNumPlants, MYCON, ORGANIC, TRANSPORT, MYCON_NUPTAKE, NORMAL); initv.assign(p_Plant->NumPlants, 0.03);//not used. HH
	Define(pP, pNumPlants, "Fungi Carbon demand Coef", initv, "-", "Nitrogen and Carbon >= 1|Mycorrhiza Fungal Growth>0", 0.001, 1.);
	mTab_FungiGrowth.push_back(pP);
	pP = new P(&RespMCoefFungi, pNumPlants, MYCON, ORGANIC, TRANSPORT, MYCON_NUPTAKE, NORMAL); initv.assign(p_Plant->NumPlants, 0.001);
	Define(pP, pNumPlants, "Resp Coef Fungi", initv, "-", "Nitrogen and Carbon >= 1|Mycorrhiza Fungal Growth>0", 0., .05);
	mTab_FungiGrowth.push_back(pP);
	pP = new P(&Fungi_MycRate, pNumPlants, MYCON, ORGANIC, TRANSPORT, MYCON_NUPTAKE, NORMAL); initv.assign(p_Plant->NumPlants, 0.01);
	Define(pP, pNumPlants, "FungiMycRate", initv, "-", "Nitrogen and Carbon >= 1|Mycorrhiza Fungal Growth>1", 0., .1);
	mTab_FungiGrowth.push_back(pP);
	pP = new P(&Fungi_MantleRate, pNumPlants, MYCON, ORGANIC, TRANSPORT, MYCON_NUPTAKE, NORMAL); initv.assign(p_Plant->NumPlants, 0.0014);
	Define(pP, pNumPlants, "FungiMantleRate", initv, "-", "Nitrogen and Carbon >= 1|Mycorrhiza Fungal Growth>1", 0., .1);
	mTab_FungiGrowth.push_back(pP);
	//AM 200911 Name change for fungi litter rate to make equal to root rate (before fungicomrate);
	;
	pP = new P(&Fungi_MycFrac, pNumPlants, MYCON, ORGANIC, TRANSPORT, MYCON_NUPTAKE, NORMAL); initv.assign(p_Plant->NumPlants, 0.5);
	Define(pP, pNumPlants, "Mycelia Frac", initv, "-", "Nitrogen and Carbon >= 1|Mycorrhiza Fungal Growth>0", 0., 1.);
	mTab_FungiGrowth.push_back(pP);



	pP = new P(&Fungi_LitterRateC1, pNumPlants, MYCON, ORGANIC, TRANSPORT, MYCON_NUPTAKE, NORMAL); initv.assign(p_Plant->NumPlants, 0.004);
	Define(pP, pNumPlants, "FungiRate1", initv, "-", "Nitrogen and Carbon >= 1|Mycorrhiza Fungal Growth=1", 0., .1); p_vec.push_back(pP);
	mTab_FungiGrowth.push_back(pP);
	pP = new P(&Fungi_LitterRateC2, pNumPlants, MYCON, ORGANIC, TRANSPORT, MYCON_NUPTAKE, NORMAL); initv.assign(p_Plant->NumPlants, 0.01);
	Define(pP, pNumPlants, "FungiRate2", initv, "-", "Nitrogen and Carbon >= 1|Mycorrhiza Fungal Growth=1", 0., .1); p_vec.push_back(pP);
	mTab_FungiGrowth.push_back(pP);
	p_vec.push_back(m_pModelMap->GetPPointer("RootTsum1"));
	p_vec.push_back(m_pModelMap->GetPPointer("RootTsum2"));
	p_vec.push_back(m_pModelMap->GetPPointer("Dorm_FinalDayNo"));

	pP = new P(&AreaCoverFungiCoef, pNumPlants, MYCON, ORGANIC, TRANSPORT, MYCON_NUPTAKE, NORMAL); initv.assign(p_Plant->NumPlants, 0.5);
	Define(pP, pNumPlants, "Myco Degree Fungi Coef", initv, "-", "Nitrogen and Carbon >= 1|Mycorrhiza Fungal Growth>0", 0.1, 1.);
	mTab_FungiGrowth.push_back(pP);
	pP = new P(&NretFungi, pNumPlants, MYCON, NITROGEN, TRANSPORT, MYCON_NUPTAKE, NORMAL); initv.assign(p_Plant->NumPlants, 0.54);
	Define(pP, pNumPlants, "N retention Fungi", initv, "-", "Nitrogen and Carbon >= 1|Mycorrhiza Fungal Growth>0", 0.01, 1.);
	mTab_FungiGrowth.push_back(pP);
	pP = new P(&NavailCoef, pNumPlants, MYCON, NITROGEN, TRANSPORT, MYCON_NUPTAKE, NORMAL); initv.assign(p_Plant->NumPlants, 0.009);
	Define(pP, pNumPlants, "N avail Coef", initv, "-", "Nitrogen and Carbon >= 1|Mycorrhiza Fungal Growth>0", 0.0001, 1.0);
	mTab_FungiGrowth.push_back(pP);
	//HH 
	pP = new P(&PretFungi, pNumPlants, MYCON, PHOSPHORUS, TRANSPORT, MYCON_NUPTAKE, NORMAL); initv.assign(p_Plant->NumPlants, 0.54);
	Define(pP, pNumPlants, "P retention Fungi", initv, "-", "Nitrogen and Carbon >= 1|Mycorrhiza Fungal Growth>0|Phosphorus>0", 0.01, 1.);
	mTab_FungiGrowth.push_back(pP);
	pP = new P(&PavailCoef, pNumPlants, MYCON, PHOSPHORUS, TRANSPORT, MYCON_NUPTAKE, NORMAL); initv.assign(p_Plant->NumPlants, 0.009);
	Define(pP, pNumPlants, "P avail Coef", initv, "-", "Nitrogen and Carbon >= 1|Mycorrhiza Fungal Growth>0|Phosphorus>0", 0.0001, 1.0);
	mTab_FungiGrowth.push_back(pP);



	pP = new P(&CN_RatioMaxFungi, pNumPlants, MYCON, ORGANIC, TRANSPORT, MYCON_NUPTAKE, NORMAL); initv.assign(p_Plant->NumPlants, 30.);
	Define(pP, pNumPlants, "Fungi Max CN", initv, "-", "Nitrogen and Carbon >= 1|Mycorrhiza Fungal Growth>0", 10., 50.);
	mTab_FungiTrans.push_back(pP);
	pP = new P(&CN_RatioMinFungi, pNumPlants, MYCON, ORGANIC, TRANSPORT, MYCON_NUPTAKE, NORMAL); initv.assign(p_Plant->NumPlants, 7.5);
	Define(pP, pNumPlants, "Fungi Min CN", initv, "-", "Nitrogen and Carbon >= 1|Mycorrhiza Fungal Growth>0", 2., 30.);
	mTab_FungiTrans.push_back(pP);
	pP = new P(&FungiMinNSupplyCoef, pNumPlants, MYCON, NITROGEN, TRANSPORT, MYCON_NUPTAKE, NORMAL); initv.assign(p_Plant->NumPlants, 0.5);
	Define(pP, pNumPlants, "Fungi MinN MYCON Supply", initv, "-", "Nitrogen and Carbon >= 1|Mycorrhiza Fungal Growth>0", 0.1, 1.);
	mTab_FungiTrans.push_back(pP);
	//AM shifted here from above to gather fungi variables;
	pP = new P(&Fungi_NO3rate, pNumPlants, MYCON, NITROGEN, TRANSPORT, MYCON_NUPTAKE, NORMAL); initv.assign(p_Plant->NumPlants, 0.0002);
	Define(pP, pNumPlants, "Fungi NO3 upt rate", initv, "-", "Nitrogen and Carbon >= 1|Mycorrhiza Fungal Growth>0", 0., 0.01);
	mTab_FungiTrans.push_back(pP);		//Unit = gN gdw-1 d-1;
	pP = new P(&Fungi_NH4rate, pNumPlants, MYCON, NITROGEN, TRANSPORT, MYCON_NUPTAKE, NORMAL); initv.assign(p_Plant->NumPlants, 0.0008);
	Define(pP, pNumPlants, "Fungi NH4 upt rate", initv, "-", "Nitrogen and Carbon >= 1|Mycorrhiza Fungal Growth>0", 0., 0.02); //Unit = gN gdw-1 d-1;
	mTab_FungiTrans.push_back(pP);
	pP = new P(&Fungi_Norgrate, pNumPlants, MYCON, NITROGEN, TRANSPORT, MYCON_NUPTAKE, NORMAL); initv.assign(p_Plant->NumPlants, 0.0008);
	Define(pP, pNumPlants, "Fungi Norg upt rate", initv, "-", "Nitrogen and Carbon >= 1|Mycorrhiza Fungal Growth>0", 0., 0.02); //Unit = gN gdw-1 d-1;
	mTab_FungiTrans.push_back(pP);

	pP = new P(&CP_RatioMaxFungi, pNumPlants, MYCON, ORGANIC, TRANSPORT, MYCON_NUPTAKE, NORMAL); initv.assign(p_Plant->NumPlants, 300.);//HH
	Define(pP, pNumPlants, "Fungi Max CP", initv, "-", "Nitrogen and Carbon >= 1|Mycorrhiza Fungal Growth>0|Phosphorus>0", 75, 1000.);
	mTab_FungiTransP.push_back(pP);
	pP = new P(&CP_RatioMinFungi, pNumPlants, MYCON, ORGANIC, TRANSPORT, MYCON_NUPTAKE, NORMAL); initv.assign(p_Plant->NumPlants, 100.);//HH
	Define(pP, pNumPlants, "Fungi Min CP", initv, "-", "Nitrogen and Carbon >= 1|Mycorrhiza Fungal Growth>0|Phosphorus>0", 75., 250.);
	mTab_FungiTransP.push_back(pP);
	pP = new P(&FungiMinPSupplyCoef, pNumPlants, MYCON, PHOSPHORUS, TRANSPORT, MYCON_NUPTAKE, NORMAL); initv.assign(p_Plant->NumPlants, 0.5);
	Define(pP, pNumPlants, "Fungi MinP MYCON Supply", initv, "-", "Nitrogen and Carbon >= 1|Mycorrhiza Fungal Growth>0|Phosphorus>0", 0.01, 1.);
	mTab_FungiTransP.push_back(pP);
	//HH
	pP = new P(&Fungi_PO4rate, pNumPlants, MYCON, PHOSPHORUS, TRANSPORT, MYCON_NUPTAKE, NORMAL); initv.assign(p_Plant->NumPlants, 0.0002);
	Define(pP, pNumPlants, "Fungi PO4 upt rate", initv, "-", "Nitrogen and Carbon >= 1|Mycorrhiza Fungal Growth>0|Phosphorus>0", 0., 0.01);
	mTab_FungiTransP.push_back(pP);		//Unit = gp gdw-1 d-1;
	pP = new P(&Fungi_Porgrate, pNumPlants, MYCON, PHOSPHORUS, TRANSPORT, MYCON_NUPTAKE, NORMAL); initv.assign(p_Plant->NumPlants, 0.0008);
	Define(pP, pNumPlants, "Fungi Porg upt rate", initv, "-", "Nitrogen and Carbon >= 1|Mycorrhiza Fungal Growth>0|Phosphorus>0", 0., 0.02); //Unit = gN gdw-1 d-1??
	mTab_FungiTransP.push_back(pP);

	pTab = new Tab(&mTab_FungiGrowth, MYCON, ORGANIC, TRANSPORT, REALLOCATION, NORMAL);
	// AM 200911 parameter added to table;
	Define(pTab, "Fungal Growth Regulation", "Allo Max Fungi c1|Allo Fungi c1|Fungi Carbon demand Coef|Resp Coef Fungi|FungiMycRate|FungiMantleRate|Mycelia Frac|FungiRate1|FungiRate2|Myco Degree Fungi Coef|N retention Fungi|N avail Coef|P retention Fungi|P avail Coef",
		"Nitrogen and Carbon > 0|Growth > 0|Mycorrhiza Fungal Growth>0|Phosphorus>0");
	;
	pTab = new Tab(&mTab_FungiTrans, MYCON, ORGANIC, TRANSPORT, REALLOCATION, NORMAL);
	Define(pTab, "Fungal N Transfer and N uptake", "Fungi Max CN|Fungi Min CN|Fungi MinN Plant Supply|Fungi NO3 upt rate|Fungi NH4 upt rate|Fungi Norg upt rate",
		"Nitrogen and Carbon > 0|Growth > 0|Mycorrhiza Fungal Growth>0");
	;
	pTab = new Tab(&mTab_FungiTransP, MYCON, ORGANIC, TRANSPORT, REALLOCATION, NORMAL);
	Define(pTab, "Fungal P Transfer and P uptake", "Fungi Max CP|Fungi Min CP|Fungi MinP Plant Supply|Fungi PO4 upt rate|Fungi Porg upt rate",
		"Nitrogen and Carbon > 0|Growth > 0|Mycorrhiza Fungal Growth>0|Phosphorus>0");

	functorprop.FuncName = MyFunc::FUNGI_LITTERFALL_STATIC; functorprop.Arg = MyArgType::T_D3;
	funcname = "Fungi LitterFall Static";
	p_vec.clear();

	p_vec.push_back(m_pModelMap->GetPPointer("FungiRate1"));


	pFunc = new Func(PLANT, CARBON, NOFPROC, LITTERFALL, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec);

	functorprop.FuncName = MyFunc::FUNGI_LITTERFALL_DYNAMIC; functorprop.Arg = MyArgType::T_D3;
	funcname = "Fungi LitterFall Dynamic";
	p_vec.clear();

	p_vec.push_back(m_pModelMap->GetPPointer("FungiRate1"));
	p_vec.push_back(m_pModelMap->GetPPointer("FungiRate2"));
	p_vec.push_back(m_pModelMap->GetPPointer("RootTsum1"));
	p_vec.push_back(m_pModelMap->GetPPointer("RootTsum2"));
	p_vec.push_back(m_pModelMap->GetPPointer("Dorm_FinalDayNo"));


	pFunc = new Func(PLANT, CARBON, NOFPROC, LITTERFALL, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec);

	return true;
}
void NC_Plant_Functions::Def_Grazing() {

	NE *pNumPlants = m_pModelMap->GetNEPointer("Number of Plants");


	Sw *pSw = new Sw(&Grazing, PLANT, GENERAL, TRANSPORT, HARVEST, NORMAL);
	Define(pSw, "Grazing", 0, "off|Continous|At PG File Specified Days- Static Grazing", "Nitrogen and Carbon > 0");
	pSw = new Sw(&GrazingClosure, PLANT, GENERAL, TRANSPORT, GRAZING, NORMAL);
	Define(pSw, "Grazing Closure", 0, "Growth Estimate from Manure|Manure Estimate from Growth", "Nitrogen and Carbon > 0|Grazing>0");

	Xs *pXs;
	pXs = new Xs(&C_GrazingOrganism, PLANT, NITROGEN, STORAGE, LIVING, NORMAL);
	Define(pXs, "C GrazingOrganism", 0., UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Grazing>0");
	pXs = new Xs(&N_GrazingOrganism, PLANT, NITROGEN, STORAGE, LIVING, NORMAL);
	Define(pXs, "N GrazingOrganism", 0., UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Grazing>0");


	vector<double> initv;
	P *pP = new P(&StubbleLeaf, pNumPlants, PLANT, ORGANIC, TRANSPORT, GRAZING, NORMAL); initv.assign(pNumPlants->GetNEValue(), 15.);
	Define(pP, pNumPlants, "Stubble Leaf", initv, UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Grazing>=3", 0., 100.);
	mTab_Graz.push_back(pP);
	pP = new P(&StubbleStem, pNumPlants, PLANT, ORGANIC, TRANSPORT, GRAZING, NORMAL); initv.assign(pNumPlants->GetNEValue(), 25.);
	Define(pP, pNumPlants, "Stubble Stem", initv, UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Grazing>=3", 0., 500.);
	mTab_Graz.push_back(pP);






	//Define(pP, pNumPlants,  GrazingOfftake	,"Offtake", 75.	,p_Plant->NumPlants, UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Grazing>1",0.,500.)	;
	Ps *pPs = new Ps(&Pot_C_GrazingRate, PLANT, ORGANIC, TRANSPORT, GRAZING, NORMAL);
	Define(pPs, "Pot C GrazingRate", .5 ,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Grazing==1", 0.001, 50., .01, 1.);

	pPs = new Ps(&Max_AvailGrazingFraction, PLANT, ORGANIC, TRANSPORT, GRAZING, NORMAL);
	Define(pPs, "Max Avail Grazing Fraction", .5, "-", "Nitrogen and Carbon >= 1|Grazing>=1", 0.001, 1., .1, .8);
	pPs = new Ps(&GrazingScalingCoef, PLANT, ORGANIC, TRANSPORT, GRAZING, NORMAL);
	Define(pPs, "GrazingRate Scaling Coef", 0., "-", "Nitrogen and Carbon >= 1|Grazing==2", -2., 2., 1., 1.);




	pPs = new Ps(&GrazingOfftake, PLANT, ORGANIC, TRANSPORT, GRAZING, NORMAL);
	Define(pPs, "Offtake", 75.,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Grazing>=13", 0.001, 500.);



	Tab *pTab = new Tab(&mTab_Graz, PLANT, ORGANIC, TRANSPORT, GRAZING, NORMAL);
	Define(pTab, "Grazing of Plants", "Stubble Leaf|Stubble Stem", "Nitrogen and Carbon > 0|Grazing>0");
	
	pPs = new Ps(&GrazingRespFraction, PLANT, ORGANIC, TRANSPORT, GRAZING, NORMAL);
	Define(pPs, "Grazing Resp Fraction", .4, "-", "Nitrogen and Carbon >= 1|Grazing>0", 0.001, .9, 0.3, 0.7);

	pPs = new Ps(&C_GrazingExportFraction, PLANT, ORGANIC, TRANSPORT, GRAZING, NORMAL);
	Define(pPs, "C_Grazing Export Fraction", .1,"-", "Nitrogen and Carbon >= 1|Grazing>0", 0.001, .9, 0.3, 0.7);
	pPs = new Ps(&C_GrazingGrowthFraction, PLANT, ORGANIC, TRANSPORT, GRAZING, NORMAL);
	Define(pPs, "C Grazing Growth Fraction", .05, "-", "Nitrogen and Carbon >= 1|Grazing>0|Grazing Closure=1", 0.00, 1., 0.3, 0.7);
	pPs = new Ps(&C_GrazingManureFraction, PLANT, ORGANIC, TRANSPORT, GRAZING, NORMAL);
	Define(pPs, "C Grazing Manure Fraction", 0.56, "-", "Nitrogen and Carbon >= 1|Grazing>0|Grazing Closure=0", 0.00, 1., 0.3, 0.7);

	pPs = new Ps(&C_GrazingCH4Fraction, PLANT, ORGANIC, TRANSPORT, GRAZING, NORMAL);
	Define(pPs, "C Grazing Methane Fraction", .04, "-", "Nitrogen and Carbon >= 1|Grazing>0", 0.001, .9, 0.3, 0.7);



	pPs = new Ps(&N_GrazingExportFraction, PLANT, ORGANIC, TRANSPORT, GRAZING, NORMAL);
	Define(pPs, "N_Grazing Export Fraction", .1, "-", "Nitrogen and Carbon >= 1|Grazing>0", 0.001, .9, 0.3, 0.7);
	pPs = new Ps(&N_GrazingGrowthFraction, PLANT, ORGANIC, TRANSPORT, GRAZING, NORMAL);
	Define(pPs, "N Grazing Growth Fraction", .1, "-", "Nitrogen and Carbon >= 1|Grazing>0|Grazing Closure=1", 0.0, 1, 0.3, 0.7);
	pPs = new Ps(&N_GrazingManureFraction, PLANT, ORGANIC, TRANSPORT, GRAZING, NORMAL);
	Define(pPs, "N Grazing Manure Fraction", .8, "-", "Nitrogen and Carbon >= 1|Grazing>0|Grazing Closure=0", 0.0, 1., 0.3, 0.7);

	pPs = new Ps(&CN_GrazingOrganic, PLANT, ORGANIC, TRANSPORT, GRAZING, NORMAL);
	Define(pPs, "CN Ratio Grazing Organic Manure", 15., "-", "Nitrogen and Carbon >= 1|Grazing>0|Grazing Closure=0", 5., 30., 10., 20.);

	pPs = new Ps(&N_GrazingManureAmmoniaFraction, PLANT, ORGANIC, TRANSPORT, GRAZING, NORMAL);
	Define(pPs, "Fraction of Direct Ammonia loss Grazing", 0.2, "-", "Nitrogen and Carbon >= 1|Grazing>0", 0., 1., 0.1, .7);



	Ts  *pTs;

	
	pTs = new Ts(&C_GrazingRate, PLANT, ORGANIC, TRANSPORT, GRAZING, NORMAL);
	Define(pTs, "C Grazing Rate", .4,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Grazing>0");
	pTs = new Ts(&N_GrazingRate, PLANT, ORGANIC, TRANSPORT, GRAZING, NORMAL);
	Define(pTs, "N Grazing Rate", .4,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Grazing>0");
	
	pTs = new Ts(&GrazingRespiration, PLANT, ORGANIC, TRANSPORT, GRAZING, NORMAL);
	Define(pTs, "Grazing Respiration", .4,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Grazing>0");

	pTs = new Ts(&C_GrazingGrowth, PLANT, ORGANIC, TRANSPORT, GRAZING, NORMAL);
	Define(pTs, "C Grazing Growth", .4,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Grazing>0");
	pTs = new Ts(&N_GrazingGrowth, PLANT, ORGANIC, TRANSPORT, GRAZING, NORMAL);
	Define(pTs, "N Grazing Growth", .4,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Grazing>0");
	
	pTs = new Ts(&C_GrazingExport, PLANT, ORGANIC, TRANSPORT, GRAZING, NORMAL);
	Define(pTs, "C Grazing Export", .4,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Grazing>0");
	pTs = new Ts(&C_GrazingMethaneEmission, PLANT, ORGANIC, TRANSPORT, GRAZING, NORMAL);
	Define(pTs, "C Grazing Methane emission", .0,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Grazing>0");


	pTs = new Ts(&N_GrazingExport, PLANT, ORGANIC, TRANSPORT, GRAZING, NORMAL);
	Define(pTs, "N Grazing Export", .4,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Grazing>0");

	pTs = new Ts(&C_GrazingManure, PLANT, ORGANIC, TRANSPORT, GRAZING, NORMAL);
	Define(pTs, "C Grazing Manure", .4,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Grazing>0");
	pTs = new Ts(&N_GrazingManure, PLANT, ORGANIC, TRANSPORT, GRAZING, NORMAL);
	Define(pTs, "N Grazing Manure", .4,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Grazing>0");

	pTs = new Ts(&N_GrazingFaeces, PLANT, ORGANIC, TRANSPORT, GRAZING, NORMAL);
	Define(pTs, "N Grazing Faeces", .4,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Grazing>0");
	pTs = new Ts(&N_GrazingAmmonia, PLANT, ORGANIC, TRANSPORT, GRAZING, NORMAL);
	Define(pTs, "N Grazing Ammonia", .4,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Grazing>0");
	pTs = new Ts(&N_GrazingAmmonium, PLANT, ORGANIC, TRANSPORT, GRAZING, NORMAL);
	Define(pTs, "N Grazing Ammonium", .4,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Grazing>0");

	T  *pT;
	pT = new T(&CLeafGrazing, pNumPlants, PLANT, ORGANIC, TRANSPORT, GRAZING, NORMAL);
	initv.assign(pNumPlants->GetNEValue(), 0.);
	Define(pT, pNumPlants, "C LeafGrazing Rate",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Grazing>0");
	pT = new T(&CStemGrazing, pNumPlants, PLANT, ORGANIC, TRANSPORT, GRAZING, NORMAL);
	Define(pT, pNumPlants, "C StemGrazing Rate", initv, UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Grazing>0");
	pT = new T(&COldLeafGrazing, pNumPlants, PLANT, ORGANIC, TRANSPORT, GRAZING, NORMAL);
	Define(pT, pNumPlants, "C OldLeafGrazing Rate", initv, UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Grazing>0");

	pT = new T(&NLeafGrazing, pNumPlants, PLANT, ORGANIC, TRANSPORT, GRAZING, NORMAL);
	Define(pT, pNumPlants, "N LeafGrazing Rate", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Grazing>0");
	pT = new T(&NStemGrazing, pNumPlants, PLANT, ORGANIC, TRANSPORT, GRAZING, NORMAL);
	Define(pT, pNumPlants, "N StemGrazing Rate", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Grazing>0");
	pT = new T(&NOldLeafGrazing, pNumPlants, PLANT, ORGANIC, TRANSPORT, GRAZING, NORMAL);
	Define(pT, pNumPlants, "N OldLeafGrazing Rate", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Grazing>0");



	F *pF = new F(&m_PG_Grazing, PLANT, WATER, FYS_SHAPE, HARVEST, NORMAL);
	Define(pF, "Grazing Periods", "Grazing.bin", "Grazing =2");


}
void NC_Plant_Functions::Def_WoodEating() {

	NE *pNumPlants = m_pModelMap->GetNEPointer("Number of Plants");

	Sw *pSw = new Sw(&WoodEating, PLANT, GENERAL, TRANSPORT, WOODEATING, NORMAL);
	Define(pSw, "Wood Eating", 0, "off|on", "Nitrogen and Carbon > 0");
	
	Xs *pXs;
	pXs = new Xs(&C_WoodEater, PLANT, NITROGEN, STORAGE, WOODEATING, NORMAL);
	Define(pXs, "C Wood Eater", 0., UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Wood Eating>0");
	pXs = new Xs(&N_WoodEater, PLANT, NITROGEN, STORAGE, WOODEATING, NORMAL);
	Define(pXs, "N Wood Eater", 0., UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Wood Eating>0");


	Ps *pPs;
	pPs = new Ps(&WoodEaterRateCoef, PLANT, ORGANIC, TRANSPORT, WOODEATING, NORMAL);
	Define(pPs, "Max Wood Eater Rate Coef", .001,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Wood Eating>0", 0.0, 0.01, 1.E-5, 1.E-3);
	pPs = new Ps(&WoodEatingStartAge, PLANT, ORGANIC, TRANSPORT, WOODEATING, NORMAL);
	Define(pPs, "Wood Eating Start Age", 40., UNIT_TYPES::YEAR_AGE_UNIT, "Nitrogen and Carbon >= 1|Wood Eating>0", 0.0, 1000., 5., 100.);
	pPs = new Ps(&WoodEatingStartPeriod, PLANT, ORGANIC, TRANSPORT, WOODEATING, NORMAL);
	Define(pPs, "Wood Eating Start Period", 2., UNIT_TYPES::YEAR_AGE_UNIT, "Nitrogen and Carbon >= 1|Wood Eating>0", 0.0, 1000., 1., 10.);
	pPs = new Ps(&WoodEatingOptPeriod, PLANT, ORGANIC, TRANSPORT, WOODEATING, NORMAL);
	Define(pPs, "Wood Eating Opt Period", 2., UNIT_TYPES::YEAR_AGE_UNIT, "Nitrogen and Carbon >= 1|Wood Eating>0", 0.0, 1000., 1., 10.);
	pPs = new Ps(&WoodEatingEndPeriod, PLANT, ORGANIC, TRANSPORT, WOODEATING, NORMAL);
	Define(pPs, "Wood Eating End Period", 2., UNIT_TYPES::YEAR_AGE_UNIT, "Nitrogen and Carbon >= 1|Wood Eating>0", 0.0, 1000., 1., 10.);


	pPs = new Ps(&WoodEaterEfficiency, PLANT, ORGANIC, TRANSPORT, WOODEATING, NORMAL);
	Define(pPs, "Wood Eater Efficiency", 0.2, "-","Nitrogen and Carbon >= 1|Wood Eating>0", 0.01, 0.5, 0.1, .5);

	pPs = new Ps(&WoodEaterFaecesRateCoef, PLANT, ORGANIC, TRANSPORT, WOODEATING, NORMAL);
	Define(pPs, "Wood Eater Faeces Rate Coef", 0.001, "-", "Nitrogen and Carbon >= 1|Wood Eating>0", 0.00001, 0.5, 0.001, .5);

	pPs = new Ps(&WoodEaterAmmoniumRateCoef, PLANT, ORGANIC, TRANSPORT, WOODEATING, NORMAL);
	Define(pPs, "Wood Eater Ammonium Rate Coef", 0.01, "-", "Nitrogen and Carbon >= 1|Wood Eating>0", 0.00001, 0.5, 0.001, .5);

	pPs = new Ps(&WoodEaterRespRateCoef, PLANT, ORGANIC, TRANSPORT, WOODEATING, NORMAL);
	Define(pPs, "Wood Eater Resp Rate Coef", 0.001, "-", "Nitrogen and Carbon >= 1|Wood Eating>0", 0.00001, 0.5, 0.001, .05);



	Ts  *pTs;
	pTs = new Ts(&C_WoodEatingRate, PLANT, ORGANIC, TRANSPORT, WOODEATING, NORMAL);
	Define(pTs, "C Wood Eating Rate", .4,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Wood Eating>0");
	pTs = new Ts(&N_WoodEatingRate, PLANT, ORGANIC, TRANSPORT, WOODEATING, NORMAL);
	Define(pTs, "N Wood Eating Rate", .4,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1||Wood Eating>0");

	pTs = new Ts(&WoodEatingRespiration, PLANT, ORGANIC, TRANSPORT, WOODEATING, NORMAL);
	Define(pTs, "Wood Eating Respiration", .4,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Wood Eating>0");

	pTs = new Ts(&C_WoodEaterFaecesFlux, PLANT, ORGANIC, TRANSPORT, WOODEATING, NORMAL);
	Define(pTs, "C_Wood Eater Faeces Flux", .4,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Wood Eating>0");
	
	pTs = new Ts(&N_WoodEaterFaecesFlux, PLANT, ORGANIC, TRANSPORT, WOODEATING, NORMAL);
	Define(pTs, "N_Wood Eater Faeces Flux", .4,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Wood Eating>0");

	T  *pT;
	vector<double> initv;
	initv.assign(pNumPlants->GetNEValue(), 0.);
	pT = new T(&COldStemEating, pNumPlants, PLANT, ORGANIC, TRANSPORT, WOODEATING, NORMAL);
	Define(pT, pNumPlants, "C OldStemEating Rate", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Wood Eating>0");
	pT = new T(&COldCoarseRootEating, pNumPlants, PLANT, ORGANIC, TRANSPORT, WOODEATING, NORMAL);
	Define(pT, pNumPlants, "C OldCoarseRootEating", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Wood Eating>0");
	
	pT = new T(&NOldStemEating, pNumPlants, PLANT, ORGANIC, TRANSPORT, WOODEATING, NORMAL);
	Define(pT, pNumPlants, "N OldStemEating Rate", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Wood Eating>0");
	pT = new T(&NOldCoarseRootEating, pNumPlants, PLANT, ORGANIC, TRANSPORT, WOODEATING, NORMAL);
	Define(pT, pNumPlants, "N OldCoarseRootEating", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Wood Eating>0");
}

void NC_Plant_Functions::Def_NFixation()
{
	Func *pFunc;
	SoilModel* pSoil = (SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
	p_Soil_WaterF = &pSoil->m_SoilFrost;
	PlantModel* pPlant = (PlantModel*)m_pModelInfo->GetModelPtr("Plant Model");
	WaterUptake* p_Plant = &pPlant->m_WaterUptake;
	NE *pNL = m_pModelMap->GetNEPointer("Number of Layers");
	NE *pNumPlants = m_pModelMap->GetNEPointer("Number of Plants");

	// Parameters;
	string funcname;
	FunctorProp functorprop;
	vector<Ps*> ps_vec;
	vector<P*> p_vec;
	ps_vec.clear();
	vector<double> initv;	initv.assign(p_Plant->NumPlants, 0.);
	Ps *pPs; P *pP;
	Tab *pTab;

	functorprop.FunctorInitOption = string::npos;
	functorprop.pModelInfo = m_pModelInfo;


	pP = new P(&UptakeFracCoefDef, pNumPlants, PLANT, ORGANIC, TRANSPORT, NFIXATION, NORMAL); initv.assign(p_Plant->NumPlants, 0.8);
	Define(pP, pNumPlants, "NFixCoef", initv, "-", "Nitrogen and Carbon >= 1|N fixation=1", 0., 1.);
	mTab_NFix.push_back(pP);
	pP = new P(&Root_NoduleRatio, pNumPlants, PLANT, ORGANIC, TRANSPORT, NFIXATION, NORMAL); initv.assign(p_Plant->NumPlants, 6.);
	Define(pP, pNumPlants, "Root Nodule Ratio", initv, "-", "Nitrogen and Carbon >= 1|N fixation=2", 0., 1.);
	mTab_NFix.push_back(pP);
	pP = new P(&StemLeaf_NoduleRatio, pNumPlants, PLANT, ORGANIC, TRANSPORT, NFIXATION, NORMAL); initv.assign(p_Plant->NumPlants, 60.);
	Define(pP, pNumPlants, "StemLeaf Nodule Ratio", initv, "-", "Nitrogen and Carbon >= 1|N fixation=3", 0., 1.);
	mTab_NFix.push_back(pP);
	functorprop.FuncName = MyFunc::NFIXATION_RESPONSE_MIN_SOIL; functorprop.Arg = MyArgType::T_D1;
	funcname = "Nitrogen Fixation Response to Mineral N ";
	p_vec.clear();

	pP = new P(&SpecificMaxNFixation, pNumPlants, PLANT, ORGANIC, TRANSPORT, NFIXATION, NORMAL); initv.assign(p_Plant->NumPlants, 73.E-3);
	Define(pP, pNumPlants, "Specific Max Fixation", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|N fixation>1", 0., 500.);
	mTab_NFix.push_back(pP); p_vec.push_back(pP);
	pP = new P(&SpecificMinNFixation, pNumPlants, PLANT, ORGANIC, TRANSPORT, NFIXATION, NORMAL); initv.assign(p_Plant->NumPlants, 17E-3);
	Define(pP, pNumPlants, "Specific Min Fixation", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|N fixation>1", 0., 500.);
	mTab_NFix.push_back(pP); p_vec.push_back(pP);
	pP = new P(&NFixationSensitivity, pNumPlants, PLANT, ORGANIC, TRANSPORT, NFIXATION, NORMAL); initv.assign(p_Plant->NumPlants, -.8);
	Define(pP, pNumPlants, "Specific N sensivity", initv, "-", "Nitrogen and Carbon >= 1|N fixation>1", -100., 0.);
	mTab_NFix.push_back(pP); p_vec.push_back(pP);
	pFunc = new Func(PLANT, CARBON, NOFPROC, NFIXATION, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec);


	pTab = new Tab(&mTab_NFix, PLANT, NITROGEN, TRANSPORT, NFIXATION, NORMAL);
	Define(pTab, "Nitrogen Fixation", "NFixCoef|Root Nodule Ratio|StemLeaf Nodule Ratio|Specific Max Fixation|Specific Min Fixation|Specific N sensivity", "Nitrogen and Carbon > 0|Growth > 0|N fixation>0");
	;


}

bool NC_Plant_Functions::FunctionDef() {
	Func *pFunc;

	
	double (NC_Plant_Functions::*fpoint1)(size_t, double);
	double (NC_Plant_Functions::*fpoint2)(size_t, double, double);
	




	return true;
}

double NC_Plant_Functions::LogisticGrowth(size_t index, double Day) {
	
	double TimePeriod=Day-UptakeStart[index];
	if(TimePeriod>0.&&Day<UptakeEnd[index]) {
		return UptakeACoef[index]*(UptakeACoef[index]-UptakeBCoef[index])/UptakeBCoef[index]	
			*UptakeCCoef[index]*exp(-TimePeriod*UptakeCCoef[index])										
				/pow(1+(UptakeACoef[index]-UptakeBCoef[index])/UptakeBCoef[index]*exp(-UptakeCCoef[index]*TimePeriod),2);
	}
	else
		return 0.;
}

double NC_Plant_Functions::AlloFromRootToLeaf(size_t index, double mass_root, double mass_leaf) {
     if((mass_root+0.01)/(mass_leaf+0.01)>AlloFromRootLeafRatio[index]) 
             return AlloFromRootLeafCoef[index]*(mass_root-mass_leaf*AlloFromRootLeafRatio[index]);
     else 
			return 0.;
}

double NC_Plant_Functions::LogisticGrowthRateFunc(size_t index, double JDay) {
	double  TimePeriod;
	bool WithinPeriod;
	
	TimePeriod=JDay-UptakeStart[index];
	if(TimePeriod>0&&JDay<UptakeEnd[index]) 
		WithinPeriod=true;
	else
		WithinPeriod=false;
	
	
	if(UptakeEnd[index]<UptakeStart[index]&&JDay<UptakeEnd[index]&&TimePeriod<=0.) {
		TimePeriod=TimePeriod+365.;
		WithinPeriod=true;
	}
	else if(UptakeEnd[index]<UptakeStart[index]&&JDay>UptakeStart[index]&&TimePeriod>0.) 
		WithinPeriod=true;
	

	if(WithinPeriod) {
 		return UptakeACoef[index]*(UptakeACoef[index]-UptakeBCoef[index])/UptakeBCoef[index]	
			*UptakeCCoef[index]*exp(-TimePeriod*UptakeCCoef[index])										  
				/pow(1+(UptakeACoef[index]-UptakeBCoef[index])/UptakeBCoef[index]  
				*exp(-UptakeCCoef[index]*TimePeriod),2.);
	}
	else
		return 0.;
}





double NC_Plant_Functions::CN_RatioLeafFixed(size_t index,double Response) {
	
	return CNLeafThres[index]+Response*(CNLeafOpt[index]-CNLeafThres[index]) ;
}


double NC_Plant_Functions::PhoSoilWaterResponse(double EtaEtpRatio) {

	return EtaEtpRatio;
}
double NC_Plant_Functions::WoodEatingRateCoefFunction(double timediff) {

	if (timediff < WoodEatingStartPeriod)
		return timediff / WoodEatingStartPeriod* WoodEaterRateCoef;
	else if(timediff<( WoodEatingStartPeriod+ WoodEatingOptPeriod))
		return WoodEaterRateCoef;
	else if (timediff < (WoodEatingStartPeriod + WoodEatingOptPeriod + WoodEatingEndPeriod))
		return (1. - (timediff - WoodEatingStartPeriod - WoodEatingOptPeriod) / WoodEatingEndPeriod)* WoodEaterRateCoef;
	else
		return 0.;

}

double NC_Plant_Functions::CAlloFungi(size_t index) {
  
    return AlloFungiC1[index];
}

double NC_Plant_Functions::CAlloMaxFungi(size_t index) {
    
    return AlloMaxC1[index];
    
}






double NC_Plant_Functions::CAlloRootNitrogen(size_t index, double CN_RatioLeaf) {
	//Integer index
	//Real	CN_RatioLeaf

	if(Root_Alloc_N_Leaf==0) 
		return AlloRoot_CN1[index];
	else if(Root_Alloc_N_Leaf==1) 
		return AlloRoot_CN1[index]+AlloRoot_CN2[index]*f_PhotoNLeaf(index,CN_RatioLeaf);
	else
		return AlloRoot_CN1[index]+AlloRoot_CN2[index]*exp(AlloRoot_CN3[index]*f_PhotoNLeaf(index,CN_RatioLeaf));
}


double NC_Plant_Functions::CAlloRootWater(size_t index, double EtRatio) {

	if(Root_Alloc_WaterStress==0) 
		return AlloRootWater1[index];
	else if(Root_Alloc_WaterStress==1) 
		return AlloRootWater1[index]+AlloRootWater2[index]*EtRatio;
	else
		return AlloRootWater1[index]+ AlloRootWater2[index]*exp(AlloRootWater3[index]*EtRatio);
}



double NC_Plant_Functions::CAlloRootMass(size_t index,double Mass) {
	if(Root_Alloc_TotMass==0) 
		return AlloRootTotMass1[index];
	else if(Root_Alloc_TotMass==1) 
		return AlloRootTotMass1[index]+AlloRootTotMass2[index]*Mass;
	else
		return AlloRootTotMass1[index]+ AlloRootTotMass2[index]*exp(AlloRootTotMass3[index]*Mass);
}


double NC_Plant_Functions::PhoRadEffResponseL(size_t index) {
	//Integer index
	double	GrainFillingStage;
	if(Growth==4) {
		if(Max_GSI[index]>2) {
			GrainFillingStage=max(0., min(GrowthStageIndex[index]-2.,1.));
			return PhoRadEffResponse(index, GrainFillingStage)*Fix::BiomassToCarbon;
		}
		else
			return QEff[index]*Fix::BiomassToCarbon;
	}
	else {
		if(Max_GSI[index]>2) {
			GrainFillingStage=max(0., min(GrowthStageIndex[index]-2.,1.));
			return PhoRadEffResponse(index, GrainFillingStage)*Fix::BiomassToCarbon;
		}
		else
			return PhoRadEfficiency[index]*Fix::BiomassToCarbon;
		

	}
}


double NC_Plant_Functions::PhoRadEffResponse(size_t index, double GrainFilling) {

	if(Growth==4) 
		return QEff[index]*PhoRadGrainFillResponse(index, GrainFilling);
	else
		return PhoRadEfficiency[index]*PhoRadGrainFillResponse(index, GrainFilling) ;
}
double NC_Plant_Functions::PhoRadGrainFillResponse(size_t index, double GrainFilling) {

		return max(0.,(1.-(PhoRadEff_Reduc[index]/100.)*GrainFilling));
}


double NC_Plant_Functions::PhoSatResponseL(size_t index) {

	if(PhoSaturation) 
		return PhoSatResponse(index, PhoRadEffResponseL(index)*RadPlantAdsorbed[index]);
	else
		return PhoRadEffResponseL(index)*RadPlantAdsorbed[index];
}

double NC_Plant_Functions::PhoSatResponse(size_t index, double PhoLevel) {
	
	return PhoMax[index]*(1-exp(-PhoLevel/PhoMax[index]));
}

																										    
double NC_Plant_Functions::CN_MinLeafFunc(size_t i) {
	if(N_Fixed_Supply>0) 
        return CN_RatioMinLeaf[i];
	else
		return CN_RatioMinLeaf[i];
}
//	else											   
//		CN_MinLeafFunc=1*CN_RatioMinLeaf[i]
//	}
double NC_Plant_Functions::CP_MinLeafFunc(size_t i) {
			return CP_RatioMinLeaf[i];
	}

double NC_Plant_Functions::CN_MinStemFunc(size_t i) {
//	Integer index
//	if(N_Demand_dynamics==0) {
		return CN_RatioMinStem[i];
//	else
//		CN_MinStemFunc=1*CN_RatioMinStem[i]
//	}
}

double NC_Plant_Functions::CP_MinStemFunc(size_t i) {
	//	Integer index
	//	if(N_Demand_dynamics==0) {
	return CP_RatioMinStem[i];
}

double NC_Plant_Functions::CN_MinRootsFunc(size_t index) {
	//if(N_Demand_Dynamics==0) 
		return 1.*CN_RatioMinRoots[index];
}
//	else
//		CN_MinRootsFunc=1.*CN_RatioMinRoots[index]
//	}

double NC_Plant_Functions::CP_MinRootsFunc(size_t index) {
	//if(N_Demand_Dynamics==0) 
	return 1.*CP_RatioMinRoots[index];
}

double NC_Plant_Functions::CN_MinCoarseRootsFunc(size_t index) {
	
//	if(N_Demand_dynamics==0) {
		return 1.*CN_RatioMinCoarseRoots[index];
//	else
//		CN_MinRootsFunc=1.*CN_RatioMinRoots[index]
//	}
}

double NC_Plant_Functions::CP_MinCoarseRootsFunc(size_t index) {

	//	if(N_Demand_dynamics==0) {
	return 1.*CP_RatioMinCoarseRoots[index];
	//	else
	//		CN_MinRootsFunc=1.*CN_RatioMinRoots[index]
	//	}
}

  

bool NC_Plant_Functions::KillAboveGroundPlant(size_t index) {
	for (size_t jj = 0;jj<m_pModelStructure->m_NumActPlantElements; jj++) {
		for (size_t j = 0; j <= _OLD_STEM; j++) P_LitterFall[j][jj][index] = P_State[j][jj][index] / T_Step;
		if (jj == _C_) for (size_t j = 0; j <= _OLD_STEM; j++) P_Respiration[j][index] = 0.;
	}

	//AtmNewMobile[index]=0.;
	if(P_State[_GRAIN][_C_][index]>0.||P_State[_STEM][_C_][index]>0.||P_State[_LEAF][_C_][index]>0.) 
		return true;
	else
		return false;
}


bool NC_Plant_Functions::KillBelowGroundPlant(size_t index)	{
	for (size_t jj = 0; jj< m_pModelStructure->m_NumActPlantElements; jj++) {
		for (size_t j = _ROOT; j <= _OLD_COARSE_ROOT; j++) P_LitterFall[j][jj][index] = P_State[j][jj][index] / T_Step;
		if (jj == _C_) for (size_t j = _ROOT; j <= _OLD_COARSE_ROOT; j++) P_Respiration[j][index] = 0.;
	}
	GrowthStageIndex[index]=Min_GSI[index];
	SimPlantAge[index]=0.;

	if(P_State[_ROOT][_C_][index]>0.||P_State[_OLD_ROOT][_C_][index]>0.) 
		return true;
	else
		return false;
}




// Farquhar
GRespons NC_Plant_Functions::P_Responses(size_t index) {
		GRespons out;
	  	out.Temp=f_PhotoTemp(index,pAtm->MC_TempCanopy[index], GrowthTempSum[index])*TempResponseCurrent[index]+
			(1-TempResponseCurrent[index])*GPP_Current.Temp[index];
		if(N_Fixed_Supply>0) 
				out.N=N_MinDegreeSupply[index];
		else
				out.N= f_PhotoNLeaf(index,P_State_CarbonRatio[_LEAF_][_N_-1][index]);
		
		out.W=PhoSoilWaterResponse(NDrivWaterStress[index]);
		return out;
}

double NC_Plant_Functions::P_PhoSiB(size_t index) {
		
		Photo Sib;
		Sib=PhoSib(index);
		PRubOut[index]=Sib.Rubisco;
		PLightOut[index]=Sib.Light;
		PSinkOut[index]=Sib.Sink;
		return Sib.Gross*Fix::MOL_C*86400;	// Enhet: gC / m2 / day

}


double NC_Plant_Functions::PMinFunc(double P_Rub, double P_light,double  P_Sink, double Atheta, double Btheta) {
		
		double in, P_RubLight;

		in=max(0., pow(P_Rub+P_light,2)-4*Atheta*P_Rub*P_light);
		P_RubLight=( ( P_light+P_Rub)- sqrt(in))/(2.*Atheta);
	   
		in=max(0., pow(P_Sink+P_RubLight,2)-4*Btheta*P_Sink*P_RubLight);

		return max(0., ( (P_Sink+P_RubLight)- sqrt(in) )/(2.*Btheta) );
}

		//PminFunc=Max(0., Min(P_Rub, P_Light, P_Sink))




double NC_Plant_Functions::MMO2(double Qtemp) {	//Michaelis-Menten


	   double Kc=30*pow(2.1,Qtemp);
	   double Ko2=30000*pow(1.2,Qtemp);
	   
	   return Kc*(1+Fix::PO2/Ko2);
}


double NC_Plant_Functions::P_RubiscolimitedC3(double Vmax, double Pco2, double Gamma, double MMO2) {
	   
	   return max(0.,Vmax*(Pco2-Gamma)/(Pco2+MMO2)	);
}

double NC_Plant_Functions::P_RubiscolimitedC4(double Vmax) {

	   return Vmax;
}


double NC_Plant_Functions::P_LightLimitedC3(double Jm,double  Pco2, double Gamma) {
	   

	   return max(0., Jm*(Pco2-Gamma)/(Pco2+2*Gamma));		// Enhet: mol / m2 / s
}
 

double NC_Plant_Functions::P_LightLimitedC4(double Jm) {

	   return Jm;		// Enhet: mol / m2 / s
}


double NC_Plant_Functions::P_SinkLimitedC3(double Vmax) {
	
	return Vmax/2.0;
}


double NC_Plant_Functions::P_SinkLimitedC4(double Vmax) {   
	   return Vmax*2.e4;
}


double NC_Plant_Functions::P_SinkLimitedC4_CO2(double PSink1,double Pco2) {
	   
	   return PSink1*Pco2/P_Surface;

}


double NC_Plant_Functions::TP_Corr() {

	return Fix::ABS_ZERO*P_Surface/Fix::NORM_ATMPRES;	// Degrees kelvin
}


double NC_Plant_Functions::CasCo2cap(double displ,double  tpcorr,double TempAir) {
	
	return max(4.0, displ) * Fix::MOL_VOL_AIR * tpcorr / (TempAir + Fix::ABS_ZERO);		// Moles air / m2

}
double NC_Plant_Functions::GBoundaryFunc(double RBoundary, double TP, double Temp) {
// Conversion from s/m to mol/m2*sec
	
	return .5/RBoundary*Fix::MOL_VOL_AIR*TP/(Temp+Fix::ABS_ZERO);

}

double NC_Plant_Functions::GStomataFunc(double RStomata,double TP,double  Temp, double WStress) {
// Conversion from s/m to mol/m2*sec
//	Real RStomata, TP, Temp, Wstress

	return max(1.E-4, 1./max(1., RStomata)*Fix::MOL_VOL_AIR*TP/(Temp+Fix::ABS_ZERO)*WStress);
}


double NC_Plant_Functions::GAirFunc(double ResAir,double  TP, double Temp) {
	//Use PotTranspiration, only	: Lohammar_L
	//Real TP, Temp, ResAir
//	Integer index

//	cond=(1/(Lohammar_L[index]*LeafAreaFunctionL[index]))*NDrivWaterStress[index]
	// Kanske bör vi ej multiplicera med LAI om vi räknar per LA?

	return 1./max(1., ResAir )*Fix::MOL_VOL_AIR*TP/(Temp+Fix::ABS_ZERO); // Conversion from s/m to mol/m2*sec
}
//C===================SUBROUTINE SORTIN===================================
//C
void NC_Plant_Functions:: SortIn(vector<double> FarqRange, vector<double> Gammas, size_t i_step, size_t NumPlants, size_t index) {
//C
//C=======================================================================
//C
//C     ARRANGES SUCCESSIVE PCO2/ERROR PAIRS IN ORDER OF INCREASING PCO2.
//C       ESTIMATES NEXT GUESS FOR PCO2 USING COMBINATION OF LINEAR AND
//C       QUADRATIC FITS.
//C
//C=======================================================================
//C

//c     work arrays

      vector<double> eyyi1,eyyi2,eyyi3,eyyis,
          eyyisp,pco2yis,pco2yisp,
          pco2x,pco2yi1,pco2yi2,pco2yi3;

	 	eyyi1.resize(NumPlants);eyyi2.resize(NumPlants);eyyi3.resize(NumPlants);eyyis.resize(NumPlants);
          eyyisp.resize(NumPlants);pco2yis.resize(NumPlants);pco2yisp.resize(NumPlants);
          pco2x.resize(NumPlants);pco2yi1.resize(NumPlants);pco2yi2.resize(NumPlants);pco2yi3.resize(NumPlants);
	 
	

      double     pco2yl;
      vector<size_t> is; is.resize(NumPlants);
      vector<bool> bitx; bitx.resize(NumPlants);


      //integer i, ix, i1, i2, i3, isp, n, l, j, index

//C

      if( i_step< 3 ) {
		  // First CO2
          PCo2y[index][0] = Gammas[index] + 0.5*FarqRange[index];

		  // Second CO2
		  PCo2y[index][1] = Gammas[index] + FarqRange[index]*0.5;
		  if(EYY[index][0]<0.) 
			  PCo2y[index][1] +=0.3*FarqRange[index];
		  else
			  PCo2y[index][1] +=0.3*FarqRange[index];
		  // Set 3rd CO2
          PCo2y[index][2] = PCo2y[index][0]- (PCo2y[index][0]-PCo2y[index][1]) 
                      /(EYY[index][0]-EYY[index][1]+1.E-10)*EYY[index][0];

          if ( min(   EYY[index][0],   EYY[index][1] ) > 0. && PCo2y[index][1]> min( PCo2y[index][0], PCo2y[index][1]) )
			  PCo2y[index][2] = Gammas[index];
	  }
      else {


          if(abs(EYY[index][i_step-1])<0.1)
			  // Set Current Value to the previous step
			  PCo2y[index][i_step] = PCo2y[index][i_step-1];

          else {
			// Estimate current value by previos errors and CO2
			//EYY[index][i_step] =EYY[index][i_step-1] ;
			PCo2y[index][i_step] =PCo2y[index][i_step-1];

			for(size_t J=2;J<i_step+1; J++) {
              double A = EYY[index][J-1];
              double B = PCo2y[index][J-1];

			  for(int i=J-1;i>0;i--) {	
                if(EYY[index][i-1] <= A ) {
					EYY[index][i] = A;
					PCo2y[index][i] = B;					
					break;	
				}
                EYY[index][i] = EYY[index][i-1];
                PCo2y[index][i] = PCo2y[index][i-1];
			  }

			}
			pco2x[index] = 0.;
			double v_x=0.;
			size_t i_x=1;

			for(size_t i=0; i<i_step; i++) {
              if( EYY[index][i] < 0. )  {
                pco2x[index] = PCo2y[index][i];
				v_x=PCo2y[index][i];
				i_x=i;
               }
			}
				size_t i_1 = min(i_step-2, max(size_t(0),i_x));
				auto  i_sp   = min(i_step, i_x + 1);
//
				i_x = i_sp - 1;
				
				eyyisp[index] =EYY[index][i_sp];
				eyyis[index] = EYY[index][i_x];
				
				eyyi1[index] = EYY[index][i_1];
				eyyi2[index] = EYY[index][i_1+1];
				eyyi3[index] = EYY[index][i_1+2];
				
				pco2yisp[index] = PCo2y[index][i_sp];
				pco2yis[index] = PCo2y[index][i_x];
				
				pco2yi1[index] = PCo2y[index][i_1];
				pco2yi2[index] = PCo2y[index][i_1+1];
				pco2yi3[index] = PCo2y[index][i_1+2];

 //       enddo

 //       do index = 1,NumPlants


//citb...Neil Suits' patch to check for zero in the denominator...
            if(eyyis[index] != eyyisp[index])
               pco2yl=pco2yis[index] - (pco2yis[index]-pco2yisp[index]) /(eyyis[index]-eyyisp[index])*eyyis[index];
            else
               pco2yl = pco2yis[index] * 1.01;
            
//C
//C   METHOD USING A QUADRATIC FIT
//C
           double AC1 = eyyi1[index]*eyyi1[index] - eyyi2[index]*eyyi2[index];
           double AC2 = eyyi2[index]*eyyi2[index] - eyyi3[index]*eyyi3[index];
           double BC1 = eyyi1[index] - eyyi2[index];
           double BC2 = eyyi2[index] - eyyi3[index];
           double CC1 = pco2yi1[index] - pco2yi2[index];
           double CC2 = pco2yi2[index] - pco2yi3[index];

//citb...Neil Suits' patch to prevent zero in denominator...
            if(BC1*AC2-AC1*BC2 != 0.0 && AC1 != 0.0){
               double BTERM = (CC1*AC2-CC2*AC1)/(BC1*AC2-AC1*BC2);
               double ATERM = (CC1-BC1*BTERM)/AC1;
               double CTERM = pco2yi2[index] -ATERM*eyyi2[index]*eyyi2[index]-BTERM*eyyi2[index];
               double PCO2YQ= CTERM;
               PCO2YQ= max( PCO2YQ, pco2x[index] );
               PCo2y[index][i_step] = ( pco2yl+PCO2YQ)/2.;
			}
            else
               PCo2y[index][i_step] = PCo2y[index][i_step]* 1.01;
            


      }
 //     do i = 1,NumPlants
        PCo2y[index][i_step]= max(PCo2y[index][i_step],0.01);
 //     enddo
	  }
      return;
}


// Farquhar
Photo NC_Plant_Functions:: PhoSib(size_t index) {
	SoilOrgModel* p_Soil=(SoilOrgModel*)m_pModelInfo->GetModelPtr("Soil Org Model");
	NC_Organic* pNCSoil=&p_Soil->m_NC_SoilOrg;
		Photo out;
		//Use Physconst, only	:	Po2

		//Real P_Rub, P_Light, P_Sink, P_Sink1, Photo_G, Photo_N
		
		double PCO2A, PCO2IN, Co2b, pCo2Bp;
		
		double  TPCorr, co2cap, RespSoil, RespMPlant, RootMRespiration;



		TPCorr=TP_Corr();	//   	Korrektion av fryspunkten beroende av förhållandet mellan aktuellt och normalt lufttryck
		co2cap=CasCo2cap(pPlant->MC_DisplVar[index], TPCorr, NDrivTAir);		// mol air / m2
		RespSoil=pNCSoil->CTotSoilRespRate/Fix::MOL_C/86400;		// konvertering till mol/m2/s

		RootMRespiration = P_Respiration[_ROOT][index] + P_Respiration[_OLD_ROOT][index];
		RespMPlant = (P_Respiration[_LEAF][index] + P_Respiration[_STEM][index] + P_Respiration[_OLD_LEAF][index] +
			P_Respiration[_OLD_LEAF][index] + P_Respiration[_GRAIN][index] + RootMRespiration) / Fix::MOL_C / 86400;
		
		GBoundary[index]=GBoundaryFunc(RBoundary[index], TPCorr, pAtm->MC_TempCanopy[index]);
		GStomata[index]=GStomataFunc(pPlant->MC_ResSurfVar[index], TPCorr, pAtm->MC_TempCanopy[index], GPP_Current.Water[index]);
		GAir[index]=GAirFunc(pPlant->MC_ResAir[index], TPCorr, pAtm->MC_TempCanopy[index]);   

		Qt[index]=0.1*(pAtm->MC_TempCanopy[index]-25.0);
		if(RadInterception>=1) 
			Vmax[index]=1.e-6*Vcmax[index]*(1-exp(-pAtm->RntLAIv[index]*pPlant->LAI[index]))/pAtm->RntLAIv[index];	// Enhet mol/m2/s
		else
			Vmax[index]=1.e-6*Vcmax[index]*(1-exp(-pAtm->RntLAIv[index]*pPlant->SimLeafAreaIndex[index]))/pAtm->RntLAIv[index];	// Enhet mol/m2/s
	
	   
	   	VmaxRubisco[index]=Vmax[index]*GPP_Current.Temp[index]*GPP_Current.Nitrogen[index]*GPP_Current.Water[index];		
		Jmax[index]=min(RadPlantAdsorbed[index]*PhoRadEffResponseL(index)/86400/Fix::MOL_C		//Enhet mol/m2/s
		,Vmax[index]*2.1*GPP_Current.Temp[index]*GPP_Current.Water[index]/4);
	   
		double P_Sink1;
		double P_Rub, P_Light, P_Sink;
		P_Sink1 = P_Sink = 0;
		if(C3Type[index]>0) {
			 Gammas[index]=0.5*Fix::PO2/(2600.*pow(0.57,Qt[index]));
			 MMO2V[index]=MMO2(Qt[index]);
			 P_Sink=P_SinkLimitedC3(Vmax[index]);
		}
		else
			 P_Sink1=P_SinkLimitedC4(Vmax[index]);
		

		PCO2A		 = CO2_A * P_Surface*1.E-6;
		FarqRange[index]    = PCO2A  - Gammas[index];
		ICConv[index] = 0;



		
		PCo2y[index].assign(6,0.);
		EYY[index].assign(6,0.);


		pCo2Bp=PCO2_Boundary/10.;
//Iterations:	
		for(size_t ICount=0; ICount<6; ICount++) {
			SortIn( FarqRange, Gammas, int(ICount), pPlant->NumPlants, index);

			if(C3Type[index]>0) {	
			   P_Rub=P_RubiscolimitedC3(VmaxRubisco[index], PCo2y[index][ICount], Gammas[index], MMO2V[index]) ;	
			   P_Light=P_LightLimitedC3(Jmax[index],PCo2y[index][ICount], Gammas[index]);
			}
			else {
			   P_Rub=P_RubiscolimitedC4(VmaxRubisco[index]) ;	
			   P_Light=P_LightLimitedC4(Jmax[index]);
			   P_Sink=P_SinkLimitedC4_CO2(P_Sink1, PCo2y[index][ICount]);
			}

			ASSIMY[index][ICount]=PMinFunc(P_Rub, P_Light, P_Sink, P_ATheta, P_BTheta);	// Cycalc	
			ASSIMNY[index][ICount]=ASSIMY[index][ICount]*(1-RespGCoef[index]);
// Not to be converted here			/MOL_C/86400)
			// Dessa är nu per markyta

 			PCO2IN = StomataCO2Func(PCO2A, pCo2Bp,ASSIMNY[index][ICount] ,RespSoil,RespMPlant, GBoundary[index], GStomata[index], GAir[index], Vmax[index], co2cap);
			EYY[index][ICount] = PCo2y[index][ICount]- PCO2IN;

			if(ICount>0) {
				if(abs(EYY[index][ICount-1])>=0.1)
					ICConv[index]			= ICount;
				else {
					EYY[index][ICount]		= EYY[index][ICount-1];
					PCo2y[index][ICount]	= PCo2y[index][ICount-1];
				}
			}
			
		}

		out.Gross=ASSIMY[index][ICConv[index]];	// Denna är per markyta
		out.Net=ASSIMNY[index][ICConv[index]];	// Denna är per markyta

//		Följande ekvationer beräknar de slutgiltiga värdena på CO2 cons inne i bladet och
//		i bladverket. De är viktiga för nästa tidssteg.

		PCo2I[index] = PCo2y[index][ICConv[index]]+out.Net*P_Surface/(4000*Vmax[index]); // + AssimNY(index, Icconv[index])/(4000*Vmax)*P_surface		
		PCo2s[index] = PCo2I[index] + out.Net*P_Surface*1.6/GStomata[index];

	
		if (CanopyCO2_Storage_Sw(CanopyCO2_Storage) == CanopyCO2_Storage_Sw::Dynamic) {
			Co2b = pCo2Bp / P_Surface;	// The real C_B forecast with the iterated fluxes.   

			Co2b = (Co2b + (RespSoil + RespMPlant - out.Net + CO2_A * GAir[index] * 1.E-6) * (T_Step * 86400 / co2cap)) / (1 + T_Step * 86400 / co2cap * GAir[index]);
			//Denna ekvation är en omskrivning av: cb = cbp + (resp-bruttofoto+ga*(ca-cb))*(tid/kapacitet)
			//där kvoten tid/kapacitet omvandlar från flöde till mängd

			pCo2Bp = Co2b * P_Surface;		// go back from molC / mol air to Pascals. Denna används i nästa tidssteg
		}
		else
			pCo2Bp = CO2_A * P_Surface*1.E-6;

		// Outputs
        Cflux[index]	= GAir[index]/1.4*(Co2b-CO2_A);		// carbon flux between CAS and reference level

		PCO2_Boundary=pCo2Bp*10.;  // Multiply by 10 to convert from Pa to PPM as concentration (1.E6/Pressure)
		PCO2_Internal[index]=PCo2I[index]*10.;
		PCO2_Surface[index]=PCo2s[index]*10.;
		
		if(C3Type[index]>0) {	
			out.Rubisco=P_RubiscolimitedC3(VmaxRubisco[index], PCo2y[index][ICConv[index]], Gammas[index], MMO2V[index])*10e6;
			out.Light=P_LightLimitedC3(Jmax[index],PCo2y[index][ICConv[index]], Gammas[index])*10e6;
			out.Sink=P_SinkLimitedC3(Vmax[index])*10e6;
		}
		else {
				out.Rubisco=P_RubiscolimitedC4(VmaxRubisco[index])*10e6;
				out.Light=P_LightLimitedC4(Jmax[index])*10e6;
				out.Sink=P_SinkLimitedC4_CO2(P_Sink1, PCo2y[index][ICConv[index]])*10e6;
		}
		return out;

}

							  

double NC_Plant_Functions::StomataCO2Func(double PCO2A, double pCo2Bp, double Assim, double RespSoil, double RespMPlant, double GB, double GS, double GA, double Vmax, double co2cap) {
	// Return the value of CO2 at the surface of the leaf
	double CO2_B, P_CO2_S, CO2_A, P_CO2_B;

	CO2_B	= pCo2Bp/	P_Surface;	//CO2B is given the value calc in the previous timestep	
	CO2_A	= PCO2A/	P_Surface;
   if(CanopyCO2_Storage_Sw(CanopyCO2_Storage) == CanopyCO2_Storage_Sw::Dynamic)
	CO2_B	= (CO2_B+(RespSoil+RespMPlant-Assim+CO2_A*GA)*(T_Step*86400/co2cap))/(1+T_Step*86000/co2cap*GA) ;

	//Sedan beräknas co2A på nytt

	P_CO2_B	= CO2_B * P_Surface;

	P_CO2_S	= P_CO2_B - (1.4/GB * Assim * P_Surface);

	return  max(0.1, P_CO2_S - Assim * P_Surface * (1.6/GS + 1.0/(Vmax*4000)));				
}












bool NC_Plant_Functions:: CheckFromPhotoFile() {

	if(!m_PG_PlantBiomass.ReOpen()) return false;

	CAtmNewMobile_Position.resize(pPlant->NumPlants);
	LAI_Position.resize(pPlant->NumPlants);
	RootDepth_Position.resize(pPlant->NumPlants);
	size_t CAtm_Count, LAI_Count, RootDepth_Count;
	CAtm_Count=LAI_Count=RootDepth_Count=0;

	//ResetReadWrite=true;
    NumVariablesPhotoInput=0;
    NumValidInputPhotoInput=m_PG_PlantBiomass.GetNumVariables();
    

	for(size_t i=0; i<m_PG_PlantBiomass.GetNumVariables(); i++) {
		 string name=m_PG_PlantBiomass.GetVarName(i+1);
		 string Id=m_PG_PlantBiomass.GetVarId(i+1);

		 for(size_t ii=0; ii<20;ii++) {
			if(int(name[ii])>=97) {
				name[ii]=char(int(name[ii])-32);
			}
		 }


		
		// Variables that could possibly be given as input from external growth model:
		// CAtmNewMobile     - 'C AtmNewMobile'  'Plant #'
		// LAI               - 'LeafAreaIndex'   'Plant #'
		// RootDepth         - 'RootDepth'       'Plant #'
		
		// RUE output to be used as Input to soil C N:
		// C_N_LitterFall(2) = Above ground litter C and N (daily values)
		// C_N_LitterFallR(2) = Below ground litter C and N for total soil profile (daily values)
		
		// Output from the RUE plant module (to be used as Input to Coup plant C N) is: 
		// W = standing aboveground biomass
		// WDead = standing dead aboveground biomass
		// FW_AtoS= Yearly above ground production (= stem production)
		// FW_StoD= Yearly mortality of above ground production (= stem mortality)
		// FW_StoH= Harvest of living above ground biomass
		// FW_DtoH= Harvest of dead above ground biomass
		
		if(name.find("C ATMNEWMOBILE")>=0) {
		    if(CAtmNewMobile_Position.size()==0) {
		        CAtmNewMobile_Position.resize(pPlant->NumPlants);
				CAtmNewMobile_Position[0].ivar = 0;
		    }
			
		  //  read(AA(34:36),*)PlantNumber
		    if(CAtm_Count<pPlant->NumPlants){
				CAtmNewMobile_Position[CAtm_Count].ivar = i;
				NumVariablesPhotoInput++;
				CAtm_Count++;
			}
		}
		if(name.find("LEAFAREAINDEX")>=0||name.find("LAI")>=0) {
			if(LAI_Count<pPlant->NumPlants){
				LAI_Position[LAI_Count].ivar = i;
				NumVariablesPhotoInput++;
				LAI_Count++;
			}

		}
		if(name.find("ROOTDEPTH")>0) {
		    if(RootDepth_Count<pPlant->NumPlants){
				RootDepth_Position[RootDepth_Count].ivar = i;
				NumVariablesPhotoInput++;
				RootDepth_Count++;
			}
		}
	}

	if(NumVariablesPhotoInput==0){
		Message("Warning: No Plant Biomass varables in file :");
		return false;
	}
	return true;

}
bool NC_Plant_Functions::CheckFromGrazingFile() {
	m_PG_Grazing.ResetPos();

	if (!m_PG_Grazing.ReOpen()) return false;



	for (size_t i = 0; i<m_PG_Grazing.GetNumVariables(); i++) {
		string name = m_PG_Grazing.GetVarName(i + 1);
		string Id = m_PG_Grazing.GetVarId(i + 1);

		for (size_t ii = 0; ii<20; ii++) {
			if (int(name[ii]) >= 97) {
				name[ii] = char(int(name[ii]) - 32);
			}
		}
		if (name.find("GRAZING") != string::npos) {

			GrazingRate_Pos.ivar = i+1;

		}



	}

	GrazingRate_Pos.pgfile = &m_PG_Grazing;
	GrazingRate_Pos.discrete = false;
	GrazingRate_Pos.shift = 0;
	
	GrazingDatesPGMin.resize(m_PG_Grazing.GetNumRecords());
	for (size_t i = 0; i < m_PG_Grazing.GetNumRecords(); i++) {
		GrazingDatesPGMin[i] = m_PG_Grazing.GetLongTime(i + 1);
	};


	if (m_PG_Grazing.GetNumVariables() == 0&& m_PG_Grazing.GetNumRecords()==0) {
		Message("Warning: No Grazing variables in file :");
		return false;
	}

	return true;

}



bool NC_Plant_Functions::TerminatingHarvestFunction(size_t i) {
    double RootLittF_Terminator=0.95;
    double StemLittF_Terminator=0.05;
	auto Terminator_HarvestComp = [&](size_t i) {
		double HarvestFrac;
		for (size_t jj = 0; m_pModelStructure->m_NumActPlantElements; jj++) {
			HarvestFrac = 1-StemLittF_Terminator;	
			P_Harvest[_GRAIN][jj][i] = HarvestFrac * P_State[_GRAIN][jj][i] * 1. / T_Step;
			P_Harvest[_STEM][jj][i] = HarvestFrac * P_State[_STEM][jj][i] * 1. / T_Step;
			P_Harvest[_OLD_STEM][jj][i] = HarvestFrac * P_State[_OLD_STEM][jj][i] * 1. / T_Step;
			P_Harvest[_LEAF][jj][i] = HarvestFrac * P_State[_LEAF][jj][i] * 1. / T_Step;
			P_Harvest[_OLD_LEAF][jj][i] = HarvestFrac * P_State[_OLD_LEAF][jj][i] * 1. / T_Step;
		
			HarvestFrac = 1 - RootLittF_Terminator;
			P_Harvest[_ROOT][jj][i] = HarvestFrac * P_State[_ROOT][jj][i] * 1. / T_Step;
			P_Harvest[_OLD_ROOT][jj][i] = HarvestFrac * P_State[_OLD_ROOT][jj][i] * 1. / T_Step;
			P_Harvest[_COARSE_ROOT][jj][i] = HarvestFrac * P_State[_COARSE_ROOT][jj][i] * 1. / T_Step;
			P_Harvest[_OLD_COARSE_ROOT][jj][i] = HarvestFrac * P_State[_OLD_COARSE_ROOT][jj][i] * 1. / T_Step;
		}
	};
	auto Terminator_LitterFallComp = [&](size_t i) {
		double HarvestFrac, LitterFrac;
		HarvestFrac = 1 - StemLittF_Terminator;
		LitterFrac = StemLittF_Terminator;
		for (size_t jj = 0; m_pModelStructure->m_NumActPlantElements; jj++) {
			HarvestFrac = 1 - StemLittF_Terminator;
			LitterFrac = StemLittF_Terminator;
			P_LitterFall[_STEM][jj][i] = LitterFrac * (P_State[_STEM][jj][i] - (P_State[_STEM][jj][i] * HarvestFrac))*1. / T_Step;
			P_LitterFall[_OLD_STEM][jj][i] = LitterFrac * (P_State[_OLD_STEM][jj][i] - (P_State[_OLD_STEM][jj][i] * HarvestFrac))*1. / T_Step;
			P_LitterFall[_LEAF][jj][i] = LitterFrac * (P_State[_LEAF][jj][i] - (P_State[_LEAF][jj][i] * HarvestFrac))*1. / T_Step;
			P_LitterFall[_OLD_LEAF][jj][i] = LitterFrac * (P_State[_OLD_LEAF][jj][i] - (P_State[_OLD_LEAF][jj][i] * HarvestFrac))*1. / T_Step;
			P_LitterFall[_GRAIN][jj][i] = LitterFrac * (P_State[_GRAIN][jj][i] - (P_State[_GRAIN][jj][i] * HarvestFrac))*1. / T_Step;
			P_LitterFall[_NEWMOBILE][jj][i] = P_State[_NEWMOBILE][jj][i] * 1. / T_Step;
			if (SimPlantAge[i] >= Max_Plant_LifeTime[i] * 365. - 365 || HarvFracStem[i] == 1 || LitterFracStem[i] == 1.)
				P_LitterFall[_MOBILE][jj][i] = P_State[_MOBILE][jj][i] * 1. / T_Step;

			HarvestFrac = 1 - RootLittF_Terminator; LitterFrac = RootLittF_Terminator;
			P_LitterFall[_ROOT][jj][i] = max(0., LitterFrac* (P_State[_ROOT][jj][i] - (P_State[_ROOT][jj][i] * HarvestFrac))*1. / T_Step);
			P_LitterFall[_OLD_ROOT][jj][i] = max(0., LitterFrac* (P_State[_OLD_ROOT][jj][i] - (P_State[_OLD_ROOT][jj][i] * HarvestFrac))*1. / T_Step);
			P_LitterFall[_COARSE_ROOT][jj][i] = max(0., LitterFrac * (P_State[_COARSE_ROOT][jj][i] - (P_State[_COARSE_ROOT][jj][i] * HarvestFrac))*1. / T_Step);
			P_LitterFall[_OLD_COARSE_ROOT][jj][i] = max(0., LitterFrac * (P_State[_OLD_COARSE_ROOT][jj][i] - (P_State[_OLD_COARSE_ROOT][jj][i] * HarvestFrac))*1. / T_Step);
		}
	};
	auto Reset_ConditionsatHarvest = [&](size_t i) {
		for (size_t j = 0; j <= _OLD_STEM; j++)
			P_Respiration[j][i] = 0.;

		for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
			for (size_t j = 0; j <= ROOT_GRAIN; j++)
				P_ReAllocation[j][jj][i] = 0.;

			P_ReAllocation[NEWMOBILE_FUNGI][jj][i] = 0.;
		}

		GrowthStageIndex[i] = Min_GSI[i];
		SimPlantAge[i] = 0.;
		GrowingSum[i] = 0.;
	};

	Terminator_HarvestComp(i);
	Terminator_LitterFallComp(i);
 	if (GrowthStageIndex[i] == 4) Reset_ConditionsatHarvest(i);
			    
	if(ICountHarvestCycle==1) {
		C_TotHarvest_1=(P_Harvest[_GRAIN][_C_][i]+ P_Harvest[_STEM][_C_][i]+ P_Harvest[_OLD_STEM][_C_][i]+P_Harvest[_LEAF][_C_][i] + P_Harvest[_OLD_LEAF][_C_][i])*T_Step;
		C_Thinning_1=AccHarvest[_C_];
		N_TotHarvest_1= (P_Harvest[_GRAIN][_N_][i] + P_Harvest[_STEM][_N_][i] + P_Harvest[_OLD_STEM][_N_][i] + P_Harvest[_LEAF][_N_][i] + P_Harvest[_OLD_LEAF][_N_][i])*T_Step;
		N_Thinning_1=AccHarvest[_N_];
	}
	else if(ICountHarvestCycle==2) {
		C_TotHarvest_2= (P_Harvest[_GRAIN][_C_][i] + P_Harvest[_STEM][_C_][i] + P_Harvest[_OLD_STEM][_C_][i] + P_Harvest[_LEAF][_C_][i] + P_Harvest[_OLD_LEAF][_C_][i])*T_Step;
		C_Thinning_2=AccHarvest[_C_]-C_TotHarvest_1-C_Thinning_1;
		N_TotHarvest_2= (P_Harvest[_GRAIN][_C_][i] + P_Harvest[_STEM][_N_][i] + P_Harvest[_OLD_STEM][_N_][i] + P_Harvest[_LEAF][_N_][i] + P_Harvest[_OLD_LEAF][_N_][i])*T_Step;
		N_Thinning_2=AccHarvest[_N_]-N_TotHarvest_1-N_Thinning_1;
	}			    
    			
    HarvestToBeDone[i]=false;
    PreviousThinningYear=0;
    ICountHarvestCycle++;
	return true;
}



// Functions for fungal growth regulation 
// Calculating mycorrhization degree (FungiRootCover)
double NC_Plant_Functions::CoverFungiAreaFuncL(size_t i) {
    return CoverFungiFunc(P_State[_FUNGI][_C_][i], P_State[_ROOT][_C_][i],P_State[_OLD_ROOT][_C_][i], AlloFungiC1[i], AreaCoverFungiCoef[i]);
}

double NC_Plant_Functions::CoverFungiFunc(double C_Fungi, double C_Root, double C_OldRoot, double AlloFungiC1, double Coef) {
    if((C_Root+C_OldRoot)<=0.) 
        return 0;
    else    
        return min(1., C_Fungi/((C_Root+C_OldRoot)*Coef*AlloFungiC1));
}   







