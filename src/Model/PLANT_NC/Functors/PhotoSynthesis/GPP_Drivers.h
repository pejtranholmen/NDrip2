#pragma once
#include "../../../../std.h"
#include "../../NC_Drivings.h"
class GPPImpact {




};
class GPP_Daily_Drivers {
public:
	GPP_Daily_Drivers(DRIVERSOnGPP Actual, size_t Growth, size_t GrowthResponseVar, ModelInfo *pModelinfo) : Growth(Growth),
		GrowthResponseVar(GrowthResponseVar), m_pModelInfo(pModelinfo) {
		GPP_Driver=GPP_Deficit = GPP_Potential = Actual; ICountDay = 0;
	};
	DRIVERSOnGPP operator() (DRIVERSOnGPP &GPP_Current, vector<double> &AtmNewMobile) {
		if (Growth_Sw(Growth) >= Growth_Sw::Radiation_use_efficiency&&
			GrowthResponseVar_Sw(GrowthResponseVar) >= GrowthResponseVar_Sw::Mean_and_Delayed) {
			bool DayShift;
			for (size_t i = 0; i < GPP_Potential.Nitrogen.size(); i++) {
				if (AtmNewMobile[i] > 0.) {
					GPP_Potential.Light[i] = GPP_Potential.Light[i] + AtmNewMobile[i] / GPP_Current.Light[i];
					GPP_Potential.Nitrogen[i] = GPP_Potential.Nitrogen[i] + AtmNewMobile[i] / GPP_Current.Nitrogen[i];
					GPP_Potential.Water[i] = GPP_Potential.Water[i] + AtmNewMobile[i] / GPP_Current.Water[i];
					GPP_Potential.Temp[i] = GPP_Potential.Temp[i] + AtmNewMobile[i] / GPP_Current.Temp[i];
					GPP_Potential.Salt[i] = GPP_Potential.Salt[i] + AtmNewMobile[i] / GPP_Current.Salt[i];
					GPP_Potential.Phosphorus[i] = GPP_Potential.Phosphorus[i] + AtmNewMobile[i] / GPP_Current.Phosphorus[i];
					
					GPP_Deficit.Light[i] = GPP_Deficit.Light[i] + AtmNewMobile[i] / GPP_Current.Light[i] * (1 - GPP_Current.Light[i]);
					GPP_Deficit.Nitrogen[i] = GPP_Deficit.Nitrogen[i] + AtmNewMobile[i] / GPP_Current.Nitrogen[i] * (1 - GPP_Current.Nitrogen[i]);
					GPP_Deficit.Water[i] = GPP_Deficit.Water[i] + AtmNewMobile[i] / GPP_Current.Water[i] * (1 - GPP_Current.Water[i]);
					GPP_Deficit.Temp[i] = GPP_Deficit.Temp[i] + AtmNewMobile[i] / GPP_Current.Temp[i] * (1 - GPP_Current.Temp[i]);
					GPP_Deficit.Salt[i] = GPP_Deficit.Salt[i] + AtmNewMobile[i] / GPP_Current.Salt[i] * (1 - GPP_Current.Salt[i]);
					GPP_Deficit.Phosphorus[i] = GPP_Deficit.Phosphorus[i] + AtmNewMobile[i] / GPP_Current.Phosphorus[i] * (1 - GPP_Current.Phosphorus[i]);
				}
			}

			if (ICountDay == m_pModelInfo->JDayNum /*|| (m_pModelInfo->JDayNum == 1 && ICountDay > 365)*/) {
				DayShift = true;
				ICountDay = size_t(m_pModelInfo->JDayNum + 1);
			}
			else {
				DayShift = false;
				if (ICountDay == 0)
					ICountDay = size_t(m_pModelInfo->JDayNum + 1);
			}
			for (size_t i = 0; i < GPP_Potential.Nitrogen.size(); i++) {
				if (GPP_Potential.Light[i] > 0.) GPP_Driver.Light[i] = 1. - GPP_Deficit.Light[i] / GPP_Potential.Light[i];
				if (GPP_Potential.Nitrogen[i] > 0.) GPP_Driver.Nitrogen[i] = 1. - GPP_Deficit.Nitrogen[i] / GPP_Potential.Nitrogen[i];
				if (GPP_Potential.Water[i] > 0.) GPP_Driver.Water[i] = max(0., min(1., 1. - GPP_Deficit.Water[i] / GPP_Potential.Water[i]));
				if (GPP_Potential.Temp[i] > 0.) GPP_Driver.Temp[i] = 1. - GPP_Deficit.Temp[i] / GPP_Potential.Temp[i];
				if (GPP_Potential.Salt[i] > 0.) GPP_Driver.Salt[i] = 1. - GPP_Deficit.Salt[i] / GPP_Potential.Salt[i];
				if (GPP_Potential.Phosphorus[i] > 0.) GPP_Driver.Phosphorus[i] = 1. - GPP_Deficit.Phosphorus[i] / GPP_Potential.Phosphorus[i];
				if (GPP_Potential.Nutrient_Water[i] > 0.) GPP_Driver.Nutrient_Water[i] = 1. - GPP_Deficit.Nutrient_Water[i] / GPP_Potential.Nutrient_Water[i];
			}

			for (size_t i = 0; i < GPP_Potential.Nitrogen.size(); i++) {
				if (DayShift) {
					GPP_Potential.Light[i] = 0.;
					GPP_Potential.Nitrogen[i] = 0.;
					GPP_Potential.Water[i] = 0.;
					GPP_Potential.Temp[i] = 0.;
					GPP_Potential.Salt[i] = 0.;
					GPP_Potential.Phosphorus[i] = 0.;
					GPP_Potential.Nutrient_Water[i] = 0.;

					GPP_Deficit.Light[i] = 0.;
					GPP_Deficit.Nitrogen[i] = 0.;
					GPP_Deficit.Water[i] = 0.;
					GPP_Deficit.Temp[i] = 0.;
					GPP_Deficit.Salt[i] = 0.;
					GPP_Deficit.Phosphorus[i] = 0.;
					GPP_Deficit.Nutrient_Water[i] = 0.;
				}
			}
		}
		return GPP_Driver;
	};





private:

	DRIVERSOnGPP GPP_Potential, GPP_Deficit, GPP_Driver;
	size_t Growth, GrowthResponseVar;
	ModelInfo *m_pModelInfo;
	size_t ICountDay;
};



