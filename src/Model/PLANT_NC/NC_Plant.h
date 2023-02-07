#pragma once

#include "./NC_Plant_Functions.h"
class NC_Plant : public NC_Plant_Functions
{
	public:
		void setNC_Plant(ModelInfo* pInfo, ModelMap* pMap, Structure* pStruc);
		NC_Plant(void);
		~NC_Plant(void);
		bool Ini();
		bool IniFunctors();
		bool Dyn();
		void Flux();
		void Integ();
		bool End();

		void PlantStage_CNRatio_PhysSize();
		void Light_And_PhotoSynthesis();
		void AllocationAndReAllocation_DuringGrowth();
		void AboveGroundPlantRespiration();
		void BelowGrandPlantRespiration();
		void LitterFall();
		void MobilePool();
		void Cleaning_Harvest_Events();
		void Grazing_Events();
		void WoodEatingProcess();
		void Seedling_Management_Events();
		void At_Harvest(bool WithGrazingRelatedEvent=false);
		void At_Cleaning();

		void MineralOrganic_Uptake();
		void At_YearShifting();
		void TraceElementUptake();
		void Summation_Fluxes();

	private:
		vector<size_t> SoilLayerForLitterInput;


};
