#pragma once


class AbioticResponses_Functions : public NC_ExternalInputs
{
	public:
		AbioticResponses_Functions(void);
		~AbioticResponses_Functions(void);

	
 // Soil Water Flow module

//Integer IUnit




	double GetFunctionValue(size_t option, double x1);
	double GetFunctionValue(size_t option, size_t index, double x1);

	double CommonThetaFunction(size_t Layer, double Theta);
	double CommonThetaFunctionL(size_t Layer);
	double CommonThetaDryResponse(size_t Layer, double Theta);
	double CommonCO2ResponseFunction(double CO2_Conc);  // Added to make new response on decompoistion substituting "CommonThetaFunction"
	double CommonTempFunction(double Temp);
	double CommonTempFunctionL(size_t layer);
	double DenitTempFunctionL(size_t Layer);
	double DenitTempFunction(double Temp);
	double DenitThetaFunctionL(size_t Layer);
	double DenitThetaFunction(double Theta, size_t Layer);
	double NitriTempFunctionL(size_t Layer);
	double NitriTempFunction(double Temp);
	double RespTempFunctionL(size_t Layer);
	double RespTempFunctionAir();
	double RespTempFunction(double Temp);


};

