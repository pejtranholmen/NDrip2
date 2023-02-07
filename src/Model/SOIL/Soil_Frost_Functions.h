#pragma once
#include "./Soil_WaterF.h"
class Soil_Frost_Functions : public Soil_WaterF
{
	public:
		Soil_Frost_Functions(void);
		~Soil_Frost_Functions(void);
		bool Def();
		bool FunctionDef();
		bool  GetFunctionValue(size_t option);
		double GetFunctionValue(size_t option, double x1);
		double GetFunctionValue(size_t option, double x1, double x2);
		double GetFunctionValue(size_t option, double x1, double x2, double x3);
		double GetFunctionValue(size_t option, double x1, double x2, double x3, double x4);
		double GetFunctionValue(size_t option, size_t index, double x1);
		double GetFunctionValue(size_t option, size_t index, double x1, double x2);
		double GetFunctionValue(size_t option, size_t index, double x1, double x2, double x3);
		double GetFunctionValue(size_t option, size_t index, double x1, double x2, double x3, double x4);


	double FpointF0, FpointF1,FpointFWi;
	double FreezingStartTemp, OsmoticScaleSens;

	vector<double>		 WaterHFD;		//

	vector<double> Infreeze, WaterflowHFD, WaterflowHD_LD, WaterflowLD_HD;

	size_t	 LoadPotential, FrostInfil, FrostSwelling, FrFlowDomain; //	! Previous FRLOADP
	size_t  FreezingPointOptions;
   
	double	AlphaHT, MaxSwell, ShrinkRateFraction;			


	double FrostUpperB_1, FrostLowerB_1, Swelling, FrostUpperB_2, FrostLowerB_2;


	bool OBOUNDF;
	size_t NL_Swell;
    bool Frozen,  Shift, OldFrost;	//! state of current layer

	size_t NL_Frost;
	
	double InfilHighDomain;
	
	double Swells();
	vector<double> Refreezing();
	void WaterFlowFrost();
	double HeatTransFunction(double Temp);
	double ShrinkStage(double Days);	
	void FrostBoundaries(vector<double>, double);

};

