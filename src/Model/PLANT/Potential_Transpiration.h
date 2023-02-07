#pragma once
#include "./Plant.h"
class Potential_Transpiration : public Plant
{
	public:
		Potential_Transpiration(void);
		~Potential_Transpiration(void);
		bool Ini();
		bool IniFunctors();
		bool Def();
		bool Dyn();
		void Flux();
		void Integ();
		bool End();

		size_t Roughness, Displacement, Roughnessfunc, MultiRoughness;
		size_t RSMethod, ResAirStabCorr;
		size_t ConductanceCO2_Dependence;

		double EPMaxRate, EPMaxDay, EPPeriod;
		double CanDensMax, PAddIndex, RoughLMin;
		double CondRis, CondVPD, CondMax, CondMaxWinter;

		double CO2_BaseLevel, CO2_PowerCoef;
		double KBMinusOne;
		// From Soil Evaporation
		double  StabCoefStableRich, StabCoefStableExp, StabCoefUnStableRich, StabCoefUnStableExp;

		vector<double> MC_CanDensMax, MC_PAddIndex, MC_RoughLMin, MC_RoughLMax;
		vector<double> MC_CondRis, MC_CondVPD, MC_CondMax;

		vector<double> MC_ResSurfVarMin, MC_PotTranspMax, MC_ResponseAtm;

		vector<double> RoughLength, Displace, ResSurface;
		vector<double> MC_ResAir, MC_RoughLVar, MC_ResSurfVar, MC_RaLAI_Effect;
		vector<double> MC_PotTransp, MC_DisplVar;

		vector<double> Z_StartValue, Z_OptValue, Z_EndValue;
		vector<double> D_StartValue, D_OptValue, D_EndValue;

		double	 DisplVar, RoughLVar, ResSurfVar, WindLessExchangeCanopy;
		double	ResAir;

		//Local variables
		vector<double> RAir, Rsurf;
		size_t Index, Icase, IC, INEW, MaxIndex;

		double CO2_Scaling(double co2conc);
		double Lohammar(double LAI);
		double CanopyRSMax(size_t index);
		double Lohammar_LL(size_t index);
		double Lohammar_Ris(double RadIn);
		double Lohammar_Vpd(double vpd);
		double Lohammar_Ris_L(size_t index, double RadIn);
		double Lohammar_Vpd_L(size_t index, double vpd);
		//double PotTranspFunction(double daynum);
		//double RoughDayFunction(double day);
		//double ResSurfDayFunction(double day);
		//double DisplaceDayFunction(double day);
		double RAFunctionPotTransp(double DiffZ, double z_0M, double WS, double TC);
		double RoughLinFunc(size_t index, double PlantH, double day);
		double ParRoughCoefFunction(size_t index, double day);
		double ParDisplCoefFunction(size_t index, double day);
		double Displ_LinFunc(size_t index, double PlantH, double day);
		// Functions created only for Display purpose

		double PotentialTranspiration_Radiation(size_t index, double NetRadiation, double HumRel, double Temp, double LAI);
		double PotentialTranspiration_Temperature(size_t index, double Temp, double NetRadiation, double HumRel, double LAI);
		double SurfaceResistance_Rad(size_t, double GlobalRad, double VPD, double LAI);
		double SurfaceResistance_VPD(size_t, double VPD, double GlobalRad, double LAI);
		double AerodynamicResistance_Wind(size_t, double Wind);
		double Roughness_Height(size_t, double Height);

		function<double(double)> f_CanopyResistance_Time, f_Roughness_Time, f_Displacement_Time;
		function<double(double)> f_PotTranspiration_Time;
		function<double(double, double)> f_CanopyResistance, f_Roughness_SingePlant;
		function<double(double, double, double)> f_Displacement_SingePlant, f_ResAirCanopy_Neutral;

		function<double(double, double, double, double, double)> f_ResAirCanopy;
		function<double(bool)> f_MaxConductance;
		function<double(bool, double RIS, double VPD)> f_Conductance;
		function<double(bool, size_t)> f_MaxConductance_mc;
		function<double(double)> f_Conductance_CO2;
		function<double(size_t, double, double, double)> f_RoughLengthLinear;
		function<double(size_t, double, double)> f_DisplacementLinear;






};
