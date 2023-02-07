#pragma once

namespace ModelFunctions
{

	struct PlantInterp {
		std::vector<double> Start;
		std::vector<double> Opt;
		std::vector<double> End;
		std::vector<double> ShapeStart;
		std::vector<double> ShapeEnd;
		std::vector<double> StartValue;
		std::vector<double> OptValue;
		std::vector<double> EndValue;
	};




	double SteadyStateSol_BoundaryTemp(double Upperheight, double UpperTh, double Lowerheight, double LowerTh, double  Tempabove, double Tempbelow);
	double SVP(double temp);
	double DSVP(double temp);
	double PMEquation(double TAir, double RadAvail, double VapourPDef, double ResAir, double ResSurf, double LAI = 1.0);
	double SVP_David(double temp);
	double Conc_Vap(double pressure, double temp);
	double PMRad(double TAir, double RadAvail, double VapourPDef, double ResAir, double ResSurf, double LAI);
	double PMVent(double TAir, double RadAvail, double VapourPDef, double ResAir, double ResSurf, double LAI);
	double ShawP_Rough(double CanDensMax, double PAI, double CanopyHeight, double RoughLmin, double RoughLmax);
	double ShawP_Displ(double CanDensMax, double PAI, double  RefHeight, double CanopyHeight);
	double RHPsi(double Psivalue, double Tempvalue, double EcorrValue);
	double D_vap(double DvapB, double AirP, double Temp);
	double D_CO2(double DturtCO2, double AirP, double Temp);
	double GasDiff_TempFunc(double Temp);
	double D_oxygen(double DturtO2, double AirP, double Temp);
	double D_NO(double DturtO2, double AirP, double Temp);

	double SinTemp(double TAirMean, double TAirAmpl, double TAirPhase, double YCH, double Damp, double Time, double day, double depth);
	double TempVapourPressure(double VapourPressure);
	double Sens_AirF(double TempDiff, double Ra);
	double Latent_AirF(double VapourDiff, double Ra);
	double RichardsonsNumber(double DiffHeight, double DiffTemp, double TempAir, double WindSpeed);
	double AirDensityTempFunc(double temp);

}