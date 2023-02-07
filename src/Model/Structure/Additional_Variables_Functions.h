#pragma once
#include "./Technical.h"

class ZadokShift2 {
public: ZadokShift2(vector<double>& z, vector<double>& k) :zadokindex(z), zadokStages(k) {
	previndex.push_back(1.); act_stage = 0;

};

	  void operator ()(double dayofYear, vector<double>& zadokvalues) {
		  if (previndex[0] < zadokStages[act_stage] && zadokindex[0] >= zadokStages[act_stage]) {
			  zadokvalues[act_stage] = dayofYear;
			  if (act_stage < zadokStages.size() - 1) act_stage++;
		  }
		  previndex[0] = zadokindex[0];
		  if (zadokindex[0] <= 1) {
			  act_stage = 0;
			  for (size_t i = 0; i < zadokvalues.size(); i++) {
				  zadokvalues[i] = 400;
			  }


		  }
	  };

private:
	vector<double> zadokStages;
	vector<double>& zadokindex, previndex;
	size_t act_stage;
};


struct SOIL_SENSORS {
		double UpperWeight1;
		double UpperWeight2;
		double LowerWeight1;
		double LowerWeight2;
		size_t UpperLayer1;
		size_t UpperLayer2;
		size_t LowerLayer1;
		size_t LowerLayer2;
};
struct ORG_PROBES
{  double Carbon;
	double Nitrogen;
};

class NC_SoilManagement;
class Additional_Variables_Functions : public Technical
{
	public:
		Additional_Variables_Functions(void);
		~Additional_Variables_Functions(void);
		bool Def();
		double GetTotalWaterStorage();
		double GetWaterBalanceCheck();
		ORG_PROBES Get_Org_SondStorage(size_t i);
		double  GetSondStorage(size_t i, vector<double> *values);



		size_t NSonds, ResetAccValues, NTempSensors, NThetaSensors;
		size_t NZadokDays;
		size_t Soil_TempSensors, Soil_ThetaSensors,IGBP_LandID_Selector ;
		size_t Multi_PGFile_Var_Selector, Multi_PGFile_ID_Selector;

		function<void(double, vector<double>&)> f_ZadokShifts;


	vector<double> SondDepth, AccWStorage, AccSStorage, AccRStorage;
	vector<double> ZadokDayValues, ZadokDays;
	vector<double>  AccMinNStorage, AccNH4Storage, AccNO3Storage;
	vector<double>  AccCSoilStorage,AccNSoilStorage;
    vector<double>  TempSensorLow, TempSensorHigh, ThetaSensorLow, ThetaSensorHigh;
    vector<double>  ThetaSensors, TempSensors;

	vector<P*> mTab_Temp, mTab_Theta, mTab_Sonds, mTab_Zadoks;
	double  Evapotranspiration, X_coordinate, Y_coordinate, Z_coordinate, Scale;
	double Land_ID;
	double SeqNo_MetFile, IDNo_MetFile;
	double TotalLatentFlow;
	double AccTotalRunoff, AccSurfaceRunoff, TotalDiffStorage;
	double AccSoilEvap, AccSoilEvapAndTransp, AccTotalEvap, WaterBalanceCheck;
	double AccPrecipitation, AccPrecandIrrig, AccPrecAdjustSnow;
	double AccTotalWaterInput, AccSpoolRunOn, AccLateralInflowWater;
	double AccThroughfall;
	double TotalSensibleFlow;
	double InitialTotalStorage;	//! Local variable in module
	double LateralInflowWater;//	! Local variable in module
	double PlantWaterStorage;

   double TDiff, TDiffC;// !Local variables
   vector<double> MC_TDiff, MC_TDiffC;
   vector<SOIL_SENSORS> TempSond, ThetaSond;
   /*Real, Dimension, Allocatable :: T_UpperWeight1,T_UpperWeight2,T_LowerWeight1, T_LowerWeight2
   Integer, Dimension , Allocatable :: T_UpperLayer1, T_UpperLayer2, T_LowerLayer1, T_LowerLayer2

   Real, Dimension, Allocatable :: Theta_UpperWeight1,Theta_UpperWeight2,Theta_LowerWeight1, Theta_LowerWeight2
   Integer, Dimension , Allocatable :: Theta_UpperLayer1, Theta_UpperLayer2, Theta_LowerLayer1, Theta_LowerLayer2

	*/	

   vector<double> GetSensorValues(vector<double> Value, vector<SOIL_SENSORS> *Sens);
   NC_SoilManagement *m_pNCSoil;

};

