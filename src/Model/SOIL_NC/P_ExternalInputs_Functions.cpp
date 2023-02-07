#include "P_ExternalInputs_Functions.h"
#include "../PLANT_NC/PlantOrgModel.h"
#include "../SOIL/SoilModel.h"
#include "../SOIL/Soil_Frost.h"
#include "../PLANT/PlantModel.h"
#include "./Functors/NC_Soil_Functors.h"


P_ExternalInputs_Functions::P_ExternalInputs_Functions(void)
{


}
P_ExternalInputs_Functions::~P_ExternalInputs_Functions(void)
{
}


bool P_ExternalInputs_Functions::Def()
{
	NC_ExternalInputs::Def();
	ModuleNo=37;
	SoilModel* p_Soil=(SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
	Soil_Frost* pSoil=&p_Soil->m_SoilFrost;
	NE *pNL=m_pModelMap->GetNEPointer("Number of Layers");
	PlantModel* pPlant=(PlantModel*)m_pModelInfo->GetModelPtr("Plant Model");

	WaterUptake* p_Plant=&pPlant->m_WaterUptake;
	NE *pNumPlants=m_pModelMap->GetNEPointer("Number of Plants");

	Sw* pSw; Ps *pPs; P *pP; X *pX; T *pT; G *pG;  Gs *pGs; Xs* pXs; Ts* pTs;



	pXs = new Xs(&P_Fert, SOILSURFACE, PHOSPHORUS, STORAGE, FORCING_BIOLOGY, NORMAL);
	Define(pXs, "P Fertilizer", 0.,UNIT_TYPES::MASS_UNIT, "Phosphorus> 0|N Fertilization >0");

	pTs = new Ts(&P_Fert_App, SOILSURFACE, PHOSPHORUS, TRANSPORT, FORCING_BIOLOGY, NORMAL);
	Define(pTs, "P Fert Appl Rate", 0.,UNIT_TYPES::MASS_UNIT, "Phosphorus >= 1|N Fertilization >0");
	pTs = new Ts(&P_Fert_Sol, SOILSURFACE, PHOSPHORUS, TRANSPORT, FORCING_BIOLOGY, NORMAL);
	Define(pTs, "P Fert Solution Rate", 0.,UNIT_TYPES::MASS_UNIT, "Phosphorus >= 1|N Fertilization >0");

	vector<double> initv; 
	NE *pNFertDayNo=GetNEPointer("Num of Fert Days");
	pP = m_pModelMap->GetPPointer("Fert DayNo");
	mTab_PFert.push_back(pP);
	pP = new P(&P_Fert_Rate, pNFertDayNo, SOILSURFACE, PHOSPHORUS, TRANSPORT, FORCING_BIOLOGY, NORMAL); initv.assign(NFertDayNo, 0.);
	Define(pP, pNFertDayNo, "P Fert Rate", initv,UNIT_TYPES::MASSFLOW_UNIT, "Phosphorus >= 1", 0., 100.);

	mTab_PFert.push_back(pP);


	Tab *pTab;
	pTab = new Tab(&mTab_PFert, SOILSURFACE, PHOSPHORUS, TRANSPORT, FORCING_BIOLOGY, NORMAL);

	Define(pTab, "P Fertilization", "Fert DayNo|N Fert Rate", "Phosphorus >= 1|N Fertilization =1");

	pPs = new Ps(&Dep_P_DryRate, SOILSURFACE, PHOSPHORUS, TRANSPORT, FORCING_BIOLOGY, NORMAL);
	Define(pPs, "Dep P DryRate", 0.000074,UNIT_TYPES::MASSFLOW_UNIT, "Phosphorus >= 1|N Deposition >0", 0., 0.1);
	pPs = new Ps(&P_FertDiss_k, SOILSURFACE, PHOSPHORUS, TRANSPORT, FORCING_BIOLOGY, NORMAL);
	Define(pPs, "P Fert Dis k", 0.015, "-", " Phosphorus >= 1|N Fertilization >=1", 0.001, 1.0);



		Func *pFunc;
		// Parameters;
		string funcname;
		FunctorProp functorprop;
		vector<Ps*> ps_vec;
		vector<P*> p_vec;
		ps_vec.clear();


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


	return true;
}

