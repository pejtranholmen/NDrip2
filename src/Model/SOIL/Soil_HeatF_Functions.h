#pragma once

#include "Soil_WaterF.h"
#include "./Thermal.h"
class WaterUptake;

class Soil_HeatF_Functions : public Thermal
{
	public:
		Soil_HeatF_Functions(void);
		~Soil_HeatF_Functions(void);

		bool Def();
		
		bool FunctionDef();
		bool  GetFunctionValue(size_t option);
		double GetFunctionValue(size_t option, double x1);
		double GetFunctionValue(size_t option,  double x1, double x2);
		double GetFunctionValue(size_t option,  double x1, double x2, double x3);
		double GetFunctionValue(size_t option,  double x1, double x2, double x3, double x4);
		double GetFunctionValue(size_t option, size_t index, double x1);
		double GetFunctionValue(size_t option, size_t index, double x1, double x2);
		double GetFunctionValue(size_t option, size_t index, double x1, double x2, double x3);
		double GetFunctionValue(size_t option, size_t index, double x1, double x2, double x3, double x4);

 size_t HeatPump;
 size_t  SoilHeatConv,  HeatVapourF,  InitialHeat,	HeatLowerB ,PrecTemperature, Analytical_Solution, HeatSource, ThCondOutput;
 size_t SingleCableControl;
 WaterUptake* p_Plant;


 double SoilInitTemp, GeothermalF ,TempDiffPrec_Air, SurfHeatFlow,TotalGroundSensFlow,TotalGroundLatFlow;
 double TempSoilSurf;
 vector <double> SoilInitTempVar, SoilHeatInitial, ExactTemperature,HeatProdCoef_A,HeatProdCoef_A2;
 double HeatProdCoef_B,  HeatProdCoef_AA, HeatProdCoef_AA2, HeatProd_Threshold, HeatSourceRateCoef1, HeatSourceRateCoef2;
 double HeatConversionCoef;
 vector <double> ThCondV, Temp;


 vector <double> SoilHeat, SoilHeatFlow, SoilHeatSink, SoilHeatSource;
 
 //! Water Box Heat
 size_t InsulatedWaterPipe, WaterPipeBox, NPS, NHB;
 
 vector <double>  HeatBoxBoundaryFlux, HeatBoxStorage, HeatBoxHeaterFlux, HeatBoxTemp, WaterPipeLayer;
 double  ThCondInsulator,  HeaterFluxRate, HeaterTempThreshold, HeaterDuration;
 vector <double> ColdPipeHeat, OutPipeHeat, ColdPipeFlux,ColdPipeSinkFlux, OutPipeFlux, OutPipeSinkFlux;
 vector <double> ColdPipeTemp, OutPipeTemp, PipeSegments;
 double SewageHeatStore, SewageSinkFlux, SewageInflow, SewageOutflow, SewageWaterStore;
 double SewageWaterIn, SewageWaterOut, OutPipeOutFlux ,  ColdPipeInFlux,  WaterPipeFlux, WaterPipeInTemp;
 double WaterSewageBuffer, WaterSewageInputAdd, HeatPipeResistance, WaterPipeFluxFluc, HeatBoxEnhancer;
 double SewageTemp,   HeatSewageResistance, 	TravelDistance, HeatBoxReturnFlow;

 double AgeofSourceAtStart;
 
 
// Heat Cable based on Magnus
 
 
double   StartHCableDay, OptHCableDay, EndHCableDay, ShapeOnHeatCable, ShapeEndHeatCable;
double    HeatCableOptValue, HeatCableLayer, HeatCableDelay;

double HighTempControl, LowTempControl, HighTempCooling, LowTempCooling;
 


//!	Local variable
 double Ycycle;
 size_t IntegerWaterPipeLayer;
 double WaterPipeVolume, HeatCapacityPipeWater;
 bool EmptyContainer;
 bool m_HeatCablePower;

 double m_SurfHeatFlowSnow;

	double SoilHeatCableFunction();
	double RegulatedHeatCableAirTempFunction(double TempSurf, double TAir);
	double RegulatedHeatCableDewPointTempFunction(double TempSurf, double VapourPressureAir);
	double HeatSourceAgeingFunction(double Days, double DecayCoef);
	double HeatSourceAgeingFunctionL(size_t index);
	double SoilHeatSourceFunc(double Temp);
	double SoilHeatSourceFuncL2(size_t index, double Temp);
	double SoilHeatSourceFuncL(size_t Index, double Temp);
	double SurfaceVapourFlow(double Temp);

	vector<P*> mTab_HeatSource, mTab_InitTemp, mTab_InitHeat,mTab_PipeL,  mTab_PipeBox;


};

