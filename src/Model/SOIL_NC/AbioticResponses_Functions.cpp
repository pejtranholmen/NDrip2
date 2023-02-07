#include "AbioticResponses_Functions.h"



AbioticResponses_Functions::AbioticResponses_Functions(void)
{
	Denit_Temp_Response=0;
	DenitTempMax=20;
	DenitTemQ10Bas=20;
	DenitTemQ10=20;
}
AbioticResponses_Functions::~AbioticResponses_Functions(void)
{
}




double AbioticResponses_Functions::GetFunctionValue(size_t opt, double x1) {
	return (*this.*m_fpointer0_1.at(opt))(x1);
}
double AbioticResponses_Functions::GetFunctionValue(size_t opt, size_t index, double x1) {
	return (*this.*m_fpointer1_1.at(opt))(index, x1);
}
double AbioticResponses_Functions::CommonTempFunction(double Temp) {
	double response=1;

	if(Common_Temp_Response<=1) {
		response=pow(CommonTemQ10,(Temp-CommonTemQ10Bas)/10.);
		if(Common_Temp_Response==1&&Temp<CommonTemQ10Threshold) {
			if(Temp>0) 
				response=Temp/CommonTemQ10Threshold*response;
			else
				response=0.;
		}
	}
	else if(Common_Temp_Response==2) { // Then	!Ratkowsky function
		if(Temp>CommonTempMax) 
			response=1.;
		else if(Temp<CommonTempMin) 
			response=0.;
		else
			response=pow((Temp-CommonTempMin)/(CommonTempMax-CommonTempMin),2.);
	
	}
	else {
	    if(Temp>CommonNeillMax)
            response=0.;
        else
            response=pow((CommonNeillMax-Temp)/(CommonNeillMax-CommonNeillOpt),CommonNeillForm)
				*exp(CommonNeillForm*(Temp-CommonNeillOpt)/(CommonNeillMax-CommonNeillOpt));       
        

	}
	return response;
}
double AbioticResponses_Functions::CommonTempFunctionL(size_t layer) {

	return CommonTempFunction(p_NC_Plant->NDrivTemperature[layer]);
}

double AbioticResponses_Functions::DenitTempFunctionL(size_t Layer) {
//Use NC_Drivings, only	: NDrivTemperature
//	Integer Layer
	if(Denit_Temp_Response==0) 
		return CommonTempFunction(p_NC_Plant->NDrivTemperature[Layer]);
	else
		return DenitTempFunction(p_NC_Plant->NDrivTemperature[Layer]);
}
double AbioticResponses_Functions::DenitTempFunction(double Temp) {
	double out;
	if(Denit_Temp_Response<=2) {
		out=pow(DenitTemQ10,(Temp-DenitTemQ10Bas)/10.);
		if(Denit_Temp_Response==2&&Temp<DenitTemQ10Threshold) {
			if(Temp>0) 
				out=Temp/DenitTemQ10Threshold*out;
			else
				out=0.;
			
		}
		return out;
	}
	else {	//!Ratkowsky function
		if(Temp>DenitTempMax) 
			return 1.;
		else if(Temp<DenitTempMin) 
			return 0.;
		else
			return pow((Temp-DenitTempMin)/(DenitTempMax-DenitTempMin),2.);
	}
}

double AbioticResponses_Functions::DenitThetaFunction(double Theta, size_t Layer) {
	if(Theta>=p_Soil_WaterF->M_Saturation[Layer]) 
		return 1.;
	else if(p_Soil_WaterF->M_Saturation[Layer]-Theta>DenitThetaRange) 
		return 0.;
	else		
		return pow((Theta-(p_Soil_WaterF->M_Saturation[Layer]-DenitThetaRange))/DenitThetaRange,DenitThetaPowerC);
}

double AbioticResponses_Functions::NitriTempFunction(double Temp) {
	double out;
	if(Nitrific_Temp_Response<=2) {
		out=pow(NitriTemQ10,(Temp-NitriTemQ10Bas)/10.);
		if(Nitrific_Temp_Response==2&&Temp<NitriTemQ10Threshold) {
			if(Temp>0) 
				out=Temp/NitriTemQ10Threshold*out;
			else
				out=0.;
		}
		return out;
	}
	else {  //	!Ratkowsky function
		if(Temp>NitriTempMax) 
			return 1.;
		else if(Temp<NitriTempMin) 
			return 0.;	
		else
			return pow((Temp-NitriTempMin)/(NitriTempMax-NitriTempMin),2.);
	}
}


double AbioticResponses_Functions::DenitThetaFunctionL(size_t Layer) {

	return DenitThetaFunction(p_NC_Plant->NDrivTheta[Layer],Layer);
}
double AbioticResponses_Functions::NitriTempFunctionL(size_t Layer) {
	if(Nitrific_Temp_Response==0)
		return CommonTempFunction(p_NC_Plant->NDrivTemperature[Layer]);
	else
		return NitriTempFunction(p_NC_Plant->NDrivTemperature[Layer]);

}
double AbioticResponses_Functions::CommonThetaFunction(size_t Layer,double Theta) {
	if(Theta>=p_Soil_WaterF->M_Saturation[Layer]) 
		return SaturationActivity;
	else if(Theta<p_Soil_WaterF->M_Wilting[Layer]) 
		return 0.;
	else {
		double out;
		out= min(1., pow( (p_Soil_WaterF->M_Saturation[Layer]-Theta)/ThetaUpperRange,ThetaPowerCoef)  
		 *(1.-SaturationActivity)+SaturationActivity);
		return min(out, pow((Theta-p_Soil_WaterF->M_Wilting[Layer])/ThetaLowerRange,ThetaPowerCoef));
	}
	
}
double AbioticResponses_Functions::CommonThetaDryResponse(size_t Layer, double Theta) {
	if (Theta- p_Soil_WaterF->M_Wilting[Layer] >= ThetaLowerRange)
		return 1.;
	else if (Theta < p_Soil_WaterF->M_Wilting[Layer])
		return 0.;
	else
		return pow((Theta - p_Soil_WaterF->M_Wilting[Layer]) / ThetaLowerRange, ThetaPowerCoef);
}
double AbioticResponses_Functions::CommonCO2ResponseFunction( double CO2_Conc) {
	if (CO2_Conc < CO2LowestImpact)
		return 1.;
	else
		return max(0., min(1., 1.-(CO2_Conc-CO2LowestImpact)/(CO2CutOffThreshold- CO2LowestImpact)));
}


double AbioticResponses_Functions::CommonThetaFunctionL(size_t Layer) {
	if (Common_Moisture_Response == 0)
		return CommonThetaFunction(Layer, p_NC_Plant->NDrivTheta[Layer]);
	else
		return CommonThetaDryResponse(Layer, p_NC_Plant->NDrivTheta[Layer])*CommonCO2ResponseFunction(CO2_Conc[Layer]);
}
double AbioticResponses_Functions::RespTempFunctionL(size_t Layer) {

	double Temp;
//	if(Layer==0) 
//		Temp=p_NC_Plant->NDrivTAir;
//	else
		Temp=p_NC_Plant->NDrivTemperature[Layer];
	

	if(p_NC_Plant->Resp_Temp_Response==0) 
		return CommonTempFunction(Temp);
	else
		return RespTempFunction(Temp);
	
}
double AbioticResponses_Functions::RespTempFunctionAir() {

	double Temp;

	Temp=p_NC_Plant->NDrivTAir;


	if(p_NC_Plant->Resp_Temp_Response==0) 
		return CommonTempFunction(Temp);
	else
		return RespTempFunction(Temp);
	
}
double AbioticResponses_Functions::RespTempFunction(double Temp) {
	double out=0.;
	if(p_NC_Plant->Resp_Temp_Response<=2) {
		out=pow(p_NC_Plant->RespTemQ10,(Temp-p_NC_Plant->RespTemQ10Bas)/10.);
		if(p_NC_Plant->Resp_Temp_Response==2&&Temp<p_NC_Plant->RespTemQ10Threshold) {
			if(Temp>0) 
				out=Temp/p_NC_Plant->RespTemQ10Threshold*out;
			else
				out=0.;
			
		}
	}
	return out;
}