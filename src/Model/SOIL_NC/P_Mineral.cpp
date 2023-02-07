#include "P_Mineral.h"
#include "NC_Organic.h"
#include "../SOIL/SoilModel.h"
#include "../SOIL_NC/SoilOrgModel.h"
#include "../SOIL_NC/NC_SoilManagement.h"
#include "./Functors/NC_Soil_Functors.h"

#include "../Structure/Structure.h"
#include "../Structure/Functors/Structure_Functors.h"
#include "../../ModelTypes/FunctFactory.h"
#include "../Atmosphere/AtmModel.h"



P_Mineral::P_Mineral(void)
{


}
P_Mineral::~P_Mineral(void)
{
}

bool P_Mineral::Ini()
{
	AbioticResponses::Ini();
	
	function<vector<double>(bool)> f_density;
	f_density=GetVector_boolBlank_Functor(MyFunc::DRYBULKDENSITY);
	vector<double> dens = f_density(false);

	if (Phosphorus_Model_Sw(m_pModelStructure->Phosphorus_Model) == Phosphorus_Model_Sw::off) return true;
	f_FirstOrder = FirstOrderResponse(Weath_RateCoef);
	f_pH_Response = PH_Response_Weathering(Weath_pH_Shape, Weath_pH0);
	f_Adsorption = Get_D1_Functor(MyFunc::PO4_ADSORPTION);
	f_PSurfaceRunoff_Conc = Get_D1_Functor(MyFunc::P_SURFACERUNOFF_CONC);

	MassPerLayer.resize(Soil_Mineral_P.size());

	for (size_t i = 0; i < Soil_Mineral_P.size(); i++) {
		MassPerLayer[i] = dens[i] * pSoil->AThicknessLayers[i]*1.E6;
		Soil_Mineral_P[i] = InitMineral_P_ContentZ[i]* MassPerLayer[i];
		if (InitialMineral_P == 0)
			Soil_PO4[i] = InitPhosphateConc*pSoil->WaterStorage[i];
		else
			Soil_PO4[i] = InitPhosphateConcZ[i]*pSoil->WaterStorage[i];
	};

	PMineral_MassBalanceCheck=AccInflowP=AccOutFlowP= TotDiffStorageP=0.;
	InitialStorageP = sum(Soil_Mineral_P)+sum(Soil_PO4);
	PTotPhosphate_Soil = sum(Soil_PO4);
	PTot_PSolidMinerals = sum(Soil_Mineral_P);
	return true;
}
bool P_Mineral::IniFunctors()
{
	AbioticResponses::IniFunctors();

	function<vector<double>(bool)> f_density;
	f_density = GetVector_boolBlank_Functor(MyFunc::DRYBULKDENSITY);
	vector<double> dens = f_density(false);

	if (Phosphorus_Model_Sw(m_pModelStructure->Phosphorus_Model) == Phosphorus_Model_Sw::off) return true;
	f_FirstOrder = FirstOrderResponse(Weath_RateCoef);
	f_pH_Response = PH_Response_Weathering(Weath_pH_Shape, Weath_pH0);
	f_Adsorption = Get_D1_Functor(MyFunc::PO4_ADSORPTION);
	f_PSurfaceRunoff_Conc = Get_D1_Functor(MyFunc::P_SURFACERUNOFF_CONC);
	PSurfRunoff = 0.;


	return true;
}

bool P_Mineral::Dyn()
{
	AbioticResponses::Dyn();
	if (Phosphorus_Model_Sw(m_pModelStructure->Phosphorus_Model) == Phosphorus_Model_Sw::off) return true;
	Flux();
	Integ();
	PTotPhosphate_Soil = sum(Soil_PO4);
	PTot_PSolidMinerals = sum(Soil_Mineral_P);
	Tot_PWeatheringRate_Soil = sum(WeatheringRate);
	return true;
}

void P_Mineral::Flux()


{
	SoilOrgModel* p_Soil = (SoilOrgModel*)m_pModelInfo->GetModelPtr("Soil Org Model");
	NC_SoilManagement* pNCSoil = &p_Soil->m_NC_SoilOrg;

	for (size_t i = 0; i < WeatheringRate.size(); i++) {
		WeatheringRate[i] = f_CommonTempResponse(p_NC_Plant->NDrivTemperature[i])*f_pH_Response( pNCSoil->pH_Value[i])*f_FirstOrder(Soil_Mineral_P[i]);
	}

	for (size_t i = 0; i < Soil_PO4_Sol.size(); i++) {
		Soil_PO4_Sol[i] = f_Adsorption(Soil_PO4[i]/MassPerLayer[i]);
	}




	//	PO4 concentrations
	for (size_t i = 0; i < Soil_PO4_Conc.size(); i++) {
		Soil_PO4_Conc[i] = 100.*Soil_PO4_Sol[i] / p_NC_Plant->NDrivTheta[i] / pSoil->AThicknessLayers[i];
		if (Soil_PO4_Sol[i] < 0.)  Soil_PO4_Conc[i] = 1.E-7;
	}



	for (size_t i = 0; i < SoilPhosphateFlow.size(); i++) {
		if (p_NC_Plant->NDrivWaterFlow[i] > 0.)
			SoilPhosphateFlow[i] = Soil_PO4_Conc[i] * p_NC_Plant->NDrivWaterFlow[i] / 1000.;
		else if (i < Soil_PO4_Conc.size() - 1) {
			SoilPhosphateFlow[i] = Soil_PO4_Conc[i + 1] * p_NC_Plant->NDrivWaterFlow[i] / 1000.;
		}
		else
			SoilPhosphateFlow[i] = 0;
	}

	if (p_NC_Plant->DeepPercolationInput > 0)
		SoilPhosphateFlow.back() = SoilPhosphateFlow.back() + p_NC_Plant->NDrivDeepPercolation*Soil_PO4_Conc.back() / 1000.;

	//	Horizontal P flow
	if (p_NC_Plant->SoilDrainageInput > 0)
		for (size_t i = 0; i < SoilPhosphateDrainage.size(); i++) SoilPhosphateDrainage[i] = p_NC_Plant->NDrivDrainage[i] * Soil_PO4_Conc[i] / 1000.;

	PTotPO4_Drainage = sum(SoilPhosphateDrainage);
	PTotPO4Conc_Drainage = 1000. * PTotPO4_Drainage / sum(p_NC_Plant->NDrivDrainage); //HH
	Tot_MinP_DrainageConc = (PSurfRunoff + PTotPO4_Drainage) / sum(p_NC_Plant->NDrivDrainage) * 1000.;//HH;



	// Surfacerunoff P flow
	//if (Theta.front() - M_Saturation.front() > 0)
	AtmModel* pAtm = (AtmModel*)m_pModelInfo->GetModelPtr("Atm Model");
	Irrigation *pMet = (Irrigation*)&pAtm->m_Irrigation;
	PSurfRunoff = pMet->SpoolRunoff *f_PSurfaceRunoff_Conc(pMet->SpoolRunoff);

}

void P_Mineral::Integ()

{

	// Weatering balance
	InFlow(Soil_PO4, WeatheringRate);
	OutFlow(Soil_Mineral_P, WeatheringRate);
	OutFlow(Soil_Mineral_P[0], PSurfRunoff);


	
	OutFlow(Soil_PO4, SoilPhosphateUptake);// Plant Uptake loss from soil
	OutFlow(Soil_PO4, SoilPhosphateDrainage); // Drainage loss from soil
	IntFlow(Soil_PO4, SoilPhosphateFlow);// Redistribution within soil profile



	OutFlow(Soil_PO4.back(), SoilPhosphateFlow.back()); // loss from deepest horizon
	TotDiffStorageP = sum(Soil_PO4) + sum(Soil_Mineral_P) - InitialStorageP;


	for(size_t i=0; i<SoilPhosphateDrainage.size(); i++) //AccLoss drainage
		InFlow(AccOutFlowP, SoilPhosphateDrainage[i]);

	for (size_t i = 0; i < SoilPhosphateUptake.size(); i++) //AccLoss drainage
		InFlow(AccOutFlowP, SoilPhosphateUptake[i]);


	InFlow(AccOutFlowP, PSurfRunoff);

	InFlow(AccOutFlowP, SoilPhosphateFlow.back());	// Accloss deep percolation
	InFlow(AccInflowP, P_Fert_App);
	InFlow(AccInflowP, Dep_P_DryRate);
	

	
	TotDiffStorageP = sum(Soil_PO4)+sum(Soil_Mineral_P) - InitialStorageP+P_Fert;

	PMineral_MassBalanceCheck = AccOutFlowP- AccInflowP + TotDiffStorageP ;


}

bool P_Mineral::End()
{
	return true;
}

