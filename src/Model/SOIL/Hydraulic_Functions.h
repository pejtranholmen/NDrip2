#pragma once

#include "./Profile.h"


class Hydraulic_Functions : public Profile
{
	public:
		Hydraulic_Functions(void);
		~Hydraulic_Functions(void);

		bool Def();
		bool FunctionDef();

		 SimModule *pHeat;
		 SimModule *pPlant;
		 SimModule *pAtm;
		bool ReCalc();	 

		double ThetaFlow(size_t i, double Flow, double Thickness); 

	
		double PsiFunc_Derivate(size_t ipar, double Theta);


		double WCondB_Derivate(size_t ipar, double Theta, double temp=20.);


		double EvaRateMax(double);


		void AdjustMatricConductivity();
		//	Viscosity_Temp_Func Viscosity_Func;





		size_t	HydraulicFunc, ConductivityFunc, MatricCondFunc, ScalingRetention;
		size_t PedoFunction, Replace_MissingLowCond;

/*		Ps CommonConductivityValue_t, SensAboveThreshold_t;
		Ps ScaleCoefResidual_t, SaturationDiff_t;
		Ps A0Temp_t, A1Temp_t, CMinUC_t;
		Ps FrCondImped_t, HighFlowDampC_t; // Define in Frost
		Ps FixedResidualPar_t;
  */
		double CommonConductivityValue, SensAboveThreshold;
		double ScaleCoefResidual, SaturationDiff;
		double A0Temp, A1Temp, CMinUC;
		double FrCondImped, HighFlowDampC; // Define in Frost
		double FixedResidualPar;


	// Measured Layer Properties	
/*		P UpperDepth_t, LowerDepth_t, Lambda_t;
		P PsiEntry_t, Saturation_t, Wilting_t, ResWater_t, MacroPore_t;
		P UpperBoundary_t, UpperBoundaryTheta_t;
		P ClayContent_t, SandContent_t, ProfileNo_t, ProfileRepNo_t, ProfileLayer_t;
		P ScalePsi_t;
		P ValidRangeUpper_t, ValidRangeLower_t, RegLine_R2_t,RegLine_N_t;
		P SatMatCond_t, SatTotCond_t, NTortuosity_t, NSR_t, NSE_t;
		P Galpha_t, GN_t, GM_t;	   */
	//	size_t NoLayerM;
 		vector<double> Lambda;
		vector<double> PsiEntry, Saturation, Wilting, ResWater, MacroPore;
		vector<double> UpperBoundary, UpperBoundaryTheta;
		vector<double> ProfileNo, ProfileRepNo, ProfileLayer;
		vector<double> ScalePsi;
		vector<double> ValidRangeUpper, ValidRangeLower, RegLine_R2,RegLine_N;
		vector<double> SatMatCond, SatTotCond, NTortuosity, NSR, NSE;
		vector<double> Galpha, GN, GM;
	// Boundary Layer Properties
/*		P B_Lambda_t,  B_PsiEntry_t, B_Saturation_t,   B_Wilting_t, B_ResWater_t, B_MacroPore_t;
		P B_UpperBPsi_t,  B_UpperBTheta_t, B_ScalePsi_t, B_SatMatCond_t, B_SatTotCond_t;
		P B_NTortuosity_t,	B_NSR_t, B_NSE_t;
		P B_Galpha_t, B_GN_t, B_GM_t;  */
  		vector<double> B_Lambda,  B_PsiEntry, B_Saturation,   B_Wilting, B_ResWater, B_MacroPore;
		vector<double> B_UpperBPsi,  B_UpperBTheta, B_ScalePsi, B_SatMatCond, B_SatTotCond;
		vector<double> B_NTortuosity,	B_NSR, B_NSE;
		vector<double> B_Galpha, B_GN, B_GM;
	
 	//Middle layer properties (For calculation of water retention
	//	P M_Lambda_t,  M_PsiEntry_t, M_Saturation_t,   M_Wilting_t, M_ResWater_t, M_MacroPore_t;
	/*	P M_UpperBPsi_t,  M_UpperBTheta_t, M_ScalePsi_t, M_SatMatCond_t, M_SatTotCond_t;
		P M_NTortuosity_t,	M_NSR_t, M_NSE_t;
  		P M_Galpha_t, M_GN_t, M_GM_t;	*/
		vector<double> M_Lambda,  M_PsiEntry, M_Saturation,   M_Wilting, M_ResWater, M_MacroPore;
		vector<double> M_UpperBPsi,  M_UpperBTheta, M_ScalePsi, M_SatMatCond, M_SatTotCond;
		vector<double> M_NTortuosity,	M_NSR, M_NSE;
  		vector<double> M_Galpha, M_GN, M_GM;

		function<double(size_t, double)> f_PsiTheta_M, f_PsiTheta_B, f_PsiTheta_Measured, f_ThetaPsi_Measured, f_ThetaPsi_M, f_ThetaPsi_B;
		function<double(size_t, double, double)> f_CondTheta_Measured, f_CondTheta, f_CondPsi, f_CondPsi_Measured;
		function<double(bool, size_t, double, double)> f_CondThetaMatric;
		function<double(size_t, double, double, double)> f_CondThetaFrozen_Measured, f_CondThetaFrozen;
		function<double(size_t)> f_CondSaturated;
		function<double(double)> f_CondTempFactor;
		function<double(double)> f_CondHighFlowDampFactor;
		function<double(double)> f_MatricCondEstimate;
		function<double(double)> f_MaxEvaporationRate_Psi;
		function<double(size_t, double, double)> f_ThetafromWaterFlow;

		vector<P*> mTab_BC_Measured, mTab_Pedo, mTab_HC_Measured,  mTab_Gen_Measured;


 
};

