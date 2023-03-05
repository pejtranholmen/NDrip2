#include "NC_Plant.h"
#include "./Functors/NC_Plant_Functors.h"
#include "../SOIL_NC/SoilOrgModel.h"
#include "../SOIL_NC/NC_SoilManagement.h"
#include "../SOIL_NC/NC_Mineral_Functions.h"
#include "../Structure/Structure.h"
#include "../Structure/Functors/Structure_Functors.h"
#include "../../Simulator/Simulator.h"



class ZadokOutCreateFile{

public:	 
	ZadokOutCreateFile(bool australia) {
		countstage = 0; shift = true;
		Australia = australia;
		if (Australia) {
			sitename = { "Eradu","LakeBolac","Minnipa","SpringRidge","Bungunya","Corrigin","Nangwee","Temora","Turretfield","Walpeup" };

			ZadokOutValues = { 0., 10.f, 30.f, 60.f, 90.f };
		}
		else {
			sitename = { "FORESTE", "FORESTE","MERY_SUR_SEINE", "ROUVRES_EN_PLAINE", "Cesseville", "IVILLE" , "VILLETTES", "EPREVILLE_PRES_LE_NEUBOURG",
			"CRESTOT", "OUZOUER_LE_MARCHE", "BIGNAN" ,"BOIGNEVILLE" };
			ZadokOutValues = { 0.f, 10.f, 30.f, 55.f };

		}
	};
	~ZadokOutCreateFile(void) {};
	void Reset(bool clearstrings) {
	
		outfile.close();
		if (Australia)
			outfile.open("CSIRO_Results_CoupModel_LisbetLewan.txt", std::ios::app);
		else {
			outfile.open("France_Results_CoupModel_LisbetLewan.txt.txt", std::ios::app);
			shift = true;	
			ZadokDates.clear();
		}
		if (clearstrings) {
			zadokoutstrings.clear();
			
		}
	
	
	
	};
	double Update(double Zadok, int DayNum, size_t ActMin ,int FileIndex) {

		int Year;
		//m_pModelInfo->
		Year = PGUtil::YearFunction(ActMin);

		auto FixDatum = [&](int Year, int DayNum) {
			size_t min = PGUtil::cminutx(Year, 1, 1, 12, 00);
			size_t mindiff = (DayNum - 1) * 1440;
			size_t minact = min + mindiff;
			std::string datumstr = PGUtil::StringDatum(minact);
			std::string out = datumstr.substr(8, 2) + "/" + datumstr.substr(5, 2) + "/" + datumstr.substr(0, 4);
			return out;
		};
		auto MakeLine = [&]() {
			string outline;
			outline = ZadokDates[0].substr(6, 4) + "\t";
			
			if(sitename.size()>FileIndex)
				outline += sitename[FileIndex - 1] + "\t";
			else
				outline += "Site\t";

			if(!Australia) outline+= "Variety\t";
			for (size_t i = 0; i < ZadokDates.size(); i++) { outline += ZadokDates[i]; if (i < ZadokDates.size()-1) outline += "\t"; }

			outline += "\n";
			zadokoutstrings.push_back(outline);
			if (!Australia) ZadokDates.clear();
			
		};


		if (Zadok <= .1&&shift) {
			if (ZadokDates.size() == 0) {
				if(Australia) ZadokDates.resize(5);
				else ZadokDates.resize(4);
				countstage = 0;
			}
			shift = false;
			ZadokDates[countstage] = FixDatum(Year, DayNum);
		}
		
		if (Zadok > ZadokOutValues[countstage]&&!shift) {

			ZadokDates[countstage] = FixDatum(Year, DayNum);
			countstage++;
			if (countstage == ZadokOutValues.size()) {
				MakeLine();
				countstage = 0;
				shift = true;
				return ZadokOutValues[ZadokOutValues.size()-1];

			}
			return ZadokOutValues[countstage];
			
		}
		return ZadokOutValues[countstage];

	};
	void   CreateFileOutput(size_t ActualNoMulRun) {
	
		if (((ActualNoMulRun) % 3 == 0 && zadokoutstrings.size() > 2)&&Australia) {
			for (size_t i = 0; i < (zadokoutstrings.size() / 3); i++) {
				for (size_t j = 0; j < 3; j++) {
					string str = zadokoutstrings[i + j * zadokoutstrings.size() / 3];
					outfile.write((char*)&str[0], str.size());
				}
			}

		}
		else if (!Australia) {
			for (size_t i = 0; i < zadokoutstrings.size(); i++) {			
					string str = zadokoutstrings[i];
					outfile.write((char*)&str[0], str.size());			
			}



		}
		outfile.close();
	
	
	
	
	};
private:

	double CurrentDayShift;
	bool Australia;
	bool shift;
	int countstage;
	vector<string> sitename;
	fstream outfile;
	vector<string> ZadokDates, zadokoutstrings;
	vector<float>  ZadokOutValues;

};
class SaltImpactonGrowthRespiration {
public:
	SaltImpactonGrowthRespiration(NC_Plant *pNCPlant) : pNC_Plant(pNCPlant) {	}
	double operator() (PlantComp comp, size_t i) {
		switch (comp) {
		case PlantComp::LEAF:
			return (1 - pNC_Plant->GPP_Driver.Salt[i])*pNC_Plant->P_CarbonAlloNewMobile.Leaf[i];//P_CarbonAlloNewMobile.Leaf[i];
		case PlantComp::STEM:
			return (1 - pNC_Plant->GPP_Driver.Salt[i])*pNC_Plant->P_CarbonAlloNewMobile.Stem[i];
		case PlantComp::ROOT:
			return (1 - pNC_Plant->GPP_Driver.Salt[i])*pNC_Plant->P_CarbonAlloNewMobile.Roots[i];
		case PlantComp::COARSEROOT:
			return (1 - pNC_Plant->GPP_Driver.Salt[i])*pNC_Plant->P_CarbonAlloNewMobile.CoarseRoots[i];
		default:
			return 0.;
		}
	}

private:
	NC_Plant *pNC_Plant;

};



class NuptakeImpactonRespiration {
public:
NuptakeImpactonRespiration(vector<double> RespNO3, vector<double> RespNH4, vector<double> RespNOrgCoef, NC_Organic *pNCSoil) :
RespNO3(RespNO3), RespNH4(RespNH4), RespNOrgCoef(RespNOrgCoef), pNCSoil(pNCSoil)
{}
double operator() (size_t layer, size_t iplant) {
return pNCSoil->SoilNitrateUptake[layer] * RespNO3[iplant]
+ pNCSoil->SoilAmmoniumUptake[layer] * RespNH4[iplant]
+ (pNCSoil->O_Transform[L1_PLANTUPTAKE][_N_][layer] + pNCSoil->O_Transform[H_PLANTUPTAKE][_N_][layer])*RespNOrgCoef[iplant];
}


private:
vector<double> RespNO3, RespNH4, RespNOrgCoef; // Parameters
NC_Organic* pNCSoil;
};

void NC_Plant::setNC_Plant(ModelInfo* pInfo, ModelMap* pMap, Structure* pStruc)
{
	ModuleName = "Plant Growth";
	ModuleNo = 24;
	ModuleConditions = "Nitrogen and Carbon>0";
	pZadokOutCreateFile = nullptr;
	m_pModelInfo = pInfo; m_pModelMap = pMap; m_pModelStructure = pStruc;
	pT_Step = &m_pModelInfo->T_Step;
	T_Step = *pT_Step;
};
						   
NC_Plant::NC_Plant(void)
{
	ModuleName="Plant Growth";
	ModuleNo=24;
	ModuleConditions="";
	pZadokOutCreateFile = nullptr;
};

bool NC_Plant::Ini()	{

	m_ICountDay = m_pModelInfo->JDayNum + 1;
	m_ResetMade = false;

	if((m_pModelInfo->ActualNoMulRun-1)%3==0) zadokoutstrings.clear();
	if (ZadokScale_Sw(ZadokScale) ==ZadokScale_Sw::withOutPut2File) {
		if(pZadokOutCreateFile==nullptr) pZadokOutCreateFile = new ZadokOutCreateFile(true);
		if ((m_pModelInfo->ActualNoMulRun - 1) % 3 == 0) pZadokOutCreateFile->Reset(true);
		else pZadokOutCreateFile->Reset(false);
	}
	else if (ZadokScale_Sw(ZadokScale) == ZadokScale_Sw::withOutPut3File) {
		if (pZadokOutCreateFile == nullptr) pZadokOutCreateFile = new ZadokOutCreateFile(false);
		 pZadokOutCreateFile->Reset(true);

	}
	if (ZadokForcingFromFileInput_Sw(ZadokForcingFromFileInput) == ZadokForcingFromFileInput_Sw::on) {

			string filename = m_PG_Zadok.GetFileName();
			size_t LL = filename.rfind("_");
			m_PG_Zadok.ReOpen();
			auto numrec = m_PG_Zadok.GetNumRecords();
			auto numvar = m_PG_Zadok.GetNumVariables();
			vector<pair<size_t, size_t>> sowvector, emergencevector, harvestvector;
			for (size_t irec = 1; irec <= numrec; irec++) {
				auto min=m_PG_Zadok.GetDate(irec);
				auto zadok_value = m_PG_Zadok.GetVarValueRead(1, irec);
				auto daynum = m_PG_Zadok.GetDayNumber(irec);
				auto year = m_PG_Zadok.GetYear(irec);
				if (zadok_value == 0) {
					sowvector.push_back(pair<size_t, size_t>(year, daynum));
				}
				else if (zadok_value == 10) {
					emergencevector.push_back(pair<size_t, size_t>(year, daynum));
				}
				else if (zadok_value == 95) {
					harvestvector.push_back(pair<size_t, size_t>(year, daynum+2));
				}
			}

			f_sow = DayNoControl(sowvector);
			f_emergence = DayNoControl(emergencevector);
			f_harvest = DayNoControl(harvestvector);


	}



	NC_Drivings::Ini();
	
	NC_Organic* pNCSoil;
	auto PointerstoOtherModules = [&]() {
		SoilOrgModel* p_Soil = (SoilOrgModel*)m_pModelInfo->GetModelPtr("Soil Org Model");
		pNCSoil = &p_Soil->m_NC_SoilOrg;	
		//m_pModelStructure = ((Simulator*)m_pModelInfo)->GetSimStructurePointer();
	};
	PointerstoOtherModules();


	auto SetInitialValueforStateVariables = [&]() {
		DoHarvest = false;
		DoFinalHarvest = true;
		DoCleaning = false;
		YearToClean = false;
		CleaningToBeDone = true;
		GrowthTempSum.assign(pPlant->NumPlants, 0.0);
		DormingTempSum.assign(pPlant->NumPlants, 0.0);
		SpringDevDayNum.assign(pPlant->NumPlants, 0.0);
		DormingStartDayNum.assign(pPlant->NumPlants, 0.0);
		PreviousThinningYear = 0;
		ICountHarvestCycle = 1;
		m_pModelInfo->StartStateValue = false;

		m_pModelStructure->m_NumActPlantElements = 2;
		if (m_pModelStructure->Phosphorus_Model > 0) m_pModelStructure->m_NumActPlantElements = 3;

		if (!m_pModelInfo->StartStateValue) {

			TotalPlantLitterFall->assign(m_pModelStructure->m_NumActPlantElements,0.);

			AccNPP_Plant.assign(pPlant->NumPlants, 0.0);
			AccNPP_Roots.assign(pPlant->NumPlants, 0.0);
			AccNPP_CoarseRoots.assign(pPlant->NumPlants, 0.0);
			AccNPP_Stem.assign(pPlant->NumPlants, 0.0);
			AccNPP_Leaf.assign(pPlant->NumPlants, 0.0);
			P_CarbonAlloNewMobile.Leaf.resize(pPlant->NumPlants);
			P_CarbonAlloNewMobile.Stem.resize(pPlant->NumPlants);
			P_CarbonAlloNewMobile.Roots.resize(pPlant->NumPlants);
			P_CarbonAlloNewMobile.CoarseRoots.resize(pPlant->NumPlants);
			for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
				TotalPlantLitterFall[jj].assign(pPlant->NumPlants,0.);
  				for (size_t j = 0; j < NUM_REALLOCATION; j++)
					P_ReAllocation[j][jj].assign(pPlant->NumPlants, 0.0);
				for (size_t j = 0; j < NUM_PLANT_FUNGI_COMP; j++) {
					P_State[j][jj].assign(pPlant->NumPlants, 0.0);
					P_Respiration[j].assign(pPlant->NumPlants, 0.0);
					P_LitterFall[j][jj].assign(pPlant->NumPlants, 0.0);
					if(j< NUM_PLANT_FUNGI_COMP-1) P_Harvest[j][jj].assign(pPlant->NumPlants, 0.0);
					P_State_IniValue[j][jj].resize(pPlant->NumPlants);
					P_State_IniCarbonRatio[j][jj].resize(pPlant->NumPlants);
					P_State_CarbonRatio[j][jj].resize(pPlant->NumPlants);
				}
				if (jj > 0) {
					for (size_t j = 0; j < NUM_UPTAKE; j++)
						P_Uptake[j][jj - 1].assign(pPlant->NumPlants, 0.0);
					P_UptakeNewMobile[jj-1].Leaf.resize(pPlant->NumPlants);
					P_UptakeNewMobile[jj - 1].Stem.resize(pPlant->NumPlants);
					P_UptakeNewMobile[jj - 1].Roots.resize(pPlant->NumPlants);
					P_UptakeNewMobile[jj - 1].CoarseRoots.resize(pPlant->NumPlants);
				}
				for (size_t j = 0; j < 2; j++)
					pNCSoil->SurfaceLitter[j][jj] = 0.0;
				P_SumPlant[_ROOT_ALL][jj].resize(pPlant->NumPlants);
				P_SumPlant[_PLANT_WHOLE][jj].resize(pPlant->NumPlants);
				P_SumPlant[_LEAF_ALL][jj].resize(pPlant->NumPlants);
				P_SumPlant[_ABOVE_GROUND][jj].resize(pPlant->NumPlants);
				C_RatioPlant[jj].resize(pPlant->NumPlants);
			}

			NRootUptake.assign(pPlant->NumPlants, 0.0);//HH initi
			if (m_pModelStructure->Phosphorus_Model > 0) PRootUptake.assign(pPlant->NumPlants, 0.0);//HH P initi
			for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
				for (size_t j = _LEAF; j < NUM_PLANT_FUNGI_COMP; j++) {
					if (j == _MOBILE&& Winter_Regulation_Sw(Winter_Regulation) == Winter_Regulation_Sw::off) continue;
					for (size_t i = 0; i < pPlant->NumPlants; i++) {
						if (Emergence_Switch_Sw(Emergence_Switch) == Emergence_Switch_Sw::off|| Winter_Regulation_Sw(Winter_Regulation) == Winter_Regulation_Sw::on)
							P_State[j][jj][i] = P_State_IniValue[j][jj][i];
						else
							P_State[j][jj][i] = 0.;
					}
				}
			};


			for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
				for (size_t j = _LEAF; j < NUM_PLANT_FUNGI_COMP; j++) {
					if (FungalGrowth_Sw(FungalGrowth) > FungalGrowth_Sw::off) {
						for (size_t i = 0; i < pPlant->NumPlants; i++) {
							P_State[_FUNGI][jj][i] = P_State_IniValue[_ROOT][jj][i] * AlloFungiC1[i];
							P_State[_ROOT][jj][i] = P_State_IniValue[_ROOT][jj][i] * (1. - AlloFungiC1[i]);
							if(jj>0)
								P_State_CarbonRatio[_FUNGI][jj-1][i] =  P_State[_FUNGI][_C_][i]/ P_State[_FUNGI][jj][i];//HH
						}
					}
				}
				if (jj == _N_ && InitialPlantConditions_Sw(InitialPlantConditions) == InitialPlantConditions_Sw::As_Nitrogen) {
					for (size_t j = _LEAF; j < NUM_PLANT_FUNGI_COMP; j++) {
						//if (j != _MOBILE) {
							for (size_t i = 0; i < pPlant->NumPlants; i++)
								if (Emergence_Switch_Sw(Emergence_Switch) == Emergence_Switch_Sw::off|| Winter_Regulation_Sw(Winter_Regulation) == Winter_Regulation_Sw::on)
									P_State[j][_C_][i] = P_State_IniValue[j][_N_][i] * P_State_IniCarbonRatio[j][_N_ - 1][i];
								else
									P_State[j][_C_][i] = 0.;
						//}

					}
				}//HH
				if (jj == _P_ && InitialPlantPConditions_Sw(InitialPlantPConditions) == InitialPlantPConditions_Sw::As_CP_ratio_and_Carbon) {
					for (size_t j = _LEAF; j < NUM_PLANT_FUNGI_COMP; j++) {
						//if (j != _MOBILE) {
							for (size_t i = 0; i < pPlant->NumPlants; i++)
								P_State[j][_P_][i] = P_State_IniValue[j][_C_][i] / P_State_IniCarbonRatio[j][_P_ - 1][i];
						//}
					}
				}
			}
			if (FungalGrowth > 0) {
				for (size_t i = 0; i < P_State[_ROOT][_C_].size(); i++) {
					if (FungalGrowth == 2) {
						Fungi_LitterRateC1[i] = (Fungi_MycFrac[i] * Fungi_MycRate[i]) + (1 - Fungi_MycFrac[i]) * Fungi_MantleRate[i];
						Fungi_LitterRateC2[i] = Fungi_LitterRateC1[i];
					}
				}
			}
			auto GetAllRoots = [&](size_t i_plant, size_t jj_element) {
				double out = 0; for (size_t j = _ROOT; j <= _OLD_COARSE_ROOT; j++)
					out += P_State[j][jj_element][i_plant];	return out;	};
			auto GetEntirePlant = [&](size_t i_plant, size_t jj_element) {
				double out = P_SumPlant[_ROOT_ALL][jj_element][i_plant];
				for (size_t j = _NEWMOBILE; j <= _OLD_STEM; j++)	out += P_State[j][jj_element][i_plant];
				return out;	};
			auto GetLeaves = [&](size_t i_plant, size_t jj_element) {
				return P_State[_LEAF][jj_element][i_plant] + P_State[_OLD_LEAF][jj_element][i_plant]; };

			for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
				for (size_t i = 0; i < pPlant->NumPlants; i++) {
					P_SumPlant[_ROOT_ALL][jj][i] = GetAllRoots(i, jj);
					P_SumPlant[_PLANT_WHOLE][jj][i] = GetEntirePlant(i, jj);
					P_SumPlant[_LEAF_ALL][jj][i] = GetLeaves(i, jj);
				}
				P_AllPlants[_ROOT_ALL][jj] = sum(P_SumPlant[_ROOT_ALL][jj]);
				P_AllPlants[_PLANT_WHOLE][jj] = sum(P_SumPlant[_PLANT_WHOLE][jj]);
				P_AllPlants[_LEAF_ALL][jj] = sum(P_SumPlant[_LEAF_ALL][jj]);
			}

			for (size_t i = 0; i < Initial_GSI.size(); i++) {
				SimPlantAge[i] = InitPlantAge[i];
				SimPlantYearAge[i] = SimPlantAge[i] / 365.25 + 0.1;
				if (Initial_GSI[i] >= -1.&&Initial_GSI[i] < 5)
					GrowthStageIndex[i] = Initial_GSI[i];
				else {
					Initial_GSI[i] = 1;
					GrowthStageIndex[i] = 1;
				}
				GrowthTempSum[i] = 0.;
				DormingTempSum[i] = 0.;
			}
		} // StartValues
		C_Seed_Added = 0.;
		N_Seed_Added = 0.;
		P_Seed_Added = 0.;
	};
	auto SetSizeofVariablesandOtherSettings = [&]() {
		if (FungalGrowth > 0) {
			for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++) {//HH
				P_AllPlants[_PLANT_WHOLE][jj] = sum(P_SumPlant[_PLANT_WHOLE][jj]);
				P_AllPlants[_FUNGI_][jj] = sum(P_State[_FUNGI][jj]);
			}

		}

		// Set Positions for AboveGroundLitterfalls, introduced Oct 2019
		SoilLayerForLitterInput.resize(pPlant->NumPlants);
		for (size_t i = 0; i < pPlant->NumPlants; ++i) {
			if (AboveGroundLitterFall_Position4Target[i] < 0.) {
				for (size_t isoil = 0; isoil < pSoil->NL; ++isoil) {
					if (pSoil->DepthLayers[isoil] < abs(AboveGroundLitterFall_Position4Target[i]))
						SoilLayerForLitterInput[i] = isoil + 1;
				}
			}
			else
				SoilLayerForLitterInput[i] = string::npos;

		}

		if (P_AllPlants[_PLANT_WHOLE][_N_] > 0) C_RatioTotPlant[_N_ - 1] = P_AllPlants[_PLANT_WHOLE][_C_] / P_AllPlants[_PLANT_WHOLE][_N_];
		if (P_AllPlants[_ROOT_ALL][_N_] > 0) C_RatioTotRoots[_N_ - 1] = P_AllPlants[_ROOT_ALL][_C_] / P_AllPlants[_ROOT_ALL][_N_];
		if (P_AllPlants[_PLANT_WHOLE][_P_] > 0) C_RatioTotPlant[_P_ - 1] = P_AllPlants[_PLANT_WHOLE][_C_] / P_AllPlants[_PLANT_WHOLE][_P_];
		if (P_AllPlants[_ROOT_ALL][_P_] > 0) C_RatioTotRoots[_P_ - 1] = P_AllPlants[_ROOT_ALL][_C_] / P_AllPlants[_ROOT_ALL][_P_];
		GrowingSum.resize(pPlant->NumPlants, 0);
		DormingSum.resize(pPlant->NumPlants, 0);
		ZadokIndex.resize(pPlant->NumPlants, 0);

		HarvestToBeDone.resize(pPlant->NumPlants, false);
		HarvestDoneThisYear.resize(pPlant->NumPlants, false);
		YearShiftToBeDone.resize(pPlant->NumPlants, false);

		ResetHarvestToBeDoneDay.resize(pPlant->NumPlants, 0);


		for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++)
			RemainOldLeaf[jj].resize(pPlant->NumPlants, 0);


		ResponseWeightCoef.resize(pPlant->NumPlants, 0);
		PreviousCPlant.resize(pPlant->NumPlants, 0);
		for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++)
			for (size_t i = 0; i < HarvestToBeDone.size(); i++) {
				RemainOldLeaf[jj][i] = P_State[_OLD_LEAF][jj][i] * (1. - 1. / max(1., (Max_Leaf_Lifetime[i] - 1.)));
				HarvestToBeDone[i] = true;
				YearShiftToBeDone[i] = true;
				HarvestDoneThisYear[i] = false;
				PreviousCPlant[i] = 0.0;
				P_State[_NEWMOBILE][jj][i] = 0.;

				ResetHarvestToBeDoneDay[i] = 365;

				GrowingSum[i] = 0.;
				DormingSum[i] = 0.;
			}
		ResetYearShiftDay = 2;

		ContinStartSum = 0.;
		ContinStopSum = 0.;
		SpringDev = true;
		Growing = true;
		Emergence_Done.resize(pPlant->NumPlants);
		if (!Winter_Regulation) {
			Growing = true;
			SpringDev = false;

			for (size_t i = 0; i < Initial_GSI.size(); i++)    if (Initial_GSI[i] > 1.) Emergence_Done[i] = false;
		}
		else {
			SpringDev = false;
			for (size_t i = 0; i < Initial_GSI.size(); i++)    if (Initial_GSI[i] <= 1.) Emergence_Done[i] = false;

		}
	
		

		UptakeDistWithDepth.resize(pPlant->NumPlants);
		for (size_t i = 0; i < UptakeDistWithDepth.size(); i++)
			UptakeDistWithDepth[i].resize(pSoil->NL);
		pPlant->Num_NUptMax = pSoil->NL;


		//UptakeDistWithDepth.resize(pSoil->NL*pPlant->NumPlants);
		UptakeDistTotal[0].resize(pSoil->NL);
		UptakeFungiDistTotal[0].resize(pSoil->NL);
		NUptakeDeficitTotal.resize(pPlant->NumPlants);
		UptakeDistTotal[1].resize(pSoil->NL);
		UptakeFungiDistTotal[1].resize(pSoil->NL);
		PUptakeDeficitTotal.resize(pPlant->NumPlants);

		NUptakeDeficitTotal.assign(pSoil->NL, 0.0);
		UptakeDistTotal[0].assign(pSoil->NL, 0.0);
		UptakeFungiDistTotal[0].assign(pSoil->NL, 0.0);
		PUptakeDeficitTotal.assign(pSoil->NL, 0.0);
		UptakeDistTotal[1].assign(pSoil->NL, 0.0);
		UptakeFungiDistTotal[1].assign(pSoil->NL, 0.0);
		//UptakeDistWithDepth=0.
		if (Growth == 1)
			Max_Plant_LifeTime.assign(pPlant->NumPlants, 0.0);


		if (Growth == 4) {//Allocated(EYY)) {
			EYY.resize(pPlant->NumPlants);
			PCo2y.resize(pPlant->NumPlants);
			FarqRange.resize(pPlant->NumPlants);
			Gammas.resize(pPlant->NumPlants);
			VmaxRubisco.resize(pPlant->NumPlants);
			Vmax.resize(pPlant->NumPlants);
			Jmax.resize(pPlant->NumPlants);
			PCo2s.resize(pPlant->NumPlants);
			ASSIMNY.resize(pPlant->NumPlants);
			ASSIMY.resize(pPlant->NumPlants);
			PCo2I.resize(pPlant->NumPlants);
			GBoundary.resize(pPlant->NumPlants);
			GStomata.resize(pPlant->NumPlants);
			GAir.resize(pPlant->NumPlants);
			Qt.resize(pPlant->NumPlants);
			MMO2V.resize(pPlant->NumPlants);
			ICConv.resize(pPlant->NumPlants);
			IGath.resize(pPlant->NumPlants);

			for (size_t i = 0; i < pPlant->NumPlants; i++) {
				EYY[i].resize(6);
				PCo2y[i].resize(6);
				ASSIMNY[i].resize(6);
				ASSIMY[i].resize(6);



			}
		}

	
		GPP_Current.Light.resize(pPlant->NumPlants);
		GPP_Current.Nitrogen.resize(pPlant->NumPlants);
		GPP_Current.Water.resize(pPlant->NumPlants);
		GPP_Current.Temp.resize(pPlant->NumPlants);
		GPP_Current.Salt.resize(pPlant->NumPlants);
		GPP_Current.Nutrient_Water.resize(pPlant->NumPlants);
		GPP_Current.Phosphorus.resize(pPlant->NumPlants);
		GPP_Current.AllCombined.resize(pPlant->NumPlants);


		PCO2_Boundary = CO2_A ;


		GPP_Current.Temp.assign(pPlant->NumPlants, 1);
		GrazingRespiration = WoodEatingRespiration = WoodEaterRespFlux = C_WoodEatingRate = N_WoodEatingRate = 0.;

		if (PhotoInput == 1) {

			if (m_PG_PlantBiomass.Open(m_PG_PlantBiomass.GetFileName())) {
				CheckFromPhotoFile();
				if (NumVariablesPhotoInput > 0) {
					// INIECO(1, 1, NumValidInputPhotoInput, NUnitdriv12, IPhotoInput)
					// if(//Allocated(VUTPG_PhotoFILE)) Allocate(VUTPG_PhotoFile(NumValidInputPhotoInput))
				}
			}
			else
				return false;

		}
		if (Grazing > 0) {

			CLeafGrazing.assign(pPlant->NumPlants, 0.);
			COldLeafGrazing.assign(pPlant->NumPlants, 0.);
			CStemGrazing.assign(pPlant->NumPlants, 0.);
			NLeafGrazing.assign(pPlant->NumPlants, 0.);
			NOldLeafGrazing.assign(pPlant->NumPlants, 0.);
			NStemGrazing.assign(pPlant->NumPlants, 0.);
			if (GrazingClosure == 0)
				C_GrazingManureFraction = min(1. - GrazingRespFraction - C_GrazingExportFraction, C_GrazingManureFraction);
			else
				C_GrazingGrowthFraction = min(1. - GrazingRespFraction - C_GrazingExportFraction, C_GrazingGrowthFraction);
		}
		if (Grazing == 2) {

			if (m_PG_Grazing.Open(m_PG_Grazing.GetFileName())) {
				CheckFromGrazingFile();
			}
			else
				return false;

		}

		if (Harvest_Day_Sw(Harvest_Day) == Harvest_Day_Sw::PG_File_specified) {
			m_PG_HarvestDate.Open(m_PG_HarvestDate.GetFileName());
			NumValidHarvest = m_PG_HarvestDate.GetNumRecords();

			if (NumValidHarvest > 0. && NumValidHarvest < 1000) {
				IHarvestDates.resize(NumValidHarvest);
				for (size_t i = 0; i < NumValidHarvest; i++)
					IHarvestDates[i] = m_PG_HarvestDate.GetLongTime(i + 1);

				ICountHarvest = NumValidHarvest + 1;

				for (int i = NumValidHarvest - 1; i >= 0; i--)
					if (m_pModelInfo->m_ActMin < IHarvestDates[i]) ICountHarvest = i;

			}
			else
				NumValidHarvest = 0;

			m_PG_HarvestDate.CloseFile();


		}
		if (Seedling_Management_Sw(Seedling_Management) == Seedling_Management_Sw::After_Final_Harvest) {
			Estimated_Seedling_Year.assign(pPlant->NumPlants,0.);
		}

		return true;


	};
	auto SetManagementFunctors = [&]() {

		f_Sowing = PlantTempSum(T_Thres_Sowing, T_Sum_Sowing);
		f_Emergence = PlantTempSum(T_Thres_Emerg, T_Sum_Emerg);
		f_EmergenceWithMoistControl = PlantTempSum(T_Thres_Emerg, T_Sum_Emerg, p_Soil_WaterF->M_Wilting[0], MoistureRangeEmergence);
		f_GrainStart = GrainStart(GrainSI_Step, GrainSI_StepDay, GrainSI_ThresDay, GrainSI_StepTemp, GrainSI_ThresTemp);
		f_GrainFill = PlantTempSum(T_Thres_GrainFill, T_Sum_GrainFill);
		f_Harvest = PlantTempSum(T_Thres_Ripening, T_Sum_Ripening);
		f_DormingAndGrowthTempSum = GrowingSumF(T_Thres_Emerg, T_Thres_Dormancy, pAtm->Latitude, T_Sum_Initial);

		if (ZadokScale_Sw(ZadokScale) >= ZadokScale_Sw::on) {
			f_Zadok = ZadokFunctionFromGSI(ZadokRangeGSI2, GSI_DeltaZadok30, GSI_DeltaZadok55);
		};
	};
	f_PhotoTempSum = Get_D1_TableFunctor(MyFunc::PHOTO_TEMPSUM);
	//koll = f_PhotoTempSum(0, 50.);
	f_PhotoGrainFill = Get_D1_TableFunctor(MyFunc::PHOTO_GRAINFILL);
	//koll = f_PhotoGrainFill(0, .5);

	SetInitialValueforStateVariables();
	SetSizeofVariablesandOtherSettings();
	SetManagementFunctors();

	m_YearTest = true;
	m_CountStartYear=0;
	m_LatestSowingDay = 220.;

	return true;
}

bool NC_Plant::IniFunctors() {





	NC_Drivings::IniFunctors();

	NC_Organic* pNCSoil;
	auto PointerstoOtherModules = [&]() {
		SoilOrgModel* p_Soil = (SoilOrgModel*)m_pModelInfo->GetModelPtr("Soil Org Model");
		pNCSoil = &p_Soil->m_NC_SoilOrg;
		//m_pModelStructure = ((Simulator*)m_pModelInfo)->GetSimStructurePointer();
	};
	PointerstoOtherModules();
	auto SetBiomassFunctors = [&]() {
		f_CarbonRatioWithOldBiomass = CarbonRatio(Max_Plant_LifeTime);
		f_CarbonNitrogenRatioWithOldBiomass = CarbonRatioLeaf(Max_Plant_LifeTime, CN_RatioMaxLeaf);
		f_CarbonPhosporousRatioWithOldBiomass = CarbonRatioLeaf(Max_Plant_LifeTime, CP_RatioMaxLeaf);

		f_CarbonRatio = CarbonRatio(Max_Plant_LifeTime);
		f_CarbonNitrogenRatio = CarbonRatioLeaf(Max_Plant_LifeTime, CN_RatioMaxLeaf);
		f_CarbonPhosporousRatio = CarbonRatioLeaf(Max_Plant_LifeTime, CP_RatioMaxLeaf);

	};
	auto SetGrowthFunctors = [&]() {

		f_PhotoTempDirect = Get_D1_TableFunctor(MyFunc::PHOTO_DIRECT_TEMP);
		auto koll = f_PhotoTempDirect(0, 10.);


		if (GreenIndexForcing_Sw(GreenIndexForcing) == GreenIndexForcing_Sw::on) {
			f_PhotoGreenIndex = Get_D1_TableFunctor(MyFunc::PHOTO_GREENINDEX);
			f_PhotoTemp = PhotoTempResponse(f_PhotoTempSum, f_PhotoTempDirect, f_PhotoGreenIndex);
		}
		else
			f_PhotoTemp = PhotoTempResponse(f_PhotoTempSum, f_PhotoTempDirect);



		f_PhotoNLeaf = Get_D1_TableFunctor(MyFunc::PHOTO_NLEAF);
		koll = f_PhotoNLeaf(0, .5);
		f_PhotoPLeaf = Get_D1_TableFunctor(MyFunc::PHOTO_PLEAF);
		koll = f_PhotoPLeaf(0, .5);
		f_LogisticGrowth = Get_D1_TableFunctor(MyFunc::LOGISTIC_GROWTH);
		koll = f_LogisticGrowth(0, 200);


		if (Plant_Stress_Sw(PlantStress) >= Plant_Stress_Sw::Combined_Water_and_Nitrogen_function) {
			f_Photo_Combined_N_Water_Response = Get_D2_TableFunctor(MyFunc::PHOTO_COMBINED_N_WATER_RESPONSE);
			koll = f_Photo_Combined_N_Water_Response(0, .6, .6);
		}
		if (Growth_Sw(Growth) == Growth_Sw::Radiation_use_efficiency) {
			f_MaxGPP = Get_D1_TableFunctor(MyFunc::PHOTO_EFF_LIGHT);
		}
		if (Growth_Sw(Growth) == Growth_Sw::Farquhar) f_MaxGPP = Get_D1_TableFunctor(MyFunc::PHOTO_EFF_FARQUAR);
		if (PhoSaturation_Sw(PhoSaturation) == PhoSaturation_Sw::on) {
			f_PhotoSaturation = Get_D1_TableFunctor(MyFunc::PHOTO_SATURATION);
		}
		f_PhotoSaturation = Get_D1_TableFunctor(MyFunc::PHOTO_SATURATION);
		koll = f_PhotoSaturation(0, .5);



		if (Growth_Sw(Growth) >= Growth_Sw::Radiation_use_efficiency&&
			GrowthResponseVar_Sw(GrowthResponseVar) >= GrowthResponseVar_Sw::Mean_and_Delayed) {
			f_IntegratedAuxGPPResponse = GPP_Daily_Drivers(GPP_Driver, Growth, GrowthResponseVar, m_pModelInfo);


		}

	};
	auto SetAllocationFunctors = [&]() {
		if (Root_Alloc_N_Leaf_Sw(Root_Alloc_N_Leaf) == Root_Alloc_N_Leaf_Sw::Independent) f_CAlloRootNitrogen = Get_D1_TableFunctor(MyFunc::ALLO_ROOT_NITROGEN_FIXED);
		else if (Root_Alloc_N_Leaf_Sw(Root_Alloc_N_Leaf) == Root_Alloc_N_Leaf_Sw::Linear) f_CAlloRootNitrogen = Get_D1_TableFunctor(MyFunc::ALLO_ROOT_NITROGEN_LINEAR);
		else if (Root_Alloc_N_Leaf_Sw(Root_Alloc_N_Leaf) == Root_Alloc_N_Leaf_Sw::Exp) f_CAlloRootNitrogen = Get_D1_TableFunctor(MyFunc::ALLO_ROOT_NITROGEN_EXP);

		if (Root_Allocation_TotMass_Sw(Root_Alloc_TotMass) == Root_Allocation_TotMass_Sw::Independent) f_CAlloRootMass = Get_D1_TableFunctor(MyFunc::ALLO_ROOT_MASS_FIXED);
		else if (Root_Allocation_TotMass_Sw(Root_Alloc_TotMass) == Root_Allocation_TotMass_Sw::Linear) f_CAlloRootMass = Get_D1_TableFunctor(MyFunc::ALLO_ROOT_MASS_LINEAR);
		else if (Root_Allocation_TotMass_Sw(Root_Alloc_TotMass) == Root_Allocation_TotMass_Sw::Exp) f_CAlloRootMass = Get_D1_TableFunctor(MyFunc::ALLO_ROOT_MASS_EXP);

		if (Root_Alloc_WaterStress_Sw(Root_Alloc_WaterStress) == Root_Alloc_WaterStress_Sw::Independent) f_CAlloRootWater = Get_D1_TableFunctor(MyFunc::ALLO_ROOT_WATER_FIXED);
		else if (Root_Alloc_WaterStress_Sw(Root_Alloc_WaterStress) == Root_Alloc_WaterStress_Sw::Linear) f_CAlloRootWater = Get_D1_TableFunctor(MyFunc::ALLO_ROOT_WATER_LINEAR);
		else if (Root_Alloc_WaterStress_Sw(Root_Alloc_WaterStress) == Root_Alloc_WaterStress_Sw::Exp) f_CAlloRootWater = Get_D1_TableFunctor(MyFunc::ALLO_ROOT_WATER_EXP);

		if (Root_Alloc_P_Leaf_Sw(Root_Alloc_P_Leaf) == Root_Alloc_P_Leaf_Sw::Independent) f_CAlloRootPhosphorus = Get_D1_TableFunctor(MyFunc::ALLO_ROOT_PHOSPHORUS_FIXED);//HH
		else if (Root_Alloc_P_Leaf_Sw(Root_Alloc_P_Leaf) == Root_Alloc_P_Leaf_Sw::Linear) f_CAlloRootPhosphorus = Get_D1_TableFunctor(MyFunc::ALLO_ROOT_PHOSPHORUS_LINEAR);
		else if (Root_Alloc_P_Leaf_Sw(Root_Alloc_P_Leaf) == Root_Alloc_P_Leaf_Sw::Exp) f_CAlloRootPhosphorus = Get_D1_TableFunctor(MyFunc::ALLO_ROOT_PHOSPHORUS_EXP);
		
        f_AlloCarbonPlant = AlloNewMobile_To_All(f_CAlloRootMass, f_CAlloRootNitrogen, f_CAlloRootPhosphorus, f_CAlloRootWater, AlloRateCoef, Root_Allocation_Combination, AlloLeaf1, AlloLeaf2, AlloLeaf3, Leaf_Alloc_TotMass, AlloCoarseRoots);
        
		if (N_Fixed_Supply_Sw(N_Fixed_Supply) == N_Fixed_Supply_Sw::off) {
			f_AlloUptakePlant[0] = AlloUptake_To_All(CN_RatioMinRoots, CN_RatioMinStem, CN_RatioMinCoarseRoots);
			if(m_pModelStructure->m_NumActPlantElements>=3) 
				f_AlloUptakePlant[1] = AlloUptake_To_All(CP_RatioMinRoots, CP_RatioMinStem, CP_RatioMinCoarseRoots);
		}
		else {
			f_AlloUptakePlant[0] = AlloUptake_To_All(CN_RatioMinRoots, CN_RatioMinStem, CN_RatioMinCoarseRoots, CNLeafThres, CNLeafOpt, N_MinDegreeSupply);
			if (m_pModelStructure->m_NumActPlantElements >= 3)
				f_AlloUptakePlant[1] = AlloUptake_To_All(CP_RatioMinRoots, CP_RatioMinStem, CP_RatioMinCoarseRoots, CPLeafThres, CPLeafOpt, N_MinDegreeSupply);//HH
		}

		function<double(size_t, double)> f_Avail = NitrogenAvailableforFungi(NavailCoef);
		function<double(size_t, double, double, double)> f_Supply = FungiPlantDemand(FungiMinNSupplyCoef);
		
        f_AlloFungi = AllocationToFungi(f_Avail, f_Supply, AlloMaxC1, AlloFungiC1);
		if (m_pModelStructure->m_NumActPlantElements >= 3) {
			function<double(size_t, double)> f_AvailP = PhosphorusAvailableforFungi(PavailCoef);
			function<double(size_t, double, double, double)> f_SupplyP = FungiPlantDemand(FungiMinPSupplyCoef);
            f_AlloFungiwithP = AllocationToFungiwithP(f_Avail, f_AvailP, f_Supply, f_SupplyP, AlloMaxC1, AlloFungiC1);
			}
		f_AlloCoefRoot = AllocationToRoots(f_CAlloRootMass, f_CAlloRootNitrogen, f_CAlloRootPhosphorus, f_CAlloRootWater, AlloRateCoef, Root_Allocation_Combination);
		f_AlloCoefLeaf = AllocationToLeaf(AlloLeaf1, AlloLeaf2, AlloLeaf3,AlloRateCoef, Leaf_Alloc_TotMass);
		if (ReAllocationToLeaf_Sw(ReAllocationToLeaf) == ReAllocationToLeaf_Sw::on)
			f_RootToLeaf = FromRootToLeafAfterHarvest(AlloFromRootLeafCoef, AlloFromRootLeafRatio);
		vector<vector<double>> c_coef; c_coef.push_back(Allo_C_LeafGrain); c_coef.push_back(Allo_C_StemGrain); c_coef.push_back(Allo_C_RootsGrain);
		vector<vector<double>> N_coef; N_coef.push_back(Allo_N_LeafGrain); N_coef.push_back(Allo_N_StemGrain); N_coef.push_back(Allo_N_RootsGrain);
		vector<vector<double>> P_coef;
		if (Phos_ReAllocation_Sw(Phos_ReAllocation) == Phos_ReAllocation_Sw::As_Carbon)
			P_coef = c_coef;
		if (Phos_ReAllocation_Sw(Phos_ReAllocation) == Phos_ReAllocation_Sw::As_Nitrogen)
			P_coef = N_coef;
		else
			P_coef.push_back(Allo_P_LeafGrain); P_coef.push_back(Allo_P_StemGrain); P_coef.push_back(Allo_P_RootsGrain);

		if (GrainFillAllocation == 0) 	f_GrainSupply = GrainFillingSupply(c_coef, N_coef, P_coef);
		else f_GrainSupply = GrainFillingSupply_WithReduction(c_coef, N_coef, P_coef, Allo_GrainReductionRange);






		f_ReAlloCarbonToStemFromLeaf = ReAlloCarbonToStemFromLeaf(Allo_C_LeafStem);


		f_ReAlloNitrogenToStemFromLeaf = ReAlloNitrogenToStemFromLeaf(CN_RatioMinStem);
		f_ReAlloPhosphorusToStemFromLeaf = ReAlloPhosphorusToStemFromLeaf(CP_RatioMinStem);
	};
	auto SetLitterFallFunctors = [&]() {
		if (LitterFall_Dynamics_Sw(LitterFall_Dynamics) > LitterFall_Dynamics_Sw::Static) {
			f_LeafLitterFall = Get_boolD4_TableFunctor(MyFunc::LEAF_LITTERFALL_DYNAMIC);
			f_StemLitterFall = Get_boolD3_TableFunctor(MyFunc::STEM_LITTERFALL_DYNAMIC);
			f_GrainLitterFall = Get_D3_TableFunctor(MyFunc::GRAIN_LITTERFALL_DYNAMIC);
			f_RootLitterFall = Get_boolD3_TableFunctor(MyFunc::ROOT_LITTERFALL_DYNAMIC);
			f_CoarseRootLitterFall = Get_boolD3_TableFunctor(MyFunc::COARSEROOT_LITTERFALL_DYNAMIC);
			f_FungiLitterFall = Get_D3_TableFunctor(MyFunc::FUNGI_LITTERFALL_DYNAMIC);
		}
		else {
			f_LeafLitterFall = Get_boolD4_TableFunctor(MyFunc::LEAF_LITTERFALL_STATIC);
			f_StemLitterFall = Get_boolD3_TableFunctor(MyFunc::STEM_LITTERFALL_STATIC);
			f_GrainLitterFall = Get_D3_TableFunctor(MyFunc::GRAIN_LITTERFALL_STATIC);
			f_RootLitterFall = Get_boolD3_TableFunctor(MyFunc::ROOT_LITTERFALL_STATIC);
			f_CoarseRootLitterFall = Get_boolD3_TableFunctor(MyFunc::COARSEROOT_LITTERFALL_STATIC);
			f_FungiLitterFall = Get_D3_TableFunctor(MyFunc::FUNGI_LITTERFALL_STATIC);
		}
	};
	auto SetRespirationFunctors = [&]() {
		f_GrowthRespiration = Get_D1_TableFunctor(MyFunc::GROWTH_RESPIRATION);

		if (Resp_Temp_Response_Sw(Resp_Temp_Response) == Resp_Temp_Response_Sw::Common) {
			if (!pNCSoil->f_CommonTempResponse) pNCSoil->Ini_Responses();
			f_TempResponseRespiration = pNCSoil->f_CommonTempResponse;
		}
		else if (Resp_Temp_Response_Sw(Resp_Temp_Response) == Resp_Temp_Response_Sw::Q10_whole_range)
			f_TempResponseRespiration = Get_D1_Functor(MyFunc::PLANTRESP_Q10_RESPONSE);
		else if (Resp_Temp_Response_Sw(Resp_Temp_Response) == Resp_Temp_Response_Sw::Q10_above_threshold)
			f_TempResponseRespiration = Get_D1_Functor(MyFunc::PLANTRESP_Q10THRESHOLD_RESPONSE);



		if (PlantRespiration_Sw(PlantRespiration) == PlantRespiration_Sw::Maintenance_Only) {
			f_MaintenanceRespiration = MaintenanceRespiration(RespRateCoef);

		}
		else {
			vector<vector<double>> AllRespCoef;
			vector<double> Dummy;
			Dummy.resize(RespMCoefLeaf.size());
			//Supply vectors based on PlantComp order
			AllRespCoef.push_back(Dummy);
			AllRespCoef.push_back(RespMCoefLeaf);
			AllRespCoef.push_back(RespMCoefStem);
			AllRespCoef.push_back(RespMCoefGrain);
			AllRespCoef.push_back(Dummy);
			AllRespCoef.push_back(RespMCoefLeaf);
			AllRespCoef.push_back(RespMCoefStem);
			AllRespCoef.push_back(RespMCoefRoot);
			AllRespCoef.push_back(RespMCoefCoarseRoot);
			AllRespCoef.push_back(RespMCoefRoot);
			AllRespCoef.push_back(RespMCoefCoarseRoot);
			AllRespCoef.push_back(RespMCoefFungi);
			f_MaintenanceRespiration = MaintenanceRespiration(AllRespCoef);
		}


		if (PlantRespiration_Sw(PlantRespiration) == PlantRespiration_Sw::Growth_and_Maintainance_and_NitrogenUptake) {

			SoilOrgModel* p_Soil = (SoilOrgModel*)m_pModelInfo->GetModelPtr("Soil Org Model");
			NC_Organic* pNCSoil = &p_Soil->m_NC_SoilOrg;
			f_NuptRespiration = NuptakeImpactonRespiration(RespNO3Coef, RespNH4Coef, RespNOrgCoef, pNCSoil);// P not included yet HH
			if(Salinity_Stress_Sw(Salinity_Stress)==Salinity_Stress_Sw::on_respiration)  f_SaltImpactOnGrowthRespiration = SaltImpactonGrowthRespiration(this);
			f_PlantPhotoRespiration = PlantRespirationFunctions(f_MaintenanceRespiration, f_TempResponseRespiration, f_GrowthRespiration, f_SaltImpactOnGrowthRespiration);
			f_PlantPhotoBelowGroundRespiration = PlantRespirationFunctions(f_MaintenanceRespiration, f_TempResponseRespiration, f_GrowthRespiration, f_SaltImpactOnGrowthRespiration);

		}
		else {
			if (Salinity_Stress_Sw(Salinity_Stress) == Salinity_Stress_Sw::on_respiration)  f_SaltImpactOnGrowthRespiration = SaltImpactonGrowthRespiration(this);
			f_PlantPhotoRespiration = PlantRespirationFunctions(f_MaintenanceRespiration, f_TempResponseRespiration, f_GrowthRespiration, f_SaltImpactOnGrowthRespiration);
			f_PlantPhotoBelowGroundRespiration = PlantRespirationFunctions(f_MaintenanceRespiration, f_TempResponseRespiration, f_GrowthRespiration);
			f_PlantRespiration = PlantRespirationFunctions(f_MaintenanceRespiration, f_TempResponseRespiration);
			f_PlantBelowGroundRespiration = PlantRespirationFunctions(f_MaintenanceRespiration, f_TempResponseRespiration);
		}



	};

	auto SetRootUptakeFunctors = [&]() {

		if (N_Fixed_Supply_Sw(N_Fixed_Supply) > N_Fixed_Supply_Sw::off) f_PlantDemand[_N_ - 1] = PlantDemand(CN_RatioMinRoots, CN_RatioMinCoarseRoots, CN_RatioMinStem, CN_RatioMinLeaf);
		else f_PlantDemand[_N_ - 1] = PlantDemand(CN_RatioMinRoots, CN_RatioMinCoarseRoots, CN_RatioMinStem, CNLeafThres, CNLeafOpt, N_MinDegreeSupply);

		if (Phosphorus_Model_Sw(m_pModelStructure->Phosphorus_Model) > Phosphorus_Model_Sw::off) {
			f_PlantDemand[_P_ - 1] = PlantDemand(CP_RatioMinRoots, CP_RatioMinCoarseRoots, CP_RatioMinStem, CPLeafThres, CPLeafOpt, N_MinDegreeSupply);//P demand
		}

		if (FungalGrowth_Sw(FungalGrowth) > FungalGrowth_Sw::off) {
			f_FungiDemand = FungiDemand(CN_RatioMinFungi);
			f_FungiPotentialUptake = FungiPotentialSoilUptake(Fungi_MycFrac, Fungi_Norgrate, Fungi_NO3rate, Fungi_NH4rate);
			f_FungiCover = FungiCoverage(AlloFungiC1, AreaCoverFungiCoef);
			f_FungiNAvail = FungiNitrogenExcess(CN_RatioMaxFungi);
			if (Phosphorus_Model_Sw(m_pModelStructure->Phosphorus_Model) > Phosphorus_Model_Sw::off) {
				f_FungiPDemand = FungiDemand(CP_RatioMinFungi);
				f_FungiPotentialPUptake = FungiPotentialSoilPUptake(Fungi_MycFrac, Fungi_Porgrate, Fungi_PO4rate);

				f_FungiPAvail = FungiNitrogenExcess(CP_RatioMaxFungi);
			}
			else {
				PFungiDemand.assign(pPlant->NumPlants, 0.0);
				FungiPorgUptpot.assign(pPlant->NumPlants, 0.0);
				FungiPhosphateUptpot.assign(pPlant->NumPlants, 0.0);
				
			}
		}
		else {
			NFungiDemand.assign(pPlant->NumPlants, 0.0);
			FungiNorgUptpot.assign(pPlant->NumPlants, 0.0);
			FungiNitrateUptpot.assign(pPlant->NumPlants, 0.0);
			FungiAmmoniumUptpot.assign(pPlant->NumPlants, 0.0);



		}
		f_RootDensity = RootDensity(pSoil->ThicknessLayers);
		f_Avail = FractionOfAvailability(pNCSoil->NUptMaxAvailFrac);
			if (m_pModelStructure->m_NumActPlantElements >= 3) f_AvailP = FractionOfAvailability(pNCSoil->PUptMaxAvailFrac);
		if (FungalGrowth_Sw(FungalGrowth) > FungalGrowth_Sw::off) {
				f_FungiImpactOnPlant = FungiCoverImpactOnPlantAvailability(pNCSoil->FungiCoverRedCoef);//HH
				f_ImpactOnFungiMinUptake = ImpactOnFungiAvailability(pNCSoil->FungiMinEnhanceCoef, pNCSoil->FungiNH4_Enhance);
				if (m_pModelStructure->m_NumActPlantElements >= 3) f_ImpactOnFungiPhosphateUptake = ImpactOnFungiPAvailability(pNCSoil->FungiMinEnhanceCoef, pNCSoil->FungiPO4EnhanceCoef);
		}
		bool FungiUptake = true;
		vector<double> FlexDegree;
		if (pNCSoil->Flexible_Uptake == 0)
			for (size_t i = 0; i < pPlant->NumPlants; i++) FlexDegree.push_back(pNCSoil->NUptFlexDegree);
		else
			for (size_t i = 0; i < pPlant->NumPlants; i++) FlexDegree.push_back(pNCSoil->NUptPlantFlexDegree[i]);
		// Organic enhance uptake coef of fungi for N and P are the same!!! HH
		if (Root_Density_Influence_Sw(Root_Density_Influence) == Root_Density_Influence_Sw::on_nitrogen_uptake) {
			f_Density = DensityImpactOnPlantAvailability(pNCSoil->NUptRootCoefMin);

			if (FungalGrowth_Sw(FungalGrowth) == FungalGrowth_Sw::off) {
				f_MineralUptakeWithDensity = MineralUptakeByRootsAndFungi(f_Avail, f_Density, pSoil->NL, pNCSoil->NUptMaxAvailFrac, FlexDegree);
				f_OrganicUptakeWithDensity[0] = OrganicUptakeByRootsAndFungi(pNCSoil->UptOrgRateCoef_L, pNCSoil->UptOrgRateCoef_H, pNCSoil->NUptRootCoefOrg, pSoil->NL);
				if (m_pModelStructure->m_NumActPlantElements >= 3) {
					f_PMineralUptakeWithDensity = PMineralUptakeByRootsAndFungi(f_AvailP, f_Density, pSoil->NL, pNCSoil->PUptMaxAvailFrac, FlexDegree);//HH
					f_OrganicUptakeWithDensity[1] = OrganicUptakeByRootsAndFungi(pNCSoil->P_UptOrgRateCoef_L, pNCSoil->P_UptOrgRateCoef_H, pNCSoil->NUptRootCoefOrg, pSoil->NL);
				}
			}
			else {
				
				f_MineralUptakeWithDensityImpactWithFungi = MineralUptakeByRootsAndFungi(f_Avail, f_Density, f_FungiImpactOnPlant, f_ImpactOnFungiMinUptake, pSoil->NL, pNCSoil->NUptMaxAvailFrac, FlexDegree);
				f_OrganicUptakeWithDensitytoFungi[0] = OrganicUptakeByRootsAndFungi(FungiUptake, pNCSoil->UptOrgRateCoef_L, pNCSoil->UptOrgRateCoef_H, pNCSoil->NUptRootCoefOrg, pNCSoil->FungiOrgEnhanceCoef, pSoil->NL);
				if (m_pModelStructure->m_NumActPlantElements >= 3) {
					f_PMineralUptakeWithDensityImpactWithFungi = PMineralUptakeByRootsAndFungi(f_AvailP, f_Density, f_FungiImpactOnPlant, f_ImpactOnFungiPhosphateUptake, pSoil->NL, pNCSoil->PUptMaxAvailFrac, FlexDegree);
					f_OrganicUptakeWithDensitytoFungi[1] = OrganicUptakeByRootsAndFungi(FungiUptake, pNCSoil->P_UptOrgRateCoef_L, pNCSoil->P_UptOrgRateCoef_H, pNCSoil->NUptRootCoefOrg, pNCSoil->FungiOrgPEnhanceCoef, pSoil->NL);
				}
			}
		}
		else {
			if (FungalGrowth_Sw(FungalGrowth) == FungalGrowth_Sw::off&&Root_Density_Influence_Sw(Root_Density_Influence) == Root_Density_Influence_Sw::no) {
				f_MineralUptake = MineralUptakeByRootsAndFungi(f_Avail, pSoil->NL, pNCSoil->NUptMaxAvailFrac, FlexDegree);
				f_OrganicUptake[0] = OrganicUptakeByRootsAndFungi(pNCSoil->UptOrgRateCoef_L, pNCSoil->UptOrgRateCoef_H, pSoil->NL);
				if (m_pModelStructure->m_NumActPlantElements >= 3) {
					f_PMineralUptake = PMineralUptakeByRootsAndFungi(f_AvailP, pSoil->NL, pNCSoil->PUptMaxAvailFrac, FlexDegree);//HH
					f_OrganicUptake[1] = OrganicUptakeByRootsAndFungi(FungiUptake, pNCSoil->P_UptOrgRateCoef_L, pNCSoil->P_UptOrgRateCoef_H, pNCSoil->NUptRootCoefOrg, pNCSoil->FungiOrgPEnhanceCoef, pSoil->NL);
				}
			}
			else {
				f_MineralUptaketoFungi = MineralUptakeByRootsAndFungi(f_Avail, f_FungiImpactOnPlant, f_ImpactOnFungiMinUptake, pSoil->NL, pNCSoil->NUptMaxAvailFrac, FlexDegree);//HH
				f_OrganicUptake[0] = OrganicUptakeByRootsAndFungi(pNCSoil->UptOrgRateCoef_L, pNCSoil->UptOrgRateCoef_H, pSoil->NL);
				f_OrganicUptaketoFungi[0] = OrganicUptakeByRootsAndFungi(FungiUptake, pNCSoil->UptOrgRateCoef_L, pNCSoil->UptOrgRateCoef_H, pNCSoil->FungiOrgEnhanceCoef, pSoil->NL);
				if (m_pModelStructure->m_NumActPlantElements >= 3) {
					f_PMineralUptaketoFungi = PMineralUptakeByRootsAndFungi(f_AvailP, f_FungiImpactOnPlant, f_ImpactOnFungiPhosphateUptake, pSoil->NL, pNCSoil->PUptMaxAvailFrac, FlexDegree);//HH
					f_OrganicUptaketoFungi[1] = OrganicUptakeByRootsAndFungi(FungiUptake, pNCSoil->P_UptOrgRateCoef_L, pNCSoil->P_UptOrgRateCoef_H, pNCSoil->FungiOrgPEnhanceCoef, pSoil->NL);//HH
				}
			}

		}





		//f_CompUptake = MineralUptakeByRootsAndFungi(f_Avail, pSoil->NL, pNCSoil->NUptMaxAvailFrac);
		//Should P limit N fixation??? HH
		if (N_Fixation_Sw(N_Fixation) == N_Fixation_Sw::C_demand_driven) {
			f_DemandDrivenNFixation = NitrogenFixationbyDemand(UptakeFracCoefDef);
		}
		else if (N_Fixation_Sw(N_Fixation) == N_Fixation_Sw::Root_biomass_driven) {
			function<double(size_t, double)> f_ResponseMin = Get_D1_TableFunctor(MyFunc::NFIXATION_RESPONSE_MIN_SOIL);
			if (!pNCSoil->f_CommonTempResponse) pNCSoil->Ini_Responses();
			f_BelowGroundDrivenNFixation = NitrogenFixationIndependentDemand(pNCSoil->f_Theta_Response, pNCSoil->f_CommonTempResponse, f_ResponseMin,
				Root_NoduleRatio);
		}
		if (N_Fixation_Sw(N_Fixation) == N_Fixation_Sw::Stem_and_Leaf_biomass_driven) {
			function<double(size_t, double)> f_ResponseMin = Get_D1_TableFunctor(MyFunc::NFIXATION_RESPONSE_MIN_SOIL);
			if (!pNCSoil->f_CommonTempResponse) pNCSoil->Ini_Responses();
			f_AboveGroundDrivenNFixation = NitrogenFixationIndependentDemand(pNCSoil->f_Theta_Response, pNCSoil->f_CommonTempResponse, f_ResponseMin,
				StemLeaf_NoduleRatio);
		}





	};

	auto SetManagementFunctors = [&]() {

		f_Sowing = PlantTempSum(T_Thres_Sowing, T_Sum_Sowing);
		f_Emergence = PlantTempSum(T_Thres_Emerg, T_Sum_Emerg);
		f_EmergenceWithMoistControl = PlantTempSum(T_Thres_Emerg, T_Sum_Emerg, p_Soil_WaterF->M_Wilting[0], MoistureRangeEmergence);
		f_GrainStart = GrainStart(GrainSI_Step, GrainSI_StepDay, GrainSI_ThresDay, GrainSI_StepTemp, GrainSI_ThresTemp);
		f_GrainFill = PlantTempSum(T_Thres_GrainFill, T_Sum_GrainFill);
		f_Harvest = PlantTempSum(T_Thres_Ripening, T_Sum_Ripening);
		if (!f_DormingAndGrowthTempSum) {
			f_DormingAndGrowthTempSum = GrowingSumF(T_Thres_Emerg, T_Thres_Dormancy, pAtm->Latitude, T_Sum_Initial);
		}
		if (ZadokScale_Sw(ZadokScale) >= ZadokScale_Sw::on) {
			f_Zadok = ZadokFunctionFromGSI(ZadokRangeGSI2, GSI_DeltaZadok30, GSI_DeltaZadok55);
		};
	};

	SetBiomassFunctors();
	SetGrowthFunctors();
	SetAllocationFunctors();
	SetLitterFallFunctors();
	SetRespirationFunctors();

	SetRootUptakeFunctors();
	SetManagementFunctors();

	f_Emergence = PlantTempSum(T_Thres_Emerg, T_Sum_Emerg);
	f_EmergenceWithMoistControl = PlantTempSum(T_Thres_Emerg, T_Sum_Emerg, p_Soil_WaterF->M_Wilting[0], MoistureRangeEmergence);
	f_GrainFill = PlantTempSum(T_Thres_GrainFill, T_Sum_GrainFill);
	f_Harvest = PlantTempSum(T_Thres_Ripening, T_Sum_Ripening);
	return true;
}
bool NC_Plant::Dyn()
{
	if(m_pModelStructure->Nitrogen_Carbon==0) return false;
	NC_Drivings::Dyn();

	Flux();
	Integ();
	return true;

}

void NC_Plant::Flux()
{

	if(Growth==0) return;
	PlantStage_CNRatio_PhysSize();
	Light_And_PhotoSynthesis();

    
    // Input from Plant Biomass Input File (Switch: PhotoInput)
    if(CAtmNewMobile_Position.size()>0) {
        for(size_t i=0; i<AtmNewMobile.size(); i++)
			if(CAtmNewMobile_Position[i].ivar>0)
                AtmNewMobile[i]=GetEcoDat(&m_PGPos_PlantBiomass, CAtmNewMobile_Position[i]);   
    }

	AllocationAndReAllocation_DuringGrowth();
	AboveGroundPlantRespiration();
	BelowGrandPlantRespiration();
	LitterFall();
	MobilePool();



	Cleaning_Harvest_Events();

	if (Grazing_Sw(Grazing) > Grazing_Sw::off) Grazing_Events();
	if (WoodEating > 0) WoodEatingProcess();
	if (Seedling_Management > 0) Seedling_Management_Events();
// Nitrogen demand and reallocation
//   New role of N Mobile for reallocation 
	AccResponseTemp=0.;
	AccResponseWater=0.;
	AccResponseWaterN=0.;
	AccResponseN=0.;
	MineralOrganic_Uptake();
	
	At_YearShifting();
	TraceElementUptake();


	Summation_Fluxes();

   
}
void NC_Plant::Integ()
{
	SoilOrgModel* p_Soil = (SoilOrgModel*)m_pModelInfo->GetModelPtr("Soil Org Model");
	NC_Organic* pNCSoil = &p_Soil->m_NC_SoilOrg;

	//	New mobile pool	
	InFlow(P_State[_NEWMOBILE][_C_], AtmNewMobile);


	auto ReAllocationFlow = [&]() {


		OutFlow(P_State[_NEWMOBILE][_C_], P_CarbonAlloNewMobile.Leaf);
		if (P_State[_LEAF][_C_][0] > -10.&&P_State[_LEAF][_C_][0] < 1000.) {

		}
		else
			int koll = 1;
		InFlow(P_State[_LEAF][_C_], P_CarbonAlloNewMobile.Leaf);
		OutFlow(P_State[_NEWMOBILE][_C_], P_CarbonAlloNewMobile.Stem);
		InFlow(P_State[_STEM][_C_], P_CarbonAlloNewMobile.Stem);
		OutFlow(P_State[_NEWMOBILE][_C_], P_CarbonAlloNewMobile.Roots);
		InFlow(P_State[_ROOT][_C_], P_CarbonAlloNewMobile.Roots);
		OutFlow(P_State[_NEWMOBILE][_C_], P_CarbonAlloNewMobile.CoarseRoots);
		InFlow(P_State[_COARSE_ROOT][_C_], P_CarbonAlloNewMobile.CoarseRoots);
		for (size_t jj = 1; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
			OutFlow(P_State[_NEWMOBILE][jj], P_UptakeNewMobile[jj - 1].Roots);
			InFlow(P_State[_ROOT][jj], P_UptakeNewMobile[jj - 1].Roots);
			OutFlow(P_State[_NEWMOBILE][jj], P_UptakeNewMobile[jj - 1].CoarseRoots);
			InFlow(P_State[_COARSE_ROOT][jj], P_UptakeNewMobile[jj - 1].CoarseRoots);
			OutFlow(P_State[_NEWMOBILE][jj], P_UptakeNewMobile[jj - 1].Stem);
			InFlow(P_State[_STEM][jj], P_UptakeNewMobile[jj - 1].Stem);
			OutFlow(P_State[_NEWMOBILE][jj], P_UptakeNewMobile[jj - 1].Leaf);
			InFlow(P_State[_LEAF][jj], P_UptakeNewMobile[jj - 1].Leaf);
		}

		for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++) {

			for (size_t j = LEAF_GRAIN; j <= ROOT_LEAF; j++) {
				if (P_ReAllocation[j][jj][0] > -10. &&P_ReAllocation[j][jj][0] < 1000.) {
					OutFlow(P_State[P_ReLink[j].first][jj], P_ReAllocation[j][jj]);
					InFlow(P_State[P_ReLink[j].second][jj], P_ReAllocation[j][jj]);
				}
				else
					P_ReAllocation[j][jj][0] = 0.;
			}

			for (size_t j = LEAF_MOBILE_RETURN; j <= OLD_LEAF_MOBILE_RETURN; j++) {
				OutFlow(P_State[P_ReLink[j].first][jj], P_ReAllocation[j][jj]);
				InFlow(P_State[P_ReLink[j].second][jj], P_ReAllocation[j][jj]);
			}
			for (size_t j = OLD_LEAF_MOBILE; j <= COARSE_ROOT_MOBILE; j++) {
				if (j != 17) {
					OutFlow(P_State[P_ReLink[j].first][jj], P_ReAllocation[j][jj]);
					InFlow(P_State[P_ReLink[j].second][jj], P_ReAllocation[j][jj]);
				}
			}
			for (size_t j = MOBILE_LEAF; j < MOBILE_NEWMOBILE; j++) {
				OutFlow(P_State[P_ReLink[j].first][jj], P_ReAllocation[j][jj]);
				InFlow(P_State[P_ReLink[j].second][jj], P_ReAllocation[j][jj]);
			}




			if (FungalGrowth > 0) {
				OutFlow(P_State[P_ReLink[NEWMOBILE_FUNGI].first][jj], P_ReAllocation[NEWMOBILE_FUNGI][jj]);
				InFlow(P_State[P_ReLink[NEWMOBILE_FUNGI].second][jj], P_ReAllocation[NEWMOBILE_FUNGI][jj]);
			}



			//OutFlow(P_State[_MOBILE], P_ReAllocation[MOBILE_NEWMOBILE]);
			//InFlow(P_State[_NEWMOBILE], P_ReAllocation[MOBILE_NEWMOBILE]);

		}
	};

	auto RespirationFlow = [&]() {
		for (size_t j = 0; j < NUM_PLANT_COMP; j++) 
			OutFlow(P_State[j][_C_], P_Respiration[j]);
			if (FungalGrowth > 0) OutFlow(P_State[_FUNGI][_C_], P_Respiration[_FUNGI]);


	};

	auto LitterFallFlow = [&]() {
		// Add for P in surface litter by HH
		auto InflowToSurfaceLitter = [&](size_t jj_element, size_t i_plant, double Fraction) {
			for (size_t j = _LEAF; j <= _OLD_STEM; j++) {
				if (P_LitterFall[j][jj_element][i_plant] >= 0.&&P_LitterFall[j][jj_element][i_plant] < 100000.) {
					if (jj_element == _C_) InFlow(pNCSoil->SurfaceLitter[_L1][_C_], P_LitterFall[j][jj_element][i_plant] * Fraction);
					else if (jj_element == _N_) InFlow(pNCSoil->SurfaceLitter[_L1][_N_], P_LitterFall[j][jj_element][i_plant] * Fraction);
					else if (jj_element == _P_) InFlow(pNCSoil->SurfaceLitter[_L1][_P_], P_LitterFall[j][jj_element][i_plant] * Fraction);

					if (Fraction < 1.) {
						if (jj_element == _C_) InFlow(pNCSoil->SurfaceLitter[_L2][_C_], P_LitterFall[j][jj_element][i_plant] * (1 - Fraction));
						else if (jj_element == _N_) InFlow(pNCSoil->SurfaceLitter[_L2][_N_], P_LitterFall[j][jj_element][i_plant] * (1 - Fraction));
						else if (jj_element == _P_) InFlow(pNCSoil->SurfaceLitter[_L2][_P_], P_LitterFall[j][jj_element][i_plant] * (1 - Fraction));

					}
				}
				else {
					P_LitterFall[j][jj_element][i_plant] = 0.;


				}

			}
		};

		auto InflowAboveGroundComponentsToSoilHorizon = [&](size_t jj_element, size_t i_plant, size_t i_soil_layer, double Fraction) {
			for (size_t j = _LEAF; j <= _OLD_STEM; j++) {
				if (P_LitterFall[j][jj_element][i_plant] >= 0. && P_LitterFall[j][jj_element][i_plant] < 100000.) {
					if (jj_element == _C_) InFlow(pNCSoil->O_State[_L1][_C_][i_soil_layer], P_LitterFall[j][jj_element][i_plant] * Fraction);
					else if (jj_element == _N_) InFlow(pNCSoil->O_State[_L1][_N_][i_soil_layer], P_LitterFall[j][jj_element][i_plant] * Fraction);
					else if (jj_element == _P_) InFlow(pNCSoil->O_State[_L1][_P_][i_soil_layer], P_LitterFall[j][jj_element][i_plant] * Fraction);

					if (Fraction < 1.) {
						if (jj_element == _C_) InFlow(pNCSoil->O_State[_L2][_C_][i_soil_layer], P_LitterFall[j][jj_element][i_plant] * (1 - Fraction));
						else if (jj_element == _N_) InFlow(pNCSoil->O_State[_L2][_N_][i_soil_layer], P_LitterFall[j][jj_element][i_plant] * (1 - Fraction));
						else if (jj_element == _P_) InFlow(pNCSoil->O_State[_L2][_P_][i_soil_layer], P_LitterFall[j][jj_element][i_plant] * (1 - Fraction));

					}
				}
				else {
					P_LitterFall[j][jj_element][i_plant] = 0.;


				}

			}
		};

		auto mobilepoolLitterFall = [&](size_t jj, size_t i) {
			if (jj == _C_) {
				InFlow(pNCSoil->SurfaceLitter[_L1][_C_], P_LitterFall[_NEWMOBILE][_C_][i]);	//LK 010906
				InFlow(pNCSoil->SurfaceLitter[_L1][_C_], P_LitterFall[_MOBILE][_C_][i]);	//LK 020524
			}
			else if (jj == _N_) {

				InFlow(pNCSoil->SurfaceLitter[_L1][_N_], P_LitterFall[_NEWMOBILE][_N_][i]);
				InFlow(pNCSoil->SurfaceLitter[_L1][_N_], P_LitterFall[_MOBILE][_N_][i]);	//LK 020524
			}
			else if (jj == _P_) {

				InFlow(pNCSoil->SurfaceLitter[_L1][_P_], P_LitterFall[_NEWMOBILE][_P_][i]);
				InFlow(pNCSoil->SurfaceLitter[_L1][_P_], P_LitterFall[_MOBILE][_P_][i]);
			}

		};


		for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
			for (size_t j = 0; j < NUM_PLANT_COMP; j++) {
				OutFlow(P_State[j][jj], P_LitterFall[j][jj]);
			};
			//auto cnleaf = P_State[_OLD_LEAF][_C_][1] / P_State[_OLD_LEAF][_N_][1];
			//auto cnlitterfall= P_LitterFall[_OLD_LEAF][_C_][1] / P_LitterFall[_OLD_LEAF][_N_][1];

			if (FungalGrowth > 0) OutFlow(P_State[_FUNGI][jj], P_LitterFall[_FUNGI][jj]);


			for (size_t i = 0; i < P_LitterFall[_ROOT][_C_].size(); i++) {
				if (pNCSoil->LitterPools > 0) {
					if (SoilLayerForLitterInput[i] == string::npos)
						InflowToSurfaceLitter(jj, i, (1 - Leaf_FracLitter2[i]));
					else
						InflowAboveGroundComponentsToSoilHorizon(jj, i, SoilLayerForLitterInput[i], (1 - Leaf_FracLitter2[i]));

				}
				else {
					if (SoilLayerForLitterInput[i] == string::npos)
						InflowToSurfaceLitter(jj, i, 1.);
					else
						InflowAboveGroundComponentsToSoilHorizon(jj, i, SoilLayerForLitterInput[i], 1.);
				}
				mobilepoolLitterFall(jj, i);
			}
		}
	};
	auto HarvestFlow = [&]() {
		for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++)
			for (size_t j = 0; j < NUM_PLANT_COMP; j++) {
				OutFlow(P_State[j][jj], P_Harvest[j][jj]);
			};

	};

	auto PlantUptakeFlow = [&]() {

		InFlow(P_State[_NEWMOBILE][_N_], P_Uptake[PLANT_TOT_N][_N_ - 1]);
		InFlow(P_State[_NEWMOBILE][_P_], P_Uptake[PLANT_TOT_N][_P_ - 1]);//HH

		if (FungalGrowth > 0) {
			InFlow(P_State[_FUNGI][_N_], P_Uptake[FUNGI_TOT_N][_N_ - 1]);//HH
			InFlow(P_State[_FUNGI][_N_], P_Uptake[FUNGI_ORG_N][_N_ - 1]);//HH
			InFlow(P_State[_FUNGI][_P_], P_Uptake[FUNGI_TOT_N][_P_ - 1]);
			InFlow(P_State[_FUNGI][_P_], P_Uptake[FUNGI_ORG_N][_P_ - 1]);
			OutFlow(P_State[_FUNGI][_N_], NFungiPlant);//HH
			OutFlow(P_State[_FUNGI][_P_], PFungiPlant);//HH
			//InFlow(P_State[_FUNGI][_N_], P_Uptake[FUNGI_TOT_N]);
			//InFlow(P_State[_FUNGI][_N_], NOrgFungiUptake);
		}
	};

	ReAllocationFlow();
	RespirationFlow();
	LitterFallFlow();
	HarvestFlow();
	PlantUptakeFlow();
}




	/*if (m_pModelStructure->Phosphorus_Model > 0) Phosphorus_Sw = true;
	Flow_NewMobile(P_State, P_Flux, Phosphorus_Sw);
	Flow_Root(P_State, P_Flux, Phosphorus_Sw);
	Flow_Atm(P_State, P_Flux, true);*/
	


	
	/*OutFlow(P_State[_NEWMOBILE][_C_], P_CarbonAlloNewMobile.Roots);
	OutFlow(P_State[_NEWMOBILE][_C_], P_CarbonAlloNewMobile.Leaf);
	OutFlow(P_State[_NEWMOBILE][_C_], P_CarbonAlloNewMobile.Stem);
	OutFlow(P_State[_NEWMOBILE][_C_], P_CarbonAlloNewMobile.CoarseRoots);*/


//	InFlow(&P_State[_NEWMOBILE][_N_], &NPlantFixation);
/*	OutFlow(P_State[_NEWMOBILE][_N_], P_ReAllocation{NEWMOBILE_ROOT][_N_]);
	OutFlow(P_State[_NEWMOBILE][_N_], P_ReAllocation[NEWMOBILE_LEAF][_N_]);
	OutFlow(P_State[_NEWMOBILE][_N_], P_ReAllocation[NEWMOBILE_STEM][_N_]);
	OutFlow(P_State[_NEWMOBILE][_N_], P_ReAllocation[NEWMOBILE_COARSEROOT][_N_]);*/

//	Root pool
//	InFlow(P_State[_ROOT][_C_], P_CarbonAlloNewMobile.Roots);
//	InFlow(P_State[_ROOT][_N_], P_ReAllocation{NEWMOBILE_ROOT][_N_]);
//	OutFlow(P_State[_ROOT][_C_], P_Respiration[_ROOT]);
//	OutFlow(P_State[_ROOT][_C_], CRootsLitter);
//	OutFlow(P_State[_ROOT][_N_], NRootsLitter);

//	if(pPlant->TraceElementUptake) {
//		 InFlow(pPlant->TE_Roots, pPlant->TE_PlantRootUptake);
//		 OutFlow(pPlant->TE_Roots, pPlant->TE_RootsLitter);
//	}

	//if(FungalGrowth>0) {
			//OutFlow(P_State[_NEWMOBILE][_C_], CNewMobileFungi);
			//InFlow(P_State[_FUNGI][_C_], CNewMobileFungi);
  			//OutFlow(P_State[_FUNGI][_C_], CFungiAtm);      
			//OutFlow(P_State[_FUNGI][_C_], P_LitterFall[_FUNGI][_C_]);
			//OutFlow(P_State[_FUNGI][_N_], P_LitterFall[_FUNGI][_N_]);
			
	//	};

//	Coarse Root pool
	//InFlow(P_State[_COARSE_ROOT][_C_], P_CarbonAlloNewMobile.CoarseRoots);
	//InFlow(P_State[_COARSE_ROOT][_N_], P_ReAllocation[NEWMOBILE_COARSEROOT][_N_]);
	//OutFlow(P_State[_COARSE_ROOT][_C_], P_Respiration[_COARSE_ROOT]);
	//OutFlow(P_State[_COARSE_ROOT][_C_], CCoarseRootsLitter);
	//OutFlow(P_State[_COARSE_ROOT][_N_], NCoarseRootsLitter);


//	Leaf pool
/*	InFlow(P_State[_LEAF][_C_], P_CarbonAlloNewMobile.Leaf);
	InFlow(P_State[_LEAF][_N_], P_ReAllocation[NEWMOBILE_LEAF][_N_]);
	OutFlow(P_State[_LEAF][_C_], P_Respiration[_LEAF]);

	OutFlow(P_State[_LEAF][_C_], CLeafReturnMobile);
	OutFlow(P_State[_OLD_LEAF][_C_], COldLeafReturnMobile);
	
	InFlow(P_State[_LEAF][_C_], CMobileLeaf);
	OutFlow(P_State[_MOBILE][_C_], CMobileLeaf);
	InFlow(P_State[_LEAF][_N_], NMobileLeaf);
	OutFlow(P_State[_MOBILE][_N_], NMobileLeaf);
	
	InFlow(P_State[_MOBILE][_C_], CLeafReturnMobile);
    InFlow(P_State[_MOBILE][_C_], COldLeafReturnMobile);

	OutFlow(P_State[_LEAF][_N_], NLeafReturnMobile);
	OutFlow(P_State[_OLD_LEAF][_N_], NOldLeafReturnMobile);
	
	InFlow(P_State[_MOBILE][_N_], NLeafReturnMobile);
    InFlow(P_State[_MOBILE][_N_], NOldLeafReturnMobile);
//  Leaf Stem Pool reallocation (new June 2007)

    OutFlow(P_State[_LEAF][_C_], CLeafStem);
    InFlow(P_State[_STEM][_C_], CLeafStem);  
    
    OutFlow(P_State[_LEAF][_N_],  NLeafStem);
    InFlow(P_State[_STEM][_N_],  NLeafStem);  */
    
    
	
/*	Stem pool
	InFlow(P_State[_STEM][_C_], P_CarbonAlloNewMobile.Stem);
	InFlow(P_State[_STEM][_N_], P_ReAllocation[NEWMOBILE_STEM][_N_]);
	OutFlow(P_State[_STEM][_C_], P_Respiration[_STEM]);
	
	
	
    if(N_ReAllocation>0) {
               	InFlow(P_State[_MOBILE][_N_], NOldLeafMobile);
               	InFlow(P_State[_MOBILE][_N_], NOldRootsMobile);
     	        InFlow(P_State[_MOBILE][_N_], NOldStemMobile);
    	        InFlow(P_State[_MOBILE][_N_], NOldCoarseRootsMobile);
               	
                InFlow(P_State[_MOBILE][_N_], NLeafMobile);
               	InFlow(P_State[_MOBILE][_N_], NRootsMobile);
     	        InFlow(P_State[_MOBILE][_N_], NStemMobile);
    	        InFlow(P_State[_MOBILE][_N_], NCoarseRootsMobile);
    	      
                OutFlow(P_State[_OLD_LEAF][_N_], NOldLeafMobile);
               	OutFlow(P_State[_OLD_ROOT][_N_], NOldRootsMobile);
     	        OutFlow(P_State[_OLD_STEM][_N_], NOldStemMobile);
    	        OutFlow(P_State[_OLD_COARSE_ROOT][_N_], NOldCoarseRootsMobile);

               	OutFlow(P_State[_LEAF][_N_], NLeafMobile);
               	OutFlow(P_State[_ROOT][_N_], NRootsMobile);
     	        OutFlow(P_State[_STEM][_N_], NStemMobile);
    	        OutFlow(P_State[_COARSE_ROOT][_N_], NCoarseRootsMobile);

              	OutFlow(P_State[_MOBILE][_N_], P_ReAllocation[MOBILE_NEWMOBILE]); 
              	InFlow(P_State[_NEWMOBILE][_N_], P_ReAllocation[MOBILE_NEWMOBILE]);  
    }	

//	Grain pool reallocation
	InFlow(P_State[_GRAIN][_C_], P_Flux.Stem_Grain[_C_]);
	OutFlow(P_State[_STEM][_C_], P_Flux.Stem_Grain[_C_]);
	InFlow(P_State[_GRAIN][_C_], P_Flux.Leaf_Grain[_C_]);
	OutFlow(P_State[_LEAF][_C_], P_Flux.Leaf_Grain[_C_]);
	InFlow(P_State[_GRAIN][_C_], P_Flux.Root_Grain[_C_]);
	OutFlow(P_State[_ROOT][_C_], P_Flux.Root_Grain[_C_]);
	OutFlow(P_State[_GRAIN][_C_], P_Respiration[_GRAIN]);

	InFlow(P_State[_GRAIN][_N_], NStemGrain);
	OutFlow(P_State[_STEM][_N_], NStemGrain);
	InFlow(P_State[_GRAIN][_N_], NLeafGrain);
	OutFlow(P_State[_LEAF][_N_], NLeafGrain);
	InFlow(P_State[_GRAIN][_N_], NRootsGrain);
	OutFlow(P_State[_ROOT][_N_],NRootsGrain);

//	At Harvest
	OutFlow(P_State[_GRAIN][_C_]	, P_Harvest.Grain_Harvest[_C_]);
	OutFlow(P_State[_STEM][_C_]	, P_Harvest.Stem_Harvest[_C_]);
	OutFlow(P_State[_LEAF][_C_]	, P_Harvest.Leaf_Harvest[_C_]);
	OutFlow(P_State[_ROOT][_C_]	, P_Harvest.Root_Harvest[_C_]);
	OutFlow(P_State[_COARSE_ROOT][_C_]	, CCoarseRootsHarvest);


 	OutFlow(P_State[_GRAIN][_N_]	, NGrainHarvest);
	OutFlow(P_State[_STEM][_N_]	, NStemHarvest);
	OutFlow(P_State[_LEAF][_N_]	, NLeafHarvest);
	OutFlow(P_State[_ROOT][_N_]	, NRootsHarvest);
	OutFlow(P_State[_COARSE_ROOT][_N_]	, NCoarseRootsHarvest);

	OutFlow(P_State[_OLD_STEM][_C_]	, P_Harvest.OldStem_Harvest[_C_]);
	OutFlow(P_State[_OLD_LEAF][_C_]	, P_Harvest.OldLeaf_Harvest[_C_]);
	OutFlow(P_State[_OLD_ROOT][_C_]	, P_Harvest.OldRoot_Harvest[_C_]);
	OutFlow(P_State[_OLD_COARSE_ROOT][_C_]	, COldCoarseRootsHarvest);
	
	OutFlow(P_State[_OLD_STEM][_N_]	, NOldStemHarvest);
	OutFlow(P_State[_OLD_LEAF][_N_]	, NOldLeafHarvest);
	OutFlow(P_State[_OLD_ROOT][_N_]	, NOldRootsHarvest);
	OutFlow(P_State[_OLD_COARSE_ROOT][_N_]	, NOldCoarseRootsHarvest);

//	if(Grain_Development>0) {
		OutFlow(P_State[_GRAIN][_C_]	, P_LitterFall[_GRAIN][_C_]);
	 	OutFlow(P_State[_GRAIN][_N_]	, NGrainSurfaceLitter);
//	}

//	Stem, &Leaf, &NewMobile and Mobile outflows
	OutFlow(P_State[_STEM][_C_]	, P_LitterFall[_STEM][_C_]);
	OutFlow(P_State[_LEAF][_C_]	, P_LitterFall[_LEAF][_C_]);
	OutFlow(P_State[_NEWMOBILE][_C_]	, P_LitterFall.NewMobile_SurfaceLitter[_C_]);	//LK 010906
	OutFlow(P_State[_MOBILE][_C_]	, P_LitterFall.Mobile_SurfaceLitter[_C_]);		//LK 020524

	OutFlow(P_State[_STEM][_N_]	, NStemSurfaceLitter);
	OutFlow(P_State[_LEAF][_N_]	, P_LitterFall[_LEAF][_N_]);
	OutFlow(P_State[_NEWMOBILE][_N_]	, N_NewMobileSurfaceLitter);	//LK 010906
	OutFlow(P_State[_MOBILE][_N_]		, NMobileSurfaceLitter);		//LK 020524

//   Perennial Plant, &Surface Litter pool
//	if(Perennial_PLant==1) {
		OutFlow(P_State[_OLD_LEAF][_C_], P_LitterFall[_OLD_LEAF][_C_]);
		OutFlow(P_State[_OLD_LEAF][_N_], P_ReAllocation[_OLD_LEAF][_N_]);

		OutFlow(P_State[_OLD_STEM][_C_], P_LitterFall[_OLD_STEM][_C_]);
		OutFlow(P_State[_OLD_STEM][_N_], NOldStemSurfaceLitter);


		for(size_t i=0; i<P_LitterFall[_OLD_LEAF][_C_].size();i++) {
			if(pNCSoil->LitterPools>0) {
				InFlow(pNCSoil->SurfaceLitter[_L1][_C_], P_LitterFall[_OLD_LEAF][_C_][i]*(1-Leaf_FracLitter2[i]));
				InFlow(pNCSoil->SurfaceLitter[_L1][_N_], P_ReAllocation[_OLD_LEAF][_N_][i]*(1-Leaf_FracLitter2[i]));
	
				InFlow(pNCSoil->SurfaceLitter[_L1][_C_], P_LitterFall[_OLD_STEM][_C_][i]*(1-Stem_FracLitter2[i]));
				InFlow(pNCSoil->SurfaceLitter[_L1][_N_], NOldStemSurfaceLitter[i]*(1-Stem_FracLitter2[i]));
				
				InFlow(pNCSoil->SurfaceLitter[_L1][_C_], P_LitterFall[_LEAF][_C_][i]*(1-Leaf_FracLitter2[i]));
				InFlow(pNCSoil->SurfaceLitter[_L1][_N_], P_LitterFall[_LEAF][_N_][i]*(1-Leaf_FracLitter2[i]));
	
				InFlow(pNCSoil->SurfaceLitter[_L1][_C_], P_LitterFall[_STEM][_C_][i]*(1-Stem_FracLitter2[i]));
				InFlow(pNCSoil->SurfaceLitter[_L1][_N_], NStemSurfaceLitter[i]*(1-Stem_FracLitter2[i]));

				InFlow(pNCSoil->SurfaceLitter[_L1][_C_], P_LitterFall[_GRAIN][_C_][i]*(1-Grain_FracLitter2[i]));
				InFlow(pNCSoil->SurfaceLitter[_L1][_N_], NGrainSurfaceLitter[i]*(1-Grain_FracLitter2[i]));

				InFlow(pNCSoil->SurfaceLitter[_L2][_C_], P_LitterFall[_OLD_LEAF][_C_][i]*(Leaf_FracLitter2[i]));
				InFlow(pNCSoil->SurfaceLitter[_L2][_N_], P_ReAllocation[_OLD_LEAF][_N_][i]*(Leaf_FracLitter2[i]));
	
				InFlow(pNCSoil->SurfaceLitter[_L2][_C_], P_LitterFall[_OLD_STEM][_C_][i]*(Stem_FracLitter2[i]));
				InFlow(pNCSoil->SurfaceLitter[_L2][_N_], NOldStemSurfaceLitter[i]*(Stem_FracLitter2[i]));
				
				InFlow(pNCSoil->SurfaceLitter[_L2][_C_], P_LitterFall[_LEAF][_C_][i]*(Leaf_FracLitter2[i]));
				InFlow(pNCSoil->SurfaceLitter[_L2][_N_], P_LitterFall[_LEAF][_N_][i]*(Leaf_FracLitter2[i]));
	
				InFlow(pNCSoil->SurfaceLitter[_L2][_C_], P_LitterFall[_STEM][_C_][i]*(Stem_FracLitter2[i]));
				InFlow(pNCSoil->SurfaceLitter[_L2][_N_], NStemSurfaceLitter[i]*(Stem_FracLitter2[i]));

 				InFlow(pNCSoil->SurfaceLitter[_L2][_C_], P_LitterFall[_GRAIN][_C_][i]*(Grain_FracLitter2[i]));
				InFlow(pNCSoil->SurfaceLitter[_L2][_N_], NGrainSurfaceLitter[i]*(Grain_FracLitter2[i]));
			}
			else {
				InFlow(pNCSoil->SurfaceLitter[_L1][_C_], P_LitterFall[_OLD_LEAF][_C_][i]);
				InFlow(pNCSoil->SurfaceLitter[_L1][_N_], P_ReAllocation[_OLD_LEAF][_N_][i]);
	
				InFlow(pNCSoil->SurfaceLitter[_L1][_C_], P_LitterFall[_OLD_STEM][_C_][i]);
				InFlow(pNCSoil->SurfaceLitter[_L1][_N_], NOldStemSurfaceLitter[i]);
				
				InFlow(pNCSoil->SurfaceLitter[_L1][_C_], P_LitterFall[_LEAF][_C_][i]);
				InFlow(pNCSoil->SurfaceLitter[_L1][_N_], P_LitterFall[_LEAF][_N_][i]);
	
				InFlow(pNCSoil->SurfaceLitter[_L1][_C_], P_LitterFall[_STEM][_C_][i]);
				InFlow(pNCSoil->SurfaceLitter[_L1][_N_], NStemSurfaceLitter[i]);

				InFlow(pNCSoil->SurfaceLitter[_L1][_C_], P_LitterFall[_GRAIN][_C_][i]);
				InFlow(pNCSoil->SurfaceLitter[_L1][_N_], NGrainSurfaceLitter[i]);
			}


			InFlow(pNCSoil->SurfaceLitter[_L1][_C_], P_LitterFall.NewMobile_SurfaceLitter[_C_][i]);	//LK 010906
			InFlow(pNCSoil->SurfaceLitter[_L1][_N_], N_NewMobileSurfaceLitter[i]);	//LK 010906
			
			InFlow(pNCSoil->SurfaceLitter[_L1][_C_], P_LitterFall.Mobile_SurfaceLitter[_C_][i]);	//LK 020524
			InFlow(pNCSoil->SurfaceLitter[_L1][_N_], NMobileSurfaceLitter[i]);	//LK 020524
		}

//	Year shift perennial plant
//	 if(Perennial_Plant==1&&JDayNum==1) {
		if (m_pModelInfo->JDayNum == 1) {
			OutFlow(P_State[_STEM][_C_], P_ReAllocation[STEM_OLD_STEM][_C_]);
			OutFlow(P_State[_LEAF][_C_], P_ReAllocation[LEAF_OLD_LEAF][_C_]);
			OutFlow(P_State[_ROOT][_C_], P_ReAllocation[ROOT_OLD_ROOT][_C_]);
			OutFlow(P_State[_COARSE_ROOT][_C_], CCoarseRootsOldRoots);

			InFlow(P_State[_OLD_STEM][_C_], P_ReAllocation[STEM_OLD_STEM][_C_]);
			InFlow(P_State[_OLD_LEAF][_C_], P_ReAllocation[LEAF_OLD_LEAF][_C_]);
			InFlow(P_State[_OLD_ROOT][_C_], P_ReAllocation[ROOT_OLD_ROOT][_C_]);
			InFlow(P_State[_OLD_COARSE_ROOT][_C_], CCoarseRootsOldRoots);

			OutFlow(P_State[_STEM][_N_], P_ReAllocation[STEM_OLD_STEM][_N_]);
			OutFlow(P_State[_LEAF][_N_], P_ReAllocation[LEAF_OLD_LEAF][_N_]);
			OutFlow(P_State[_ROOT][_N_], P_ReAllocation[ROOT_OLD_ROOT][_N_]);
			OutFlow(P_State[_COARSE_ROOT][_N_], P_ReAllocation[COARSE_ROOT_OLD__COARSE_ROOT][_N_]);

			InFlow(P_State[_OLD_STEM][_N_], P_ReAllocation[STEM_OLD_STEM][_N_]);
			InFlow(P_State[_OLD_LEAF][_N_], P_ReAllocation[LEAF_OLD_LEAF][_N_]);
			InFlow(P_State[_OLD_ROOT][_N_], P_ReAllocation[ROOT_OLD_ROOT][_N_]);
			InFlow(P_State[_OLD_COARSE_ROOT][_N_], P_ReAllocation[COARSE_ROOT_OLD__COARSE_ROOT][_N_]);
		}
//	 else if(Perennial_Plant==1) {
		OutFlow(P_State[_OLD_STEM][_C_],	P_Respiration[_OLD_STEM]);
		OutFlow(P_State[_OLD_LEAF][_C_],	P_Respiration[_OLD_LEAF]);
		OutFlow(P_State[_OLD_ROOT][_C_], P_Respiration[_OLD_ROOT]);
		OutFlow(P_State[_OLD_COARSE_ROOT][_C_], P_Respiration[_OLD_COARSE_ROOT]);

		OutFlow(P_State[_OLD_ROOT][_C_], COldRootsLitter);
		OutFlow(P_State[_OLD_ROOT][_N_], NOldRootsLitter);
		OutFlow(P_State[_OLD_COARSE_ROOT][_C_], COldCoarseRootsLitter);
		OutFlow(P_State[_OLD_COARSE_ROOT][_N_], NOldCoarseRootsLitter);
//	 }

        if(ReAllocationToLeaf==1) {
        	InFlow(P_State[_LEAF][_N_]	, NRootsLeaf);
        	InFlow(P_State[_LEAF][_C_]	, CRootsLeaf);

            OutFlow(P_State[_ROOT][_N_] , NRootsLeaf);
            OutFlow(P_State[_ROOT][_C_] , CRootsLeaf);
        }
// Grazing Pool and Fluxes
		if (Grazing > 0) {
			// From Plant
			OutFlow(P_State[_LEAF][_C_], CLeafGrazing);
			OutFlow(P_State[_LEAF][_N_], NLeafGrazing);
			OutFlow(P_State[_OLD_LEAF][_C_], COldLeafGrazing);
			OutFlow(P_State[_OLD_LEAF][_N_],NOldLeafGrazing);
			OutFlow(P_State[_STEM][_C_], CStemGrazing);
			OutFlow(P_State[_STEM][_N_], NStemGrazing);

			// to Faeeces pool of uppermost layers
			InFlow(pNCSoil->O_State[_F][_C_][0], C_GrazingManure);
			InFlow(pNCSoil->O_State[_F][_N_][0], N_GrazingManure);

			// to Ammonium pool of uppermost layer
			InFlow(pNCSoil->SoilAmmonium[0], N_GrazingAmmonium);

			// to Grazing Organism

			InFlow(C_GrazingOrganism, C_GrazingGrowth);
			InFlow(N_GrazingOrganism, N_GrazingGrowth);
		}
		// Grazing Pool and Fluxes
		if (WoodEating > 0) {
			// From Plant
			OutFlow(P_State[_OLD_STEM][_C_], COldStemEating);
			OutFlow(P_State[_OLD_STEM][_N_], NOldStemEating);
			OutFlow(P_State[_OLD_COARSE_ROOT][_C_], COldCoarseRootEating);
			OutFlow(P_State[_OLD_COARSE_ROOT][_N_], NOldCoarseRootEating);




			// to Wood Eating Organism

			InFlow(C_WoodEater, C_WoodEaterGrowthFlux);
			InFlow(N_WoodEater, N_WoodEatingRate);


			// From Wood Eating Organism
			OutFlow(C_WoodEater, WoodEaterRespFlux);
			OutFlow(C_WoodEater, C_WoodEaterFaecesFlux);
			OutFlow(N_WoodEater, N_WoodEaterFaecesFlux);
			OutFlow(N_WoodEater, WoodEaterAmmoniumFlux);

			// From Wood Eating Organism to Faeeces pool of uppermost layers
			InFlow(pNCSoil->O_State[_F][_C_][0], C_WoodEaterFaecesFlux);
			InFlow(pNCSoil->O_State[_F][_N_][0], N_WoodEaterFaecesFlux);

			// to Ammonium pool of uppermost layer
			InFlow(pNCSoil->SoilAmmonium[0], WoodEaterAmmoniumFlux);



		}



//	Trace element
	if(pPlant->TraceElementUptake) {
		//Leaf
		 InFlow(pPlant->TE_Leaf	, pPlant->TE_PlantLeafUptake);
		 //Stem
		 InFlow(pPlant->TE_Stem, pPlant->TE_PlantStemUptake);
		 //Grain
		 OutFlow(pPlant->TE_Leaf	, pPlant->TE_LeafGrain	);
		 OutFlow(pPlant->TE_Stem	, pPlant->TE_StemGrain	);
		 OutFlow(pPlant->TE_Roots	, pPlant->TE_RootsGrain	);

  		 InFlow(pPlant->TE_Grain	, pPlant->TE_LeafGrain	);
		 InFlow(pPlant->TE_Grain	, pPlant->TE_StemGrain	);
		 InFlow(pPlant->TE_Grain	, pPlant->TE_RootsGrain	);


		// LitterFall
		 OutFlow(pPlant->TE_Leaf	, pPlant->TE_LeafSurfaceLitter	);
		 OutFlow(pPlant->TE_Stem	, pPlant->TE_StemSurfaceLitter	);
		 OutFlow(pPlant->TE_Grain	, pPlant->TE_GrainSurfaceLitter	);

 		 OutFlow(pPlant->TE_OldLeaf	, pPlant->TE_OldLeafSurfaceLitter	);
		 OutFlow(pPlant->TE_OldStem	, pPlant->TE_OldStemSurfaceLitter	);
		 OutFlow(pPlant->TE_OldRoots	, pPlant->TE_OldRootsLitter	);


		 //Do i=1, &NumPlants
		 for(size_t i=0; i<pPlant->TE_LeafSurfaceLitter.size(); i++) {	
			InFlow(pPlant->TE_SurfaceLitter, pPlant->TE_LeafSurfaceLitter[i]);
			InFlow(pPlant->TE_SurfaceLitter, pPlant->TE_StemSurfaceLitter[i]);
			InFlow(pPlant->TE_SurfaceLitter, pPlant->TE_GrainSurfaceLitter[i]);
			InFlow(pPlant->TE_SurfaceLitter, pPlant->TE_OldLeafSurfaceLitter[i]);
			InFlow(pPlant->TE_SurfaceLitter, pPlant->TE_OldStemSurfaceLitter[i]);
		 }
		 //Enddo


		// Reallocation
  		 OutFlow(pPlant->TE_Leaf	, pPlant->TE_LeafOldLeaf);
		 OutFlow(pPlant->TE_Stem	, pPlant->TE_StemOldStem);
		 OutFlow(pPlant->TE_Roots	, pPlant->TE_RootsOldRoots);

 		 InFlow(pPlant->TE_OldLeaf		, pPlant->TE_LeafOldLeaf );
		 InFlow(pPlant->TE_OldStem		, pPlant->TE_StemOldStem );
		 InFlow(pPlant->TE_OldRoots	, pPlant->TE_RootsOldRoots);

	}

		*/



void NC_Plant::PlantStage_CNRatio_PhysSize() {
	SnowPack *pMet = (SnowPack*)pPlant->p_Atm;
	pMet->f_daylength(m_pModelInfo->JDayNum);
	
	AtmNewMobile.assign(pPlant->NumPlants,0.);

  //  P_Harvest[_GRAIN][_C_].assign(pPlant->NumPlants,0.);

	for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
		for (size_t j = 0; j < NUM_PLANT_COMP; j++) {
			for (size_t iplant = 0; iplant < pPlant->NumPlants; iplant++) {
					P_LitterFall[j][jj][iplant] = 0.;
					P_Harvest[j][jj][iplant] = 0.;
			}
		}
		for (size_t j = 0; j < NUM_REALLOCATION; j++) P_ReAllocation[j][jj].assign(pPlant->NumPlants, 0.);
	}


	
// Input from Plant Biomass Input File (Switch: PhotoInput)
if(PhotoInput>=1)	{
    if(m_pModelStructure->TimeResolution>=1)	{
	;
    }
    if(NumValidInputPhotoInput>0){
	    //READECO(T_Current+DCH,	IPhotoInput, &NumValidInputPhotoInput,	VUTPG_PhotoFILE)
		//VUTPG_PhotoFILE
    }
}

auto CN_RatioPlant = [&]() {

	//Note that array for ratio starts at 0 for first element of No Carbon.
	//Also note the Component name is defined specific for this vector and differs from the index of P_State (_LEAF != _LEAF_)


for (size_t jj = 1; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
	if (jj == _N_)
		P_State_CarbonRatio[_LEAF_][jj - 1] = f_CarbonNitrogenRatioWithOldBiomass(P_State[_LEAF][_C_], P_State[_OLD_LEAF][_C_], P_State[_LEAF][jj], P_State[_OLD_LEAF][jj], P_State_CarbonRatio[_LEAF_][jj - 1]);
	else
		P_State_CarbonRatio[_LEAF_][jj - 1] = f_CarbonPhosporousRatioWithOldBiomass(P_State[_LEAF][_C_], P_State[_OLD_LEAF][_C_], P_State[_LEAF][jj], P_State[_OLD_LEAF][jj], P_State_CarbonRatio[_LEAF_][jj - 1]);


	P_State_CarbonRatio[_GRAIN_][jj - 1] = f_CarbonRatio(P_State[_GRAIN][_C_], P_State[_GRAIN][_N_], P_State_CarbonRatio[_GRAIN_][jj - 1]);
	P_State_CarbonRatio[_STEM_][jj - 1] = f_CarbonRatioWithOldBiomass(P_State[_STEM][_C_], P_State[_OLD_STEM][_C_], P_State[_STEM][jj], P_State[_OLD_STEM][jj], P_State_CarbonRatio[_STEM_][jj - 1]);
	P_State_CarbonRatio[_GRAIN_][jj - 1] = f_CarbonRatio(P_State[_GRAIN][_C_], P_State[_GRAIN][jj], P_State_CarbonRatio[_GRAIN_][jj - 1]);//HH
	P_State_CarbonRatio[_STEM_][jj - 1] = f_CarbonRatioWithOldBiomass(P_State[_STEM][_C_], P_State[_OLD_STEM][_C_], P_State[_STEM][jj], P_State[_OLD_STEM][jj], P_State_CarbonRatio[_STEM_][jj - 1]);
	P_State_CarbonRatio[_ROOT_][jj - 1] = f_CarbonRatioWithOldBiomass(P_State[_ROOT][_C_], P_State[_OLD_ROOT][_C_], P_State[_ROOT][jj], P_State[_OLD_ROOT][jj], P_State_CarbonRatio[_ROOT_][jj - 1]);
	P_State_CarbonRatio[_COARSE_ROOT_][jj - 1] = f_CarbonRatioWithOldBiomass(P_State[_COARSE_ROOT][_C_], P_State[_OLD_COARSE_ROOT][_C_], P_State[_COARSE_ROOT][jj], P_State[_OLD_COARSE_ROOT][jj], P_State_CarbonRatio[_COARSE_ROOT_][jj - 1]);
	if (FungalGrowth > 0) P_State_CarbonRatio[_FUNGI_][jj - 1] = f_CarbonRatio(P_State[_FUNGI][_C_], P_State[_FUNGI][jj], P_State_CarbonRatio[_FUNGI][jj - 1]); //HH
};


};
auto Growth_Stage = [&](size_t index) {
	double out;

	out = GrowthStageIndex[index]; // Keep previous value as default
	if (GrowthStageIndex[index]<0 && Max_GSI[index]>-1.) {  //// -1 Seed not in soil

		if (m_pModelInfo->JYearNum > int(m_StartYears[m_CountStartYear]) && m_StartYears.size() > m_CountStartYear + 1) {
			while (m_pModelInfo->JYearNum > int(m_StartYears[m_CountStartYear]) && m_StartYears.size() > m_CountStartYear + 1) {
				m_CountStartYear++;
			}

		}
		if (ZadokForcingFromFileInput_Sw(ZadokForcingFromFileInput) == ZadokForcingFromFileInput_Sw::on) {
			if (f_sow(m_pModelInfo->JYearNum, m_pModelInfo->JDayNum)) {
				out = 0;
				GrowingSum[index] = 0.;
				m_LatestSowingDay = m_pModelInfo->JDayNum;
			}
		}
		else if (SowingDayNo[index] > 0 && Sowing_Switch_Sw(Sowing_Switch) == Sowing_Switch_Sw::Fixed_dayNo) {                            //// 0 Seed in Soil
			if (int(m_pModelInfo->JDayNum) == int(SowingDayNo[index])) {
				out = 0.;
				GrowingSum[index] = 0.;
				m_LatestSowingDay = m_pModelInfo->JDayNum;
			}
		}
		else if (Sowing_Switch_Sw(Sowing_Switch) == Sowing_Switch_Sw::Multiple_DayNo) {
			if ((int(m_pModelInfo->JDayNum) == int(m_SowingDays[m_CountStartYear])) && (m_pModelInfo->JYearNum == int(m_StartYears[m_CountStartYear]) || m_YearTest)) {
				out = 0.;
				m_LatestSowingDay = m_pModelInfo->JDayNum;
				m_YearTest = false;
				if (m_pModelInfo->JYearNum == int(m_StartYears[m_CountStartYear]) && m_StartYears.size() > m_CountStartYear + 1 && Emergence_Switch != 3) {
					m_CountStartYear++;
					m_YearTest = true;
				}
			}
		}
		else if (Sowing_Switch_Sw(Sowing_Switch) == Sowing_Switch_Sw::Temperature_Sum) {
			out = -1 + f_Sowing(index, NDrivTAir, T_Step);
			if (out >= 0.) {
				f_Sowing = PlantTempSum(T_Thres_Sowing, T_Sum_Sowing); //SowingSum[index]=0.;
				m_LatestSowingDay = m_pModelInfo->JDayNum;
			}
		}
	}
	else if (GrowthStageIndex[index] <= 1. && Max_GSI[index] > 0. && !Emergence_Done[index]) { // // 1 emergence
		if (ZadokForcingFromFileInput_Sw(ZadokForcingFromFileInput) == ZadokForcingFromFileInput_Sw::on) {
			if (f_emergence(m_pModelInfo->JYearNum, m_pModelInfo->JDayNum)) {
				out = 1;
				Emergence_Done[index] = false;
				f_GrainStart = GrainStart(GrainSI_Step, GrainSI_StepDay, GrainSI_ThresDay, GrainSI_StepTemp, GrainSI_ThresTemp);


			}
		}
		else if (EmergenceDayNo[index] > 0 && Emergence_Switch_Sw(Emergence_Switch) == Emergence_Switch_Sw::Fixed_dayNo) {
			if (int(m_pModelInfo->JDayNum) == int(EmergenceDayNo[index])) {
				out = 1.;
				Emergence_Done[index] = false;
				GrowingStage[index] = 1.;
				f_GrainStart = GrainStart(GrainSI_Step, GrainSI_StepDay, GrainSI_ThresDay, GrainSI_StepTemp, GrainSI_ThresTemp);

			}
		}
		else if (Emergence_Switch_Sw(Emergence_Switch) == Emergence_Switch_Sw::Multiple_DayNo) {
			if ((int(m_pModelInfo->JDayNum) == int(m_EmergenceDays[m_CountStartYear])) && (m_pModelInfo->JYearNum == int(m_StartYears[m_CountStartYear]) || m_YearTest)) {
				out = 1.;
				f_GrainStart = GrainStart(GrainSI_Step, GrainSI_StepDay, GrainSI_ThresDay, GrainSI_StepTemp, GrainSI_ThresTemp);

				m_YearTest = true;
				Emergence_Done[index] = false;
				if (m_pModelInfo->JYearNum == int(m_StartYears[m_CountStartYear]) && m_StartYears.size() > m_CountStartYear + 1) {
					m_CountStartYear++;
				}
				else if (m_pModelInfo->JYearNum < int(m_StartYears[m_CountStartYear]) && m_StartYears.size() > m_CountStartYear + 1) {
					while (m_pModelInfo->JYearNum<int(m_StartYears[m_CountStartYear]) && m_StartYears.size() > m_CountStartYear + 1) {
						m_CountStartYear++;
					}

				}
			}
			else {
				auto koll2 = m_YearTest;
				if (int(m_pModelInfo->JDayNum) == int(m_EmergenceDays[m_CountStartYear])) {

				}

			}
		}
		else if ((Emergence_Switch_Sw(Emergence_Switch) == Emergence_Switch_Sw::Temperature_Sum || Winter_Regulation_Sw(Winter_Regulation) == Winter_Regulation_Sw::on) &&DormingStartDayNum[index]<1.) {
			out = max(0., Min_GSI[index]) + f_EmergenceWithMoistControl(index, NDrivTAir, NDrivTheta[0], T_Step);
			if (out >= 1.) {
				f_GrainStart = GrainStart(GrainSI_Step, GrainSI_StepDay, GrainSI_ThresDay, GrainSI_StepTemp, GrainSI_ThresTemp);

				f_Emergence = PlantTempSum(T_Thres_Emerg, T_Sum_Emerg);
				f_EmergenceWithMoistControl = PlantTempSum(T_Thres_Emerg, T_Sum_Emerg, p_Soil_WaterF->M_Wilting[0], MoistureRangeEmergence);
				Emergence_Done[index] = true;
				SpringDevDayNum[index] = m_pModelInfo->JDayNum;
				GrowingStage[index] = 1.;
			}
		}
		else if (Winter_Regulation_Sw(Winter_Regulation) == Winter_Regulation_Sw::on && DormingStartDayNum[index] > 1.) {
			out = max(0., Min_GSI[index]);
			SpringDevDayNum[index] = 0.;
			GrowingStage[index] = 0.;
		}
		else
			Emergence_Done[index] = false;
	}
	else if (GrowthStageIndex[index]<2.&&Max_GSI[index]>1.&&Emergence_Done[index]) { //
		if (Winter_Regulation_Sw(Winter_Regulation) == Winter_Regulation_Sw::on && DormingStartDayNum[index] > 1.) {
			out = Min_GSI[index];
			SpringDevDayNum[index] = 0.;
			GrowingStage[index] = 0.;
		}
		else {
			out = 1. + f_GrainStart(index, m_pModelInfo->DayLength, NDrivTAir, T_Step);
			if (out >= 2) {
				Emergence_Done[index] = false;
				f_GrainStart = GrainStart(GrainSI_Step, GrainSI_StepDay, GrainSI_ThresDay, GrainSI_StepTemp, GrainSI_ThresTemp);
			}
		}
	}
	else if (GrowthStageIndex[index]<3.&&Max_GSI[index]>2.) {
	// Grain filling start
		if (Winter_Regulation_Sw(Winter_Regulation) == Winter_Regulation_Sw::on && DormingStartDayNum[index] > 1.) {
			out = 0.;
			SpringDevDayNum[index] = 0.;
			GrowingStage[index] = 0.;
		}
		else {
			out = 2. + f_GrainFill(index, NDrivTAir, T_Step);
			if (out >= 3) {
				f_GrainFill = PlantTempSum(T_Thres_GrainFill, T_Sum_GrainFill);
			}
		}
	}
	else if (GrowthStageIndex[index]<4.&&Max_GSI[index]>3.) {	// Grain filling completed
		if (ZadokForcingFromFileInput_Sw(ZadokForcingFromFileInput) == ZadokForcingFromFileInput_Sw::on) {
			if (f_harvest(m_pModelInfo->JYearNum, m_pModelInfo->JDayNum)) {
				out = 4;
			}
		}
		else if (Harvest_Day == 5) {
			if ((int(m_pModelInfo->JDayNum) == int(m_HarvestDays[m_CountStartYear])) && (m_pModelInfo->JYearNum == int(m_StartYears[m_CountStartYear]) || m_YearTest)) {
				out = 4.;
			}
		}
		else  {
			out = 3. + f_Harvest(index, NDrivTAir, T_Step);
			if (out >= 4) {
				f_Harvest = PlantTempSum(T_Thres_Ripening, T_Sum_Ripening);
			}
		}
	}
	return out;
};
auto Zadok_Index = [&](size_t i) {
	if (ZadokScale_Sw(ZadokScale) >= ZadokScale_Sw::on) {
		ZadokIndex[i] = f_Zadok(GrowthStageIndex[i]);
		if (ZadokScale_Sw(ZadokScale) == ZadokScale_Sw::withOutPut2File)
			ZadokDayNumShift[i] = pZadokOutCreateFile->Update(ZadokIndex[i], int(m_pModelInfo->JDayNum+0.001), m_pModelInfo->m_ActMin, int(pMet->PGFileName_Index+0.0001));
		else if (ZadokScale_Sw(ZadokScale) == ZadokScale_Sw::withOutPut3File)
			ZadokDayNumShift[i] = pZadokOutCreateFile->Update(ZadokIndex[i], int(m_pModelInfo->JDayNum + 0.001), m_pModelInfo->m_ActMin, int(m_pModelInfo->ActualNoMulRun));

	}
};
auto ConditionalUpdateOfPlantAge = [&](size_t i) {

	if (GrowthStageIndex[i] >= 0.)
		SimPlantAge[i] += T_Step;


};
auto EmergenceAndRelatedUpdates = [&](size_t i) {

	if (P_State[_ROOT][_C_][i] + P_State[_OLD_ROOT][_C_][i] > 0.1) {
		C_Seed_Avail = P_State[_ROOT][_C_][i] + P_State[_OLD_ROOT][_C_][i];
		P_State[_ROOT][_C_][i] = 0.;
		P_State[_OLD_ROOT][_C_][i] = 0.;
	}
	else {
		C_Seed_Avail = C_Seed[i];
		C_Seed_Added = C_Seed_Added + C_Seed_Avail;
		N_Seed_Added = N_Seed_Added + C_Seed_Avail / CN_MinRootsFunc(i);
		P_Seed_Added = P_Seed_Added + C_Seed_Avail / CP_MinRootsFunc(i);
		Emergence_Done[i] = true;
	}

	P_State[_ROOT][_C_][i] = f_AlloCoefRoot(i)*C_Seed_Avail + P_State[_ROOT][_C_][i];
	P_State[_LEAF][_C_][i] = f_AlloCoefLeaf(i)*C_Seed_Avail;
	P_State[_STEM][_C_][i] = (1. - f_AlloCoefLeaf(i) - f_AlloCoefRoot(i))*(1 - AlloCoarseRoots[i])*C_Seed_Avail;
	P_State[_COARSE_ROOT][_C_][i] = (1. - f_AlloCoefLeaf(i) - f_AlloCoefRoot(i))*AlloCoarseRoots[i] * C_Seed_Avail;

	P_State[_ROOT][_N_][i] = P_State[_ROOT][_C_][i] / CN_MinRootsFunc(i);
	P_State[_COARSE_ROOT][_N_][i] = P_State[_COARSE_ROOT][_C_][i] / CN_MinCoarseRootsFunc(i);
	P_State[_LEAF][_N_][i] = P_State[_LEAF][_C_][i] / CN_MinLeafFunc(i);
	P_State[_STEM][_N_][i] = P_State[_STEM][_C_][i] / CN_MinStemFunc(i);

	P_State[_ROOT][_P_][i] = P_State[_ROOT][_C_][i] / CP_MinRootsFunc(i);
	P_State[_COARSE_ROOT][_P_][i] = P_State[_COARSE_ROOT][_C_][i] / CP_MinCoarseRootsFunc(i);
	P_State[_LEAF][_P_][i] = P_State[_LEAF][_C_][i] / CP_MinLeafFunc(i);
	P_State[_STEM][_P_][i] = P_State[_STEM][_C_][i] / CP_MinStemFunc(i);
};
auto LeafShootingFromMobilePool = [&](size_t i) {
	P_ReAllocation[MOBILE_LEAF][_C_][i] = max(0., ShootCoef[i] * P_State[_MOBILE][_C_][i]);
	P_ReAllocation[MOBILE_LEAF][_N_][i] = max(0., ShootCoef[i] * P_State[_MOBILE][_N_][i]);
	P_ReAllocation[MOBILE_LEAF][_P_][i] = max(0., ShootCoef[i] * P_State[_MOBILE][_P_][i]);

};
auto PhysicalCharacteristicsofLivingPlant = [&](size_t i) {
	pPlant->SimPlantHeight[i] = pPlant->f_CanopyHeight_Mass_Age(i, P_State[_STEM][_C_][i] + P_State[_LEAF][_C_][i] + P_State[_OLD_STEM][_C_][i] + P_State[_OLD_LEAF][_C_][i], SimPlantAge[i], P_State[_GRAIN][_C_][i]);
	pPlant->SimPlantAlbedo[i] = pPlant->f_Albedo_GrowthStage(i, GrowthStageIndex[i]);
	if (pPlant->SLA_Dynamic == 0) pPlant->SimLeafAreaIndex[i] = pPlant->f_LAI_Mass(i, P_State[_LEAF][_C_][i] + P_State[_OLD_LEAF][_C_][i]);
	else pPlant->SimLeafAreaIndex[i] = pPlant->f_LAI_Mass_Stage(i, P_State[_LEAF][_C_][i] + P_State[_OLD_LEAF][_C_][i], GrowthStageIndex[i]);
};


auto ConditionalUpdateofLAIfromFileInput = [&]() {
	// Leaf Area Input From Plant Biomass Input File (PhotoInput)
	if (LAI_Position.size() > 0) {
		for (size_t i = 0; i < pPlant->SimLeafAreaIndex.size(); i++) {
			if (LAI_Position[i].ivar > 0) {
				pPlant->SimLeafAreaIndex[i] = GetEcoDat(&m_PGPos_PlantBiomass, LAI_Position[i]);
			}
		}
	}
};
    
// CN ratios, &life stage update, physical characteristics
// Loop over number of plants

for (size_t jj = 1; jj < m_pModelStructure->m_NumActPlantElements; jj++)
	P_State_CarbonRatio[_LEAF_][jj - 1] = f_CarbonRatioWithOldBiomass(P_State[_LEAF][_C_], P_State[_OLD_LEAF][_C_], P_State[_LEAF][jj], P_State[_OLD_LEAF][jj], P_State_CarbonRatio[_LEAF_][jj - 1]);
CN_RatioPlant();
	
auto PreviousSeason = SpringDev;
	for(size_t i=0; i<P_State[_LEAF][_N_].size(); i++) {


		GrowthStageIndex[i]=Growth_Stage(i);
		Zadok_Index(i);
		//GrowthTempSum[i] = f_GrowthTemp(i, NDrivTAir, m_pModelInfo->JDayNum, T_Step);


		GrowthTempSum[i] = f_DormingAndGrowthTempSum(i, NDrivTAir, m_pModelInfo->JDayNum, T_Step, DormingTempSum[i], SpringDev);
		ConditionalUpdateOfPlantAge(i);
		if(Growth_Sw(Growth)>=Growth_Sw::Water_use_efficiency) {		
			
			if(GrowthStageIndex[i]>=1&& GrowthStageIndex[i]<1.1&&Min_GSI[i]<=1.&&!Emergence_Done[i]&&Max_GSI[i]>1.) { //Emergence
				EmergenceAndRelatedUpdates(i);
			}  				
			else if(GrowthStageIndex[i]>1&& GrowthStageIndex[i]<=2&&P_State[_MOBILE][_N_][i]>1.E-4&&SpringDev) 
				LeafShootingFromMobilePool(i);	
			else if (GrowthStageIndex[i] >= 1 && GrowthStageIndex[i] < 1.1) {

			}
		}
		PhysicalCharacteristicsofLivingPlant(i);
	}
	ConditionalUpdateofLAIfromFileInput();
}
void NC_Plant::Light_And_PhotoSynthesis() {

	RADDIST radout;
	if(Growth_Sw(Growth)>= Growth_Sw::Radiation_use_efficiency) {
		//Adjustment of PlantHeight when GSI is showing a plant above soil surface
		for (size_t i = 0; i < GPP_Current.Temp.size(); i++) {
			if (GrowthStageIndex[i] >= 1.)
				pPlant->PlantHeight[i] = max(0.01, pPlant->PlantHeight[i]);
			else
				pPlant->PlantHeight[i] = 0.001;
		}
		if(Rad_Interception_Sw(RadInterception)>= Rad_Interception_Sw::Assumed_Input) {
			radout=pAtm->RadInterceptFunc(NDrivRadInShort, DIRECT_SHORT);
			RadPlantAdsorbed=radout.DistFrac;
		}
		else {
			radout=pAtm->RadInterceptFunc(NDrivRadInShort, SIMLAI);
			RadPlantAdsorbed=radout.DistFrac;
			for (size_t i = 0; i < GPP_Current.Temp.size(); i++)
				if (!NumTest(RadPlantAdsorbed[i])) RadPlantAdsorbed[i] = 0.;
		}
	}


// Assimilaton Flow

	if(Growth_Sw(Growth)== Growth_Sw::Logistic_function) {	// Logistic growth
		//Do i=1, NumPlants
		for(size_t i=0; i<GPP_Driver.Salt.size(); i++) {
			GPP_Driver.Salt[i]=NDrivSaltStress[i];
			if(Salinity_Stress_Sw(Salinity_Stress)==Salinity_Stress_Sw::on_photosynthesis) 
				AtmNewMobile[i]=f_LogisticGrowth(i, m_pModelInfo->JDayNum)*UptakeCNRatio[i]*NDrivWaterStress[i]*GPP_Driver.Salt[i];
			else
				AtmNewMobile[i]=f_LogisticGrowth(i, m_pModelInfo->JDayNum)*UptakeCNRatio[i]*NDrivWaterStress[i];
			
		}
	}
	else if(Growth==2) {	// Water use efficiency
		//Do i=1, NumPlants
		for(size_t i=0; i<GPP_Driver.Salt.size(); i++) {
			GPP_Driver.Salt[i]=NDrivSaltStress[i];
			if (Salinity_Stress_Sw(Salinity_Stress) == Salinity_Stress_Sw::on_photosynthesis)
				AtmNewMobile[i]=NDrivTranspiration[i]*PhoWaterEfficiency[i]*Fix::BiomassToCarbon*GPP_Driver.Salt[i];
			else
				AtmNewMobile[i]=NDrivTranspiration[i]*PhoWaterEfficiency[i]*Fix::BiomassToCarbon;
			
		}
	}
	else  if(Growth==3) { // Light use efficiency
		//Do i=1, NumPlants
		for(size_t i=0; i<GPP_Current.Temp.size(); i++) {
			if (GrowthStageIndex[i] >= 1.) {
				
				if (NDrivRadInShort > 100.) GPP_Current.Light[i] = RadPlantAdsorbed[i] / NDrivRadInShort;
				else  GPP_Current.Light[i] = 0.;
				
				if (GreenIndexForcing_Sw(GreenIndexForcing) == GreenIndexForcing_Sw::on) {
					GPP_Current.Temp[i] = f_PhotoTemp(i, pAtm->MC_TempCanopy[i], GreenIndexDriver)*TempResponseCurrent[i] +
						(1 - TempResponseCurrent[i])*GPP_Current.Temp[i];
				}
				else {
					GPP_Current.Temp[i] = f_PhotoTemp(i, pAtm->MC_TempCanopy[i], GrowthTempSum[i])*TempResponseCurrent[i] +
						(1 - TempResponseCurrent[i])*GPP_Current.Temp[i];
				}
			
				if(N_Fixed_Supply>0) 
					GPP_Current.Nitrogen[i]=N_MinDegreeSupply[i];
				else
					GPP_Current.Nitrogen[i]=f_PhotoNLeaf(i,P_State_CarbonRatio[_LEAF_][_N_-1][i]);

				GPP_Current.Water[i]=NDrivWaterStress[i];
				GPP_Current.Salt[i]=NDrivSaltStress[i];
				if (Plant_Stress_Sw(PlantStress) >= Plant_Stress_Sw::Combined_Water_and_Nitrogen_function)
					GPP_Current.Nutrient_Water[i] = f_Photo_Combined_N_Water_Response(i, GPP_Current.Water[i], GPP_Current.Nitrogen[i]);
			    else
			        GPP_Current.Nutrient_Water[i]=GPP_Current.Nitrogen[i]*GPP_Current.Water[i];
				if (m_pModelStructure->m_NumActPlantElements == 3) {
					GPP_Current.Phosphorus[i] =  f_PhotoPLeaf(i, P_State_CarbonRatio[_LEAF_][_P_ - 1][i]);
					//GPP_Current.Nutrient_Water[i] *= GPP_Current.Phosphorus[i];//could be other options HH
					GPP_Current.Nutrient_Water[i] = min(GPP_Current.Phosphorus[i], GPP_Current.Nitrogen[i]) * GPP_Current.Water[i]; //could be other options HH but starts with law of minmum
				}

				if (Salinity_Stress_Sw(Salinity_Stress) == Salinity_Stress_Sw::on_photosynthesis)
					GPP_Current.AllCombined[i]= GPP_Current.Temp[i] * GPP_Current.Nutrient_Water[i] * GPP_Current.Salt[i];
				else
					GPP_Current.AllCombined[i] = GPP_Current.Temp[i] * GPP_Current.Nutrient_Water[i];

				double MaxGPP = f_MaxGPP(i, RadPlantAdsorbed[i]);		
				if (Max_GSI[i] > 2&& GrowthStageIndex[i]>2) {
					double GrainFillingStage = max(0., min(GrowthStageIndex[i] - 2., 1.));
					MaxGPP = MaxGPP*f_PhotoGrainFill(i, GrainFillingStage);
				}
				if (PhoSaturation_Sw(PhoSaturation) == PhoSaturation_Sw::on) {
					MaxGPP = MaxGPP*f_PhotoSaturation(i, MaxGPP);
				}
				AtmNewMobile[i]=MaxGPP*GPP_Current.AllCombined[i];				
			}
		}
		if (Growth_Sw(Growth) >= Growth_Sw::Radiation_use_efficiency&&
			GrowthResponseVar_Sw(GrowthResponseVar) >= GrowthResponseVar_Sw::Mean_and_Delayed) {
			GPP_Driver = f_IntegratedAuxGPPResponse(GPP_Current, AtmNewMobile);
		}
		else
			GPP_Driver = GPP_Current;
	}
	else if (Growth==4) {  // Farquhar
		for(size_t i=0; i<GPP_Current.Salt.size(); i++) {
			GRespons gresponse=P_Responses(i);
			GPP_Current.Salt[i]=NDrivSaltStress[i];
			GPP_Current.Nitrogen[i]=gresponse.N;
			GPP_Current.Temp[i]=gresponse.Temp;
			GPP_Current.Water[i]=gresponse.W;
			if(Salinity_Stress==1) 
				AtmNewMobile[i]=P_PhoSiB(i)*GPP_Current.Salt[i];
			else
				AtmNewMobile[i]=P_PhoSiB(i);			
		}	
		if (Growth_Sw(Growth) >= Growth_Sw::Radiation_use_efficiency &&
			GrowthResponseVar_Sw(GrowthResponseVar) >= GrowthResponseVar_Sw::Mean_and_Delayed) {
			GPP_Driver = f_IntegratedAuxGPPResponse(GPP_Current, AtmNewMobile);
		}
		else
			GPP_Driver = GPP_Current;
	}


}

void NC_Plant::AllocationAndReAllocation_DuringGrowth() {
	SoilOrgModel* p_Soil = (SoilOrgModel*)m_pModelInfo->GetModelPtr("Soil Org Model");
	NC_Organic* pNCSoil = &p_Soil->m_NC_SoilOrg;
	// ReAllocation Flows
	size_t jj = _C_; // Nitrogen and P is in the uptake section
	
	P_CarbonAlloNewMobile = f_AlloCarbonPlant(P_State[_LEAF][_C_], P_State[_STEM][_C_], GPP_Current.Nitrogen, GPP_Current.Phosphorus, NDrivWaterStress, P_State[_NEWMOBILE][jj], P_CarbonAlloNewMobile);//HH


	if (ReAllocationToLeaf_Sw(ReAllocationToLeaf) == ReAllocationToLeaf_Sw::on) {
		for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++)
			P_ReAllocation[ROOT_LEAF][jj] = f_RootToLeaf(HarvestDoneThisYear, P_State[_ROOT][jj], P_State[_LEAF][jj], P_ReAllocation[ROOT_LEAF][jj]);
	}
	for (size_t j = LEAF_GRAIN; j <= ROOT_GRAIN; j++) {
		for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++)
			if (j == ROOT_GRAIN)
				P_ReAllocation[j][jj] = f_GrainSupply(_ROOT, jj, GrowthStageIndex, P_State[_ROOT][jj], P_ReAllocation[j][jj]);
			else
				P_ReAllocation[j][jj] = f_GrainSupply(j - LEAF_GRAIN + 1, jj, GrowthStageIndex, P_State[j - LEAF_GRAIN + 1][jj], P_ReAllocation[j][jj]);
	}
	for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
		if(jj==0) P_ReAllocation[LEAF_STEM][jj] = f_ReAlloCarbonToStemFromLeaf(GrowthStageIndex, P_State[_LEAF][jj], P_ReAllocation[LEAF_STEM][jj]);
		else 
			P_ReAllocation[LEAF_STEM][jj] = f_ReAlloNitrogenToStemFromLeaf(GrowthStageIndex, P_ReAllocation[LEAF_STEM][_C_], P_ReAllocation[LEAF_STEM][jj]);

	}
	if (FungalGrowth_Sw(FungalGrowth) > FungalGrowth_Sw::off) {
		P_ReAllocation[NEWMOBILE_FUNGI][_C_] = f_AlloFungi(pNCSoil->SoilNitrate, pNCSoil->SoilAmmonium, P_State[_ROOT][_C_], P_State[_OLD_ROOT][_C_], P_State[_FUNGI][_C_],
			NFungiPlant, NRootUptake, DemandUptake[0].Demand, P_CarbonAlloNewMobile.Roots, P_Respiration[_FUNGI], P_ReAllocation[NEWMOBILE_FUNGI][_C_]);
		if (m_pModelStructure->m_NumActPlantElements >= 3)//HH
			P_ReAllocation[NEWMOBILE_FUNGI][_C_] = f_AlloFungiwithP(pNCSoil->SoilNitrate, pNCSoil->SoilAmmonium, pNCSoil->SoilPhosphate, P_State[_ROOT][_C_], P_State[_OLD_ROOT][_C_], P_State[_FUNGI][_C_],
				NFungiPlant, NRootUptake, DemandUptake[0].Demand, PFungiPlant, PRootUptake,  DemandUptake[1].Demand, P_CarbonAlloNewMobile.Roots, P_Respiration[_FUNGI], P_ReAllocation[NEWMOBILE_FUNGI][_C_]);

		for(size_t i=0; i< P_CarbonAlloNewMobile.Roots.size();i++)
			P_CarbonAlloNewMobile.Roots[i] -= P_ReAllocation[NEWMOBILE_FUNGI][_C_][i];

	}
}
void NC_Plant::AboveGroundPlantRespiration() {
	if (PlantRespiration_Sw(PlantRespiration) == PlantRespiration_Sw::Maintenance_Only) {
		P_Respiration[_STEM] = f_PlantRespiration(PlantComp::STEM, P_State[_STEM][_C_], NDrivTAir, P_Respiration[_STEM]);
		P_Respiration[_LEAF] = f_PlantRespiration(PlantComp::LEAF, P_State[_LEAF][_C_], NDrivTAir, P_Respiration[_LEAF]);
	}
	else {
		P_Respiration[_LEAF] = f_PlantPhotoRespiration(PlantComp::LEAF, P_State[_LEAF][_C_], P_CarbonAlloNewMobile.Leaf, NDrivTAir, P_Respiration[_LEAF]);
		P_Respiration[_STEM] = f_PlantPhotoRespiration(PlantComp::STEM, P_State[_STEM][_C_], P_CarbonAlloNewMobile.Stem,  NDrivTAir, P_Respiration[_STEM]);
	}
	P_Respiration[_GRAIN] = f_PlantRespiration(PlantComp::GRAIN, P_State[_GRAIN][_C_], NDrivTAir, P_Respiration[_GRAIN]);
	P_Respiration[_OLD_STEM] = f_PlantRespiration(PlantComp::OLD_STEM, P_State[_OLD_STEM][_C_], NDrivTAir, P_Respiration[_OLD_STEM]);
	P_Respiration[_OLD_LEAF] = f_PlantRespiration(PlantComp::OLD_LEAF, P_State[_OLD_LEAF][_C_], NDrivTAir, P_Respiration[_OLD_LEAF]); 
}
void NC_Plant::BelowGrandPlantRespiration() {
	
  CRootsAtmLayer.assign(CRootsAtmLayer.size(),0.0);
  CFungiAtmLayer.assign(CRootsAtmLayer.size(),0.0);
  CCoarseRootsAtmLayer.assign(CRootsAtmLayer.size(),0.0);

  PlantSoilLayers In;
  In= f_PlantBelowGroundRespiration(PlantComp::ROOT, UptakeDistWithDepth, P_State[_ROOT][_C_], NDrivTemperature);
  auto addlayerResp = [&]() {
	  for (size_t i = 0; i < CRootsAtmLayer.size(); i++) {
		  CRootsAtmLayer[i] += In.Layers[i];  // Representing also coarse roots and Fungi
	  }
  };

 // Respiration of Below Ground parts

  if (PlantRespiration_Sw(PlantRespiration) == PlantRespiration_Sw::Maintenance_Only) {
	  In = f_PlantBelowGroundRespiration(PlantComp::ROOT, UptakeDistWithDepth, P_State[_ROOT][_C_], NDrivTemperature);
	  P_Respiration[_ROOT] = In.Plants; addlayerResp();
	  In = f_PlantBelowGroundRespiration(PlantComp::COARSEROOT, UptakeDistWithDepth, P_State[_COARSE_ROOT][_C_], NDrivTemperature);
	  P_Respiration[_COARSE_ROOT] = In.Plants; addlayerResp();
	  In = f_PlantBelowGroundRespiration(PlantComp::FUNGI, UptakeDistWithDepth, P_State[_FUNGI][_C_], NDrivTemperature);
	  P_Respiration[_FUNGI] = In.Plants; addlayerResp();
  }
  else {
	  In = f_PlantPhotoBelowGroundRespiration(PlantComp::ROOT, UptakeDistWithDepth, P_State[_ROOT][_C_], P_CarbonAlloNewMobile.Roots, NDrivTemperature);
	  P_Respiration[_ROOT] = In.Plants; addlayerResp();
	  In = f_PlantPhotoBelowGroundRespiration(PlantComp::COARSEROOT, UptakeDistWithDepth, P_State[_COARSE_ROOT][_C_], P_CarbonAlloNewMobile.CoarseRoots, NDrivTemperature);
	  P_Respiration[_COARSE_ROOT] = In.Plants; addlayerResp();
	  In = f_PlantPhotoBelowGroundRespiration(PlantComp::FUNGI, UptakeDistWithDepth, P_State[_FUNGI][_C_], P_ReAllocation[NEWMOBILE_FUNGI][_C_], NDrivTemperature);
	  P_Respiration[_FUNGI] = In.Plants; addlayerResp();
  }

  In = f_PlantBelowGroundRespiration(PlantComp::OLD_ROOT, UptakeDistWithDepth, P_State[_OLD_ROOT][_C_], NDrivTemperature);
  P_Respiration[_OLD_ROOT] = In.Plants; addlayerResp();
  In = f_PlantBelowGroundRespiration(PlantComp::OLD_COARSE_ROOT, UptakeDistWithDepth, P_State[_OLD_COARSE_ROOT][_C_], NDrivTemperature);
  P_Respiration[_OLD_COARSE_ROOT] = In.Plants; addlayerResp();
  
  for (size_t i = 0; i < P_Respiration[_ROOT].size(); i++) {
	  if ( HarvestDayNo[i] == m_pModelInfo->JDayNum) {		//Harvest
		  P_Respiration[_ROOT][i] = 0.;
		  P_Respiration[_OLD_ROOT][i] = 0.;
		  P_Respiration[_GRAIN][i] = 0.;
	  }
  }


}
void NC_Plant::LitterFall() {
	auto abovegroundLitterFall = [&](size_t i, double Tsum) {
		for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
			P_LitterFall[_LEAF][jj][i] = f_LeafLitterFall(true, i, Tsum, m_pModelInfo->JDayNum, pPlant->SimLeafAreaIndex[i], P_State[_LEAF][jj][i]);
			P_LitterFall[_STEM][jj][i] = f_StemLitterFall(true, i, Tsum, m_pModelInfo->JDayNum, P_State[_STEM][jj][i]);
			P_LitterFall[_GRAIN][jj][i] = f_GrainLitterFall(i, Tsum, m_pModelInfo->JDayNum, P_State[_GRAIN][jj][i]);
			if (P_LitterFall[_LEAF][jj][i] > -10.&&P_LitterFall[_LEAF][jj][i] < 1000.) {



			}
			else
				P_LitterFall[_LEAF][jj][i] = 0.;


			if (Max_Plant_LifeTime[i] > 1.) {
				P_LitterFall[_OLD_LEAF][jj][i] = f_LeafLitterFall(false, i, Tsum, m_pModelInfo->JDayNum, pPlant->SimLeafAreaIndex[i], max(0., P_State[_OLD_LEAF][jj][i] - RemainOldLeaf[jj][i]));
				P_LitterFall[_OLD_STEM][jj][i] = f_StemLitterFall(false, i, Tsum, m_pModelInfo->JDayNum, max(0., P_State[_OLD_STEM][jj][i]));


			}
			if (Winter_Regulation_Sw(Winter_Regulation) == Winter_Regulation_Sw::on) {
				if ((Tsum >= Leaf_LitterTsum2[i] && DormingStartDayNum[i] <= 1.) || (m_pModelInfo->JDayNum >= Dorm_FinalDayNo[i] && DormingStartDayNum[i] <= 1.)) {
					DormingStartDayNum[i] = m_pModelInfo->JDayNum;
					GrowingStage[i] = 0.;
				}
				else if (Tsum <= 1.&& m_pModelInfo->JDayNum <= Dorm_FinalDayNo[i]) DormingStartDayNum[i] = 0.;
			}
		}
	};
	auto belowgroundLitterFall = [&](size_t i, double Tsum) {
		for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
			if (P_State[_ROOT][jj][i] > 0.) 
				P_LitterFall[_ROOT][jj][i] = f_RootLitterFall(true, i, Tsum, m_pModelInfo->JDayNum, P_State[_ROOT][jj][i]);
			else 
				P_LitterFall[_ROOT][jj][i] = 0;
			P_LitterFall[_COARSE_ROOT][jj][i] = max(0., f_CoarseRootLitterFall(true, i, Tsum, m_pModelInfo->JDayNum, P_State[_COARSE_ROOT][jj][i]));

			if (Max_Plant_LifeTime[i] > 1.) {
				P_LitterFall[_OLD_ROOT][jj][i] = f_RootLitterFall(false, i, Tsum, m_pModelInfo->JDayNum, P_State[_OLD_ROOT][jj][i]);
				P_LitterFall[_OLD_COARSE_ROOT][jj][i] = f_CoarseRootLitterFall(false, i, Tsum, m_pModelInfo->JDayNum, P_State[_OLD_COARSE_ROOT][jj][i]);
			}

			if (FungalGrowth_Sw(FungalGrowth) > FungalGrowth_Sw::off) {
				if(jj==_C_) P_LitterFall[_FUNGI][jj][i] = f_FungiLitterFall(i, Tsum, m_pModelInfo->JDayNum, P_State[_FUNGI][jj][i]);
				else if(jj==_N_) P_LitterFall[_FUNGI][jj][i] = f_FungiLitterFall(i, Tsum, m_pModelInfo->JDayNum, P_State[_FUNGI][_N_][i]) * (1 - NretFungi[i]);//HH
				else if (jj == _P_) P_LitterFall[_FUNGI][jj][i] = f_FungiLitterFall(i, Tsum, m_pModelInfo->JDayNum, P_State[_FUNGI][jj][i]) * (1 - PretFungi[i]);//HH
			}
		}
	};
	 for(size_t i=0; i<P_LitterFall[_LEAF][_C_].size();i++) {
		 double Tsum;
		 if (LitterFall_Dynamics_Sw(LitterFall_Dynamics) == LitterFall_Dynamics_Sw::GrowthTempSum)
			 Tsum = GrowthTempSum[i];
		 else if (LitterFall_Dynamics_Sw(LitterFall_Dynamics) == LitterFall_Dynamics_Sw::DormingTempSum)
			 Tsum = DormingTempSum[i];
		 else
			 Tsum = 0.;
		 abovegroundLitterFall(i, Tsum);
		 belowgroundLitterFall(i, Tsum);
	 }
}
void NC_Plant::Cleaning_Harvest_Events() {
	SoilOrgModel* p_Soil=(SoilOrgModel*)m_pModelInfo->GetModelPtr("Soil Org Model");
	NC_SoilManagement* pNCSoil=&p_Soil->m_NC_SoilOrg;


	if (m_ICountDay == m_pModelInfo->JDayNum) {
		m_ICountDay = size_t(m_pModelInfo->JDayNum + 1);
		m_ResetMade = false;

	}
	else {
		if (m_ICountDay >= 365)	m_ICountDay = size_t(1);
		if (!m_ResetMade) {
			for (size_t i_plant = 0; i_plant < P_ReAllocation[0][0].size(); i_plant++)
				for (size_t j = _LEAF; j <= _OLD_COARSE_ROOT; j++)
					for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++)
						P_Harvest[j][jj][i_plant] = 0;
			m_ResetMade = true;
		}
		return;

	}


	for(size_t i_plant=0; i_plant<P_ReAllocation[0][0].size();i_plant++) {
		if(Cleaning>0) {
		    DoCleaning=false;
		    if(Cleaning==1) 
		        YearToClean=true;
		    else {
		        YearToClean=false;
		       // Do jj=1, Num_CleaningYears
				for(size_t jj=0; jj<CleaningYears.size();jj++) {
		            if(int(CleaningYears[jj])==int(SimPlantYearAge[i_plant]))
		                YearToClean=true;	            
				}
		    }
		    if(YearToClean) {
		      //  Do JJP=1, Num_CleaningDays
				for(size_t jjp=0; jjp<CleaningDays.size(); jjp++) {
					if(int(CleaningDays[jjp])==m_pModelInfo->JDayNum&&CleaningPlantIndex[jjp]-1==i_plant) {
		                DoCleaning=true;
		                IndexClean=i_plant;
		                ResetCleaningDay=size_t(m_pModelInfo->JDayNum+1);
		            }
				}       
		    }	
		}
		if(Harvest_Day_Sw(Harvest_Day)==Harvest_Day_Sw::Fixed_Multiple_Days) {
		    DoHarvest=false;
            DoFinalHarvest=false;
		    HarvestDayNo[i_plant]=-1;
		    for(size_t jjp=0; jjp<SimPlantYearAge.size(); jjp++) { //Do JJP=1, NumPlants
				for(size_t jj=0; jj<HarvestYears.size(); jj++) {   //Do jj=1, Num_HarvestYears		
					if(PlantsToHarvest==0&&int(SimPlantYearAge[jjp])==int(HarvestYears[jj])) {
						DoHarvest=true;
						if(Harvest_Type_Years[jj]>0.5) 	DoFinalHarvest = true;
					}
					else if(size_t(HarvestYearPlantIndex[jj]+0.01)==i_plant&&jjp==i_plant&&int(SimPlantYearAge[jjp])==int(HarvestYears[jj])) {
						DoHarvest=true  ;
						if(jj==HarvestYears.size()-1  ) DoFinalHarvest=true;
    				}
				}
			}
		    //EndDo
		    if(DoHarvest) {
				 // Do jj=1, Num_HarvestDays  
				  for(size_t jj=0; jj<HarvestDays.size();jj++) {	
						if(PlantsToHarvest==0&&m_pModelInfo->JDayNum==int(HarvestDays[jj])) {
							HarvestDayNo[i_plant]=m_pModelInfo->JDayNum; 
						}
						else if(size_t(HarvestDayPlantIndex[jj]+0.01)==i_plant&&m_pModelInfo->JDayNum==int(HarvestDays[jj])) {
							HarvestDayNo[i_plant]=m_pModelInfo->JDayNum;
						}
						if (Harvest_Type_Days[jj] > 0.5&&DoFinalHarvest) 	DoFinalHarvest = true;
						else  DoFinalHarvest = false;
					}
   		    } 
   		    if(HarvestDayNo[i_plant]>0) HarvestToBeDone[i_plant]=true;  
		}
   		else if(Harvest_Day_Sw(Harvest_Day)==Harvest_Day_Sw::Estimated_GSI) {
   		    HarvestDayNo[i_plant]=0;
   		    if(GrowthStageIndex[i_plant]>=4)
                HarvestDayNo[i_plant]=m_pModelInfo->JDayNum;		   		      		    
   		}
   		else if(Harvest_Day_Sw(Harvest_Day)==Harvest_Day_Sw::Estimated_LAI&&SimPlantYearAge.front()<(Max_Plant_LifeTime.front()-10)) {
   		    HarvestDayNo[i_plant]=-1;
           // if(SimPlantYearAge.front()+8>PreviousThinningYear) {       		
   		        if(pPlant->SimLeafAreaIndex[i_plant]>=HarvestLAIValue[i_plant]){
   		            HarvestDayNo[i_plant]=m_pModelInfo->JDayNum;
                    //PreviousThinningYear=size_t(SimPlantYearAge.front());
   		        }  		        
   		    //}
		}
   		else if(Harvest_Day_Sw(Harvest_Day)==Harvest_Day_Sw::PG_File_specified&&ICountHarvest<NumValidHarvest) {
   		    DoHarvest=false;
   		    if(m_pModelInfo->m_ActMin>=IHarvestDates[ICountHarvest]) {
   		        HarvestDayNo[i_plant]=m_pModelInfo->JDayNum;
   		        ICountHarvest++;
   		        DoHarvest=true;
			}
   		    else
   		        HarvestDayNo[i_plant]=-1;
   		    
		    if(DoHarvest) {
		        //  Do jj=1, Num_HarvestDays  
				for(size_t jj=0; jj<HarvestDays.size();jj++) {
		            if(PlantsToHarvest==0&&m_pModelInfo->JDayNum==int(HarvestDays[jj])) 
		                HarvestDayNo[i_plant]=m_pModelInfo->JDayNum;
		            else if(HarvestDayPlantIndex[jj]==i_plant&&m_pModelInfo->JDayNum==int(HarvestDays[jj])) 
                        HarvestDayNo[i_plant]=m_pModelInfo->JDayNum;  		                
		         }
		          //EndDo
   		    }    		    
   		    
   		    
		}
   		else if(Harvest_Day_Sw(Harvest_Day)!=Harvest_Day_Sw::Fixed_Single) 
   		     HarvestDayNo[i_plant]=-1;
		
		// At Harvest
		if(((GrowthStageIndex[i_plant]==4&&(Harvest_Day_Sw(Harvest_Day)==Harvest_Day_Sw::Estimated_GSI||
			Harvest_Day_Sw(Harvest_Day)==Harvest_Day_Sw::In_Year_Table))||
			(HarvestDayNo[i_plant]==m_pModelInfo->JDayNum&&Harvest_Day_Sw(Harvest_Day) != Harvest_Day_Sw::Estimated_GSI &&
				Harvest_Day_Sw(Harvest_Day) != Harvest_Day_Sw::In_Year_Table))&&HarvestToBeDone[i_plant]) {		// Harvest day, LK 020827 Updated bugs and added harvest from old
		    ResetHarvestToBeDoneDay[i_plant]=min(int(m_pModelInfo->JDayNum+1),365);
            HarvestDoneThisYear[i_plant]=true;
			if(Grazing_Sw(Grazing)==Grazing_Sw::off) 		    
				At_Harvest(false);
		    else if(HarvestToBeDone[i_plant]&& Grazing_Sw(Grazing) == Grazing_Sw::Continous)  // When Grazing equal 2 14 May 2007   		
    			At_Harvest(true);
    		   
	////// For Grazing = 2 above		
		
		}    
		else if(SimPlantAge[i_plant]>=Max_Plant_LifeTime[i_plant]*365.-150.&&m_pModelInfo->JDayNum==1&&pAtm->Latitude>0.){
		        if(TerminatingHarvest==1) {
		            if(i_plant==1) {
                        OK=TerminatingHarvestFunction(i_plant);
                        HarvestToBeDone[i_plant]=false	;
						SimPlantAge.front()=0;
                    }
				}
		        else {
				    OK=KillAboveGroundPlant(i_plant);
				    OK=KillBelowGroundPlant(i_plant);
				    HarvestToBeDone[i_plant]=true;
				}

				GrowingSum[i_plant]=0.;
		}
		else if(SimPlantAge[i_plant]>=Max_Plant_LifeTime[i_plant]*365.-150.&&m_pModelInfo->JDayNum==180&&pAtm->Latitude<=0.){
		        if(TerminatingHarvest==1) ;
		        
		        else {
				    OK=KillAboveGroundPlant(i_plant);
				    OK=KillBelowGroundPlant(i_plant);
				    HarvestToBeDone[i_plant]=true;
				}		

				HarvestToBeDone[i_plant]=true;
				GrowingSum[i_plant]=0.;
		}
		else if(DoCleaning&&CleaningToBeDone&&IndexClean==i_plant) 
			At_Cleaning();
		else {
			for (size_t j = _LEAF; j <= _OLD_COARSE_ROOT; j++)
				for (size_t jj = 0; jj<m_pModelStructure->m_NumActPlantElements; jj++)
					P_Harvest[j][jj][i_plant] = 0;
	
		}
		
		if (m_pModelInfo->JDayNum == int(pNCSoil->PloughingDay)) {
			OK = KillAboveGroundPlant(i_plant);
			OK = KillBelowGroundPlant(i_plant);
		}
		
		if(HarvestDayNo[i_plant]==m_pModelInfo->JDayNum+1&&HarvestToBeDone[i_plant]==false) 
			HarvestToBeDone[i_plant]=true;
		else if(ResetHarvestToBeDoneDay[i_plant]==m_pModelInfo->JDayNum&&HarvestToBeDone[i_plant]==false) {
		    HarvestToBeDone[i_plant]=true	;
		}
		if(ResetCleaningDay==m_pModelInfo->JDayNum) 
		    CleaningToBeDone=true;
		
		

	}
	

}
void NC_Plant::WoodEatingProcess() {
	SoilOrgModel* p_Soil = (SoilOrgModel*)m_pModelInfo->GetModelPtr("Soil Org Model");
	NC_Organic* pNCSoil = &p_Soil->m_NC_SoilOrg;
	C_WoodEatingRate = N_WoodEatingRate= 0;
	
	for (size_t i = 0; i < SimPlantYearAge.size(); i++) {
		if (SimPlantYearAge[i] > WoodEatingStartAge) {
			double timediff = SimPlantYearAge[i] > WoodEatingStartAge;
			COldStemEating[i] = WoodEatingRateCoefFunction(timediff)*P_State[_OLD_STEM][_C_][i] * f_TempResponseRespiration(NDrivTAir);
			NOldStemEating[i] = WoodEatingRateCoefFunction(timediff)*P_State[_OLD_STEM][_N_][i] * f_TempResponseRespiration(NDrivTAir);
			POldStemEating[i] = WoodEatingRateCoefFunction(timediff)*P_State[_OLD_STEM][_P_][i] * f_TempResponseRespiration(NDrivTAir);
			C_WoodEatingRate += COldStemEating[i];
			N_WoodEatingRate += NOldStemEating[i];
			P_WoodEatingRate += POldStemEating[i];
			COldCoarseRootEating[i] = WoodEatingRateCoefFunction(timediff)*P_State[_OLD_COARSE_ROOT][_C_][i] * f_TempResponseRespiration(NDrivTAir);
			NOldCoarseRootEating[i] = WoodEatingRateCoefFunction(timediff)*P_State[_OLD_COARSE_ROOT][_N_][i] * f_TempResponseRespiration(NDrivTAir);
			POldCoarseRootEating[i] = WoodEatingRateCoefFunction(timediff)*P_State[_OLD_COARSE_ROOT][_P_][i] * f_TempResponseRespiration(NDrivTAir);
			C_WoodEatingRate += COldCoarseRootEating[i];
			N_WoodEatingRate += NOldCoarseRootEating[i];
			P_WoodEatingRate += POldCoarseRootEating[i];
		}
	}

	WoodEatingRespiration = (1 - WoodEaterEfficiency)*C_WoodEatingRate;
	C_WoodEaterGrowthFlux = WoodEaterEfficiency*C_WoodEatingRate;

	WoodEaterRespFlux = C_WoodEater*WoodEaterRespRateCoef;
	WoodEaterAmmoniumFlux= N_WoodEater*WoodEaterAmmoniumRateCoef;
	C_WoodEaterFaecesFlux = C_WoodEater*WoodEaterFaecesRateCoef;
	N_WoodEaterFaecesFlux = N_WoodEater*WoodEaterFaecesRateCoef;
	P_WoodEaterFaecesFlux = P_WoodEater * WoodEaterFaecesRateCoef;

}
void NC_Plant::Grazing_Events() {

	if (Grazing_Sw(Grazing) == Grazing_Sw::Continous) {  // New option 14 May 2007 - Related to Harvest
		double  PotentialGrazing = 0.;
		for (size_t i = 0; i < P_Harvest[_STEM][_C_].size(); i++)
			PotentialGrazing += P_Harvest[_STEM][_C_][i] + P_Harvest[_LEAF][_C_][i] + P_Harvest[_OLD_LEAF][_C_][i];
		for (size_t i = 0; i < P_Harvest[_STEM][_C_].size(); i++) {
			if (GrazingOfftake < PotentialGrazing) {
				P_Harvest[_STEM][_C_][i] = P_Harvest[_STEM][_C_][i] / PotentialGrazing * GrazingOfftake;
				P_Harvest[_STEM][_N_][i] = P_Harvest[_STEM][_C_][i] / P_State_CarbonRatio[_STEM_][_N_-1][i];
				P_Harvest[_STEM][_P_][i] = P_Harvest[_STEM][_C_][i] / P_State_CarbonRatio[_STEM_][_P_ - 1][i];
				P_Harvest[_LEAF][_C_][i] = P_Harvest[_LEAF][_C_][i] / PotentialGrazing * GrazingOfftake;
				P_Harvest[_LEAF][_N_][i] = P_Harvest[_LEAF][_C_][i] / P_State_CarbonRatio[_LEAF_][_N_-1][i];
				P_Harvest[_LEAF][_P_][i] = P_Harvest[_LEAF][_C_][i] / P_State_CarbonRatio[_LEAF_][_P_ - 1][i];
				P_Harvest[_OLD_LEAF][_C_][i] = P_Harvest[_OLD_LEAF][_C_][i] / PotentialGrazing * GrazingOfftake;
				P_Harvest[_OLD_LEAF][_N_][i] = P_Harvest[_OLD_LEAF][_C_][i] / P_State_CarbonRatio[_LEAF_][_N_-1][i];
				P_Harvest[_OLD_LEAF][_P_][i] = P_Harvest[_OLD_LEAF][_C_][i] / P_State_CarbonRatio[_LEAF_][_P_ - 1][i];
			}
		}
	}
	else {   // Options related to Potential grazing specified in external forcing file
		double pot_grazingrate;
		if (Grazing == 2)
			pot_grazingrate = pow(10., GrazingScalingCoef)*GetEcoDat(&m_PGPos_GrazingRate, GrazingRate_Pos);
		else
			pot_grazingrate = Pot_C_GrazingRate;
		GrazingRespiration = 0.;

		double TotalCSource = 0;
		for (size_t i = 0; i < P_State[_LEAF][_C_].size(); i++) {
			TotalCSource += max(0., (P_State[_LEAF][_C_][i] + P_State[_OLD_LEAF][_C_][i]) - StubbleLeaf[i] + P_State[_STEM][_C_][i] - StubbleStem[i]);
		}



		double TotalCSourceRate = max(0., TotalCSource / T_Step);


		C_GrazingRate = min(pot_grazingrate, TotalCSourceRate*Max_AvailGrazingFraction);

		double relgrazing = 0;
		if (pot_grazingrate > 0) relgrazing = C_GrazingRate / pot_grazingrate;


		for (size_t i = 0; i < P_State[_LEAF][_C_].size(); i++) {
			CLeafGrazing[i] = relgrazing * P_State[_LEAF][_C_][i] / TotalCSource * pot_grazingrate;
			COldLeafGrazing[i] = relgrazing * P_State[_OLD_LEAF][_C_][i] / TotalCSource * pot_grazingrate;
			CStemGrazing[i] = relgrazing * P_State[_STEM][_C_][i] / TotalCSource * pot_grazingrate;
			if (P_State[_LEAF][_C_][i] > 0) {
				NLeafGrazing[i] = CLeafGrazing[i] * P_State[_LEAF][_N_][i] / P_State[_LEAF][_C_][i];
				PLeafGrazing[i] = CLeafGrazing[i] * P_State[_LEAF][_P_][i] / P_State[_LEAF][_C_][i];
			}
			else {
				NLeafGrazing[i] = 0.;
				PLeafGrazing[i] = 0.;
			}
			if (P_State[_OLD_LEAF][_C_][i] > 0.0001) {
				NOldLeafGrazing[i] = COldLeafGrazing[i] * P_State[_OLD_LEAF][_N_][i] / P_State[_OLD_LEAF][_C_][i];
				POldLeafGrazing[i] = COldLeafGrazing[i] * P_State[_OLD_LEAF][_P_][i] / P_State[_OLD_LEAF][_C_][i];
			}
			else {
				NOldLeafGrazing[i] = 0.;
				POldLeafGrazing[i] = 0.;
			}
			if (P_State[_STEM][_C_][i] > 0.) {
				NStemGrazing[i] = CStemGrazing[i] * P_State[_STEM][_N_][i] / P_State[_STEM][_C_][i];
				PStemGrazing[i] = CStemGrazing[i] * P_State[_STEM][_P_][i] / P_State[_STEM][_C_][i];
			}
			else {
				NStemGrazing[i] = 0.;
				PStemGrazing[i] = 0.;
			}
		}
		N_GrazingRate = 0;
		for (size_t i = 0; i < NLeafGrazing.size(); i++) {
			N_GrazingRate += NLeafGrazing[i] + NOldLeafGrazing[i] + NStemGrazing[i];
			P_GrazingRate += PLeafGrazing[i] + POldLeafGrazing[i] + PStemGrazing[i];
		}
		GrazingRespiration = GrazingRespFraction * C_GrazingRate;
		C_GrazingExport = C_GrazingExportFraction * C_GrazingRate;
		C_GrazingMethaneEmission = C_GrazingCH4Fraction * C_GrazingRate;
		N_GrazingExport = N_GrazingExportFraction * N_GrazingRate;
		P_GrazingExport = P_GrazingExportFraction * P_GrazingRate;


		if (GrazingClosure == 0) {
			C_GrazingManure = C_GrazingManureFraction * C_GrazingRate;
			N_GrazingManure = N_GrazingManureFraction * N_GrazingRate;
			P_GrazingManure = P_GrazingManureFraction * P_GrazingRate;
			C_GrazingGrowth = C_GrazingRate - GrazingRespiration - C_GrazingExport - C_GrazingManure - C_GrazingMethaneEmission;
			N_GrazingGrowth = N_GrazingRate - N_GrazingExport - N_GrazingManure;
			P_GrazingGrowth = P_GrazingRate - P_GrazingExport - P_GrazingManure;
		}
		else {
			C_GrazingGrowth = C_GrazingGrowthFraction * C_GrazingRate;
			N_GrazingGrowth = N_GrazingGrowthFraction * N_GrazingRate;
			P_GrazingGrowth = P_GrazingGrowthFraction * P_GrazingRate;
			C_GrazingManure = C_GrazingRate - GrazingRespiration - C_GrazingExport - C_GrazingGrowth - C_GrazingMethaneEmission;
			N_GrazingManure = N_GrazingRate - N_GrazingExport - N_GrazingGrowth;
			P_GrazingManure = P_GrazingRate - P_GrazingExport - P_GrazingGrowth;
		}
		N_GrazingFaeces = min(C_GrazingManureFraction / CN_GrazingOrganic * N_GrazingRate, N_GrazingManure);
		P_GrazingFaeces = min(C_GrazingManureFraction / CP_GrazingOrganic * P_GrazingRate, P_GrazingManure);
		double ResidualMinN = max(0., N_GrazingManure - N_GrazingFaeces);
		N_GrazingAmmonia = ResidualMinN * N_GrazingManureAmmoniaFraction;
		N_GrazingAmmonium = ResidualMinN * (1. - N_GrazingManureAmmoniaFraction);
	}


}
void NC_Plant::Seedling_Management_Events() {

	auto AddPlantSeedling = [&](size_t iplant) {


		if (Seedling_CLeaf[iplant] > 0 || Seedling_CStem[iplant] > 0 || Seedling_CRoot[iplant]>0) {
			P_State[_LEAF][_C_][iplant] += Seedling_CLeaf[iplant];
			P_State[_LEAF][_N_][iplant] += Seedling_CLeaf[iplant] / CN_RatioMinLeaf[iplant];
			P_State[_STEM][_C_][iplant] += Seedling_CStem[iplant];
			P_State[_STEM][_N_][iplant] += Seedling_CStem[iplant] / CN_RatioMinStem[iplant];
			P_State[_ROOT][_C_][iplant] += Seedling_CRoot[iplant];
			P_State[_ROOT][_N_][iplant] += Seedling_CRoot[iplant] / CN_RatioMinRoots[iplant];

			SimPlantAge[iplant] = Seedling_Age[iplant] * 365.;
			SimPlantYearAge[iplant] = Seedling_Age[iplant];
		}

	};


	for (size_t iplant = 0; iplant < Seedling_DayNo.size(); iplant++) {
		if (m_pModelInfo->JDayNum == int(Seedling_DayNo[iplant])) {


			if (Seedling_Management_Sw(Seedling_Management) == Seedling_Management_Sw::After_Final_Harvest) {
				if (m_pModelInfo->JYearNum == int(Estimated_Seedling_Year[iplant])) {
					AddPlantSeedling(iplant);
				}
			}
			else if (Seedling_Management_Sw(Seedling_Management) == Seedling_Management_Sw::Specific_Years) {
				if (m_pModelInfo->JYearNum == int(Seedling_Year[iplant])) {
					AddPlantSeedling(iplant);
				}



			}
		}
	}
}
void NC_Plant::At_Harvest(bool WithGrazingEvent) {
	bool CompleteGrainHarvest = false;
	auto All_HarvestComp = [&](size_t i, bool PriorFinal) {
		double HarvestFrac;
		for (size_t jj = 0; jj<m_pModelStructure->m_NumActPlantElements; jj++) {
			HarvestFrac = HarvFracGrain[i];	 if (PriorFinal) HarvestFrac = P_HarvFracGrain[i];
			if (HarvestFrac > 0.98) CompleteGrainHarvest = true;
			P_Harvest[_GRAIN][jj][i] = HarvestFrac * P_State[_GRAIN][jj][i] * 1. / T_Step;

			HarvestFrac = HarvFracStem[i]; 	if (PriorFinal) HarvestFrac = P_HarvFracStem[i];
			P_Harvest[_STEM][jj][i] = HarvestFrac * P_State[_STEM][jj][i] * 1. / T_Step;
			P_Harvest[_OLD_STEM][jj][i] = HarvestFrac * P_State[_OLD_STEM][jj][i] * 1. / T_Step;

			HarvestFrac = HarvFracRoots[i];	if (PriorFinal)  HarvestFrac = P_HarvFracRoots[i];
			P_Harvest[_ROOT][jj][i] = HarvestFrac * P_State[_ROOT][jj][i] * 1. / T_Step;
			P_Harvest[_OLD_ROOT][jj][i] = HarvestFrac * P_State[_OLD_ROOT][jj][i] * 1. / T_Step;

			HarvestFrac = HarvFracCoarseRoots[i];	if (PriorFinal) HarvestFrac = P_HarvFracCoarseRoots[i];
			P_Harvest[_COARSE_ROOT][jj][i] = HarvestFrac * P_State[_COARSE_ROOT][jj][i] * 1. / T_Step;
			P_Harvest[_OLD_COARSE_ROOT][jj][i] = HarvestFrac* P_State[_OLD_COARSE_ROOT][jj][i] * 1. / T_Step;

			HarvestFrac = HarvFracLeaf[i];	if (PriorFinal) HarvestFrac = P_HarvFracLeaf[i];
			P_Harvest[_LEAF][jj][i] = HarvestFrac * P_State[_LEAF][jj][i] * 1. / T_Step;
			P_Harvest[_OLD_LEAF][jj][i] = HarvestFrac * P_State[_OLD_LEAF][jj][i] * 1. / T_Step;
		}
		if (!PriorFinal&&Seedling_Management_Sw(Seedling_Management)==Seedling_Management_Sw::After_Final_Harvest) {
			Estimated_Seedling_Year[i] = m_pModelInfo->JYearNum + int(Seedling_YearAfterFinalHarvest[i]);
		}
	};
	auto All_LitterFallComp=[&](size_t i, bool PriorFinal) {
		double HarvestFrac, LitterFrac;
		for (size_t jj = 0; jj<m_pModelStructure->m_NumActPlantElements; jj++) {
			HarvestFrac = HarvFracRoots[i];	LitterFrac = LitterFracRoots[i];
			if (PriorFinal) {	HarvestFrac = P_HarvFracRoots[i];	LitterFrac = P_LitterFracRoots[i];	}
			P_LitterFall[_ROOT][jj][i] = max(0.,  LitterFrac* (P_State[_ROOT][jj][i] - (P_State[_ROOT][jj][i] *HarvestFrac ))*1. / T_Step);
			P_LitterFall[_OLD_ROOT][jj][i] = max(0., LitterFrac* (P_State[_OLD_ROOT][jj][i] - (P_State[_OLD_ROOT][jj][i] * HarvestFrac))*1. / T_Step);

			HarvestFrac = HarvFracCoarseRoots[i];
			LitterFrac = LitterFracCoarseRoots[i];
			if (PriorFinal) {
				HarvestFrac = P_HarvFracCoarseRoots[i];
				LitterFrac = P_LitterFracCoarseRoots[i];
			}
			P_LitterFall[_COARSE_ROOT][jj][i] = max(0., LitterFrac * (P_State[_COARSE_ROOT][jj][i] - (P_State[_COARSE_ROOT][jj][i] * HarvestFrac))*1. / T_Step);
			P_LitterFall[_OLD_COARSE_ROOT][jj][i] = max(0., LitterFrac * (P_State[_OLD_COARSE_ROOT][jj][i] - (P_State[_OLD_COARSE_ROOT][jj][i] * HarvestFrac))*1. / T_Step);

			HarvestFrac = HarvFracStem[i];
			LitterFrac = LitterFracStem[i];
			if (PriorFinal) {
				HarvestFrac = P_HarvFracStem[i];
				LitterFrac = P_LitterFracStem[i];
			}
			P_LitterFall[_STEM][jj][i] = LitterFrac * (P_State[_STEM][jj][i] - (P_State[_STEM][jj][i] * HarvestFrac))*1. / T_Step;
			P_LitterFall[_OLD_STEM][jj][i] = LitterFrac* (P_State[_OLD_STEM][jj][i] - (P_State[_OLD_STEM][jj][i] * HarvestFrac))*1. / T_Step;
			HarvestFrac = HarvFracLeaf[i];
			LitterFrac = LitterFracLeaf[i];
			if (PriorFinal) {
				HarvestFrac = P_HarvFracLeaf[i];
				LitterFrac = P_LitterFracLeaf[i];
			}
			P_LitterFall[_LEAF][jj][i] = LitterFrac * (P_State[_LEAF][jj][i] - (P_State[_LEAF][jj][i] * HarvestFrac))*1. / T_Step;
			P_LitterFall[_OLD_LEAF][jj][i] = LitterFrac * (P_State[_OLD_LEAF][jj][i] - (P_State[_OLD_LEAF][jj][i] * HarvestFrac))*1. / T_Step;
	
			LitterFrac = LitterFracGrain[i];
			HarvestFrac = HarvFracGrain[i];
			if (PriorFinal) {
				LitterFrac = P_LitterFracLeaf[i];
				HarvestFrac = P_HarvFracGrain[i];
			}
			P_LitterFall[_GRAIN][jj][i] = LitterFrac* (P_State[_GRAIN][jj][i] - (P_State[_GRAIN][jj][i]*HarvestFrac))*1. / T_Step;
			P_LitterFall[_NEWMOBILE][jj][i] = P_State[_NEWMOBILE][jj][i] * 1. / T_Step;
			if (SimPlantAge[i] >= Max_Plant_LifeTime[i] * 365. - 365 || HarvFracStem[i] == 1 || LitterFracStem[i] == 1.)
				P_LitterFall[_MOBILE][jj][i] = P_State[_MOBILE][jj][i] * 1. / T_Step;
		}
	};
	auto Reset_ConditionsatHarvest = [&](size_t i) {
		for (size_t j = 0; j <= _OLD_STEM; j++)
			P_Respiration[j][i] = 0.;

		for (size_t jj = 0; jj<m_pModelStructure->m_NumActPlantElements; jj++) {
			for (size_t j = 0;j<= ROOT_GRAIN; j++) 
				P_ReAllocation[j][jj][i] = 0.;

			P_ReAllocation[NEWMOBILE_FUNGI][jj][i] = 0.;
		}

		GrowthStageIndex[i] = Min_GSI[i];
		//SimPlantAge[i] = 0.;

	};
	auto GrazingExportAsHarvest = [&](size_t i) { 
		P_Harvest[_STEM][_C_][i] = max(0., (P_State[_STEM][_C_][i] - StubbleStem[i] - P_LitterFall[_STEM][_C_][i]))  *1. / T_Step;
		double Ratio_C_O = P_State[_LEAF][_C_][i] / (P_State[_LEAF][_C_][i] + P_State[_OLD_LEAF][_C_][i]);
		P_Harvest[_LEAF][_C_][i] = max(0., (P_State[_LEAF][_C_][i] + P_State[_OLD_LEAF][_C_][i] - StubbleLeaf[i] - P_LitterFall[_LEAF][_C_][i]))*Ratio_C_O *1. / T_Step;
		P_Harvest[_OLD_LEAF][_C_][i] = max(0., (P_State[_LEAF][_C_][i] + P_State[_OLD_LEAF][_C_][i] - StubbleLeaf[i] - P_LitterFall[_OLD_LEAF][_C_][i]))*(1 - Ratio_C_O) *1. / T_Step;

		P_Harvest[_LEAF][_N_][i] = P_Harvest[_LEAF][_C_][i] / P_State_CarbonRatio[_LEAF_][_N_-1][i];
		P_Harvest[_STEM][_N_][i] = P_Harvest[_STEM][_C_][i] / P_State_CarbonRatio[_STEM_][_N_-1][i];
		P_Harvest[_OLD_LEAF][_N_][i] = P_Harvest[_OLD_LEAF][_C_][i] / P_State_CarbonRatio[_LEAF_][_N_-1][i];

		P_Harvest[_LEAF][_P_][i] = P_Harvest[_LEAF][_P_][i] / P_State_CarbonRatio[_LEAF_][_P_ - 1][i];
		P_Harvest[_STEM][_P_][i] = P_Harvest[_STEM][_C_][i] / P_State_CarbonRatio[_STEM_][_P_ - 1][i];
		P_Harvest[_OLD_LEAF][_P_][i] = P_Harvest[_OLD_LEAF][_C_][i] / P_State_CarbonRatio[_LEAF_][_P_ - 1][i];
	};

	for(size_t i=0; i<P_Harvest[_LEAF][_C_].size();i++) {
	            if(DoFinalHarvest) {
					All_HarvestComp(i, false);
					All_LitterFallComp(i, false);
				}
                else {
					All_HarvestComp(i, true);
					All_LitterFallComp(i, true);
    		     }

				if(WithGrazingEvent)  GrazingExportAsHarvest(i);

				if (DoFinalHarvest) {
					if (GrowthStageIndex[i] == 4) Reset_ConditionsatHarvest(i);
					HarvestToBeDone[i] = false;
					if (ResetGSI_AtHarvest_Sw(ResetGSI_AtHarvest) == ResetGSI_AtHarvest_Sw::on)
						GrowthStageIndex[i] = GSI_PostHarvest[i];
					else if (ResetGSI_AtHarvest_Sw(ResetGSI_AtHarvest) == ResetGSI_AtHarvest_Sw::on_at_completegrainharvest && CompleteGrainHarvest)
						GrowthStageIndex[i] = GSI_PostHarvest[i];
				}
      }
}


void NC_Plant::At_Cleaning() {
	for (size_t i = 0; i < P_Harvest[_GRAIN][_C_].size(); i++) {
		for (size_t j = 0; j < m_pModelStructure->m_NumActPlantElements; j++) {
			for (size_t jj = 0; jj < NUM_PLANT_COMP; jj++) {
				P_Harvest[jj][j][i] = 0.;
				CleaningToBeDone = false;
				P_LitterFall[jj][j][IndexClean] = CleaningDegree[0] * P_State[jj][j][IndexClean] * 1. / T_Step;
			}
		}
	}

}
void NC_Plant::MobilePool() {
	 for(size_t i=0; i< P_ReAllocation[LEAF_MOBILE][_N_].size();i++) {

		if(SimPlantAge[i]<Max_Plant_LifeTime[i]*365.&&Winter_Regulation>0) {		//LK 020524
			 P_ReAllocation[LEAF_MOBILE_RETURN][_C_][i]=P_LitterFall[_LEAF][_C_][i]*MobileAlloCoef[i];
			 P_ReAllocation[LEAF_MOBILE_RETURN][_N_][i] = P_ReAllocation[LEAF_MOBILE_RETURN][_C_][i] / CN_RatioMinLeaf[i];
			 if (P_State[_LEAF][_N_][i] > 0) {
				 auto ratio_state = P_State[_LEAF][_C_][i] / P_State[_LEAF][_N_][i];
				 if (ratio_state > CN_RatioMaxLeaf[i]) {
					 P_ReAllocation[LEAF_MOBILE_RETURN][_C_][i] = 0.;
					 P_ReAllocation[LEAF_MOBILE_RETURN][_N_][i] = 0.;
				 }
			 }
			 else {
				 P_ReAllocation[LEAF_MOBILE_RETURN][_C_][i] = 0.;
				 P_ReAllocation[LEAF_MOBILE_RETURN][_N_][i] = 0.;
			 }
		 
			 if (m_pModelStructure->m_NumActPlantElements >= 3) {			
				 P_ReAllocation[LEAF_MOBILE_RETURN][_P_][i] = P_ReAllocation[LEAF_MOBILE_RETURN][_C_][i] / CP_RatioMinLeaf[i];
				 if (P_State[_LEAF][_P_][i] > 0) {
					 auto ratio_state = P_State[_LEAF][_C_][i] / P_State[_LEAF][_P_][i];
					 if (ratio_state > CP_RatioMaxLeaf[i]) {
						 P_ReAllocation[LEAF_MOBILE_RETURN][_P_][i] = 0.;
					 }
				 }
				 else
					 P_ReAllocation[LEAF_MOBILE_RETURN][_P_][i] = 0.;
			 }

			 P_ReAllocation[OLD_LEAF_MOBILE_RETURN][_C_][i] = P_LitterFall[_OLD_LEAF][_C_][i] * MobileAlloCoef[i];
			 P_ReAllocation[OLD_LEAF_MOBILE_RETURN][_N_][i] = P_ReAllocation[OLD_LEAF_MOBILE_RETURN][_C_][i] / CN_RatioMinLeaf[i];

			 if (P_State[_OLD_LEAF][_N_][i] > 0.) {
				 auto ratio_state = P_State[_OLD_LEAF][_C_][i] / P_State[_OLD_LEAF][_N_][i];
				 if (ratio_state > CN_RatioMaxLeaf[i]) {
					 P_ReAllocation[OLD_LEAF_MOBILE_RETURN][_C_][i] = 0.;
					 P_ReAllocation[OLD_LEAF_MOBILE_RETURN][_N_][i] = 0.;
				 }
			 }
			 else {
				 P_ReAllocation[OLD_LEAF_MOBILE_RETURN][_C_][i] = 0.;
				 P_ReAllocation[OLD_LEAF_MOBILE_RETURN][_N_][i] = 0.;

			 }
			 if (m_pModelStructure->m_NumActPlantElements >= 3) {
				 P_ReAllocation[OLD_LEAF_MOBILE_RETURN][_P_][i] = P_ReAllocation[OLD_LEAF_MOBILE_RETURN][_C_][i] / CP_RatioMinLeaf[i];
				 if (P_State[_OLD_LEAF][_P_][i] > 0) {
					 auto ratio_state = P_State[_OLD_LEAF][_C_][i] / P_State[_OLD_LEAF][_P_][i];
					 if (ratio_state > CP_RatioMaxLeaf[i]) {
						 P_ReAllocation[OLD_LEAF_MOBILE_RETURN][_P_][i] = 0.;
					 }
				 }
				 else
					 P_ReAllocation[OLD_LEAF_MOBILE_RETURN][_P_][i] = 0.;
			 }
		}

		if(N_ReAllocation>0) {
			double cnratio, koll;
			if (P_State[_LEAF][_N_][i] > 0.) {
				cnratio = P_State[_LEAF][_C_][i] / P_State[_LEAF][_N_][i];
				if (cnratio < CN_RatioMinLeaf[i]) {
					koll = max(0., P_State[_LEAF][_N_][i] - P_State[_LEAF][_C_][i] / CN_RatioMinLeaf[i]);
				}
				else
					koll = max(0., P_State[_LEAF][_N_][i] - P_State[_LEAF][_C_][i] / CN_RatioMinLeaf[i]);
			}
				P_ReAllocation[OLD_LEAF_MOBILE][_N_][i] = max(0., P_State[_OLD_LEAF][_N_][i] - P_State[_OLD_LEAF][_C_][i] / CN_RatioMinLeaf[i]);
				P_ReAllocation[OLD_STEM_MOBILE][_N_][i] = max(0., P_State[_OLD_STEM][_N_][i] - P_State[_OLD_STEM][_C_][i] / CN_RatioMinStem[i]);
				P_ReAllocation[OLD_ROOT_MOBILE][_N_][i] = max(0., P_State[_OLD_ROOT][_N_][i] - P_State[_OLD_ROOT][_C_][i] / CN_RatioMinRoots[i]);
				P_ReAllocation[OLD_COARSE_ROOT_MOBILE][_N_][i] = max(0., P_State[_OLD_COARSE_ROOT][_N_][i] - P_State[_OLD_COARSE_ROOT][_C_][i] / CN_RatioMinCoarseRoots[i]);
				P_ReAllocation[LEAF_MOBILE][_N_][i] = max(0., P_State[_LEAF][_N_][i] - P_State[_LEAF][_C_][i] / CN_RatioMinLeaf[i]);
				P_ReAllocation[STEM_MOBILE][_N_][i] = max(0., P_State[_STEM][_N_][i] - P_State[_STEM][_C_][i] / CN_RatioMinStem[i]);
				P_ReAllocation[ROOT_MOBILE][_N_][i] = max(0., P_State[_ROOT][_N_][i] - P_State[_ROOT][_C_][i] / CN_RatioMinRoots[i]);
				P_ReAllocation[COARSE_ROOT_MOBILE][_N_][i] = max(0., P_State[_COARSE_ROOT][_N_][i] - P_State[_COARSE_ROOT][_C_][i] / CN_RatioMinCoarseRoots[i]);

        }

		if (Phos_ReAllocation >= 0) {
			P_ReAllocation[OLD_LEAF_MOBILE][_P_][i] = max(0., P_State[_OLD_LEAF][_P_][i] - P_State[_OLD_LEAF][_C_][i] / CP_RatioMinLeaf[i]);
			P_ReAllocation[OLD_STEM_MOBILE][_P_][i] = max(0., P_State[_OLD_STEM][_P_][i] - P_State[_OLD_STEM][_C_][i] / CP_RatioMinStem[i]);
			P_ReAllocation[OLD_ROOT_MOBILE][_P_][i] = max(0., P_State[_OLD_ROOT][_P_][i] - P_State[_OLD_ROOT][_C_][i] / CP_RatioMinRoots[i]);
			P_ReAllocation[OLD_COARSE_ROOT_MOBILE][_P_][i] = max(0., P_State[_OLD_COARSE_ROOT][_P_][i] - P_State[_OLD_COARSE_ROOT][_C_][i] / CP_RatioMinCoarseRoots[i]);
			P_ReAllocation[LEAF_MOBILE][_P_][i] = max(0., P_State[_LEAF][_P_][i] - P_State[_LEAF][_C_][i] / CP_RatioMinLeaf[i]);
			P_ReAllocation[STEM_MOBILE][_P_][i] = max(0., P_State[_STEM][_P_][i] - P_State[_STEM][_C_][i] / CP_RatioMinStem[i]);
			P_ReAllocation[ROOT_MOBILE][_P_][i] = max(0., P_State[_ROOT][_P_][i] - P_State[_ROOT][_C_][i] / CP_RatioMinRoots[i]);
			P_ReAllocation[COARSE_ROOT_MOBILE][_P_][i] = max(0., P_State[_COARSE_ROOT][_P_][i] - P_State[_COARSE_ROOT][_C_][i] / CP_RatioMinCoarseRoots[i]);
		}

	 }
}

void NC_Plant::MineralOrganic_Uptake() {
	SoilOrgModel* p_Soil = (SoilOrgModel*)m_pModelInfo->GetModelPtr("Soil Org Model");
	NC_Organic* pNCSoil = &p_Soil->m_NC_SoilOrg;
	Soil_Frost* pcSoil = (Soil_Frost*)pSoil;

	if (pNCSoil->OrganicUptake > 0) pNCSoil->UptakeOrgReset();

	pNCSoil->NitrogenUptakeResetForAllSoilLayers();

	auto EstimateDemandfromPlant = [&]() {
		for (size_t jj = 1; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
			DemandUptake[jj - 1].Demand = f_PlantDemand[jj - 1](P_CarbonAlloNewMobile.Roots, P_CarbonAlloNewMobile.CoarseRoots, P_CarbonAlloNewMobile.Stem, P_CarbonAlloNewMobile.Leaf,
				P_Respiration[_ROOT], P_Respiration[_COARSE_ROOT], P_Respiration[_STEM], P_Respiration[_LEAF], DemandUptake[jj - 1].Demand);
			DemandUptake[jj - 1].TotalDemand = sum(DemandUptake[jj - 1].Demand);


			if (N_ReAllocation_Sw(N_ReAllocation) > N_ReAllocation_Sw::off) {
				for (size_t i = 0; i < DemandUptake[jj - 1].Demand.size(); i++) {
					//P_ReAllocation[MOBILE_NEWMOBILE][jj][i] = 0.;
					//P_ReAllocation[MOBILE_NEWMOBILE][jj][i] = max(0., min(P_State[_MOBILE][jj][i]/T_Step, DemandUptake[jj-1].Demand[i]));//HH
					//DemandUptake[jj - 1].DemandSoil[i] = max(0., DemandUptake[jj - 1].Demand[i] - P_ReAllocation[MOBILE_NEWMOBILE][jj][i]);
					DemandUptake[jj - 1].DemandSoil[i] = max(0., DemandUptake[jj - 1].Demand[i]- P_ReAllocation[MOBILE_LEAF][jj][i]);
				}
			}
			else
				DemandUptake[jj - 1].DemandSoil = DemandUptake[jj - 1].Demand;

			DemandUptake[jj - 1].TotalDemandSoil = sum(DemandUptake[jj - 1].DemandSoil);
		}
		if (FungalGrowth > 0) {
			NFungiDemand = f_FungiDemand(P_State[_FUNGI][_C_], P_State[_FUNGI][_N_], NFungiDemand);
			FungiNorgUptpot = f_FungiPotentialUptake(0, P_State[_FUNGI][_C_], FungiNorgUptpot);// [i] / Fix::BiomassToCarbon; //potential Nitrate uptake by fungi
			FungiNitrateUptpot = f_FungiPotentialUptake(1, P_State[_FUNGI][_C_], FungiNitrateUptpot); //Fungi_NO3rate[i] * Fungi_MycFrac[i] * P_State[_FUNGI][_C_][i] / Fix::BiomassToCarbon;
			FungiAmmoniumUptpot = f_FungiPotentialUptake(2, P_State[_FUNGI][_C_], FungiAmmoniumUptpot);//
			FungiRootCover = f_FungiCover(P_State[_FUNGI][_C_], P_State[_ROOT][_C_], P_State[_OLD_ROOT][_C_], FungiRootCover);
			NFungiAvail = f_FungiNAvail(P_State[_FUNGI][_N_], P_State[_FUNGI][_C_], NFungiAvail);
			DemandFungiUptake[0].TotalDemand = sum(NFungiDemand);//HH
			if (m_pModelStructure->m_NumActPlantElements >= 3){//HH
				PFungiDemand = f_FungiPDemand(P_State[_FUNGI][_C_], P_State[_FUNGI][_P_], PFungiDemand);
				FungiPorgUptpot = f_FungiPotentialPUptake(0, P_State[_FUNGI][_C_], FungiPorgUptpot);// [i] / Fix::BiomassToCarbon; //potential ORGANIC P uptake by fungi
				FungiPhosphateUptpot = f_FungiPotentialPUptake(1, P_State[_FUNGI][_C_], FungiPhosphateUptpot); //
				PFungiAvail = f_FungiPAvail(P_State[_FUNGI][_P_], P_State[_FUNGI][_C_], PFungiAvail);
				DemandFungiUptake[1].TotalDemand = sum(PFungiDemand);//HH
			}
		}



	};



	EstimateDemandfromPlant();

	// Set distributed Uptake of all plants and elements to 0
	for (size_t jj = 1; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
		UptakeDistTotal[jj - 1].assign(pcSoil->NL, 0.);
		UptakeFungiDistTotal[jj - 1].assign(pcSoil->NL, 0.);
	}

	// Calculate RootDepth and NUptake distrubtion of Plant
	pPlant->Num_NUptMax = 0;
	UptakeResults upt;
	PUptakeResults upt_p;

	auto DistributionofRelativeUptakeBasedOnDifferentPlants = [&]() {
		DEPTH_type in;

		for (size_t i = 0; i < pPlant->SimRootDepth.size(); i++) {
			// Roots with distributed flow of N Uptake
			pPlant->SimRootDepth[i] = pPlant->f_RootDepth_Mass(i, P_State[_ROOT][_C_][i] + P_State[_OLD_ROOT][_C_][i]);

			// Calculate Uptake Distribution - now a matrix of size (NL,NumPlants)	
			pPlant->SimRootLength[i] = pPlant->f_RootLength_Mass(i, P_State[_ROOT][_C_][i]+ P_State[_OLD_ROOT][_C_][i]);
			in = pPlant->f_WaterUptakeDistribution(1., pPlant->SimRootDepth[i]);
			UptakeDistWithDepth[i] = in.Dist;
			pPlant->Num_NUptMax = max(in.NumLay, pPlant->Num_NUptMax);
			
			for (size_t jj = 1; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
				if (DemandUptake[jj - 1].TotalDemandSoil > 0.0) {
					for (size_t ii = 0; ii < in.NumLay; ii++)  UptakeDistTotal[jj - 1][ii] += UptakeDistWithDepth[i][ii] * DemandUptake[jj - 1].DemandSoil[i] / DemandUptake[jj - 1].TotalDemandSoil;
				}
				else {
					for (size_t ii = 0; ii < in.NumLay; ii++)  UptakeDistTotal[jj - 1][ii] += UptakeDistWithDepth[i][ii] / pPlant->SimRootDepth.size();
				}
			}
		}
		upt.UptNH4.resize(pPlant->Num_NUptMax);
		upt.UptNO3.resize(pPlant->Num_NUptMax);
		upt_p.UptPhosphate.resize(pPlant->Num_NUptMax);
	};
	DistributionofRelativeUptakeBasedOnDifferentPlants();
	// Mineral N Uptake
	UptDeficitTotal[_N_ - 1] = 0.0;
	UptFungiDeficitTotal[_N_ - 1] = 0.0;
	UptDeficitTotal[_P_ - 1] = 0.0;
	UptFungiDeficitTotal[_P_ - 1] = 0.0;
	pNCSoil->NitrogenUptakeResetForAllSoilLayers();
	pNCSoil->PhosphorusUptakeResetForAllSoilLayers();
	auto UpdateN_MineralUptakeFromSoil = [&](UptakeResults upt) {
		for (size_t i = 0; i < upt.UptNO3.size(); i++) {
			pNCSoil->SoilNitrateUptake[i] += upt.UptNO3[i];
			pNCSoil->SoilAmmoniumUptake[i] += upt.UptNH4[i];
		}};
	auto UpdateP_MineralUptakeFromSoil = [&](PUptakeResults upt_p) {
		for (size_t i = 0; i < upt_p.UptPhosphate.size(); i++) {
			pNCSoil->SoilPhosphateUptake[i] += upt_p.UptPhosphate[i];
		}
	};


	auto FirstStepofMineralUptakeFromSoil = [&](size_t i, size_t jj) {
		double remain;
		if (jj == 1) { // Nitrogen
			//UptakeResults upt;
			if (Root_Density_Influence_Sw(Root_Density_Influence) == Root_Density_Influence_Sw::on_nitrogen_uptake) {
				if (FungalGrowth_Sw(FungalGrowth) == FungalGrowth_Sw::off)
					upt = f_MineralUptakeWithDensity(UptakeDistWithDepth[i],  i, DemandUptake[0].DemandSoil[i], DemandUptake[jj - 1].TotalDemandSoil, UptakeDistTotal[jj - 1], pNCSoil->SoilNitrate, pNCSoil->SoilAmmonium, m_RootDensity, upt);
				else {
					upt = f_MineralUptakeWithDensityImpactWithFungi(UptakeDistWithDepth[i],  i, DemandUptake[0].DemandSoil[i], DemandUptake[jj - 1].TotalDemandSoil, UptakeDistTotal[jj - 1], pNCSoil->SoilNitrate, pNCSoil->SoilAmmonium, m_RootDensity, FungiRootCover, false, upt);
				}
			}
			else {
				if (FungalGrowth_Sw(FungalGrowth) == FungalGrowth_Sw::off&&Root_Density_Influence_Sw(Root_Density_Influence) == Root_Density_Influence_Sw::no) {
					upt = f_MineralUptake(UptakeDistWithDepth[i], i, DemandUptake[0].DemandSoil[i], DemandUptake[jj - 1].TotalDemandSoil, UptakeDistTotal[jj - 1], pNCSoil->SoilNitrate, pNCSoil->SoilAmmonium, upt);
				}
				else {
					upt = f_MineralUptaketoFungi(UptakeDistWithDepth[i],  i, DemandUptake[0].DemandSoil[i], DemandUptake[jj - 1].TotalDemandSoil, UptakeDistTotal[jj - 1], pNCSoil->SoilNitrate, pNCSoil->SoilAmmonium, FungiRootCover,true, upt);
				}
			}
			P_Uptake[PLANT_TOT_N][jj - 1][i] = sum(upt.UptNO3) + sum(upt.UptNH4);
			UpdateN_MineralUptakeFromSoil(upt);
			remain = upt.RemainFrac;
		}
		else {
				// P Uptake below
			if (Root_Density_Influence_Sw(Root_Density_Influence) == Root_Density_Influence_Sw::on_nitrogen_uptake) {
				if (FungalGrowth_Sw(FungalGrowth) == FungalGrowth_Sw::off)
					upt_p = f_PMineralUptakeWithDensity(UptakeDistWithDepth[i],  i, DemandUptake[jj - 1].DemandSoil[i], DemandUptake[jj - 1].TotalDemandSoil, UptakeDistTotal[jj - 1], pNCSoil->Soil_PO4, m_RootDensity, upt_p);
				else {
					upt_p = f_PMineralUptakeWithDensityImpactWithFungi(UptakeDistWithDepth[i],  i, DemandUptake[jj - 1].DemandSoil[i], DemandUptake[jj - 1].TotalDemandSoil, UptakeDistTotal[jj - 1], pNCSoil->Soil_PO4, m_RootDensity, FungiRootCover, false, upt_p);
				}
			}
			else {
				if (FungalGrowth_Sw(FungalGrowth) == FungalGrowth_Sw::off&&Root_Density_Influence_Sw(Root_Density_Influence) == Root_Density_Influence_Sw::no) {
					upt_p = f_PMineralUptake(UptakeDistWithDepth[i],  i, DemandUptake[jj - 1].DemandSoil[i], DemandUptake[jj - 1].TotalDemandSoil, UptakeDistTotal[jj - 1], pNCSoil->Soil_PO4, upt_p);
				}
				else {
					upt_p = f_PMineralUptaketoFungi(UptakeDistWithDepth[i],  i, DemandUptake[jj - 1].DemandSoil[i], DemandUptake[jj - 1].TotalDemandSoil, UptakeDistTotal[jj - 1], pNCSoil->Soil_PO4, FungiRootCover, true, upt_p);
				}
			}
			P_Uptake[PLANT_TOT_N][jj - 1][i] = sum(upt_p.UptPhosphate);
			UpdateP_MineralUptakeFromSoil(upt_p);
			remain = upt_p.RemainFrac;//HH
		}
		return remain;
	};

	for (size_t jj = 1; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
		for (size_t i = 0; i < NFungiPlant.size(); i++) {

			if (FungalGrowth > 0) {// should DemandUptake[0].DemandSoil[i] be UptDeficit_1?? HH
				if (jj == _N_) {
					NFungiPlant[i] = min(DemandUptake[0].DemandSoil[i], NFungiAvail[i]);
					DemandUptake[0].DemandSoil[i] = DemandUptake[0].DemandSoil[i] - NFungiPlant[i];
					DemandUptake[_N_ - 1].TotalDemandSoil = sum(DemandUptake[0].DemandSoil);//HH
					NFungiDemand[i] = NFungiDemand[i] + NFungiPlant[i];
					DemandFungiUptake[0].TotalDemand = DemandFungiUptake[0].TotalDemand + sum(NFungiPlant);//HH
					factNdemandFungi[i] = max(0., 1 - (CN_RatioMinFungi[i] / P_State_CarbonRatio[_FUNGI][_N_ - 1][i]));//HH
				}
				if (jj == _P_) {//HH
					PFungiPlant[i] = min(DemandUptake[1].DemandSoil[i], PFungiAvail[i]);
					DemandUptake[1].DemandSoil[i] = DemandUptake[1].DemandSoil[i] - PFungiPlant[i];
					DemandUptake[_P_ - 1].TotalDemandSoil = sum(DemandUptake[1].DemandSoil);//HH
					PFungiDemand[i] = PFungiDemand[i] + PFungiPlant[i];
					DemandFungiUptake[1].TotalDemand = DemandFungiUptake[1].TotalDemand + sum(PFungiPlant);//HH
					factPdemandFungi[i] = max(0., 1 - (CP_RatioMinFungi[i] / P_State_CarbonRatio[_FUNGI][_P_ - 1][i]));//HH
				}
			}
			else {
				NFungiPlant[i] = 0.;
				DemandFungiUptake[0].TotalDemand = 0.;
				if (m_pModelStructure->m_NumActPlantElements >= 3) //
					PFungiPlant[i] = 0.;
					DemandFungiUptake[1].TotalDemand = 0.;
			}
			if (DemandUptake[jj - 1].TotalDemandSoil < 0.E-8) {
				P_Uptake[PLANT_TOT_N][jj - 1][i] = 0.;
				UptDeficit_1[jj - 1][i] = 0.;
			}
			else {
				//in_upt = pNCSoil->NUptake(UptakeDistWithDepth[i], pPlant->Num_NUptMax, i, DemandUptake[0].DemandSoil[i], N_Tot_PlantSoilDemand, UptakeDistTotal, factNdemandFungi[i], FungiNitrateUptpot[i], FungiAmmoniumUptpot[i], NFungiDemand[i], N_Tot_FungiDemand);

				auto Sum_RemainAvail = FirstStepofMineralUptakeFromSoil(i, jj);


				UptDeficit_1[jj - 1][i] = max(0., DemandUptake[jj - 1].DemandSoil[i] - P_Uptake[PLANT_TOT_N][jj - 1][i]);
	
				//similar for P HH
				if (FungalGrowth_Sw(FungalGrowth) > FungalGrowth_Sw::off) {
					if (jj == _N_) {
						if (Root_Density_Influence_Sw(Root_Density_Influence) == Root_Density_Influence_Sw::on_nitrogen_uptake) {//HH
							upt = f_MineralUptaketoFungiWithDensityImpact(UptakeDistWithDepth[i], i, DemandUptake[0].DemandSoil[i], DemandUptake[_N_ - 1].TotalDemandSoil, UptakeDistTotal[0], pNCSoil->SoilNitrate, pNCSoil->SoilAmmonium, m_RootDensity, FungiRootCover, true, upt);
						}
						else
							upt = f_MineralUptaketoFungi(UptakeDistWithDepth[i], i, DemandUptake[0].DemandSoil[i], DemandUptake[_N_ - 1].TotalDemandSoil, UptakeDistTotal[0], pNCSoil->SoilNitrate, pNCSoil->SoilAmmonium, FungiRootCover, true, upt);
						P_Uptake[FUNGI_TOT_N][_N_ - 1][i] = sum(upt.UptNO3) + sum(upt.UptNH4);
						UpdateN_MineralUptakeFromSoil(upt);
						UptFungiDeficit_1[_N_ - 1][i] = max(0., NFungiDemand[i] - P_Uptake[FUNGI_TOT_N][_N_ - 1][i]);
						Sum_RemainAvail += upt.RemainFrac;
					}
						if (jj == _P_) {
							if (Root_Density_Influence_Sw(Root_Density_Influence) == Root_Density_Influence_Sw::on_nitrogen_uptake) {
								upt_p = f_PMineralUptakeWithDensityImpactWithFungi(UptakeDistWithDepth[i], i, DemandUptake[1].DemandSoil[i], DemandUptake[_P_ - 1].TotalDemandSoil, UptakeDistTotal[1], pNCSoil->Soil_PO4, m_RootDensity, FungiRootCover, true, upt_p);
							}
							else
								upt_p = f_PMineralUptaketoFungi(UptakeDistWithDepth[i], i, DemandUptake[1].DemandSoil[i], DemandUptake[_P_ - 1].TotalDemandSoil, UptakeDistTotal[1], pNCSoil->Soil_PO4, FungiRootCover, true, upt_p);
							P_Uptake[FUNGI_TOT_N][_P_ - 1][i] = sum(upt_p.UptPhosphate);
							UpdateP_MineralUptakeFromSoil(upt_p);
							UptFungiDeficit_1[_P_ - 1][i] = max(0., PFungiDemand[i] - P_Uptake[FUNGI_TOT_N][_P_ - 1][i]);
							Sum_RemainAvail += upt_p.RemainFrac;
						}
				}

			}

		}
		if (jj == _N_) {
			pNCSoil->NTotNO3_PlantUptake = sum(pNCSoil->SoilNitrateUptake);
			pNCSoil->NTotNH4_PlantUptake = sum(pNCSoil->SoilAmmoniumUptake);
			pNCSoil->NTotMin_PlantUptake = pNCSoil->NTotNO3_PlantUptake + pNCSoil->NTotNH4_PlantUptake;

		}
		else if (jj == _P_) {
			pNCSoil->PTotPO4_PlantUptake = sum(pNCSoil->SoilPhosphateUptake);
		}
	}



	for (size_t jj = 1; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
		UptDeficitTotal[jj - 1] = sum(UptDeficit_1[jj - 1]);
		UptDeficit_2[jj-1].assign(UptDeficit_2[jj-1].size(), 0.0);
		P_Uptake[PLANT_ORG_N][jj - 1].assign(P_Uptake[PLANT_ORG_N][jj - 1].size(), 0.0);
	}
	if (pNCSoil->OrganicUptake >= 1) {
		bool FungiUptake = true;
	/*	auto resetOrgUptake = [&]() {
			for (size_t jj = 1; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
				if (FungalGrowth_Sw(FungalGrowth) > FungalGrowth_Sw::off) {
					for (size_t j = L1_FUNGIUPTAKE; j <= H_FUNGIUPTAKE; j++)
						pNCSoil->O_Transform[j][jj].assign(pNCSoil->O_Transform[j][jj].size(), 0.);
				}

				pNCSoil->O_Transform[L1_PLANTUPTAKE][jj].assign(pNCSoil->O_Transform[L1_PLANTUPTAKE][jj].size(), 0.);
				pNCSoil->O_Transform[H_PLANTUPTAKE][jj].assign(pNCSoil->O_Transform[H_PLANTUPTAKE][jj].size(), 0.);
			}
		};*/
		OrganicUptakeResults in_for_update;
		auto UpdateOrgPlantUptake = [&](bool Fungi, size_t jj) {
				if (Fungi) {
					for (size_t i = 0; i < in_for_update.Litter1.size(); i++) {
						pNCSoil->O_Transform[L1_FUNGIUPTAKE][jj][i] += in_for_update.Litter1[i];
						pNCSoil->O_Transform[H_FUNGIUPTAKE][jj][i] += in_for_update.Humus[i];
					}
				}
				else {
					for (size_t i = 0; i < in_for_update.Litter1.size(); i++) {
						pNCSoil->O_Transform[L1_PLANTUPTAKE][jj][i] += in_for_update.Litter1[i];
						pNCSoil->O_Transform[H_PLANTUPTAKE][jj][i] += in_for_update.Humus[i];
					}
				}
			};

		auto selectOrgUptake = [&](size_t i,size_t jj, bool toFungi) {//HH
			OrganicUptakeResults out;
			if (Root_Density_Influence_Sw(Root_Density_Influence) == Root_Density_Influence_Sw::on_nitrogen_uptake) {

				if (FungalGrowth_Sw(FungalGrowth) == FungalGrowth_Sw::off) {
					out= f_OrganicUptakeWithDensity[jj-1](UptakeDistWithDepth[i], pPlant->Num_NUptMax, UptDeficit_1[jj - 1][i], UptakeDistTotal[jj - 1], pNCSoil->O_State[_L1][jj], pNCSoil->O_State[_H][jj],m_RootDensity);
				}
				else {
					
					out=f_OrganicUptakeWithDensitytoFungi[jj - 1] (FungiUptake,UptakeDistWithDepth[i], pPlant->Num_NUptMax, UptDeficit_1[jj - 1][i], UptakeDistTotal[jj - 1], pNCSoil->O_State[_L1][jj], pNCSoil->O_State[_H][jj], m_RootDensity);
				}
			}
			else {
				if (FungalGrowth_Sw(FungalGrowth) == FungalGrowth_Sw::off&&Root_Density_Influence_Sw(Root_Density_Influence) == Root_Density_Influence_Sw::no) {
					out = f_OrganicUptake[jj-1](UptakeDistWithDepth[i], pPlant->Num_NUptMax, UptDeficit_1[jj - 1][i], UptakeDistTotal[jj-1], pNCSoil->O_State[_L1][jj], pNCSoil->O_State[_H][jj]);
				}
				else {
					out=f_OrganicUptaketoFungi[jj - 1] (FungiUptake, UptakeDistWithDepth[i], pPlant->Num_NUptMax, UptDeficit_1[jj - 1][i], UptakeDistTotal[jj - 1], pNCSoil->O_State[_L1][jj], pNCSoil->O_State[_H][jj]);
				}
			}
			return out;

		};
		//resetOrgUptake();
		for (size_t jj = 1; jj < m_pModelStructure->m_NumActPlantElements; jj++) { 
			for (size_t i = 0; i < NFungiPlant.size(); i++) {
				in_for_update = selectOrgUptake(i,jj,  false);
				UpdateOrgPlantUptake(false, jj);
				P_Uptake[PLANT_TOT_N][jj - 1][i] += sum(in_for_update.Litter1) + sum(in_for_update.Humus);
				P_Uptake[PLANT_ORG_N][jj - 1][i] = sum(in_for_update.Litter1) + sum(in_for_update.Humus);
				if (FungalGrowth_Sw(FungalGrowth) > FungalGrowth_Sw::off) {
					in_for_update = selectOrgUptake(i,jj, true);
					UpdateOrgPlantUptake(true, jj);
					P_Uptake[FUNGI_ORG_N][jj - 1][i] = sum(in_for_update.Litter1) + sum(in_for_update.Humus);
				}
				UptDeficit_2[jj - 1][i] = max(0., DemandUptake[jj-1].DemandSoil[i] - P_Uptake[PLANT_TOT_N][jj- 1][i]);//HH
			}
			UptDeficitTotal[jj - 1] = sum(UptDeficit_2[jj - 1]);
		}
	}


	// N fixation

	if (N_Fixation_Sw(N_Fixation) == N_Fixation_Sw::Root_biomass_driven) {

		NFixationPlant = f_BelowGroundDrivenNFixation(UptakeDistWithDepth, pNCSoil->SoilNitrateConc, pNCSoil->SoilAmmoniumConc, P_State[_ROOT][_C_], P_State[_OLD_ROOT][_C_],
			NDrivTheta, NDrivTemperature, NFixationPlant);

	}
	else if (N_Fixation_Sw(N_Fixation) == N_Fixation_Sw::Stem_and_Leaf_biomass_driven) {
		NFixationPlant = f_AboveGroundDrivenNFixation(UptakeDistWithDepth, pNCSoil->SoilNitrateConc, pNCSoil->SoilAmmoniumConc,
			P_State[_ROOT][_C_], P_State[_OLD_ROOT][_C_], P_State[_STEM][_C_], P_State[_OLD_STEM][_C_],
			NDrivTheta, NDrivTemperature, NFixationPlant);
	}

	for (size_t i = 0; i < NFixationPlant.size(); i++) {
		DemandUptake[0].DemandSoil[i] -= NFixationPlant[i];
		DemandUptake[_N_-1].TotalDemandSoil += DemandUptake[0].DemandSoil[i];
	}

	UptDeficitTotal[_N_ - 1] = sum(UptDeficit_2[_N_ - 1]);
	UptFungiDeficitTotal[_N_ - 1] = sum(UptFungiDeficit_1[_N_-1]);

	// Uptake of N deposition
	if (pNCSoil->N_Deposition) {
		for (size_t i = 0; i < P_Uptake[PLANT_TOT_N][_N_ - 1].size(); i++)
			P_Uptake[PLANT_TOT_N][_N_ - 1][i] += pNCSoil->N_Deposition_Leaf[i];
	}

	// Update of UptakeDistTotal with regard to NUptDeficit_1
	for (size_t jj = 1; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
		UptakeDistTotal[jj - 1].assign(pSoil->NL, 0.);
		UptakeFungiDistTotal[jj - 1].assign(pSoil->NL, 0.);
		for (size_t i = 0; i < UptDeficit_2[jj - 1].size(); i++) {
			if (UptDeficitTotal[jj - 1] > 0.0)
				for (size_t ii = 0; ii < pSoil->NL; ii++) UptakeDistTotal[jj - 1][ii] += UptakeDistWithDepth[i][ii] * UptDeficit_2[jj - 1][i] / UptDeficitTotal[jj - 1]; //converted deficit 1 to 2 AM 7/4
			if (UptFungiDeficitTotal[jj - 1] > 0.0)
				for (size_t ii = 0; ii < pSoil->NL; ii++) UptakeFungiDistTotal[jj - 1][ii] += UptakeDistWithDepth[i][ii] * UptFungiDeficit_2[jj - 1][i] / UptFungiDeficitTotal[jj - 1]; //converted deficit 1 to 2 AM 7/
		}
	}
	//taken organic uptake from here to above AM

	if (N_Fixation_Sw(N_Fixation) == N_Fixation_Sw::C_demand_driven) {
		if (pNCSoil->OrganicUptake)
			NFixationPlant = f_DemandDrivenNFixation(UptDeficit_2[_N_ - 1]);
		else
			NFixationPlant = f_DemandDrivenNFixation(UptDeficit_1[_N_ - 1]);

		for (size_t i = 0; i < UptDeficit_2[_N_ - 1].size(); i++) {
			P_Uptake[PLANT_TOT_N][_N_-1][i] += NFixationPlant[i];
			UptDeficit_2[_N_ - 1][i] = max(0., DemandUptake[0].DemandSoil[i] - P_Uptake[PLANT_TOT_N][_N_ - 1][i]);
			NRootUptake[i] = P_Uptake[PLANT_TOT_N][_N_ - 1][i];
		}
	}

	// N Fungal Uptake
	if (FungalGrowth > 0) {
		for (size_t i = 0; i < NRootUptake.size(); i++) {
			NRootUptake[i] = P_Uptake[PLANT_TOT_N][_N_ - 1][i]; //AM 161211 new variable to describe net plant uptake if fungalgrowth >0
			P_Uptake[PLANT_TOT_N][_N_ - 1][i] = P_Uptake[PLANT_TOT_N][_N_ - 1][i] + NFungiPlant[i];
			if (m_pModelStructure->m_NumActPlantElements >= 3) //
				PRootUptake[i] = P_Uptake[PLANT_TOT_N][_P_ - 1][i]; //HH
				P_Uptake[PLANT_TOT_N][_P_ - 1][i] = P_Uptake[PLANT_TOT_N][_P_ - 1][i] + PFungiPlant[i];
		}
	}

	// N fixed supply		
	if (N_Fixed_Supply) {
		for (size_t i = 0; i < P_Uptake[PLANT_TOT_N][_N_ - 1].size(); i++) {
			if (P_Uptake[PLANT_TOT_N][_N_ - 1][i] < DemandUptake[0].DemandSoil[i]) {
				NPlantAddSupply[i] = DemandUptake[0].DemandSoil[i] - P_Uptake[PLANT_TOT_N][_N_ - 1][i];
				P_Uptake[PLANT_TOT_N][_N_ - 1][i] = DemandUptake[0].DemandSoil[i];
				UptDeficit_1[_N_ - 1][i] = 0.;
			}
			else
				NPlantAddSupply[i] = 0.;

		}
	}

	// Allocation of Nitrogen Phosphorus to Plant components below
	//P_CarbonAlloNewMobile;

	for (size_t jj = 1; jj < m_pModelStructure->m_NumActPlantElements; jj++)
		P_UptakeNewMobile[jj - 1] = f_AlloUptakePlant[jj - 1](P_Respiration[_LEAF], P_Respiration[_STEM], P_Respiration[_ROOT], P_Respiration[_COARSE_ROOT], P_State[_NEWMOBILE][jj], P_CarbonAlloNewMobile);
	
}



void NC_Plant::At_YearShifting() {//year shift for P added by HH

	auto MakeShiftCurrent2Old = [&](size_t i) {

		for (size_t j = LEAF_OLD_LEAF; j <= STEM_OLD_STEM; j++) {
			size_t index = j - LEAF_OLD_LEAF + _LEAF;
			P_ReAllocation[j][_C_][i] = P_State[index][_C_][i] * 1. / T_Step; //-P_Respiration[index][i] - P_LitterFall[index][_C_][i];
			P_ReAllocation[j][_N_][i] = P_State[index][_N_][i] * 1. / T_Step; //-P_LitterFall[index][_N_][i];
			P_ReAllocation[j][_P_][i] = P_State[index][_P_][i] * 1. / T_Step;// -P_LitterFall[index][_P_][i];
		}

		for (size_t j = ROOT_OLD_ROOT; j <= COARSE_ROOT_OLD_COARSE_ROOT; j++) {
			size_t index = j - ROOT_OLD_ROOT + _ROOT;
			P_ReAllocation[j][_C_][i] = P_State[index][_C_][i] * 1. / T_Step;// -P_Respiration[index][i] - P_LitterFall[index][_C_][i];
			P_ReAllocation[j][_N_][i] = P_State[index][_N_][i] * 1. / T_Step;// -P_LitterFall[index][_N_][i];
			P_ReAllocation[j][_P_][i] = P_State[index][_P_][i] * 1. / T_Step;// -P_LitterFall[index][_P_][i];
		}

		DormingTempSum[i] = 0.;
		Emergence_Done[i] = false;
		f_GrainFill = PlantTempSum(T_Thres_GrainFill, T_Sum_GrainFill);




	};


	for (size_t iplant = 0; iplant < P_State[_STEM][_C_].size(); iplant++) {
		for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++)
			for (size_t j = LEAF_OLD_LEAF; j <= COARSE_ROOT_OLD_COARSE_ROOT; j++) {
				P_ReAllocation[j][jj][iplant] = 0.;
			}
		yearshift = false;

		if (m_pModelInfo->JDayNum == 1 && Max_Plant_LifeTime[iplant] > 1. && pAtm->Latitude > 0 && SimPlantAge[iplant] > 180.0 && YearShiftToBeDone[iplant]) {

			MakeShiftCurrent2Old(iplant);
			for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
				RemainOldLeaf[jj][iplant] = max(0., (P_State[_OLD_LEAF][jj][iplant]) * (1. - 1. / max(1., (Max_Leaf_Lifetime[iplant] - 1.))));
				RemainOldLeaf[jj][iplant] = P_State[_OLD_LEAF][jj][iplant] + P_ReAllocation[LEAF_OLD_LEAF][jj][iplant];
			}
			if (Winter_Regulation == 1)
				GrowthStageIndex[iplant] = Min_GSI[iplant];

			yearshift = true;
			YearShiftToBeDone[iplant] = false;
			ResetYearShiftDay = 2;

			AccNPP_Leaf[iplant] = 0.;
			AccNPP_Stem[iplant] = 0.;
			AccNPP_CoarseRoots[iplant] = 0.;
			AccNPP_Roots[iplant] = 0.;
			AccNPP_Plant[iplant] = 0.;
			Annual_D_CPlant[iplant] = P_SumPlant[_PLANT_WHOLE][_C_][iplant] - PreviousCPlant[iplant];
			PreviousCPlant[iplant] = P_SumPlant[_PLANT_WHOLE][_C_][iplant];
		}
		else if (m_pModelInfo->JDayNum == 180 && Max_Plant_LifeTime[iplant] > 1. && pAtm->Latitude < 0 && SimPlantAge[iplant]>180.0 && YearShiftToBeDone[iplant]) {

			MakeShiftCurrent2Old(iplant);
			for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
				RemainOldLeaf[jj][iplant] = (P_State[_OLD_LEAF][jj][iplant]) * (1. - 1. / max(1., (Max_Leaf_Lifetime[iplant] - 1.)));
			}

			yearshift = true;
			YearShiftToBeDone[iplant] = false;
			ResetYearShiftDay = 181;
			AccNPP_Leaf[iplant] = 0.;
			AccNPP_Stem[iplant] = 0.;
			AccNPP_CoarseRoots[iplant] = 0.;
			AccNPP_Roots[iplant] = 0.;
			AccNPP_Plant[iplant] = 0.;
			Annual_D_CPlant[iplant] = P_SumPlant[_PLANT_WHOLE][_C_][iplant] - PreviousCPlant[iplant];
			PreviousCPlant[iplant] = P_SumPlant[_PLANT_WHOLE][_C_][iplant];
		}
		else if (m_pModelInfo->JDayNum == ResetYearShiftDay) {
			for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
				RemainOldLeaf[jj][iplant] = max(0., (P_State[_OLD_LEAF][jj][iplant]) * (1. - 1. / max(1., (Max_Leaf_Lifetime[iplant] - 1.))));
				P_LitterFall[_OLD_LEAF][jj][iplant] = 0.;
				P_ReAllocation[_OLD_LEAF][jj][iplant] = 0.;
			}

			YearShiftToBeDone[iplant] = true;
		}

		if (yearshift) 	SimPlantYearAge[iplant] = SimPlantAge[iplant] / 365.25 + 0.5;

	}

	if (yearshift) {
		for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
			for (size_t j = LEAF_OLD_LEAF; j <= COARSE_ROOT_OLD_COARSE_ROOT; j++) {
				auto root = P_State[_ROOT][0];
				auto oldroot = P_State[_OLD_ROOT][0];
				auto fluxes = P_ReAllocation[j][jj];
				OutFlow(P_State[P_ReLink[j].first][jj], fluxes);
				InFlow(P_State[P_ReLink[j].second][jj], fluxes);
				root = P_State[_ROOT][0];
				oldroot = P_State[_OLD_ROOT][0];
			}
		}
	}



}
void NC_Plant::TraceElementUptake() {

	for(size_t i=0; i<P_State[_STEM][_C_].size(); i++) {
	if(pPlant->TraceElementUptake) {

		// Concentrations of TraceElements
			pPlant->TEC_RatioLeaf[i]=0.;
			pPlant->TEC_RatioStem[i]=0.;
			pPlant->TEC_RatioGrain[i]=0.;
			pPlant->TEC_RatioRoots[i]=0.;
			pPlant->TEC_RatioOldLeaf[i]=0.;
			pPlant->TEC_RatioOldStem[i]=0.;
			pPlant->TEC_RatioOldRoots[i]=0.	;
			if(P_State[_LEAF][_C_][i]>0.001&&pPlant->TE_Leaf[i]>0.)	pPlant->TEC_RatioLeaf[i]			=1.E-3*	pPlant->TE_Leaf[i]/P_State[_LEAF][_C_][i];
			if(P_State[_STEM][_C_][i]>0.001&&pPlant->TE_Stem[i]>0.)	pPlant->TEC_RatioStem[i]			=1.E-3*	pPlant->TE_Stem[i]/P_State[_STEM][_C_][i];
			if(P_State[_GRAIN][_C_][i]>0.001&&pPlant->TE_Grain[i]>0.)	pPlant->TEC_RatioGrain[i]		=1.E-3*	pPlant->TE_Grain[i]/P_State[_GRAIN][_C_][i];
			if(P_State[_ROOT][_C_][i]>0.001&&pPlant->TE_Roots[i]>0.)	pPlant->TEC_RatioRoots[i]		=1.E-3*	pPlant->TE_Roots[i]/P_State[_ROOT][_C_][i];
			if(P_State[_OLD_LEAF][_C_][i]>0.001&&pPlant->TE_OldLeaf[i]>0.)	pPlant->TEC_RatioOldLeaf[i] =1.E-3*	pPlant->TE_OldLeaf[i]/P_State[_OLD_LEAF][_C_][i];
			if(P_State[_OLD_STEM][_C_][i]>0.001&&pPlant->TE_OldStem[i]>0.)	pPlant->TEC_RatioOldStem[i] =1.E-3*	pPlant->TE_OldStem[i]/P_State[_OLD_STEM][_C_][i];
			if(P_State[_OLD_ROOT][_C_][i]>0.001&&pPlant->TE_OldRoots[i]>0.)	pPlant->TEC_RatioOldRoots[i]=1.E-3*	pPlant->TE_OldRoots[i]/P_State[_OLD_ROOT][_C_][i];

		// fluxes of TraceElements
			pPlant->TE_LeafSurfaceLitter[i]		=1.E3*P_LitterFall[_LEAF][_C_][i] *pPlant->TEC_RatioLeaf[i] ;
			pPlant->TE_OldLeafSurfaceLitter[i]	=1.E3*P_LitterFall[_OLD_LEAF][_C_][i] *pPlant->TEC_RatioOldLeaf[i] ;
			pPlant->TE_StemSurfaceLitter[i]		=1.E3*P_LitterFall[_STEM][_C_][i] *pPlant->TEC_RatioStem[i] ;
			pPlant->TE_OldStemSurfaceLitter[i]	=1.E3*P_LitterFall[_OLD_STEM][_C_][i] *pPlant->TEC_RatioOldStem[i] ;
			pPlant->TE_GrainSurfaceLitter[i]	=1.E3*P_LitterFall[_GRAIN][_C_][i]*pPlant->TEC_RatioGrain[i];

			pPlant->TE_RootsLitter[i]			=1.E3*P_LitterFall[_ROOT][_C_][i]*pPlant->TEC_RatioRoots[i];
			pPlant->TE_OldRootsLitter[i]		=1.E3*P_LitterFall[_OLD_ROOT][_C_][i]*pPlant->TEC_RatioOldRoots[i];

			pPlant->TE_StemGrain[i]				=1.E3*P_ReAllocation[STEM_GRAIN][_C_][i]	*pPlant->TEC_RatioStem[i];
			pPlant->TE_LeafGrain[i]				=1.E3*P_ReAllocation[LEAF_GRAIN][_C_][i]	*pPlant->TEC_RatioLeaf[i];
			pPlant->TE_RootsGrain[i]			=1.E3*P_ReAllocation[ROOT_GRAIN][_C_][i]	*pPlant->TEC_RatioRoots[i]			;
			
			if(yearshift) {
	 			pPlant->TE_StemOldStem[i]		=pPlant->TE_Stem[i]*1./T_Step-pPlant->TE_StemSurfaceLitter[i];
				pPlant->TE_LeafOldLeaf[i]		=pPlant->TE_Leaf[i]*1./T_Step-pPlant->TE_LeafSurfaceLitter[i];
				pPlant->TE_RootsOldRoots[i]		=pPlant->TE_Roots[i]*1./T_Step-pPlant->TE_RootsLitter[i];
			}
			else {
				pPlant->TE_StemOldStem[i]=0.;
				pPlant->TE_LeafOldLeaf[i]=0.;
				pPlant->TE_RootsOldRoots[i]=0.;
			}
		}
	}


}
void NC_Plant::Summation_Fluxes() {
	for (size_t i = 0; i < P_State[_STEM][_C_].size(); i++) {
		if (NDrivRadInShort > 0 && Growth == 3) {
			// David, 2005-11-18, to avoid division by zeero with new compiler
			if (sum(RadPlantAdsorbed) > 0.)
				ResponseWeightCoef[i] = RadPlantAdsorbed[i] / sum(RadPlantAdsorbed);
			else
				ResponseWeightCoef[i] = 0.0;

			AccResponseWater = AccResponseWater + ResponseWeightCoef[i] * GPP_Driver.Water[i];
			AccResponseTemp = AccResponseTemp + ResponseWeightCoef[i] * GPP_Driver.Temp[i];
			AccResponseN = AccResponseN + ResponseWeightCoef[i] * GPP_Driver.Nitrogen[i];
			AccResponseWaterN = AccResponseWaterN + ResponseWeightCoef[i] * GPP_Driver.Nutrient_Water[i];
		}
		else
			ResponseWeightCoef[i] = 0.;
		CPlantAtm[i] = P_Respiration[_LEAF][i] + P_Respiration[_STEM][i] + P_Respiration[_ROOT][i] + P_Respiration[_GRAIN][i]
			+ P_Respiration[_OLD_LEAF][i] + P_Respiration[_OLD_STEM][i] + P_Respiration[_OLD_ROOT][i] + P_Respiration[_COARSE_ROOT][i] + P_Respiration[_OLD_COARSE_ROOT][i];

		CPlantAtm_AboveG[i] = P_Respiration[_LEAF][i] + P_Respiration[_STEM][i] + P_Respiration[_GRAIN][i] + P_Respiration[_OLD_LEAF][i] + P_Respiration[_OLD_STEM][i];

		NPP_Leaf[i] = P_CarbonAlloNewMobile.Leaf[i] - P_Respiration[_LEAF][i] - P_Respiration[_OLD_LEAF][i];
		NPP_Stem[i] = P_CarbonAlloNewMobile.Stem[i] - P_Respiration[_STEM][i] - P_Respiration[_OLD_STEM][i];
		NPP_Roots[i] = P_CarbonAlloNewMobile.Roots[i] - P_Respiration[_ROOT][i] - P_Respiration[_OLD_ROOT][i];
		NPP_CoarseRoots[i] = P_CarbonAlloNewMobile.CoarseRoots[i] - P_Respiration[_COARSE_ROOT][i] - P_Respiration[_OLD_COARSE_ROOT][i];
		NPP_Plant[i] = NPP_Leaf[i] + NPP_Stem[i] + NPP_CoarseRoots[i] + NPP_Roots[i] - P_Respiration[_GRAIN][i];

		AccNPP_Leaf[i] += NPP_Leaf[i] * T_Step;
		AccNPP_Stem[i] += NPP_Stem[i] * T_Step;
		AccNPP_CoarseRoots[i] += NPP_CoarseRoots[i] * T_Step;
		AccNPP_Roots[i] += NPP_Roots[i] * T_Step;
		AccNPP_Plant[i] += NPP_Plant[i] * T_Step;
	}

	for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
		for (size_t i = 0; i < P_State[_STEM][_C_].size(); i++) {

			// Auxiliary Variables
			P_SumPlant[_ROOT_ALL][jj][i] = P_State[_ROOT][jj][i] + P_State[_OLD_ROOT][jj][i] + P_State[_COARSE_ROOT][jj][i] + P_State[_OLD_COARSE_ROOT][jj][i];
			P_SumPlant[_LEAF_ALL][jj][i] = P_State[_OLD_LEAF][jj][i] + P_State[_LEAF][jj][i];
			P_SumPlant[_PLANT_WHOLE][jj][i] = P_State[_GRAIN][jj][i] + P_State[_LEAF][jj][i] + P_State[_STEM][jj][i] + P_State[_OLD_LEAF][jj][i] +
				P_State[_OLD_STEM][jj][i] + P_SumPlant[_ROOT_ALL][jj][i] +P_State[_NEWMOBILE][jj][i] + P_State[_MOBILE][jj][i];
			P_SumPlant[_ABOVE_GROUND][jj][i] = P_SumPlant[_PLANT_WHOLE][jj][i] - P_SumPlant[_ROOT_ALL][jj][i];


		}

		P_AllPlants[_ROOT_ALL][jj] = sum(P_SumPlant[_ROOT_ALL][jj]);
		P_AllPlants[_PLANT_WHOLE][jj] = sum(P_SumPlant[_PLANT_WHOLE][jj]);

		P_AllPlants[_ABOVE_GROUND][jj] = sum(P_SumPlant[_ABOVE_GROUND][jj]);
		P_AllPlants[_ABOVE_GROUND][jj] = sum(P_SumPlant[_ABOVE_GROUND][jj]);


		for (size_t jj = 1; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
			for (size_t i = 0; i < P_State[_STEM][_C_].size(); i++) {
				if (P_SumPlant[_PLANT_WHOLE][jj][i] > 0) C_RatioPlant[jj - 1][i] = P_SumPlant[_PLANT_WHOLE][_C_][i] / P_SumPlant[_PLANT_WHOLE][jj][i];//HH
			}
		}


		if (jj >= _N_) {
			if (P_AllPlants[_PLANT_WHOLE][jj] > 0) C_RatioTotPlant[jj - 1] = P_AllPlants[_PLANT_WHOLE][_C_] / P_AllPlants[_PLANT_WHOLE][jj];
			if (P_AllPlants[_ROOT_ALL][jj] > 0) C_RatioTotRoots[jj - 1] = P_AllPlants[_ROOT_ALL][_C_] / P_AllPlants[_ROOT_ALL][jj];
		}

		TotHarvest[jj] = sum(P_Harvest[_NEWMOBILE][jj]);
		for (size_t j = 1; j <= _OLD_COARSE_ROOT; j++) TotHarvest[jj] += sum(P_Harvest[j][jj]);


		if (sum(P_Harvest[_GRAIN][jj]) > 0.) {
			YearGrainHarvest[jj] = 0;
			for (size_t iplant = 0; iplant<P_Harvest[_GRAIN][jj].size(); iplant++) {
				YearGrainHarvest[jj] += P_Harvest[_GRAIN][jj][iplant]*T_Step;
			}
		}

		if (TotHarvest[jj] > 0) {
			YearTotalHarvest[jj] = 0;
			for (size_t iplant = 0;iplant< P_Harvest[_GRAIN][jj].size(); iplant++) {
				YearTotalHarvest[jj] += TotHarvest[jj]*T_Step;
			}
		}

		TotPlantLitter[jj] = sum(P_LitterFall[_NEWMOBILE][jj]);
		TotalPlantLitterFall[jj] = P_LitterFall[_NEWMOBILE][jj];
		for (size_t j = 1; j <= _OLD_COARSE_ROOT; j++) {
			TotPlantLitter[jj] += sum(P_LitterFall[j][jj]);
			if (j == _OLD_STEM) TotAbovePlantLitter[jj] = TotPlantLitter[jj];
			for (size_t iplant = 0; iplant <P_LitterFall[j][jj].size(); iplant++) {
				TotalPlantLitterFall[jj][iplant] += P_LitterFall[j][jj][iplant];
			}
			
		}

		// AM 200911
		if (jj >= _N_) 	Tot_PlantUptake[jj - 1] = sum(P_Uptake[PLANT_TOT_N][jj - 1]);

		if (FungalGrowth_Sw(FungalGrowth) > FungalGrowth_Sw::off) {
			P_AllPlants[_FUNGI_][jj] = sum(P_State[_FUNGI][jj]);
			TotFungiLitter[jj] = sum(P_LitterFall[_FUNGI][jj]);

			if (jj >= _N_) {
				Tot_FungiUptake[_N_ - 1] = sum(P_Uptake[FUNGI_TOT_N][_N_ - 1]) + sum(P_Uptake[FUNGI_ORG_N][_N_ - 1]); //- sum(NFungiPlant); //to avoid double summing up
				Tot_FungiUptake[_P_ - 1] = sum(P_Uptake[FUNGI_TOT_N][_P_ - 1]) + sum(P_Uptake[FUNGI_ORG_N][_P_ - 1]); //- sum(PFungiPlant);//HH
			}
		}


	if (N_ReAllocation > 0) {
		Mobile_Plant_Flux[jj].assign(Mobile_Plant_Flux[jj].size(), 0.);
		//Do i=1, NumPlants
		for (size_t i = 0; i < Mobile_Plant_Flux[jj].size(); i++) {
			Mobile_Plant_Flux[jj][i] = P_ReAllocation[OLD_LEAF_MOBILE][jj][i];
			for (size_t j = OLD_STEM_MOBILE; j <= COARSE_ROOT_MOBILE; j++)
				Mobile_Plant_Flux[jj][i] += P_ReAllocation[j][jj][i];//HH

		}

		Mobile_TotPlant_Flux[jj] = sum(Mobile_Plant_Flux[jj]);

	}
}

	if(NDrivRadInShort>0&&Growth==3) {
	// David, 2005-11-18, to avoid division by zeero with new compiler
	    if(sum(RadPlantAdsorbed)>0.){
			Tot_Response_Temp = AccResponseTemp;// sum(RadPlantAdsorbed);
			Tot_Response_Water = AccResponseWater;// sum(RadPlantAdsorbed);
			Tot_Response_N = AccResponseN;// sum(RadPlantAdsorbed);
			Tot_Response_N_Water = AccResponseWaterN;// sum(RadPlantAdsorbed);
		}
		else {
		    Tot_Response_Temp=0.0;
		    Tot_Response_Water=0.0;
		    Tot_Response_N=0.0;
		    Tot_Response_N_Water=0.0;
		}
	}



}
bool NC_Plant::End() {
	if (ZadokScale_Sw(ZadokScale) ==ZadokScale_Sw::withOutPut2File) pZadokOutCreateFile->CreateFileOutput(m_pModelInfo->ActualNoMulRun);
	else if (ZadokScale_Sw(ZadokScale)== ZadokScale_Sw::withOutPut3File) pZadokOutCreateFile->CreateFileOutput(m_pModelInfo->ActualNoMulRun);
return true;

}
NC_Plant::~NC_Plant(void)
{
	if(pZadokOutCreateFile!=nullptr) delete pZadokOutCreateFile;
}


