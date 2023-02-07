#include "NC_SoilManagement.h"
#include "../SOIL/SoilModel.h"
#include "../SOIL/Soil_Frost.h"
#include "../PLANT/PlantModel.h"
#include "../../Simulator/Simulator.h"
#include "./Functors/NC_Organic_Functors.h"
void NC_SoilManagement::setNC_SoilManagement(ModelInfo* pInfo, ModelMap* pMap, Structure* pStruc)
{
	ModuleName = "Soil Management";
	ModuleNo = 30;
	ModuleConditions = "";
	m_pModelInfo = pInfo; m_pModelMap = pMap; m_pModelStructure = pStruc;
	pT_Step = &m_pModelInfo->T_Step;
	T_Step = *pT_Step;
}
NC_SoilManagement::NC_SoilManagement(void)
{
	ModuleName="Soil Management";
	ModuleNo=30;
	ModuleConditions="";
}

bool NC_SoilManagement::Ini()	{

//	Use Parameters
//Use Profile
//Use NC_Drivings, Only	:	NDrivTheta
	NC_GasFlux::Ini();
	SoilModel* p_Soil=(SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
	Soil_Frost* pSoil=&p_Soil->m_SoilFrost;
	//m_pModelStructure = ((Simulator*)m_pModelInfo)->GetSimStructurePointer();
	if(Microbes==0) RateCoefSurf_Hum=0.;
	if(LitterPools==0) RateCoefSurf_L2=0.;
	
	

	return true;
}
bool NC_SoilManagement::IniFunctors() {

	//	Use Parameters
	//Use Profile
	//Use NC_Drivings, Only	:	NDrivTheta
	NC_GasFlux::IniFunctors();



	return true;
}
bool NC_SoilManagement::Dyn()
{
	if(m_pModelStructure->Nitrogen_Carbon==0) return false;
	NC_GasFlux::Dyn();
	Flux();
	Integ();
	return true;
}

void NC_SoilManagement::Flux()
{
PlantModel* pPlant=(PlantModel*)m_pModelInfo->GetModelPtr("Plant Model");
WaterUptake* p_Plant=&pPlant->m_WaterUptake;
SoilModel* p_Soil=(SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
Soil_Frost* pSoil=&p_Soil->m_SoilFrost;


	bool OK;
	Do_Cultivating=false;



	if(int(m_pModelInfo->JDayNum)>0&&(int(m_pModelInfo->JDayNum)==int(SurfaceCultDay)||int(m_pModelInfo->JDayNum)==int(PloughingDay))) {

 		Do_Cultivating=true;
	}
	
	if(int(m_pModelInfo->JDayNum)==int(PloughingDay)&&PloughingDay>0) {
		auto GetDepthDistributionofLitterFall = [&](size_t i, size_t PlantComp, size_t PlantElement) {
			DEPTH_type out;
			out = p_Plant->f_WaterUptakeDistribution(p_NC_Plant->P_LitterFall[PlantComp][PlantElement][i], p_Plant->SimRootDepth[i]);
			return out.Dist;

		};
		//Reset all Fluxes to zero
		for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++)
			for (size_t j = 0; j < 2; j++)
				LitterInputToSoilLayers[j][jj].assign(pSoil->NL, 0.);
			//Do i=1, NumPlant
			DEPTH_type out;
			for (size_t i = 0; i < p_Plant->NumPlants; i++) {
				OK = p_NC_Plant->KillBelowGroundPlant(i);
				vector<double> LitterFallToLayers;

				for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
					for (size_t j = _ROOT; j <= _OLD_COARSE_ROOT; j++) {
						LitterFallToLayers = GetDepthDistributionofLitterFall(i, j, jj);
						if (j <= _COARSE_ROOT || LitterPools_Sw(LitterPools) == LitterPools_Sw::one) {
							for (size_t layer = 0; layer < pSoil->NL; layer++) {
								LitterInputToSoilLayers[_L1][jj][layer] += LitterFallToLayers[layer];
							}
						}
						else {
							for (size_t layer = 0; layer < pSoil->NL; layer++) {
								LitterInputToSoilLayers[_L1][jj][layer] += LitterFallToLayers[layer] * (1 - p_NC_Plant->Root_FracLitter2[i]);
								LitterInputToSoilLayers[_L2][jj][layer] += LitterFallToLayers[layer] * p_NC_Plant->Root_FracLitter2[i];
							}
						}
					}
				}
			}
  			Do_Ploughing=true;
	}
	else
			Do_Ploughing=false;
	
 
}
void NC_SoilManagement::Integ()
{
PlantModel* pPlant=(PlantModel*)m_pModelInfo->GetModelPtr("Plant Model");
WaterUptake* p_Plant=&pPlant->m_WaterUptake;
SoilModel* p_Soil=(SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
Soil_Frost* pSoil=&p_Soil->m_SoilFrost;
size_t Num;
	if(Do_Ploughing) {

		Soil_Frost* p_Soil=(Soil_Frost*)pSoil;
		DEPTH_type out;
		out=p_Soil->DepthDist(2,  1., PloughingDepth, 0.1);
		DistFrac=out.Dist;
		Num=out.NumLay;
	
		double SumCheck=sum(DistFrac)-1.0;
		if(abs(SumCheck)>1.E-8) {
			if(SumCheck>DistFrac.back()||SumCheck<-1.E-3) 
				DistFrac.back()=DistFrac.back()-SumCheck;
			else {
				for(size_t i=0; i<Num;i++) 
					DistFrac[i]/=(SumCheck+1.0);	                
			}
		}
		MixFrac(&O_State[_L1][_N_],  Num);
		MixFrac(&O_State[_L2][_N_], Num);
		MixFrac(&O_State[_H][_N_], Num);
		MixFrac(&O_State[_MIC][_N_], Num);
		MixFrac(&O_State[_F][_N_], Num);
		MixFrac(&O_State[_L1][_C_], Num);
		MixFrac(&O_State[_L2][_C_], Num);
		MixFrac(&O_State[_H][_C_], Num);
		MixFrac(&O_State[_MIC][_C_], Num);
		MixFrac(&O_State[_F][_C_], Num);
	}
}

bool NC_SoilManagement::End()
{
return true;
}
NC_SoilManagement::~NC_SoilManagement(void)
{
}

