#pragma once
#include "../SimModule.h"
#include "../SOIL/Soil_Frost.h"
#include "../PLANT/WaterUptake.h"
#include "../Atmosphere/Irrigation.h"


class NC_Drivings_Functions : public SimModule
{
	public:
		NC_Drivings_Functions(void);
		~NC_Drivings_Functions(void);
		bool Def();
	
 Soil_Frost *pSoil; // Soil Water Flow module
 WaterUptake *pPlant;
 Irrigation *pAtm;
 SimModule *pHeat;

//Integer IUnit



 
size_t SoilTempInput,SoilWaterInput,SoilWaterFlowInput;	//	! Nitrogen Switches
size_t WaterStressInput, SoilInfilInput, SoilDrainageInput;							// Nitrogen Switches
size_t DeepPercolationInput;//										! Nitrogen Switches
//! Parameter Values
double SoilTemperature, SoilWaterContent, SoilWaterFlow, SoilDrainage;
double SoilInfiltration, PlantWaterStress, DeepPercolation, WindSpeed;
double GreenIndexDriver;

//! New Driving variables for Nitrogen & Carbon
vector<double>		NDrivTheta,NDrivWaterFlow,NDrivDrainage,NDrivWaterStress, NDrivSaltStress;
vector <double>		NDrivTemperature;
double NDrivInfiltration, NDrivDeepPercolation, NDrivTAir;

//! Accumulators for arrays to Driving variables for Nitrogen & Carbon
vector<double> AccNDrivTheta ,AccNDrivWaterFlow, AccNDrivDrainage;
vector<double> AccNDrivTemperature, AccNDrivTranspiration, NDrivTranspiration, AccNDrivWaterStress, AccNDrivSaltStress;

//! Accumulators to Driving variables for Nitrogen & Carbon

double	 AccNDrivInfiltration, AccNDrivDeepPercolation;
double	NDrivRadInShort,AccNDrivRadInShort, AccNDrivTair;

size_t	NumAccNDriv;


size_t GreenIndexForcing;

bool	Extracted;


CPG m_PG_Abiotic, m_PG_GreenIndex;
PGPOS m_PGPos_Abiotic, m_PGPos_GreenIndex;
ININFO TempIpos, ThetaIpos, WaterFIpos, DrainageIpos, InfiltrationIpos, StressIpos, DeepPercIpos;
ININFO GreenIndexIpos;

//!	Positions in 2nd Driving variable file

bool DrivPG2, DrivNPG1;
//Real, Dimension(:), Allocatable :: NDrivEcoDat	! Driving variables
bool CheckEcoDriv3();
void NC_Drivings_Update(); 

};

