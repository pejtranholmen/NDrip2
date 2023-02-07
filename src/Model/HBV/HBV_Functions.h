#pragma once
#include <string>
//#include "../../Base\ModelType.h"
//#include "../NewModelType.h" 
#include "../SimModule.h"	

#include "../../ModelTypes/SimB.h"	

class Irrigation;
class WaterUptake;
class HBV_Functions : public SimModule
{
	public:
		HBV_Functions(void);
		~HBV_Functions(void);
		bool Def();

		//Parameters
  		double Initial_BaseStorage, Initial_PeakStorage, Initial_SoilStorage;
		double FieldCapacity;
		double DisCoefk1, DisCoefk2, DisCoefAlfa, MaxPerc;
		double CriticalUptakeFrac, BetaCoef;
		//State Variables
		double SoilStorage, PeakStorage, BaseStorage;
		// Flow Variables
		double HBV_Input, HBV_Input_Soil, HBV_Input_GW, HBV_GW_Redist;
		double Q_Base, Q_Peak, Q, HBV_Evaporation;	
		double HBV_GWLev, Min_GWLev, Max_GWLev, Sens_GWLev, PowerCoef_GWLev;


		function<double(double, double)> f_ActEvaporation; //ActualEvaporation
		function<double(double, double, double)> f_Infiltration;
		function<double(double)>  f_PeakRunOff, f_BaseRunOff, f_GroundWaterLevel, f_GroundWaterDistribution;



		Irrigation* m_pMet;
		WaterUptake* m_pPlant;



};

