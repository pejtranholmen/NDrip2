#include "NC_Organic_Functions.h"
#include "./Functors/NC_Organic_Functors.h"
#include "../PLANT/PlantModel.h"
#include "../SOIL/SoilModel.h"
#include "../SOIL/Soil_Frost.h"
#include "../Structure/Structure.h"



NC_Organic_Functions::NC_Organic_Functions(void)
{
	Org_Link[L1_HUM] = pair<size_t, size_t>(_L1, _H);
	Org_Link[L2_HUM] = pair<size_t, size_t>(_L2, _H);
	Org_Link[F_HUM] = pair<size_t, size_t>(_F, _H);
	Org_Link[L1_PLANTUPTAKE] =pair<size_t, size_t>(_L1, string::npos);
	Org_Link[L2_PLANTUPTAKE] = pair<size_t, size_t>(_L2,string::npos);
	Org_Link[F_PLANTUPTAKE] = pair<size_t, size_t>(_F, string::npos);
	Org_Link[H_PLANTUPTAKE] = pair<size_t, size_t>(_H, string::npos);
	Org_Link[L1_DIS] = pair<size_t, size_t>(_L1, _DIS);
	Org_Link[L2_DIS] = pair<size_t, size_t>(_L2, _DIS);
	Org_Link[F_DIS] = pair<size_t, size_t>(_F, _DIS);
	Org_Link[H_DIS] = pair<size_t, size_t>(_H, _DIS);
	Org_Link[L1_MIC] = pair<size_t, size_t>(_L1, _MIC);
	Org_Link[L2_MIC] = pair<size_t, size_t>(_L2, _MIC);
	Org_Link[F_MIC] = pair<size_t, size_t>(_F, _MIC);
	Org_Link[H_MIC] = pair<size_t, size_t>(_H, _MIC);
	Do_Cultivating = false;

}
NC_Organic_Functions::~NC_Organic_Functions(void)
{
};


bool NC_Organic_Functions::Def() {

	NC_Mineral::Def();
	ModuleNo=25;
	SoilModel* p_Soil=(SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
	Soil_Frost* pSoil=&p_Soil->m_SoilFrost;
	m_pSoil=pSoil;
	NE *pNL=m_pModelMap->GetNEPointer("Number of Layers");
	PlantModel* p_Plant=(PlantModel*)m_pModelInfo->GetModelPtr("Plant Model");
	pPlant=(SimModule*)&p_Plant->m_WaterUptake;

	Sw* pSw; Ps *pPs; P *pP; X *pX; T *pT; G *pG;  Ts *pTs; Gs *pGs; Xs *pXs; Tab *pTab;	   
	pSw= new Sw(&DissolvedOrganics ,  SIMPROFILE,ORGANIC, TRANSPORT,NOBPROC, NORMAL);
		Define(pSw, "Dissolved Organics",0,"off|On","Nitrogen and Carbon > 0");
	pSw= new Sw(&Microbes ,  SIMPROFILE,ORGANIC, TRANSPORT,NOBPROC, NORMAL);
		Define(pSw, "Microbes",0,"off|on-substate dependent|on-microbe dependent","Nitrogen and Carbon > 0");
	pSw= new Sw(&LitterPools,  SIMPROFILE,ORGANIC, TRANSPORT,NOBPROC, NORMAL);
		Define(pSw, "LitterPools",0,"one|two","Nitrogen and Carbon > 0");
	pSw= new Sw(&FaecesPool,  SIMPROFILE,ORGANIC, TRANSPORT,NOBPROC, NORMAL);
		Define(pSw, "FaecesPool",0,"no|one","Nitrogen and Carbon > 0");
	pSw= new Sw(& CN_Ratio_Influence,  SIMPROFILE,ORGANIC, TRANSPORT,NOBPROC, NORMAL);
		Define(pSw,"CN Ratio Influence",0,"no|on litter consumption","Nitrogen and Carbon > 0|Microbes > 0");

	pSw= new Sw(&Microbial_Consumption,  SIMPROFILE,ORGANIC, TRANSPORT,NOBPROC, NORMAL);
		Define(pSw, "Microbial consumption",0,"Static|F(Temp)|F(Temp, Moisture)","Nitrogen and Carbon > 0|Microbes > 0");
	pSw= new Sw(&Microbial_Mortality	,  SIMPROFILE,ORGANIC, TRANSPORT,NOBPROC, NORMAL);
		Define(pSw, "Microbial mortality",0,"Static|F(Temp)|F(Temp, Moisture)","Nitrogen and Carbon > 0|Microbes > 0");

	pSw= new Sw(&Initial_Soil_Organic,  SIMPROFILE,ORGANIC, TRANSPORT,NOBPROC, NORMAL);
		Define(pSw, "Initial Soil Organic",1,"Table|Linear decrease|Constant|Exponential|Table with previous final state values","Nitrogen and Carbon > 0",6);

	pSw= new Sw(&OrganicUptake,  SIMPROFILE,ORGANIC, TRANSPORT,NOBPROC, NORMAL);
		Define(pSw,  "Organic Uptake",0,"off|on|on - with threshold","Nitrogen and Carbon > 0");
//	Define(pSw, Q_Model,"Q Model",0,"off|on","Nitrogen and Carbon >0");

	pSw= new Sw(&InitialCarbonConditions,  SIMPROFILE,ORGANIC, TRANSPORT,NOBPROC, NORMAL);
		Define(pSw, "Initial Carbon Conditions", 0,"As Nitrogen|As Nitogen and Carbon","Nitrogen and Carbon > 0");
			pSw= new Sw(&InitialCarbonConditions,  SIMPROFILE,ORGANIC, TRANSPORT,NOBPROC, NORMAL);

	pSw = new Sw(&InitialPhosphorusConditions, SIMPROFILE, ORGANIC, TRANSPORT, NOBPROC, NORMAL);
			Define(pSw, "Initial Phosphorus Conditions", 0, "As Phosphorus|As CP ratio and Carbon", "Nitrogen and Carbon > 0|Phosphorus>0");

	pSw= new Sw(&Measured_Organic_Table,  SIMPROFILE,ORGANIC, TRANSPORT,NOBPROC, NORMAL);

	Define(pSw, "Measured Input Organic layers", 0,"As model layers|As measured layers","Nitrogen and Carbon > 0|Initial Soil Organic=0");

	pSw= new Sw(&DisplacementOrganicLayers,  SIMPROFILE,ORGANIC, TRANSPORT,NOBPROC, NORMAL);

	Define(pSw, "Displacement of organics between layers", 0,"Static|At Year Shift|At Year shift Delay|At week Shift|At week shift Delay","Nitrogen and Carbon > 0|Initial Soil Organic=0");

// State Variables
	vector<double> initv;
	initv.assign(pSoil->NL,10.);
	string Item[3]{ "C","N","P" };
	string Comp[6]{ "Litter1","Litter2","Faeces","Humus","Microbes","Dissolved Organics" }; //Add comp[6] _DIS by HH
	for (size_t jj = 0; jj < NUM_BIOELEM - 1; jj++) {
		string name = Item[jj];
		string condition = "";

	
		
		for (size_t j = _L1 ; j <= _DIS; j++) {
			string name = Item[jj];
			condition = "";  // make _H visible
		if (jj == _P_) condition = "|Phosphorus>0";
		if (j == _L2) condition += "|LitterPools >0";
		else if (j == _F) condition += "|FaecesPool >0";
		else if (j == _MIC) condition += "|Microbes >0";
		else if (j == _DIS) condition += "|Dissolved Organics >0";   
			name = Item[jj]+ Comp[j] ;
			pX = new X(&O_State[j][jj], pNL, SIMPROFILE, elements(jj + CARBON), STORAGE, NON_LIVING, NORMAL);
			Define(pX, pNL, name, initv,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1"+condition);
		}

		condition = "";
		for (size_t j = _L1; j <= _L2; j++) {
			name = Item[jj];
			condition = "";
			if (jj == _P_) condition = "|Phosphorus>0";
			if (j == _L2) condition += "|LitterPools >0";
			name += " Surface "+Comp[j];
			pXs = new Xs(&SurfaceLitter[j][jj], SOILSURFACE, elements(jj + CARBON), STORAGE, NON_LIVING, NORMAL);
			Define(pXs, name, 0.,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1"+condition);
		}


	
		for (size_t j = _L1; j <= _L2; j++) {
			name = Item[jj];
			condition = "";
			if (jj == _P_) condition = "|Phosphorus>0";
			if (j == _L2) condition += "|LitterPools >0";
			name += " SurfaceL " + Comp[j];
			pTs = new Ts(&SurfaceLitterMixingIntoUppermostlayer[j][jj], SOILSURFACE, elements(jj + CARBON), STORAGE, NON_LIVING, NORMAL);
			Define(pTs, name, 0.,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1" + condition);
		}


		initv.assign(pSoil->NL, 0.);
		for (size_t j = _L1; j <= _L2; j++) {
			name = Item[jj];
			condition = "";
			if (jj == _P_) condition = "|Phosphorus>0";
			if (j == _L2) condition += "|LitterPools >0";
			name += "Roots" + Comp[j];
			pT = new T(&LitterInputToSoilLayers[j][jj], pNL, SIMPROFILE, elements(jj + CARBON), TRANSPORT, LITTERFALL, NORMAL);
			Define(pT, pNL, name, initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1"+condition);
		}
	}


// Input N  and C Flow Variables

//	pTs= new Ts(&SurfaceLitterMixingIntoUppermostlayer[_H][_N_]	,  SOILSURFACE,NITROGEN, TRANSPORT,NOBPROC, NORMAL);
//		Define(pTs , "NSurfaceL Humus", 0.,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Microbes >0");


//	pTs= new Ts(&SurfaceLitterMixingIntoUppermostlayer[_H][_C_],  SOILSURFACE,CARBON, TRANSPORT,NOBPROC, NORMAL);
//		Define(pTs , "CSurfaceL Humus", 0.,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Microbes >0");


	/*	pT= new T(&LitterInputToSoilLayers[_L1][_N_],pNL,SIMPROFILE,CARBON, TRANSPORT,LITTERFALL, NORMAL);
	Define(pT, pNL  , "CFungiLitter1",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Mycorrhiza Fungal Growth>0");
		pT= new T(&LitterInputToSoilLayers[_L1][_N_],pNL,SIMPROFILE,CARBON, TRANSPORT,LITTERFALL, NORMAL);
	Define(pT, pNL  , "CFungiLitter2",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|LitterPools > 0|Mycorrhiza Fungal Growth>0")	;

		pT= new T(&LitterInputToSoilLayers[_F][_N_],pNL,SIMPROFILE,CARBON, TRANSPORT,LITTERFALL, NORMAL);
	Define(pT, pNL  , "NFungiLitter1",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Mycorrhiza Fungal Growth>0");
		pT= new T(&LitterInputToSoilLayers[_F][_N_],pNL,SIMPROFILE,CARBON, TRANSPORT,LITTERFALL, NORMAL);
	Define(pT, pNL  , "NFungiLitter2",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|LitterPools > 0|Mycorrhiza Fungal Growth>0")	;
;*/
// MF;

	for (size_t jj = 0; jj < NUM_BIOELEM-1; jj++) {
		string name = Item[jj];
		string condition = "";

		for (size_t j = _L1; j <= _MIC; j++) {
			condition = "";
			if (jj == _P_) condition = "|Phosphorus>0";
			if (j == _L2) condition += "|LitterPools > 0";
			else if (j == _F) condition += "|FaecesPool >0";
			else if (j == _MIC) condition += "|Microbes >0";
			name = Item[jj]+Comp[j] + "Dissolved";
			pT = new T(&O_Transform[j + L1_DIS][jj], pNL, SIMPROFILE, elements(CARBON + jj), TRANSPORT, ORGANICTRANSFORMATION, NORMAL);
			Define(pT, pNL, name, initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Dissolved Organics > 0" + condition);
		}
		condition = "";
		if (jj == _P_) condition = "|Phosphorus>0";

		name = Item[jj] + "DissolvedFlow";
		pT = new T(&Dissolved[jj].VFlow, pNL, SIMPROFILE, elements(DOC+jj), TRANSPORT, NOBPROC, NORMAL);
			Define(pT, pNL, name, initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Dissolved Organics > 0"+condition);
		name = Item[jj] + "DissolvedDrainage";
		pT = new T(&Dissolved[jj].HFlow, pNL, SIMPROFILE, elements(DOC + jj), TRANSPORT, NOBPROC, NORMAL);
			Define(pT, pNL, name, initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Dissolved Organics > 0"+condition);
		for (size_t j = _L1; j <= _F; j++) {
			condition = "";
			if (jj == _P_) condition = "|Phosphorus>0";
			if (j == _L2) condition += "|LitterPools > 0";
			else if (j == _F) condition += "|FaecesPool >0";
			else if (j == _MIC) condition += "|Microbes >0";
			name = Item[jj]+ Comp[j] + "HumusRate";
			pT = new T(&O_Transform[j][jj], pNL, SIMPROFILE, elements(CARBON+j), TRANSPORT, ORGANICTRANSFORMATION, NORMAL);
			Define(pT, pNL, name, initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1" + condition);
		}
;
		for (size_t j = L1_MIC; j <= H_MIC; j++) {
			
			condition = "| Microbes > 0";
			if (jj == _P_) condition = "|Phosphorus>0";
			if (j == L2_MIC) condition += "|LitterPools > 0";
			else if (j == F_MIC) condition += "|FaecesPool >0";
			name = Item[jj]+ Comp[j-L1_MIC] + "MicrobeRate";
			pT = new T(&O_Transform[j][jj], pNL, SIMPROFILE, elements(CARBON + jj), TRANSPORT, ORGANICTRANSFORMATION, NORMAL);
			Define(pT, pNL, name, initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1" + condition);
		}
		if (jj == _N_) {
			for (size_t j = _L1; j <= _MIC; j++) {
				condition = "";
				if (jj == _P_) condition = "|Phosphorus>0";
				if (j == _L2) condition += "|LitterPools > 0";
				else if (j == _F) condition += "|FaecesPool >0";
				else if (j == _MIC) condition += "|Microbes >0";
				name = Item[jj] + Comp[j] + "NH4Rate";
				pT = new T(&O_Mineralisation[j][jj - 1], pNL, SIMPROFILE, elements(CARBON + jj), NOFPROC, MINERALISATION, NORMAL);
				Define(pT, pNL, name, initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1" + condition);//HH
			}
			for (size_t j = _L1; j <= _MIC; j++) {
				condition = "";
				if (jj == _P_) condition = "|Phosphorus>0";
				if (j == _L2) condition += "|LitterPools > 0";
				else if (j == _F) condition += "|FaecesPool >0";
				else if (j == _MIC) condition += "|Microbes >0";
				name =  "CN Ratio " + Comp[j];
				pG = new G(&Org_CarbonRatio[j][jj - 1], pNL, SIMPROFILE, elements(CARBON + jj), NOFPROC, RATIO, NORMAL); ;
				Define(pG, pNL, name, initv,UNIT_TYPES::NO_UNIT, "Nitrogen and Carbon >= 1"+condition); //Raio :no unite by HH
			}

		}
		else if (jj == _P_) {
			for (size_t j = _L1; j <= _MIC; j++) {
				condition = "|Phosphorus>0";
				if (j == _L2) condition += "|LitterPools > 0";
				else if (j == _F) condition += "|FaecesPool >0";
				else if (j == _MIC) condition += "|Microbes >0";
				name = Item[jj] + Comp[j] + "PO4Rate";
				pT = new T(&O_Mineralisation[j][jj - 1], pNL, SIMPROFILE, elements(CARBON + jj), NOFPROC, MINERALISATION, NORMAL);
				Define(pT, pNL, name, initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Microbes = 0"+condition);
			}
			for (size_t j = _L1; j <= _MIC; j++) {
				condition = "";
				if (jj == _P_) condition = "|Phosphorus>0";
				if (j == _L2) condition += "|LitterPools > 0";
				else if (j == _F) condition += "|FaecesPool >0";
				else if (j == _MIC) condition += "|Microbes >0";
				name = "CP Ratio " + Comp[j];
				pG = new G(&Org_CarbonRatio[j][jj - 1], pNL, SIMPROFILE, elements(CARBON + jj), NOFPROC, RATIO, NORMAL); ;
				Define(pG, pNL, name, initv,UNIT_TYPES::NO_UNIT, "Nitrogen and Carbon >= 1" + condition);
			}
		}


		if (jj >= _N_) {//fixed the error of using N for P org uptake by HH
			name = Item[jj] + "Litter1Upt";
			pT = new T(&O_Transform[L1_PLANTUPTAKE][jj], pNL, SIMPROFILE, elements(CARBON + jj), TRANSPORT, ORGANICTRANSFORMATION, NORMAL);
			Define(pT, pNL, name, initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Organic Uptake>0");
			name = Item[jj] + "Litter1FungiUpt";
			pT = new T(&O_Transform[L1_FUNGIUPTAKE][jj], pNL, SIMPROFILE, elements(CARBON + jj), TRANSPORT, ORGANICTRANSFORMATION, NORMAL);
			Define(pT, pNL, name, initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Organic Uptake>0|Mycorrhiza Fungal Growth>0");//HH

			name = Item[jj] + "HumusUpt";
			pT = new T(&O_Transform[H_PLANTUPTAKE][jj], pNL, SIMPROFILE, elements(CARBON + jj), TRANSPORT, ORGANICTRANSFORMATION, NORMAL);
			Define(pT, pNL, name, initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Organic Uptake>0");
			name = Item[jj] + "HumusFungiUpt";
			pT = new T(&O_Transform[H_FUNGIUPTAKE][jj], pNL, SIMPROFILE, elements(CARBON + jj), TRANSPORT, ORGANICTRANSFORMATION, NORMAL);
			Define(pT, pNL, name, initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Organic Uptake>0|Mycorrhiza Fungal Growth>0");

		}
		else if (jj == _C_) {
			for (size_t j = _L1; j <= _MIC; j++) {
				condition = "";
				if (j == _L2) condition += "|LitterPools > 0";
				else if (j == _F) condition += "|FaecesPool >0";
				else if (j == _MIC) condition += "|Microbes >0";
				name = Item[jj] + Comp[j] + "RespRate";
				pT = new T(&O_RespRate[j], pNL, SIMPROFILE, CARBON , NOFPROC, RESPIRATION, NORMAL);
				Define(pT, pNL, name, initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Microbes = 0");
			}
		}
		for (size_t j = _L1; j <= _DIS; j++) {
			condition = "";
			if (jj == _P_) condition = "|Phosphorus>0";
			if (j == _L2) condition += "|LitterPools > 0";
			else if (j == _F) condition += "|FaecesPool >0";
			else if (j == _MIC) condition += "|Microbes >0";
			else if (j == _DIS) condition += "|Dissolved Organics >0";
			name = Item[jj] + "TotSoil" + Comp[j];
			pGs = new Gs(&O_State_Profile[j][jj], TOTPROFILE, elements(CARBON+jj), STORAGE, NON_LIVING, NORMAL);
			Define(pGs, name, 0.0,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1"+condition);
		}

	}

;
// C Respiration Flow Variables;
	string name = Item[_C_];
	string condition = "";

	for (size_t j = _L1; j <= _MIC; j++) {
		condition = "";
		if (j == _L2) condition += "|LitterPools > 0";
		else if (j == _F) condition += "|FaecesPool >0";
		else if (j == _MIC) condition += "|Microbes >0";
		name = Item[_C_] +"Tot"+ Comp[j] + "RespRate";
		pGs = new Gs(&O_RespRateProfile[j], TOTPROFILE, CARBON, NOFPROC, RESPIRATION, NORMAL);
		Define(pGs, name, 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1"+condition);
	}
	pGs= new Gs(&CTotSoilRespRate,	 TOTPROFILE,CARBON, NOFPROC,RESPIRATION, NORMAL);
		Define(pGs, 		"CTotSoilRespRate",		0.0,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1");

	for (size_t j = _L1; j <= _DIS; j++) {
			condition = "";

			if (j == _L2) condition += "|LitterPools > 0";
			else if (j == _F) condition += "|FaecesPool >0";
			else if (j == _MIC) condition += "|Microbes >0";
			name = Item[_N_] + "Tot" + Comp[j] + "NH4Rate";
			pGs = new Gs(&O_MineralisationProfile[j][_N_ - 1], TOTPROFILE, NITROGEN, NOFPROC, MINERALISATION, NORMAL);
			Define(pGs,name, 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1"+condition);

	}







pGs= new Gs(&NTotOrgNH4Rate,	 TOTPROFILE,NITROGEN, NOFPROC,MINERALISATION, NORMAL);
		Define(pGs, 		"NTotOrgNH4Rate",		0.0,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1");
pGs = new Gs(&PTotOrgPO4Rate, TOTPROFILE, PHOSPHORUS, NOFPROC, MINERALISATION, NORMAL);
		Define(pGs, "PTotOrgPO4Rate", 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Phosphorus>0");
		
		;
pGs= new Gs(&NTotOrg_PlantUptake,	 TOTPROFILE,NITROGEN, TRANSPORT,ORGANICTRANSFORMATION, NORMAL);
		Define(pGs, "N Tot Org Plant Uptake", 0.,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >0");

pGs = new Gs(&PTotOrg_PlantUptake, TOTPROFILE, PHOSPHORUS, TRANSPORT, ORGANICTRANSFORMATION, NORMAL);
		Define(pGs, "P Tot Org Plant Uptake", 0.,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >0|Phosphorus>0");
;
pGs= new Gs(&O_State_TotOrg[_N_], TOTPROFILE,NITROGEN, STORAGE,NON_LIVING, NORMAL);
		Define(pGs ,		"NTotSoilOrg",		0.0,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1");
pGs= new Gs(&O_State_TotOrg[_C_],	 TOTPROFILE,CARBON, STORAGE,NON_LIVING, NORMAL);
		Define(pGs ,	"CTotSoilOrg",		0.0,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1");


pGs= new Gs(&CN_Ratio_TotHumus,	 TOTPROFILE,NITROGEN, STORAGE,RATIO, NORMAL);
		Define(pGs , 	"CN Ratio TotHumus",	0.0,"-", "Nitrogen and Carbon >= 1");
pGs= new Gs(&CN_Ratio_TotSoil,	 TOTPROFILE,NITROGEN, STORAGE,RATIO, NORMAL);
		Define(pGs , 	"CN Ratio TotSoil",	0.0,"-", "Nitrogen and Carbon >= 1");

	pGs = new Gs(&CP_Ratio_TotHumus, TOTPROFILE, PHOSPHORUS, STORAGE, RATIO, NORMAL); //HH
		Define(pGs, "CP Ratio TotHumus", 0.0, "-", "Nitrogen and Carbon >= 1|Phosphorus>0");
	pGs = new Gs(&CP_Ratio_TotSoil, TOTPROFILE, PHOSPHORUS, STORAGE, RATIO, NORMAL);
		Define(pGs, "CP Ratio TotSoil", 0.0, "-", "Nitrogen and Carbon >= 1|Phosphorus>0");//HH

;
//   Humus formation;

for (size_t jj = 0; jj < NUM_BIOELEM-1; jj++) {
	name = Item[jj] + "TotHumusformation";
	condition = "";
	if (jj == _P_) condition = "|Phosphorus>0";
	pGs = new Gs(&Humusformation[jj], TOTPROFILE, elements(CARBON+jj), TRANSPORT, ORGANICTRANSFORMATION, NORMAL);
	Define(pGs, name, 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1"+condition);
	name = Item[jj] + "TotRootLitter";
	pGs = new Gs(&LitterInputProfile[_C_], TOTPROFILE, elements(CARBON + jj), TRANSPORT, ORGANICTRANSFORMATION, NORMAL);
	Define(pGs, name, 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1"+condition);
	name = Item[jj] + "TotDisOrgDrainage";
	pGs = new Gs(&Dissolved[jj].TotHFlow, TOTPROFILE, elements(CARBON + jj), TRANSPORT, ORGANICTRANSFORMATION, NORMAL);
	Define(pGs, name, 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|DissolvedOrganics >0"+condition);
	name = "Dissolved"+Item[jj] + "DrainConc";
	pGs = new Gs(&Dissolved[jj].TotHFlowConc, TOTPROFILE, elements(CARBON + jj), TRANSPORT, ORGANICTRANSFORMATION, NORMAL);
	Define(pGs, name, 0.0, "mg-N/l", "Nitrogen and Carbon >= 1|DissolvedOrganics >0"+condition);
	name = Item[jj] + "TotOrg_DisOrg";
	pGs = new Gs(&Dissolved[jj].TotSourceFlow, TOTPROFILE, elements(CARBON + jj), TRANSPORT, ORGANICTRANSFORMATION, NORMAL);
	Define(pGs, name, 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|DissolvedOrganics >0"+condition);

	name = "Dissolved"+Item[jj] + "Conc";
	pG = new G(&Dissolved[jj].Conc, pNL, SIMPROFILE, elements(DOC+jj), NOFPROC, CONCENTRATION, NORMAL);
	Define(pG, pNL,name, initv, "mg-N/l", "Nitrogen and Carbon >= 1|DissolvedOrganics >0"+condition);

}


pGs = new Gs(&Tot_P_DrainageConc, TOTPROFILE, PHOSPHORUS, TRANSPORT, ORGANICTRANSFORMATION, NORMAL);// Tot P conc by HH
Define(pGs, "Tot P Drainage Conc", 0.0, "mg/l", "Nitrogen and Carbon >= 1|DissolvedOrganics >0|Phosphorus>0");
		

//	Define(pGs CTotDisOrgDecomp,  	"CTotDisOrgDecomp",		0.0,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|DissolvedOrganics >0");
;
//! Auxiliary																										;
/*	pG= new G(&CLitter1Decomp	 ,pNL,SIMPROFILE,CARBON, NOFPROC,DECOMPOSITION, NORMAL);																													;
	Define(pG, pNL  ,	"CLitter1Decomp",	initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Microbes = 0");
	pG= new G(&CLitter2Decomp	 ,pNL,SIMPROFILE,CARBON, NOFPROC,DECOMPOSITION, NORMAL);																													;
		Define(pG, pNL  ,	"CLitter2Decomp",	initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Microbes = 0|LitterPools >0");
	pG= new G(&CFaecesDecomp	 ,pNL,SIMPROFILE,CARBON, NOFPROC,DECOMPOSITION, NORMAL);																													;
		Define(pG, pNL  ,	"CFaecesDecomp",	initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|FaecesPool > 0");
;*/
	
//MF;
;
//	Parameters Initial Conditions;
	pPs= new Ps(&Org_State_IniValue[_L1][_N_].Tot_Profile,	 TOTPROFILE,NITROGEN, NOFPROC,INITB, NORMAL);
	Define(pPs,  	"Init L1 NTot", 10.,UNIT_TYPES::MASS_UNIT,"Initial Soil Organic >0|Nitrogen and Carbon >0",0.,1000.);
	pPs= new Ps(&Org_State_IniValue[_L2][_N_].Tot_Profile,	 TOTPROFILE,NITROGEN, NOFPROC,INITB, NORMAL);
		Define(pPs,  	"Init L2 NTot", 3.,UNIT_TYPES::MASS_UNIT,"Initial Soil Organic >0|LitterPools >0",0.,1000.);
	pPs= new Ps(&Org_State_IniValue[_F][_N_].Tot_Profile,	 TOTPROFILE,NITROGEN, NOFPROC,INITB, NORMAL);
		Define(pPs,  		"Init F NTot", 2.,UNIT_TYPES::MASS_UNIT,"Initial Soil Organic >0|FaecesPool >0",0.,1000.);
	pPs= new Ps(&Org_State_IniValue[_MIC][_N_].Tot_Profile,	 TOTPROFILE,NITROGEN, NOFPROC,INITB, NORMAL);
		Define(pPs,  		"Init M NTot", 10.,UNIT_TYPES::MASS_UNIT,"Initial Soil Organic >0|Microbes >0",0., 1.E6);
	pPs= new Ps(&Org_State_IniValue[_H][_N_].Tot_Profile,	 TOTPROFILE,NITROGEN, NOFPROC,INITB, NORMAL);
		Define(pPs,  		"Init H NTot", 500.,UNIT_TYPES::MASS_UNIT,"Initial Soil Organic >0|Nitrogen and Carbon >0",0.,1.E6);
;

pPs = new Ps(&Org_State_IniValue[_L1][_P_].Tot_Profile, TOTPROFILE, PHOSPHORUS, NOFPROC, INITB, NORMAL);
Define(pPs, "Init L1 PTot", 10.,UNIT_TYPES::MASS_UNIT, "Initial Soil Organic >0|Nitrogen and Carbon >0|Phosphorus>0", 0., 1000.);
pPs = new Ps(&Org_State_IniValue[_L2][_P_].Tot_Profile, TOTPROFILE, PHOSPHORUS, NOFPROC, INITB, NORMAL);
Define(pPs, "Init L2 PTot", 3.,UNIT_TYPES::MASS_UNIT, "Initial Soil Organic >0|LitterPools >0|Phosphorus>0", 0., 1000.);
pPs = new Ps(&Org_State_IniValue[_F][_P_].Tot_Profile, TOTPROFILE, PHOSPHORUS, NOFPROC, INITB, NORMAL);
Define(pPs, "Init F PTot", 2.,UNIT_TYPES::MASS_UNIT, "Initial Soil Organic >0|FaecesPool >0|Phosphorus>0", 0., 1000.);
pPs = new Ps(&Org_State_IniValue[_MIC][_P_].Tot_Profile, TOTPROFILE, PHOSPHORUS, NOFPROC, INITB, NORMAL);
Define(pPs, "Init M PTot", 1.,UNIT_TYPES::MASS_UNIT, "Initial Soil Organic >0|Microbes >0|Phosphorus>0", 0., 1.E6);
pPs = new Ps(&Org_State_IniValue[_H][_P_].Tot_Profile, TOTPROFILE, PHOSPHORUS, NOFPROC, INITB, NORMAL);
Define(pPs, "Init H PTot", 50.,UNIT_TYPES::MASS_UNIT, "Initial Soil Organic >0|Nitrogen and Carbon >0|Phosphorus>0", 0., 1.E6);


	pPs= new Ps(&Org_State_IniValue[_L1][_C_].Tot_Profile,	 TOTPROFILE,CARBON, NOFPROC,INITB, NORMAL);
		Define(pPs,  	"Init L1 CTot", 10.,UNIT_TYPES::MASS_UNIT,"Initial Soil Organic >0|Initial Carbon Conditions>0",0.,10000.);
	pPs= new Ps(&Org_State_IniValue[_L2][_C_].Tot_Profile,	 TOTPROFILE,CARBON, NOFPROC,INITB, NORMAL);
		Define(pPs,  	"Init L2 CTot", 3.,UNIT_TYPES::MASS_UNIT,"Initial Soil Organic >0|LitterPools >0|Initial Carbon Conditions>0",0.,10000.);
	pPs= new Ps(&Org_State_IniValue[_F][_C_].Tot_Profile,	 TOTPROFILE,CARBON, NOFPROC,INITB, NORMAL);
		Define(pPs,  		"Init F CTot", 2.,UNIT_TYPES::MASS_UNIT,"Initial Soil Organic >0|FaecesPool >0|Initial Carbon Conditions>0",0.,1000.);
	pPs= new Ps(&Org_State_IniValue[_MIC][_C_].Tot_Profile,	 TOTPROFILE,CARBON, NOFPROC,INITB, NORMAL);
		Define(pPs,  		"Init M CTot", 10.,UNIT_TYPES::MASS_UNIT,"Initial Soil Organic >0|Microbes >0|Initial Carbon Conditions>0",0.,1.E6);
	pPs= new Ps(&Org_State_IniValue[_H][_C_].Tot_Profile,	 TOTPROFILE,CARBON, NOFPROC,INITB, NORMAL);
		Define(pPs,  		"Init H CTot", 5000.,UNIT_TYPES::MASS_UNIT,"Initial Soil Organic >0|Initial Carbon Conditions>0",0.,1.E6);
;
	pPs= new Ps(&Org_State_IniCarbonRatio[_L1][_N_-1].Tot_Profile, TOTPROFILE,ORGANIC, NOFPROC,INITB, NORMAL);
	Define(pPs,  	"Init L1 CN Tot", 25.,	"-","Initial Soil Organic >0|Initial Carbon Conditions=0|Nitrogen and Carbon >0",5.,100.);
	pPs= new Ps(&Org_State_IniCarbonRatio[_L2][_N_ - 1].Tot_Profile,	 TOTPROFILE,ORGANIC, NOFPROC,INITB, NORMAL);
		Define(pPs,  	"Init L2 CN Tot", 15.,	"-","Initial Soil Organic >0|LitterPools >0|Initial Carbon Conditions=0",5.,500.);
	pPs= new Ps(&Org_State_IniCarbonRatio[_F][_N_ - 1].Tot_Profile,	 TOTPROFILE,ORGANIC, NOFPROC,INITB, NORMAL);
		Define(pPs,  	"Init F CN Tot", 15.,	"-","Initial Soil Organic >0|FaecesPool >0|Initial Carbon Conditions=0",5.,40.);
	pPs= new Ps(&Org_State_IniCarbonRatio[_MIC][_N_ - 1].Tot_Profile,	 TOTPROFILE,ORGANIC, NOFPROC,INITB, NORMAL);
		Define(pPs,  	"Init M CN Tot", 8.,	"-","Initial Soil Organic >0|Microbes >0|Initial Carbon Conditions=0",5.,40.);
	pPs= new Ps(&Org_State_IniCarbonRatio[_H][_N_ - 1].Tot_Profile,	 TOTPROFILE,ORGANIC, NOFPROC,INITB, NORMAL);
		Define(pPs,  	"Init H CN Tot", 10.,	"-","Initial Soil Organic >0|Initial Carbon Conditions=0|Nitrogen and Carbon >0",5.,40.);
		//as CP and carbon =1 in IniP_sw
		pPs = new Ps(&Org_State_IniCarbonRatio[_L1][_P_ - 1].Tot_Profile, TOTPROFILE, ORGANIC, NOFPROC, INITB, NORMAL);
		Define(pPs, "Init L1 CP Tot", 75., "-", "Initial Soil Organic >0|Initial Phosphorus Conditions>0|Nitrogen and Carbon >0|Phosphorus>0", 5., 500.);
		pPs = new Ps(&Org_State_IniCarbonRatio[_L2][_P_ - 1].Tot_Profile, TOTPROFILE, ORGANIC, NOFPROC, INITB, NORMAL);
		Define(pPs, "Init L2 CP Tot", 100., "-", "Initial Soil Organic >0|LitterPools >0|Initial Phosphorus Conditions>0|Phosphorus>0", 5., 500.);
		pPs = new Ps(&Org_State_IniCarbonRatio[_F][_P_ - 1].Tot_Profile, TOTPROFILE, ORGANIC, NOFPROC, INITB, NORMAL);
		Define(pPs, "Init F CP Tot", 150., "-", "Initial Soil Organic >0|FaecesPool >0|Initial Phosphorus Conditions>0|Phosphorus>0", 5., 400.);
		pPs = new Ps(&Org_State_IniCarbonRatio[_MIC][_P_ - 1].Tot_Profile, TOTPROFILE, ORGANIC, NOFPROC, INITB, NORMAL);
		Define(pPs, "Init M CP Tot", 106., "-", "Initial Soil Organic >0|Microbes >0|Initial Phosphorus Conditions>0|Phosphorus>0", 5., 400.);
		pPs = new Ps(&Org_State_IniCarbonRatio[_H][_P_ - 1].Tot_Profile, TOTPROFILE, ORGANIC, NOFPROC, INITB, NORMAL);
		Define(pPs, "Init H CP Tot", 100., "-", "Initial Soil Organic >0|Initial Phosphorus Conditions>0|Nitrogen and Carbon >0|Phosphorus>0", 5., 500.);
;
	pPs= new Ps(&Org_State_IniValue[_L1][_C_].Depth,	 TOTPROFILE,ORGANIC, NOFPROC,INITB, NORMAL);
		Define(pPs,  	"Init L1 Depth", -0.5,"m","Initial Soil Organic >0|Nitrogen and Carbon >0",-10.,0.);
	pPs= new Ps(&Org_State_IniValue[_L2][_C_].Depth,	 TOTPROFILE,ORGANIC, NOFPROC,INITB, NORMAL);
		Define(pPs,  	"Init L2 Depth", -0.5,"m","Initial Soil Organic >0|LitterPools >0",-10.,0.);
	pPs= new Ps(&Org_State_IniValue[_F][_C_].Depth,	 TOTPROFILE,ORGANIC, NOFPROC,INITB, NORMAL);
		Define(pPs,  		"Init F Depth", -0.2,"m","Initial Soil Organic >0|FaecesPool >0",-10.,0.);
	pPs= new Ps(&Org_State_IniValue[_MIC][_C_].Depth,	 TOTPROFILE,ORGANIC, NOFPROC,INITB, NORMAL);
		Define(pPs,  		"Init M Depth", -0.5,"m","Initial Soil Organic >0|Microbes >0",-10.,0.);
	pPs= new Ps(&Org_State_IniValue[_H][_C_].Depth,	 TOTPROFILE,ORGANIC, NOFPROC,INITB, NORMAL);
		Define(pPs,  		"Init H Depth", -2.,"m","Initial Soil Organic >0|Nitrogen and Carbon >0",-10.,0.);
;
	pPs= new Ps(&Org_State_IniValue[_L1][_C_].FracExpTail,	 TOTPROFILE,ORGANIC, NOFPROC,INITB, NORMAL);
		Define(pPs,   "Init L1 FracExpTail", 0.1,"-","Initial Soil Organic =3",0.,0.7);
	pPs= new Ps(&Org_State_IniValue[_L2][_C_].FracExpTail,	 TOTPROFILE,ORGANIC, NOFPROC,INITB, NORMAL);
		Define(pPs,   "Init L2 FracExpTail", 0.1,"-","Initial Soil Organic =3|LitterPools >0",0.,0.7);
	pPs= new Ps(&Org_State_IniValue[_F][_C_].FracExpTail,	 TOTPROFILE,ORGANIC, NOFPROC,INITB, NORMAL);
		Define(pPs,  	"Init F FracExpTail", 0.1,"-","Initial Soil Organic =3|FaecesPool >0",0.,0.7);
	pPs= new Ps(&Org_State_IniValue[_MIC][_C_].FracExpTail,	 TOTPROFILE,ORGANIC, NOFPROC,INITB, NORMAL);
		Define(pPs,  	"Init M FracExpTail", 0.1,"-","Initial Soil Organic =3|Microbes >0",0.,0.7);
	pPs= new Ps(&Org_State_IniValue[_H][_C_].FracExpTail,	 TOTPROFILE,ORGANIC, NOFPROC,INITB, NORMAL);
		Define(pPs,  	"Init H FracExpTail", 0.1,"-","Initial Soil Organic =3",0.,0.7);
;
;


		pP=m_pModelMap->GetPPointer("mUpper Depth");
		mTab_OrgC.push_back(pP); mTab_OrgN.push_back(pP); mTab_OrgP.push_back(pP);
		pP=m_pModelMap->GetPPointer("mLower Depth");
		mTab_OrgC.push_back(pP);mTab_OrgN.push_back(pP); mTab_OrgP.push_back(pP);


pP= new P(&Org_State_IniValue[_L1][_N_].Layer_Mass,pNL,SIMPROFILE,NITROGEN, NOFPROC,INITB, NORMAL); initv.assign(pSoil->NL,1.);	
		Define(pP, pNL  ,	"Init L1 N", initv,UNIT_TYPES::MASS_UNIT,"Initial Soil Organic =0",0.,100.);
		mTab_OrgN.push_back(pP);
		initv.assign(pSoil->NL,.3);
pP= new P(&Org_State_IniValue[_L2][_N_].Layer_Mass,pNL,SIMPROFILE,NITROGEN, NOFPROC,INITB, NORMAL);
		Define(pP, pNL  ,	"Init L2 N", initv,UNIT_TYPES::MASS_UNIT,"Initial Soil Organic =0|LitterPools >0",0.,100.);
		mTab_OrgN.push_back(pP);
		initv.assign(pSoil->NL,0.2);
pP= new P(&Org_State_IniValue[_F][_N_].Layer_Mass,pNL,SIMPROFILE,NITROGEN, NOFPROC,INITB, NORMAL);
		Define(pP, pNL  ,	"Init F N", initv,UNIT_TYPES::MASS_UNIT,"Initial Soil Organic =0|FaecesPool >0",0.,100.);
		mTab_OrgN.push_back(pP);
		initv.assign(pSoil->NL,1.);
pP= new P(&Org_State_IniValue[_MIC][_N_].Layer_Mass,pNL,SIMPROFILE,NITROGEN, NOFPROC,INITB, NORMAL);
		Define(pP, pNL , 	"Init M N", initv,UNIT_TYPES::MASS_UNIT,"Initial Soil Organic =0|Microbes >0",0.,100.);
		mTab_OrgN.push_back(pP);
		initv.assign(pSoil->NL,500.);
pP= new P(&Org_State_IniValue[_H][_N_].Layer_Mass,pNL,SIMPROFILE,NITROGEN, NOFPROC,INITB, NORMAL);
		Define(pP, pNL , 	"Init H N", initv,UNIT_TYPES::MASS_UNIT,"Initial Soil Organic =0",0.,1.E5);
		mTab_OrgN.push_back(pP);

		pP = new P(&Org_State_IniValue[_L1][_P_].Layer_Mass, pNL, SIMPROFILE, PHOSPHORUS, NOFPROC, INITB, NORMAL); initv.assign(pSoil->NL, 1.);
		Define(pP, pNL, "Init L1 P", initv,UNIT_TYPES::MASS_UNIT, "Initial Phosphorus Conditions =0|Phosphorus>0", 0., 100.);
		mTab_OrgP.push_back(pP);
		initv.assign(pSoil->NL, .3);
		pP = new P(&Org_State_IniValue[_L2][_P_].Layer_Mass, pNL, SIMPROFILE, PHOSPHORUS, NOFPROC, INITB, NORMAL);
		Define(pP, pNL, "Init L2 P", initv,UNIT_TYPES::MASS_UNIT, "Initial Phosphorus Conditions=0|LitterPools >0|Phosphorus>0", 0., 100.);
		mTab_OrgP.push_back(pP);
		initv.assign(pSoil->NL, 0.2);
		pP = new P(&Org_State_IniValue[_F][_P_].Layer_Mass, pNL, SIMPROFILE, PHOSPHORUS, NOFPROC, INITB, NORMAL);
		Define(pP, pNL, "Init F P", initv,UNIT_TYPES::MASS_UNIT, "Initial Phosphorus Conditions=0|FaecesPool >0|Phosphorus>0", 0., 100.);
		mTab_OrgP.push_back(pP);
		initv.assign(pSoil->NL, 1.);
		pP = new P(&Org_State_IniValue[_MIC][_P_].Layer_Mass, pNL, SIMPROFILE, PHOSPHORUS, NOFPROC, INITB, NORMAL);
		Define(pP, pNL, "Init M P", initv,UNIT_TYPES::MASS_UNIT, "Initial Phosphorus Conditions=0|Microbes >0|Phosphorus>0", 0., 100.);
		mTab_OrgP.push_back(pP);
		initv.assign(pSoil->NL, 500.);
		pP = new P(&Org_State_IniValue[_H][_P_].Layer_Mass, pNL, SIMPROFILE, PHOSPHORUS, NOFPROC, INITB, NORMAL);
		Define(pP, pNL, "Init H P", initv,UNIT_TYPES::MASS_UNIT, "Initial Phosphorus Conditions=0|Phosphorus>0", 0., 1.E5);
		mTab_OrgP.push_back(pP);



		initv.assign(pSoil->NL,1.);
pP= new P(&Org_State_IniValue[_L1][_C_].Layer_Mass,pNL,SIMPROFILE,NITROGEN, NOFPROC,INITB, NORMAL);
		Define(pP, pNL ,	"Init L1 C", initv,UNIT_TYPES::MASS_UNIT,"Initial Soil Organic =0|Initial Carbon Conditions = 1",0.,100.);
		mTab_OrgC.push_back(pP);
initv.assign(pSoil->NL,.3);
pP= new P(&Org_State_IniValue[_L2][_C_].Layer_Mass,pNL,SIMPROFILE,NITROGEN, NOFPROC,INITB, NORMAL);
		Define(pP, pNL ,	"Init L2 C", initv,UNIT_TYPES::MASS_UNIT,"Initial Soil Organic =0|LitterPools >0|Initial Carbon Conditions = 1",0.,100.);
		mTab_OrgC.push_back(pP);
		initv.assign(pSoil->NL,.2);
pP= new P(&Org_State_IniValue[_F][_C_].Layer_Mass,pNL,SIMPROFILE,NITROGEN, NOFPROC,INITB, NORMAL);
		Define(pP, pNL ,	"Init F C", initv,UNIT_TYPES::MASS_UNIT,"Initial Soil Organic =0|FaecesPool >0|Initial Carbon Conditions = 1",0.,100.);
		mTab_OrgC.push_back(pP);
		initv.assign(pSoil->NL,1.);	
pP= new P(&Org_State_IniValue[_MIC][_C_].Layer_Mass,pNL,SIMPROFILE,NITROGEN, NOFPROC,INITB, NORMAL);
		Define(pP, pNL ,	"Init M C", initv,UNIT_TYPES::MASS_UNIT,"Initial Soil Organic =0|Microbes >0|Initial Carbon Conditions = 1",0.,100.);
		mTab_OrgC.push_back(pP);
		initv.assign(pSoil->NL,500.);
pP= new P(&Org_State_IniValue[_H][_C_].Layer_Mass,pNL,SIMPROFILE,NITROGEN, NOFPROC,INITB, NORMAL);
		Define(pP, pNL ,	"Init H C", initv,UNIT_TYPES::MASS_UNIT,"Initial Soil Organic =0|Initial Carbon Conditions = 1",0.,1.E5);
		mTab_OrgC.push_back(pP);



		pP=m_pModelMap->GetPPointer("mUpper Depth");
		mTab_OrgCN.push_back(pP); mTab_OrgCP.push_back(pP);
		pP=m_pModelMap->GetPPointer("mLower Depth");
		mTab_OrgCN.push_back(pP); mTab_OrgCP.push_back(pP);

pP= new P(&Org_State_IniCarbonRatio[_L1][_N_-1].Layer_Mass,pNL,SIMPROFILE,NITROGEN, NOFPROC,INITB, NORMAL); initv.assign(pSoil->NL,25.);
		Define(pP, pNL  ,	"Init L1 CN", initv,"-","Initial Soil Organic =0|Initial Carbon Conditions=0",0.,100.);
		mTab_OrgCN.push_back(pP);
		
pP= new P(&Org_State_IniCarbonRatio[_L2][_N_ - 1].Layer_Mass,pNL,SIMPROFILE,NITROGEN, NOFPROC,INITB, NORMAL);initv.assign(pSoil->NL,15.);
		Define(pP, pNL  ,	"Init L2 CN", initv,"-","Initial Soil Organic =0|LitterPools >0|Initial Carbon Conditions=0",0.,100.);
		mTab_OrgCN.push_back(pP);
		
pP= new P(&Org_State_IniCarbonRatio[_F][_N_ - 1].Layer_Mass,pNL,SIMPROFILE,NITROGEN, NOFPROC,INITB, NORMAL);initv.assign(pSoil->NL,15.);
		Define(pP, pNL  ,	"Init F CN", initv,"-","Initial Soil Organic =0|FaecesPool >0|Initial Carbon Conditions=0",0.,100.);
		mTab_OrgCN.push_back(pP);
		
pP= new P(&Org_State_IniCarbonRatio[_MIC][_N_ - 1].Layer_Mass,pNL,SIMPROFILE,NITROGEN, NOFPROC,INITB, NORMAL);initv.assign(pSoil->NL,8.);
		Define(pP, pNL  ,	"Init M CN", initv,"-","Initial Soil Organic =0|Microbes >0|Initial Carbon Conditions=0",0.,100.);
		mTab_OrgCN.push_back(pP);
		
pP= new P(&Org_State_IniCarbonRatio[_H][_N_ - 1].Layer_Mass,pNL,SIMPROFILE,NITROGEN, NOFPROC,INITB, NORMAL);initv.assign(pSoil->NL,10.);
		Define(pP, pNL  ,	"Init H CN", initv,"-","Initial Soil Organic =0|Initial Carbon Conditions=0",0.,1.E5);
		mTab_OrgCN.push_back(pP);

		pP = new P(&Org_State_IniCarbonRatio[_L1][_P_ - 1].Layer_Mass, pNL, SIMPROFILE, PHOSPHORUS, NOFPROC, INITB, NORMAL); initv.assign(pSoil->NL, 106.);
		Define(pP, pNL, "Init L1 CP", initv, "-", "Initial Soil Organic =0|Initial Phosphorus Conditions>0|Phosphorus>0", 0., 1000.);
		mTab_OrgCP.push_back(pP);

		pP = new P(&Org_State_IniCarbonRatio[_L2][_P_ - 1].Layer_Mass, pNL, SIMPROFILE, PHOSPHORUS, NOFPROC, INITB, NORMAL); initv.assign(pSoil->NL, 106.);
		Define(pP, pNL, "Init L2 CP", initv, "-", "Initial Soil Organic =0|LitterPools >0|Initial Phosphorus Conditions>0|Phosphorus>0", 0., 1000.);
		mTab_OrgCP.push_back(pP);

		pP = new P(&Org_State_IniCarbonRatio[_F][_P_ - 1].Layer_Mass, pNL, SIMPROFILE, PHOSPHORUS, NOFPROC, INITB, NORMAL); initv.assign(pSoil->NL, 150.);
		Define(pP, pNL, "Init F CP", initv, "-", "Initial Soil Organic =0|FaecesPool >0|Initial Phosphorus Conditions>0|Phosphorus>0", 0., 100.);
		mTab_OrgCP.push_back(pP);

		pP = new P(&Org_State_IniCarbonRatio[_MIC][_P_ - 1].Layer_Mass, pNL, SIMPROFILE, PHOSPHORUS, NOFPROC, INITB, NORMAL); initv.assign(pSoil->NL, 106.);
		Define(pP, pNL, "Init M CP", initv, "-", "Initial Soil Organic =0|Microbes >0|Initial Phosphorus Conditions>0|Phosphorus>0", 0., 1000.);
		mTab_OrgCP.push_back(pP);

		pP = new P(&Org_State_IniCarbonRatio[_H][_P_ - 1].Layer_Mass, pNL, SIMPROFILE, PHOSPHORUS, NOFPROC, INITB, NORMAL); initv.assign(pSoil->NL, 100.);
		Define(pP, pNL, "Init H CP", initv, "-", "Initial Soil Organic =0|Initial Phosphorus Conditions>0|Phosphorus>0", 0., 1.E5);
		mTab_OrgCP.push_back(pP);


	NE *pNoLayerM =m_pModelMap->GetNEPointer("No Measured Layers");
	initv.assign(pNoLayerM->GetNEValue(),.05);

	pP=m_pModelMap->GetPPointer("Upper Depth");mTab_MeasOrg.push_back(pP);
	pP=m_pModelMap->GetPPointer("Lower Depth");mTab_MeasOrg.push_back(pP);

pP= new P(&Init_Measured_C_Cont,pNoLayerM,MPROFILE,NITROGEN, NOFPROC,INITB, NORMAL);initv.assign(pNoLayerM->GetNEValue(),5.);
	Define(pP, pNoLayerM  ,	"Init C Cont", initv,"%","Measured Input Organic layers=1",0.,100.,.01,10.,3);
	mTab_MeasOrg.push_back(pP);
pP= new P(&Init_Measured_N_Cont,pNoLayerM,MPROFILE,NITROGEN, NOFPROC,INITB, NORMAL);initv.assign(pNoLayerM->GetNEValue(),.3);
	Define(pP, pNoLayerM  ,	"Init N Cont", initv,"%","Measured Input Organic layers=1",0.,100.,.001,1.,3);
	mTab_MeasOrg.push_back(pP);
pP= new P(&Init_Measured_BD,pNoLayerM,MPROFILE,NITROGEN, NOFPROC,INITB, NORMAL);initv.assign(pNoLayerM->GetNEValue(),1.5);
	Define(pP, pNoLayerM  ,	"Init BulkDensity", initv,"g/cm3","Measured Input Organic layers=1",0.05,2.0, .1,1.8,3);
	mTab_MeasOrg.push_back(pP);
pP= new P(&Init_Assumed_CN_Humus,pNoLayerM,MPROFILE,NITROGEN, NOFPROC,INITB, NORMAL);initv.assign(pNoLayerM->GetNEValue(),10.);
	Define(pP, pNoLayerM  ,	"Init CN Humus", initv,"-","Measured Input Organic layers=1",5.,15.,8.,12.,3);
	mTab_MeasOrg.push_back(pP);
pP= new P(&Init_Assumed_CN_Litter,pNoLayerM,MPROFILE,NITROGEN, NOFPROC,INITB, NORMAL);initv.assign(pNoLayerM->GetNEValue(),25.);
	Define(pP, pNoLayerM  ,	"Init CN Litter", initv,"-","Measured Input Organic layers=1",20.,100.,20.,50.,3);
	mTab_MeasOrg.push_back(pP);
pP= new P(&Init_Assumed_Frac_Litter2,pNoLayerM,MPROFILE,NITROGEN, NOFPROC,INITB, NORMAL);initv.assign(pNoLayerM->GetNEValue(),.1);
	Define(pP, pNoLayerM  ,	"Init Frac Litter2", initv,"-","Measured Input Organic layers=1|LitterPools=1",0.0,1.0,0.,0.9,3);
	mTab_MeasOrg.push_back(pP);

	
;

pTab= new Tab(&mTab_MeasOrg, MPROFILE,NITROGEN, NOFPROC,INITB, NORMAL);
Define(pTab,"Initial Measured Organic Contents",3);
pTab= new Tab(&mTab_OrgN, SIMPROFILE,NITROGEN, NOFPROC,INITB, NORMAL);
Define(pTab,"Initial organic Nitrogen",3);
pTab = new Tab(&mTab_OrgP, SIMPROFILE, PHOSPHORUS, NOFPROC, INITB, NORMAL);
Define(pTab, "Initial organic Phosphorus", 3);
pTab= new Tab(&mTab_OrgCN, SIMPROFILE,NITROGEN, NOFPROC,INITB, NORMAL);
Define(pTab,"Initial organic CN ratios",3);
pTab = new Tab(&mTab_OrgCP, SIMPROFILE, PHOSPHORUS, NOFPROC, INITB, NORMAL);
Define(pTab, "Initial organic CP ratios", 3);
pTab= new Tab(&mTab_OrgC, SIMPROFILE,NITROGEN, NOFPROC,INITB, NORMAL);
Define (pTab,"Initial organic Carbon",3);
//   Parameters ;

	pP=m_pModelMap->GetPPointer("mUpper Depth");		mTab_Diss.push_back(pP);
	pP=m_pModelMap->GetPPointer("mLower Depth");		mTab_Diss.push_back(pP);
	pP= new P(&RateCoefDissolved ,pNL,SIMPROFILE,NITROGEN, NOFPROC,INITB, NORMAL);	initv.assign(pSoil->NL,0.01);
		Define(pP, pNL,  	"RateCoefDissolved", initv,"1/day","Dissolved Organics >0",0.,1.);
		mTab_Diss.push_back(pP);
	pTab= new Tab(&mTab_Diss,SIMPROFILE,NITROGEN, NOFPROC,INITB, NORMAL); 
	Define(pTab,"Rate Coefficient Dissolved Organics",3);
;
	pPs= new Ps(&RateCoefLitter1Dis,	 TOTPROFILE,CARBON, NOFPROC,DECOMPOSITION, NORMAL);
		Define(pPs,  "RateCoefLitter1Dis", 0.0001  , "1/day","Nitrogen and Carbon >0|Dissolved Organics > 0",0.,0.1);
	pPs= new Ps(&RateCoefLitter2Dis,	 TOTPROFILE,CARBON, NOFPROC,DECOMPOSITION, NORMAL);
		Define(pPs,  "RateCoefLitter2Dis", 0.0001  , "1/day","Nitrogen and Carbon >0|Dissolved Organics > 0|LitterPools >0",0.,0.1);
	pPs= new Ps(&RateCoefFaecesDis,	 TOTPROFILE,CARBON, NOFPROC,DECOMPOSITION, NORMAL);
		Define(pPs,  "RateCoefFaecesDis", 0.0001  , "1/day","Nitrogen and Carbon >0|Dissolved Organics > 0|FaecesPool >0",0.,0.1);
	pPs= new Ps(&RateCoefMicrobeDis,	 TOTPROFILE,CARBON, NOFPROC,DECOMPOSITION, NORMAL);
		Define(pPs,  "RateCoefMicrobeDis", 0.0001  , "1/day","Nitrogen and Carbon >0|Dissolved Organics > 0|Microbes >0",0.,0.1);
	pPs= new Ps(&RateCoefHumusDis,	 TOTPROFILE,CARBON, NOFPROC,DECOMPOSITION, NORMAL);
		Define(pPs,  "RateCoefHumusDis", 1.E-5  , "1/day","Nitrogen and Carbon >0|Dissolved Organics > 0",0.,0.001);
	pPs= new Ps(&DocSolFraction,	 TOTPROFILE,CARBON, NOFPROC,DECOMPOSITION, NORMAL);
		Define(pPs,  "DocSolutionFrac", 1.0  , "-","Nitrogen and Carbon >0|Dissolved Organics > 0",0.0001,1.);

		
		
		;
	pPs= new Ps(&Init_CDissConc, TOTPROFILE,DOC, NOFPROC,INITB, NORMAL);
		Define(pPs,  "Init_CDissConc", 1.  , "mg/l","Nitrogen and Carbon >0|Dissolved Organics > 0",0.,300.);
	pPs= new Ps(&CN_Threshold_Org, TOTPROFILE,DOC, NOFPROC,INITB, NORMAL);
		Define(pPs,  "CN Threshold Org", 22.  , "-","Nitrogen and Carbon >0|Organic Uptake > 1",0.,300.);
//	Response function;

//	Define(pG, pNL  ResponseTemp,"Response Temp", initv,"-", "Nitrogen and Carbon >= 4");
//	Define(pG, pNL  Response.Theta,"Response Theta", initv,"-", "Nitrogen and Carbon >= 4");
//	Define(pG, pNL  ResponseTemp_Theta,"Response Temp Theta", initv,"-", "Nitrogen and Carbon >= 4");



return FunctionDef();
}
bool NC_Organic_Functions::FunctionDef() {

	// Parameters;
	string funcname;
	FunctorProp functorprop;
	vector<Ps*> ps_vec;
	vector<P*> p_vec;
	ps_vec.clear();
	vector<double> initv;	
	Ps *pPs; P *pP;
	Tab *pTab;
	Func *pFunc;
	functorprop.FunctorInitOption = string::npos;
	functorprop.pModelInfo = m_pModelInfo;

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


	pPs = new Ps(&RateCoefLitter1, TOTPROFILE, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pPs, "RateCoefLitter1", .035, "1/day", "Nitrogen and Carbon >0|Microbes = 0", 0.1E-6, 1.);
	pPs = new Ps(&RateCoefLitter2, TOTPROFILE, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pPs, "RateCoefLitter2", .1, "1/day", "Nitrogen and Carbon >0|Microbes = 0|LitterPools >0", 0.1E-6, 1.);
	pPs = new Ps(&RateCoefFaeces, TOTPROFILE, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pPs, "RateCoefFaeces", .035, "1/day", "Nitrogen and Carbon >0|FaecesPool >0|Microbes = 0", 0.1E-6, 1.);
	pPs = new Ps(&RateCoefHumus, TOTPROFILE, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pPs, "RateCoefHumus", 5.E-5, "1/day", "Nitrogen and Carbon >0|Microbes = 0", 0.1E-8, .1E-2);
	;
	pPs = new Ps(&HumFracLitter1, TOTPROFILE, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pPs, "HumFracLitter1", .2, "1/day", "Nitrogen and Carbon >0|Microbes = 0", 0.1, 0.9);
	pPs = new Ps(&HumFracLitter2, TOTPROFILE, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pPs, "HumFracLitter2", .2, "1/day", "Nitrogen and Carbon >0|Microbes = 0|LitterPools >0", 0.1, 0.9);
	pPs = new Ps(&HumFracFaeces, TOTPROFILE, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pPs, "HumFracFaeces", .2, "1/day", "Nitrogen and Carbon >0|Microbes = 0", 0.1, 0.9);
	pPs = new Ps(&PO4ConcentrationResponsePO4_Max, TOTPROFILE, CARBON, NOFPROC, DECOMPOSITION, NORMAL); 
	Define(pPs, "RateResponse PO4 Max", 50., UNIT_TYPES::MASSDENSITY_UNIT, "Nitrogen and Carbon >0|Phosphorus > 0", 10., 100.);
	pPs = new Ps(&PO4ConcentrationResponsePO4_Min, TOTPROFILE, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pPs, "RateResponse PO4 Min", 10., UNIT_TYPES::MASSDENSITY_UNIT, "Nitrogen and Carbon >0|Phosphorus > 0", 5., 20.);
	;




	pPs = new Ps(&Eff_Litter1, TOTPROFILE, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pPs, "Eff Litter1", .5, "-", "Nitrogen and Carbon >0|Microbes = 0", 0.1, 0.8);
	pPs = new Ps(&Eff_Litter2, TOTPROFILE, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pPs, "Eff Litter2", .5, "-", "Nitrogen and Carbon >0|Microbes = 0|LitterPools >0", 0.1, 0.8);
	pPs = new Ps(&Eff_Faeces, TOTPROFILE, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pPs, "Eff Faeces", .5, "-", "Nitrogen and Carbon >0|FaecesPool >0|Microbes = 0", 0.1, 0.8);
	pPs = new Ps(&Eff_Microbes, TOTPROFILE, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pPs, "Eff Microbes", .5, "-", "Nitrogen and Carbon >0|Microbes > 0", 0.1, 0.8);
	pPs = new Ps(&Eff_Humus, TOTPROFILE, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pPs, "Eff humus", .5, "-", "Nitrogen and Carbon >0", 0.1, 0.8);

	;
	pPs = new Ps(&CN_Ratio_Microbe, TOTPROFILE, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pPs, "CN Ratio Microbe", 10., "-", "Nitrogen and Carbon >0", 3., 220., 5., 15.);
	pPs = new Ps(&CP_Ratio_Microbe, TOTPROFILE, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pPs, "CP Ratio Microbe", 106., "-", "Nitrogen and Carbon >0|Phosphorus > 0", 3., 1000., 20., 300.);
	;
	;
	functorprop.FuncName = MyFunc::LITTER_TRANS_SURFACE_L1; functorprop.Arg = MyArgType::D1;
	funcname = "Mixing Rate of Surface Litter1 into uppermost layer";
	plotpar.X_Label = "Litter on Soil Surface "; plotpar.X_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::MASS_UNIT, true);
	plotpar.X_Min = 0;
	plotpar.X_Max = 3.;
	plotpar.Y_Label = "Mixing Rate "; plotpar.Y_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::MASSFLOW_UNIT, true);
	plotpar.Num_Of_Curves = 1;
	plotpar.Num_Of_X = 1;

	pPs = new Ps(&RateCoefSurf_L1, TOTPROFILE, CARBON, TRANSPORT, NOBPROC, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "RateCoefSurf L1", .005, UNIT_TYPES::RATECOEF_UNIT, "Nitrogen and Carbon >0", 0.1E-6, 1.);
	pFunc = new Func(TOTPROFILE, CARBON, TRANSPORT, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar); ps_vec.clear();


	functorprop.FuncName = MyFunc::LITTER_TRANS_SURFACE_L2; 
	funcname = "Mixing Rate of Surface Litter2 into uppermost layer";
	pPs = new Ps(&RateCoefSurf_L2, TOTPROFILE, CARBON, TRANSPORT, NOBPROC,  NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "RateCoefSurf L2", .005, UNIT_TYPES::RATECOEF_UNIT, "Nitrogen and Carbon >0|LitterPools >0", 0.1E-6, 1.);
	pFunc = new Func(PLANT, CARBON, TRANSPORT, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar); ps_vec.clear();
	
	functorprop.FuncName = MyFunc::LITTER_TRANS_SURFACE_HUM;
	funcname = "Mixing Rate of Surface Humus into uppermost layer";
	pPs = new Ps(&RateCoefSurf_Hum, TOTPROFILE, CARBON, TRANSPORT, NOBPROC, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "RateCoefSurf Hum", .05, UNIT_TYPES::RATECOEF_UNIT, "Nitrogen and Carbon >0|Microbes >0", 0.1E-6, 1.);
	pFunc = new Func(PLANT, CARBON, TRANSPORT, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar); ps_vec.clear();


	functorprop.FuncName = MyFunc::POT_GROWTHCONSUMPTION_MICROBE; functorprop.Arg = MyArgType::D1;
	funcname = "Potential Growth Consumption";
	pPs = new Ps(&RateCoefMic_Cons, TOTPROFILE, CARBON, NOFPROC, DECOMPOSITION, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "RateCoefMic Cons", .01, UNIT_TYPES::RATECOEF_UNIT, "Nitrogen and Carbon >0|Microbes >0", 0.1E-6, 1.);
	pFunc = new Func(PLANT, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec,plotpar); ps_vec.clear();




	pPs = new Ps(&RateCoefMic_Mort, TOTPROFILE, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pPs, "RateCoefMic Mort", .01, UNIT_TYPES::RATECOEF_UNIT, "Nitrogen and Carbon >0|Microbes >0", 0.1E-6, 1.); ps_vec.push_back(pPs);


	

	ps_vec.clear();
	functorprop.FuncName = MyFunc::RESPIRATION_MICROBE; functorprop.Arg = MyArgType::D1;
	funcname = "Respiration Microbes";


	pPs = new Ps(&RateCoefMic_Resp, TOTPROFILE, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pPs, "RateCoefMic Resp", .002, UNIT_TYPES::RATECOEF_UNIT, "Nitrogen and Carbon >0|Microbes >0", 0.1E-6, 1.); ps_vec.push_back(pPs);
	
	pFunc = new Func(PLANT, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar);
	
	
	
	
	functorprop.FuncName = MyFunc::GROWTH_CONSUMPTION_MICROBE; functorprop.Arg = MyArgType::D1;
	funcname = "Actual Growth Consumption";
	ps_vec.clear();
	ps_vec.push_back(m_pModelMap->GetPsPointer("Eff Microbes"));
	pPs = new Ps(&Mic_ConsFrac_L1, TOTPROFILE, CARBON, NOFPROC, DECOMPOSITION, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "Mic Cons Frac L1", .5, "-", "Nitrogen and Carbon >0|Microbes >0", 0., 1.);
	pPs = new Ps(&Mic_ConsFrac_L2, TOTPROFILE, CARBON, NOFPROC, DECOMPOSITION, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "Mic Cons Frac L2", .1, "-", "Nitrogen and Carbon >0|Microbes >0|LitterPools > 0", 0., 1.);
	pPs = new Ps(&Mic_ConsFrac_Fec, TOTPROFILE, CARBON, NOFPROC, DECOMPOSITION, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "Mic Cons Frac Fec", .1, "-", "Nitrogen and Carbon >0|Microbes >0|FaecesPool >0", 0., 1.);
	pFunc = new Func(TOTPROFILE, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec);

	functorprop.FuncName = MyFunc::RESP_GROWTH_CONSUMPTION_MICROBE; functorprop.Arg = MyArgType::D1;
	funcname = "Respiration during Growth Consumption"; //Note the same parameters as for consumption but the efficiency will be
	// used as the complementary value (1-Eff).
	pFunc = new Func(TOTPROFILE, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec);



	;
	pPs = new Ps(&Mic_HumFrac_L1, TOTPROFILE, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pPs, "Mic Hum Frac L1", .5, "-", "Nitrogen and Carbon >0|Microbes >0", 0., 1.);
	pPs = new Ps(&Mic_HumFrac_L2, TOTPROFILE, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pPs, "Mic Hum Frac L2", .1, "-", "Nitrogen and Carbon >0|Microbes >0|LitterPools > 0", 0., 1.);
	pPs = new Ps(&Mic_HumFrac_Fec, TOTPROFILE, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pPs, "Mic Hum Frac Fec", .1, "-", "Nitrogen and Carbon >0|Microbes >0|FaecesPool >0", 0., 1.);
	;
	functorprop.FuncName = MyFunc::HUMIFICATION_MICROBE; functorprop.Arg = MyArgType::T_D1;
	funcname = "Humification Function"; ps_vec.clear();
	ps_vec.push_back(m_pModelMap->GetPsPointer("Mic Cons Frac L1"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("Mic Cons Frac L2"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("Mic Cons Frac Fec"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("Mic Hum Frac L1"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("Mic Hum Frac L2"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("Mic Hum Frac Fec"));

	pFunc = new Func(PLANT, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec);

	functorprop.FuncName = MyFunc::MORTALITY_MICROBE; functorprop.Arg = MyArgType::T_D1;
	funcname = "Actual Mortality rate";
	ps_vec.clear();
	
	ps_vec.push_back(m_pModelMap->GetPsPointer("RateCoefMic Mort"));
	pPs = new Ps(&Mic_MortFrac_L1, TOTPROFILE, CARBON, NOFPROC, DECOMPOSITION, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "Mic Mort Frac L1", .1, "-", "Nitrogen and Carbon >0|Microbes >0", 0., 1.);
	pPs = new Ps(&Mic_MortFrac_L2, TOTPROFILE, CARBON, NOFPROC, DECOMPOSITION, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "Mic Mort Frac L2", .1, "-", "Nitrogen and Carbon >0|Microbes >0|LitterPools > 0", 0., 1.);
	pPs = new Ps(&Mic_MortFrac_Fec, TOTPROFILE, CARBON, NOFPROC, DECOMPOSITION, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "Mic Mort Frac Fec", .1, "-", "Nitrogen and Carbon >0|Microbes >0|FaecesPool >0", 0., 1.);
	pFunc = new Func(PLANT, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec);
	
	
	
	functorprop.FuncName = MyFunc::MICROBE_CN_RATIO_RESPONSE; functorprop.Arg = MyArgType::D1;
	funcname = "CN ratio impact on Decomposition";
	ps_vec.clear();


	pPs = new Ps(&RateResponseCN_Max, TOTPROFILE, CARBON, NOFPROC, DECOMPOSITION, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "RateResponse CN Max", 50., "1/day", "Nitrogen and Carbon >0|CN Ratio Influence > 0", 10., 100.);
	pPs = new Ps(&RateResponseCN_Min, TOTPROFILE, CARBON, NOFPROC, DECOMPOSITION, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "RateResponse CN Min", 10., "1/day", "Nitrogen and Carbon >0|CN Ratio Influence > 0", 5., 20.);

	

	
	pFunc = new Func(PLANT, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec);

	functorprop.FuncName = MyFunc::MICROBE_CONC_RESPONSE; functorprop.Arg = MyArgType::D1;
	funcname = "Substrate Concentration impact on Decomposition";
	ps_vec.clear();

	pPs = new Ps(&SubStrate_HalfRateConc, TOTPROFILE, CARBON, NOFPROC, DECOMPOSITION, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "Substate HalfRateConc", 300., UNIT_TYPES::MASSDENSITY_UNIT, "Nitrogen and Carbon >0|CN Ratio Influence > 0", 1., 30000.);

	pFunc = new Func(PLANT, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec);


	pPs = new Ps(&N_ImmobMaxAvailFrac, TOTPROFILE, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pPs, "N Immob MaxAvailFrac", 0.05, "-", "Nitrogen and Carbon >0|CN Ratio Influence > 0", 0.00001, 0.1);
	pPs = new Ps(&P_ImmobMaxAvailFrac, TOTPROFILE, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pPs, "P Immob MaxAvailFrac", 0.05, "-", "Nitrogen and Carbon >0|CP Ratio Influence > 0", 0.00001, 0.1);
	//MF;

	plotpar.Num_Of_Curves = 2;
	plotpar.Num_Of_Points = 100;
	plotpar.X_Min = 0;
	plotpar.X_Max = 100;
	plotpar.X_Label = "Litter "; plotpar.X_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::MASS_UNIT,true);
	plotpar.Y_Label = "Flow Rate "; 	plotpar.Y_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::mMASSFLOW_UNIT, true);
	plotpar.LogXScale = false;
	plotpar.LogYScale = false;
	plotpar.Y_ScalingCoef = 3;
	plotpar.Num_Of_Opt = 1;
	plotpar.Num_Of_X = 3;
	plotpar.X2_Vec = {50.,50.};  //CN ratio Litter
	plotpar.X3_Vec = { 10.,10. };
	plotpar.LegendsVec = { "Respiration", "Humification" };



	functorprop.FuncName = MyFunc::LITTER_TURNOVER_CARBON; functorprop.Arg = MyArgType::TURNOVER_D3;
	funcname = "Decomposition of Litter1";
	ps_vec.clear();
	ps_vec.push_back(m_pModelMap->GetPsPointer("CN Ratio Microbe"));	
	ps_vec.push_back(m_pModelMap->GetPsPointer("Eff Litter1"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("HumFracLitter1"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("N Immob MaxAvailFrac"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("RateCoefLitter1"));


	pFunc = new Func(PLANT, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar);

	functorprop.FuncName = MyFunc::LITTER_TURNOVER;
	funcname = "N Humification and Mineralisation from Litter1";
	plotpar.Num_Of_Curves = 4;
	plotpar.X2_Vec = { 20.,20.,40.,80. };  //CN ratio Litter
	plotpar.X3_Vec = { 1.,1., 1., 1. };
	plotpar.LegendsVec = { "N Humification", "N Mineralisation-CN 20",
	"N Mineralisation-CN 40", "N Mineralisation-CN 80" };
	
	pFunc = new Func(PLANT, NITROGEN, NOFPROC, MINERALISATION, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar);




	functorprop.FuncName = MyFunc::LITTER_CNP_TURNOVER; functorprop.Arg = MyArgType::TURNOVER_D5;
	funcname = "Decomposition of Litter1 with NP Mineralisation";

	ps_vec.push_back(m_pModelMap->GetPsPointer("P Immob MaxAvailFrac"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("CP Ratio Microbe"));

	pFunc = new Func(PLANT, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec);


	functorprop.FuncName = MyFunc::LITTER2_TURNOVER_CARBON; functorprop.Arg = MyArgType::TURNOVER_D3;
	funcname = "Decomposition of Litter2";
	plotpar.Num_Of_Curves = 2;
	plotpar.X_Label = "Litter2 "; plotpar.X_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::MASS_UNIT, true);
	plotpar.Num_Of_Opt = 1;
	plotpar.Num_Of_X = 3;
	plotpar.X2_Vec = { 50.,50. };  //CN ratio Litter
	plotpar.X3_Vec = { 10.,10. };
	plotpar.LegendsVec = { "Respiration", "Humification" };

	ps_vec.clear();
	ps_vec.push_back(m_pModelMap->GetPsPointer("CN Ratio Microbe"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("Eff Litter2"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("HumFracLitter2"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("N Immob MaxAvailFrac"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("RateCoefLitter2"));

	pFunc = new Func(PLANT, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar);

	functorprop.FuncName = MyFunc::LITTER2_TURNOVER;
	funcname = "N Humification and Mineralisation from Litter2";
	plotpar.Num_Of_Curves = 4;
	plotpar.X2_Vec = { 20.,40.,80.,160. };  //CN ratio Litter
	plotpar.X3_Vec = { 1.,1., 1., 1. };
	plotpar.LegendsVec.clear();
	plotpar.LegendsVec = { "N Humification", "N Mineralisation-CN 40","N Mineralisation-CN 80", "N Mineralisation-CN 160" };

	pFunc = new Func(SIMPROFILE, NITROGEN, NOFPROC, MINERALISATION, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar);


	functorprop.FuncName = MyFunc::LITTER2_CNP_TURNOVER; functorprop.Arg = MyArgType::TURNOVER_D5;
	funcname = "Decomposition of Litter2 with NP Mineralisation";

	ps_vec.push_back(m_pModelMap->GetPsPointer("P Immob MaxAvailFrac"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("CP Ratio Microbe"));

	pFunc = new Func(PLANT, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec);


	functorprop.FuncName = MyFunc::FAECES_TURNOVER_CARBON; functorprop.Arg = MyArgType::TURNOVER_D3;
	funcname = "Decomposition of Faeces";
	plotpar.X_Label = "Faeces "; plotpar.X_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::MASS_UNIT, true);
	plotpar.Num_Of_Curves = 2;
	plotpar.Num_Of_Opt = 1;
	plotpar.Num_Of_X = 100;
	plotpar.X2_Vec = { 50.,50. };  //CN ratio Litter
	plotpar.X3_Vec = { 10.,10. };
	plotpar.LegendsVec = { "Respiration", "Humification" };




	ps_vec.clear();
	ps_vec.push_back(m_pModelMap->GetPsPointer("CN Ratio Microbe"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("Eff Faeces"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("HumFracFaeces"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("N Immob MaxAvailFrac"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("RateCoefFaeces"));

	pFunc = new Func(SIMPROFILE, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec,plotpar);
	functorprop.FuncName = MyFunc::FAECES_TURNOVER;
	funcname = "N Humification and Mineralisation from Faeces";
	plotpar.Num_Of_Curves = 4;
	plotpar.X2_Vec = { 20.,20.,40.,80. };  //CN ratio Litter
	plotpar.X3_Vec = { 1.,1., 1., 1. };
	plotpar.LegendsVec.clear();
	plotpar.LegendsVec = { "N Humification", "N Mineralisation-CN 20","N Mineralisation-CN 40", "N Mineralisation-CN 80" };

	pFunc = new Func(SIMPROFILE, NITROGEN, NOFPROC, MINERALISATION, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar);


	functorprop.FuncName = MyFunc::FAECES_CNP_TURNOVER; functorprop.Arg = MyArgType::TURNOVER_D5;
	funcname = "Decomposition of Faeces with NP Mineralisation";

	ps_vec.push_back(m_pModelMap->GetPsPointer("P Immob MaxAvailFrac"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("CP Ratio Microbe"));

	pFunc = new Func(PLANT, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec);


	functorprop.FuncName = MyFunc::HUMUS_TURNOVER_CARBON; functorprop.Arg = MyArgType::TURNOVER_D3;
	funcname = "Decomposition of Humus";
	plotpar.Num_Of_Curves = 1;
	plotpar.X_Max = 1000.;
	plotpar.Num_Of_Opt = 1;
	plotpar.Num_Of_X = 3;
	plotpar.X_Label = "Humus "; plotpar.X_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::MASS_UNIT, true);
	plotpar.X2_Vec = { 10.,10.};  //CN ratio Litter
	plotpar.X3_Vec = { 10.,10. };
	plotpar.LegendsVec = { "Respiration", "Return" };

	ps_vec.clear();
	ps_vec.push_back(m_pModelMap->GetPsPointer("CN Ratio Microbe"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("Eff humus"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("N Immob MaxAvailFrac"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("RateCoefHumus"));

	pFunc = new Func(PLANT, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar);

	plotpar.Num_Of_Curves = 4;
	plotpar.X2_Vec = { 10.,14.,18.,22. };  //CN ratio Litter
	plotpar.X3_Vec = { 1.,1., 1., 1. };
	plotpar.LegendsVec.clear();
	plotpar.LegendsVec = { "N Mineralisation-CN 10", "N Mineralisation-CN 14","N Mineralisation-CN 18", "N Mineralisation-CN 22" };

	functorprop.FuncName = MyFunc::HUMUS_TURNOVER; functorprop.Arg = MyArgType::TURNOVER_D3;
	funcname = "Mineralisation of Humus";
	pFunc = new Func(SIMPROFILE, NITROGEN, NOFPROC, MINERALISATION, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar);


	functorprop.FuncName = MyFunc::HUMUS_CNP_TURNOVER; functorprop.Arg = MyArgType::TURNOVER_D5;
	funcname = "Decomposition of Humus with NP Mineralisation";

	ps_vec.push_back(m_pModelMap->GetPsPointer("P Immob MaxAvailFrac"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("CP Ratio Microbe"));

	pFunc = new Func(PLANT, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec);

	functorprop.FuncName = MyFunc::STEM_LITTERFALL_TSUM; functorprop.Arg = MyArgType::T_D1;
	funcname = "Decomposition - Turnover";
	p_vec.clear();


	pPs = new Ps(&UptOrgRateCoef_L, TOTPROFILE, NITROGEN, NOFPROC, PLANTORGUPTAKE, NORMAL);
	Define(pPs, "Upt OrgRateCoef L", 0.0001, "-", "Organic Uptake >=1", 0., 0.7);
	pPs = new Ps(&UptOrgRateCoef_H, TOTPROFILE, NITROGEN, NOFPROC, PLANTORGUPTAKE, NORMAL);
	Define(pPs, "Upt OrgRateCoef H", 0.0001, "-", "Organic Uptake >=1", 0., 0.7);

	pPs = new Ps(&P_UptOrgRateCoef_L, TOTPROFILE, PHOSPHORUS, NOFPROC, PLANTORGUPTAKE, NORMAL);
	Define(pPs, "Upt P OrgRateCoef L", 0.0001, "-", "Organic Uptake >=1|Phosphorus>0", 0., 0.7);
	pPs = new Ps(&P_UptOrgRateCoef_H, TOTPROFILE, PHOSPHORUS, NOFPROC, PLANTORGUPTAKE, NORMAL); //Add P_ By HH
	Define(pPs, "Upt P OrgRateCoef H", 0.0001, "-", "Organic Uptake >=1|Phosphorus>0", 0., 0.7);

	pPs = new Ps(&NUptRootCoefOrg, TOTPROFILE, NITROGEN, NOFPROC, PLANTORGUPTAKE, NORMAL);
	Define(pPs, "NUptRootCoefOrg", 0.1, "-", "Organic Uptake >=1|Root_density_Influence>0", 0.000001, 1.0);


	pPs = new Ps(&FungiOrgEnhanceCoef, TOTPROFILE, NITROGEN, NOFPROC, PLANTORGUPTAKE, NORMAL);
	Define(pPs, "FungiOrgEnhanceCoef", 5., "-", "Organic Uptake >=1|Mycorrhiza Fungal Growth>0", 0., 10.);


	pPs = new Ps(&FungiOrgPEnhanceCoef, TOTPROFILE, PHOSPHORUS, NOFPROC, PLANTORGUPTAKE, NORMAL);
	Define(pPs, "FungiOrgPEnhanceCoef", 5., "-", "Organic Uptake >=1|Mycorrhiza Fungal Growth>0|Phosphorus>0", 0., 50.);











	bool (NC_Organic_Functions::*fpoint)();
	fpoint=&NC_Organic_Functions::ReCalcOrgProfile;
		pFunc= new Func(MPROFILE,ORGANIC, STORAGE,NOBPROC, NORMAL);
	
	return true;
}
bool NC_Organic_Functions::ReCalcOrgProfile() {

vector<double> Init_L1_C_Meas, Init_L2_C_Meas, Init_F_C_Meas, Init_M_C_Meas, Init_H_C_Meas;
vector<double> Init_L1_N_Meas, Init_L2_N_Meas, Init_F_N_Meas, Init_M_N_Meas, Init_H_N_Meas;

	Init_L1_C_Meas.resize(Init_Measured_BD.size());
	Init_L2_C_Meas.resize(Init_Measured_BD.size());
	Init_F_C_Meas.resize(Init_Measured_BD.size());
	Init_M_C_Meas.resize(Init_Measured_BD.size());
	Init_H_C_Meas.resize(Init_Measured_BD.size());

	Init_L1_N_Meas.resize(Init_Measured_BD.size());
	Init_L2_N_Meas.resize(Init_Measured_BD.size());
	Init_F_N_Meas.resize(Init_Measured_BD.size());
	Init_M_N_Meas.resize(Init_Measured_BD.size());
	Init_H_N_Meas.resize(Init_Measured_BD.size());

	if(LitterPools==0) for(size_t i=0; i<Init_Assumed_Frac_Litter2.size();i++) Init_Assumed_Frac_Litter2[i]=0.;

	for(size_t i=0; i<Init_Measured_BD.size();i++) {
		double MeasCN;
		MeasCN=Init_Measured_C_Cont[i]/Init_Measured_N_Cont[i];
		double relFraclitter=(MeasCN-Init_Assumed_CN_Humus[i])/(Init_Assumed_CN_Litter[i]-Init_Assumed_CN_Humus[i]);
		if(relFraclitter<0.001) relFraclitter=0.001;
		if(relFraclitter>1.) relFraclitter=1.;

			Init_L1_C_Meas[i]=Init_Measured_C_Cont[i]*relFraclitter*(1.-Init_Assumed_Frac_Litter2[i]);
			Init_L2_C_Meas[i]=Init_Measured_C_Cont[i]*relFraclitter*Init_Assumed_Frac_Litter2[i];
			Init_H_C_Meas[i]=Init_Measured_C_Cont[i]*(1.-relFraclitter);
			Init_L1_N_Meas[i]=Init_L1_C_Meas[i]/Init_Assumed_CN_Litter[i];
			Init_L2_N_Meas[i]=Init_L2_C_Meas[i]/Init_Assumed_CN_Litter[i];
			Init_H_N_Meas[i]=Init_H_C_Meas[i]/Init_Assumed_CN_Humus[i];

	}
	function<vector<double>(vector<double>&, vector<double>&, bool)> f_conv;

	f_conv = FromMeasuredLayersToModelLayers(m_pSoil->UpperDepth, m_pSoil->LowerDepth, m_pSoil->DepthLayers, m_pSoil->ThicknessLayers, m_pSoil->DepthMidpLayer);


	O_State[_L1][_C_]= f_conv(Init_L1_C_Meas,Init_Measured_BD, true);
	O_State[_L2][_C_] =f_conv(Init_L2_C_Meas,Init_Measured_BD, true);
	O_State[_H][_C_] = f_conv(Init_H_C_Meas, Init_Measured_BD, true);

	O_State[_L1][_N_] = f_conv(Init_L1_N_Meas,Init_Measured_BD, true);
	O_State[_L2][_N_] = f_conv(Init_L2_N_Meas,Init_Measured_BD, true);
	O_State[_H][_N_] = f_conv(Init_H_N_Meas,Init_Measured_BD, true);

	return true;
}


bool NC_Organic_Functions::MakeDisplacement2() {

	double  deltaCtot; //C decides about movement (main mass), N follows according C:N ratio
	double  actCtot;


	for(size_t i=0; i<O_State[_L1][_N_].size()-1; i++) {    //go through all layers, start with uppermost, stop one before lowest

                //		   deltaCLitter1                     deltaCLitter2                     deltaCMicrobes                   deltaCHumus
        deltaCtot = (O_State[_L1][_C_][i]-Prev_O_State[_L1][_C_][i]) + (O_State[_L2][_C_][i]- Prev_O_State[_L1][_C_][i]) + 
			(O_State[_MIC][_C_][i]- Prev_O_State[_MIC][_C_][i]) + (O_State[_H][_C_][i]- Prev_O_State[_H][_C_][i]);

		if(deltaCtot >= 0)    //accumulation => move from current layer to lower layer
			actCtot = O_State[_L1][_C_][i]+O_State[_L2][_C_][i]+O_State[_MIC][_C_][i]+O_State[_H][_C_][i];
		else    //loss => move from lower layer to current layer
			actCtot = O_State[_L1][_C_][i+1]+O_State[_L2][_C_][i+1]+O_State[_MIC][_C_][i+1]+O_State[_H][_C_][i+1];
	
		O_State[_L1][_C_][i]-=deltaCtot * O_State[_L1][_C_][i] / actCtot; 
		O_State[_L1][_C_][i+1]+=deltaCtot * O_State[_L1][_C_][i] / actCtot;

		O_State[_L2][_C_][i]-=deltaCtot * O_State[_L2][_C_][i] / actCtot; 
		O_State[_L2][_C_][i+1]+=deltaCtot * O_State[_L2][_C_][i] / actCtot;

		O_State[_MIC][_C_][i]-=deltaCtot * O_State[_MIC][_C_][i] / actCtot; 
		O_State[_MIC][_C_][i+1]+=deltaCtot * O_State[_MIC][_C_][i] / actCtot;

		O_State[_H][_C_][i]-=deltaCtot * O_State[_H][_C_][i] / actCtot; 
		O_State[_H][_C_][i+1]+=deltaCtot * O_State[_H][_C_][i] / actCtot;

			
		// shift in N =              shift in C               *        N/C ratio
		for (size_t j = 0; j < NUM_ORG_FORMS; j++) {
			for (size_t jj = 1; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
				if (O_State[j][jj][i] > (deltaCtot * O_State[j][_C_][i] / actCtot * O_State[j][jj][i] / O_State[j][_C_][i])) {
					O_State[j][jj][i] -= (deltaCtot * O_State[j][_C_][i] / actCtot)  * O_State[j][jj][i] / O_State[j][_C_][i];
					O_State[j][jj][i + 1] += (deltaCtot * O_State[j][_C_][i] / actCtot)  * O_State[j][jj][i] / O_State[j][_C_][i];
				}
			}
		}
		
	}
	for (size_t j = 0; j < NUM_ORG_FORMS; j++) {
		for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++)
			Prev_O_State[j][jj].assign(O_State[j][jj].begin(), O_State[j][jj].end());
	}

	return true;

}
bool NC_Organic_Functions::MakeDisplacement() {
	double shift;
	for (size_t j = 0; j < NUM_ORG_FORMS; j++) {
		for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++)
			for (size_t i = 0; i < O_State[j][jj].size() - 1; i++) {
				shift = O_State[j][jj][i] - Prev_O_State[j][jj][i];
				O_State[j][jj][i] -= shift;
				O_State[j][jj][i + 1] += shift;
			}
	}
	return true;
}




