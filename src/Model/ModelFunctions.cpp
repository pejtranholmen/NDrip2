#include "../std.h"

#include "ModelFunctions.h"
#include "../ModelTypes/SimBConst.h"
#include <cmath>

using namespace std;

namespace ModelFunctions {

	double SinTemp(double TAirMean, double TAirAmpl, double TAirPhase, double YCH, double Damp, double Time, double day, double depth) {
		double timm;

		if (YCH < 1.0001)
			timm = Time - int(Time);
		else
			timm = day;

		if (Damp > 1.E-5)
			return TAirMean - TAirAmpl*exp(-depth / Damp)*cos(2 * Fix::PI*(timm - TAirPhase) / YCH - depth / Damp);
		else
			return TAirMean - TAirAmpl*cos(2 * Fix::PI*(timm - TAirPhase) / YCH);

	}

	double SteadyStateSol_BoundaryTemp(double Upperheight, double UpperTh, double Lowerheight, double LowerTh, double  Tempabove, double Tempbelow) {
		double ALFA = (UpperTh*Lowerheight) / (Upperheight*LowerTh);
		ALFA = max(min(1., ALFA), 0.);
		return min(0.0001, (Tempbelow + ALFA*Tempabove) / (1. + ALFA));
	}
	double SVP(double temp) {
		if (temp < 0)
			return pow(10., (12.5553 - 2667. / (273.16 + temp)));
		else
			return pow(10., (11.4051 - 2353. / (273.16 + temp)));
	}
	double DSVP(double temp) {
		if (temp < 0)
			return SVP(temp)*log(10.)*2267. / pow(273.16 + temp, 2.);
		else
			return SVP(temp)*log(10.)*2353. / pow(273.16 + temp, 2.);
	};
	double PMEquation(double TAir, double RadAvail, double VapourPDef, double ResAir, double ResSurf, double LAI) {
		return (DSVP(TAir)*(RadAvail) / Fix::LATE + 86400.*min(1., LAI)*Fix::DENSA*Fix::SPECA
			*VapourPDef / ResAir / Fix::LATE) / (DSVP(TAir) + Fix::GAMMA*(1 + ResSurf / ResAir));
	}


	double SVP_David(double temp)
	{	//SVP mbar - Changed to Pa by PEJ
		double f;
		f = (22.452*(temp)) / (temp + 273.15 - 0.61);
		return 613.68*exp(f);
	}
	double Conc_Vap(double pressure, double temp)
	{
		return pressure*Fix::MOLW / (Fix::RGAS*(temp + 273.15));
	}
	double PMRad(double TAir, double RadAvail, double VapourPDef, double ResAir, double ResSurf, double LAI) {
		return (DSVP(TAir)*(RadAvail) / Fix::LATE) / (DSVP(TAir) + Fix::GAMMA*(1 + ResSurf / ResAir));
	}

	double PMVent(double TAir, double RadAvail, double VapourPDef, double ResAir, double ResSurf, double LAI) {

		return (86400.*min(1., LAI)*Fix::DENSA*Fix::SPECA*VapourPDef / ResAir / Fix::LATE) / (DSVP(TAir) + Fix::GAMMA*(1 + ResSurf / ResAir));
	}

	double ShawP_Rough(double CanDensMax, double PAI, double CanopyHeight, double RoughLmin, double RoughLmax) {

		double EQ1 = (0.175 - 0.098*CanDensMax) + (-0.098 + 0.045*CanDensMax)*log10(PAI);
		double EQ2 = (0.150 - 0.025*CanDensMax) + (0.122 - 0.0135*CanDensMax)*log10(PAI);
		return min(RoughLmax, max(CanopyHeight*min(EQ1, EQ2), RoughLmin));

	}
	double ShawP_Displ(double CanDensMax, double PAI, double  RefHeight, double CanopyHeight) {
		double DISPLH = 0.803 + 0.108*CanDensMax - (0.462 - 0.086*CanDensMax)*
			exp(-(0.163 + 0.283*CanDensMax)*PAI);
		return max(0., min(RefHeight - 0.5, DISPLH*CanopyHeight));

	}



	double RHPsi(double Psivalue, double Tempvalue, double EcorrValue) {

		double PSIPosM = max(0.1, Psivalue) / 100.;
		return exp(-PSIPosM*EcorrValue / (Fix::RGASC*(Tempvalue + 273.16)));
	}

	double D_vap(double DvapB, double AirP, double Temp) {
		return DvapB*AirP*Fix::DNOT*pow((Temp + 273.15) / 273.15, 1.75);  //!   (-)*m2/s
	}

	double D_CO2(double DturtCO2, double AirP, double Temp) {
		//	D_oxygen= !   (-)*m2/s
		return max(Fix::DNOT_CO2 / 1.E4, DturtCO2*AirP*Fix::DNOT_CO2*pow((Temp + 273.15) / 273.15, 1.75));
	}
	double GasDiff_TempFunc(double Temp) {
		//	D_oxygen= !   (-)*m2/s
		return max(0.001, pow((Temp + 273.15) / 273.15, 1.75));
	}


	double D_oxygen(double DturtO2, double AirP, double Temp) {
		//	D_oxygen= !   (-)*m2/s
		return max(Fix::DNOT_O2 / 1.E4, DturtO2*AirP*Fix::DNOT_O2*pow((Temp + 273.15) / 273.15, 1.75));
	}
	double D_NO(double DturtO2, double AirP, double Temp) {
		//	D_oxygen= !   (-)*m2/s
		return max(Fix::DNOT_O2 / 1.E4, DturtO2*AirP*Fix::DNOT_O2*pow((Temp + 273.15) / 273.15, 1.75));
	}
	double TempVapourPressure(double VapourPressure) {
		return (241.88*log(VapourPressure / 610.708) / (17.558 - log(VapourPressure / 610.708)));

	}
	double Sens_AirF(double TempDiff, double Ra) {

		return 86400.*(TempDiff)*Fix::DENSA*Fix::SPECA / max(Ra, 0.0001);
	}

	//! 2. Latent heat flux as a function of vapour pressure difference and aerodynamic resistance
	//!
	//RealLatent_AirF(VapourDiff, Ra)
	//Real VapourDiff, Ra
	double Latent_AirF(double VapourDiff, double Ra) {
		return 86400.*(VapourDiff) / Fix::GAMMA*Fix::DENSA*Fix::SPECA / Ra;
	}

	double RichardsonsNumber(double DiffHeight, double DiffTemp, double TempAir, double WindSpeed) {
		return Fix::GRAVCONST*DiffHeight*DiffTemp / ((TempAir + 273.15)*WindSpeed*WindSpeed);
	}
	double AirDensityTempFunc(double temp)
	{
		return Fix::DENSA*293.15 / (273.15 + temp);
	}
	double RelativePressureHeightFunc(double height) {
		return pow(1 - 2.25577E-5 * height, 5.25588); //  Engineering ToolBox, (2003). Altitude above Sea Level and Air Pressure.
	}

}
