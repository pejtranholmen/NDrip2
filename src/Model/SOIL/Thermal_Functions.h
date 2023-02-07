#pragma once

#include "Soil_Frost.h"
#include "../Atmosphere/Irrigation.h"
#include "../SimModule.h"
#include "./ThermalCoef.h"
#include "../SOIL_NC/AbioticResponses.h"
#include "../PLANT_NC/NC_Drivings.h"
#include "../SOIL_NC/NC_SoilManagement.h"
#include "./Functors/Thermal_Functors.h"
/*
struct KERSTEN_COEF {
	double UC1, UC2, UC3;
	double FC1, FC2, FC3, FC4;
};*/

struct FrozenCond {
	double Temp;
	double ThQual;
};
//#include "../Structure\Structure.h"


class Thermal_Functions : public SimModule, public Thermal_Coef
{
	public:
		Thermal_Functions(void);
		~Thermal_Functions(void);

		bool Def();
		bool FunctionDef();
		bool Thermal_ReCalc() noexcept;


		Soil_Frost *p_Soil_WaterF; // Soil Water Flow module
		Irrigation *p_Atm; // Atm Module
		NC_Drivings *p_NCDrivings; // NC drivings
		NC_SoilManagement *p_NC_Soil; // NC drivings
		void *p_Struc; // NC drivings

		size_t ThCondFunction;
		KERSTEN_COEF Clay, Sand;
		double	OrganicLayerThickness,	CHumuS1, CHumuS2,CFrozenHumus,	CFrozenSurfCorr;
		double	CFrozenMaxDamp;



		double Th_Alfa, Th_Beta, Th_a;
		vector<double>	ThScale, ThScaleLog, K_IndType, BulkDenS, HeatCapSolid,	HeatCapSolidDist, ThQual;
		vector<double> m_Quarts, m_SandFrac, m_GravelFrac, m_OrgFrac;
	


		vector<P*> mTab_ThScale, mTab_HeatCap;
	

size_t SolidHeatCapDist;


// Local defined variables
double DAMPDEPTH;
size_t	HumusMixedBoundaryId;	//! (Former IHUM)
double  FractionOfOrgMixedBoundary;//	 ! (Former VIKTH)
double ResIntegDepth;		//! The depth for which resitance is calculated when
						//! using daily time step with energy balance approach
						//! Value is taken close to 0.2 meter depending on layer
						//! Thickness
double m_HeatCapSoilWater, m_HeatCapSoilIceMixture, m_FreezingStartTemperature;

	

		double SurfFlowDamp( double TempSurf);
	 	double DampDepthYear(double Thetav);
		double DampDepthDay(double Thetav);
		double SoilHeatCap(size_t index, double Theta, double ThQual);
		double SoilHeat_TempF(size_t index, double Temp, double Wat);
		FrozenCond Temp_SoilHeatF(size_t index, double SoilHeat, double Wat) ;
		double NonFrozenFuncL(size_t index, double MassWater);
		double NonFrozenFunc(double WiltingPoint, double ThicknessLayer, double MassWater);
		FrozenCond SoilThermalQualityFuncL(size_t index , double HeatDiff, double HeatCompleteFrozen, double MazFrozenMassWater, double MassWater);
		FrozenCond SoilThermalQualityFunc(double HeatDiff, double HeatCompleteFrozen, double HeatMaxIce, double HeatCompleteIce, double lambda);
		double FreezingStartFunction(double OsmoticPotential);
		double Soil_Temp_HeatDemoFunc(double Heat);



		function<void()> f_ReCalc;
		function<double(size_t, double, double)> f_ThermalCond;





};


