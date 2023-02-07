#pragma once
#include "./Salt.h"
class WaterUptake : public Salt
{
	public:
		void setWaterUptake(ModelInfo* pInfo, ModelMap* pMap, Structure* pStruc);
		WaterUptake(void);
		~WaterUptake(void);
		bool Def();
		bool Ini();
		bool IniFunctors();
		bool Dyn();
		void Flux();
		void Integ();
		bool End();

		double	WupCri,	WupF, WupFB, AirMinContent, AirRedCoef, DripScaleWUpt;

		vector<double> v_WupCri, v_WupF, v_WupFB, v_AirMinContent, v_AirRedCoef;
		
		double	WupATe, WupBTe, WupCTe, WupDTe, WupETe, WupFTe, WupTrigTemp, WupDayOff;
		vector<double>  v_WupATe, v_WupBTe, v_WupCTe, v_WupDTe, v_WupETe, v_WupFTe, v_WupTrigTemp, v_WupDayOff;
		double	UpMov;

		vector<double> WUptakeRate;




		size_t Salt_Influence, PlantResistance, SaltCoefValid, WupTempResponse,  WUptake;
		size_t PlantWaterStorage;
		size_t MultiPlantWaterUptakeFunction;
		vector<double> Transpiration, PlantWater, PlantWaterUptake, PlantWaterPotential;
		vector<double> RedWupPsi, RedWupTemp, RedWupTotal, Resist_Plant, Resist_SoilPlant;

		vector<P*> mTab_Response; //Salt Response Parameters
		vector<P*> mTab_WaterResponse; //Salt Response Parameters

		double RedWupTotal_All, Transpiration_All, RootDistMax, RootDistMin, RootDensityCoef;
		double XylemResistivity, RootResistivity, PlantMaxSuction, LeafThresholdSuction;
		double PlantWatRelLAI, PlantWatRelLAI_Height;
		double Upt_Excess;

		//Local Variables
		vector<double> WTemp, ThetaWetCrit, RedWupPsiDepth, RedGrowthSalt;

		double RedWupTempLocal, SaltAccLayers, WupCri_Initial;

		// Functors
		function<DEPTH_type(double, double)> f_WaterUptakeDistribution;

		function<double()> f_WaterUptakeTemp_No;
		function<double(double, double)> f_WaterUptakeTemp;
		function<double(size_t, double, double)> f_MultiWaterUptakeTemp;

		function<double(double, double, double)> f_WaterUptakeTemp_DynTrig;
		function<double(size_t, double, double, double)> f_MultiWaterUptakeTemp_DynTrig;

		function<double(size_t, double, double, double)> f_WaterUptakeMoisture;
		function<double(size_t, size_t, double, double, double)> f_MultiWaterUptakeMoisture;
		function<double(size_t, double)> f_WaterUptakeSalt;

		function<double(double)> f_WaterFlowDistanceInSoil, f_MaxStorage_LAI, f_RelativeStomata_PlantPotential;
		function<double(double, double)> f_PlantResistance, f_MaxStorage_LAI_Height;


		// Local Functions
private:
	double WaterUptakeMoistureFuncL(size_t index, size_t IndexP, double PotWaterUptake);
	double RhizoSphereResistanceFuncL(size_t Index, size_t IndexP);
	double RootDistFuncL(size_t Index, size_t IndexP);
	double PlantResistanceFuncL(size_t Index, size_t IndexP);
	double MaxPlantWaterStorage(size_t IndexP);
	double WaterUptakeSaltFuncL(size_t Index, size_t IndexPlant);
	double MaxUptakeRate(size_t Index, size_t IndexP);
	double PlantWaterPotentialFuncL(size_t Index);


};
