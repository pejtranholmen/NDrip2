#pragma once
#include "./Soil_Drainage.h"
class Soil_WaterF_Functions : public Soil_Drainage
{
	public:
		Soil_WaterF_Functions(void);
		~Soil_WaterF_Functions(void);
		bool Def();

		size_t Crack,	InitialWater, Hysteresis;
		size_t ConvectiveGasFlow, TransitTime, SoilWaterBarrier;

		// From Frost
		size_t FrostKUpward, FrostInteract;
		double VapourFlowSurf, SurfaceOutFlow;
		vector <double>	VapourFlow, WBypassFlow;
		vector <double> ThetaTotal, HysEffect, MeanTransitTime;
		vector <double> WaterFlow;
		vector <double>  ThetaB;

  		double TotMeanTransitTime;
//!Parameters Ps
	double	DVapTortuosity;
	double IniFlowRate, IniPsiValue, IniThetaValue, IniGWLev;
	double AScaleSorp;
// P
	vector<double> IniPsiValues, IniThetaValues;
	vector<double> HysMaxEffRet, HysMaxEffCond;

	vector<P*> mTab_InitP, mTab_InitTheta, mTab_Hyster;
//Ps
	double BarrierEfficiency, BarrierLevel;

	double HysThetaD, HysThetaMax, HysPF1, HysPF2, HysKExp;
//
	vector<double> TposShift, THETAO, ThetaDer, A_PsiSh, A_Age, A_Der, AccDer;
	vector<size_t>		Icount, IcountN;

	vector <double>  AccWaterFlow, AccWaterStorage;

// Local variables for Convective Air flow
	vector<double>  MassAir, FluxAir;
	double	FluxAirInfil;
//!	TransitTime
	double SumTransit, SumWaterFlow;



	double MassAirFuncL(size_t index, double temp);
	void ThetaMMM();
	void Soil_WaterVerticalSaturated_Dyn();
	double Hys_Eff_Func(size_t i);
	void WaterVapourF_Dyn();
	void ConvPot();
	bool Frost;

};

