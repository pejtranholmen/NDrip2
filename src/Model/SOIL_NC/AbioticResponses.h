#pragma once

#include "./P_ExternalInputs.h"
class AbioticResponses : public P_ExternalInputs
{
	public:
		AbioticResponses(void);
		~AbioticResponses(void);
		bool Ini();
		bool IniFunctors();
		bool Ini_Responses();
		bool Dyn();
		void Flux();
		void Integ();
		bool End();
		bool Def();
	bool FunctionDef();


	double RespTemQ10, RespTemQ10Bas, RespTemQ10Threshold;
	double DenitThetaRange, DenitThetaPowerC;
	double DenitTemQ10, DenitTemQ10Bas, DenitTemQ10Threshold;
	double DenitTempMax, DenitTempMin;
	double DenitNitrateHalfSat;
	size_t Denit_Temp_Response;
	size_t Nitrific_Temp_Response;

	size_t Common_Temp_Response;
	size_t Common_Moisture_Response, CO2_SoilModel;  //moved CO2_SoilModel from NC_GasFlux
	vector<double> CO2_Conc;						// moved CO2_Conc from NC_GasFlux

	double NitriTemQ10, NitriTemQ10Bas, NitriTemQ10Threshold;
	double NitriTempMax, NitriTempMin;

	//!	General abiotic responses
	double SaturationActivity, ThetaUpperRange, ThetaLowerRange, ThetaPowerCoef;

	// New CO2 Response
	double CO2CutOffThreshold, CO2LowestImpact;

	double CommonTemQ10, CommonTemQ10Bas, CommonTemQ10Threshold;
	double CommonTempMax, CommonTempMin;
	double CommonNeillForm, CommonNeillOpt, CommonNeillMax;

	double CommonArrheniusActWea, CommonArrheniusTemp0;
	Dec_Response Response;
	// ResponseTheta, ResponseTemp_Theta;

	//double  ResponseLitter, ResponseHumus;
	function<double(double)> f_CommonTempResponse, f_CO2_Response;
	function<double(double)> f_DenitTempFunction;
	function<double(double)> f_RespTempFunction;

	function<double(size_t, double)> f_Theta_Response, f_DenitThetaFunction;
	function<double(size_t, double, double)> f_CombinedThetaCO2_Response;



};
