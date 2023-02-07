#pragma once
enum class PrecInput_Sw{PARAMETERS, PG_FILE_FIRST, PG_FILE_FIXED};
enum class TAirInput_Sw { PARAMETERS, PG_FILE_FIRST, PG_FILE_FIXED };
enum class TairCycle_Sw { DIURNAL, ANNUAL};
enum class HumRelInput_Sw {PARAMETERS, PG_FILE_FIRST, PG_FILE_FIXED}; 
enum class VapourAirInput_Sw { RELATIVEHUMIDITY, PRESSURE_PG_FILE_FIRST, WETBULBTEMP_PG_FILE_FIRST, PRESSURE_PG_FILE_FIXED, WETBULBTEMP_PG_FILE_FIXED, VPD_PG_FILE_FIRST };
enum class WSpeedInput_Sw { PARAMETERS, PG_FILE_FIRST, PG_FILE_FIXED };
enum class RadNetInput_Sw { PARAMETERS, PG_FILE_FIRST, PG_FILE_FIXED };
enum class RadInLongInput_Sw { PARAMETERS, PG_FILE_FIRST, PG_FILE_FIXED };
enum class RadGlobInput_Sw{ PARAMETERS, PG_FILE_FIRST, PG_FILE_FIXED };
enum class CloudInput_Sw{ PARAMETERS, PG_FILE_FIRST, ESTIMATED_SUNSHINE, PG_FILE_FIXED, ESTIMATED_TEMPDAYDIFF };
enum class DBSunInput_Sw{ NOT_USED, PG_FILE_FIRST, PG_FILE_FIXED };
enum class PotTranspInput_Sw{ PARAMETERS, PG_FILE_FIRST};
enum class TSurfInput_Sw { NOT_USED, PG_FILE_FIRST };
enum class TAirGlobRad_Sw {NOT_USED, USED};
enum class TempCycleInput_Sw{ NOT_USED, USED};
enum class CommonRefHeight_Sw{NO,YES };
enum class SweClimScenarious_Sw{No,HadAM3_A2,HadAM3_B2,ECHAM4_A2,ECHAM4_B2};
enum class SweClimRegions_Sw{ NW,NE ,MW, ME,SW ,SE};
enum class ClimIndexFileName_Sw{OFF, SINGLE_PG_FILE, MULTI_PG_FILE};
enum class TempDayDiffInput_Sw { PARAMETERS, PG_FILE_FIRST, PG_FILE_FIXED };
enum class TrafficInput_Sw{OFF,ON};

class PrecGenerator {
public:
	PrecGenerator(double a, int b, int c) :_a(a), _b(b), _c(c) { }
	double operator() (double x) {
		if (int(x) % _b == _c + 1)
			return _a;
		else
			return 0.;
	}
private:
	double _a;
	int  _b, _c;
};
class AirTempAltCorrection {
public:
	AirTempAltCorrection(double AltMetStation, double AltSimPosition, double TairLapseRate) :a(AltMetStation), b(AltSimPosition), c(TairLapseRate) { }
	double operator() (double x) {
		return x + (a - b)*c;
	}
private:
	double a, b, c;
};
class DailyAirMeanTemp {
public:
	DailyAirMeanTemp(size_t option, double TempInit, double TimeStep) :Opt(TimeResolution(option)), MeanValue(TempInit), T_Step(TimeStep) {
		switch (Opt) {
		case TimeResolution::WITHIN_DAY:
		case TimeResolution::DAILY_MEAN:
			_v.resize(1);
			break;
		case TimeResolution::HOURLY:
			_v.resize(24);
			break;
		case TimeResolution::TEN_MIN:
			_v.resize(144);
			break;
		}
		if (_v.size() == 0) _v.resize(1);
		for_each(_v.begin(), _v.end(), [&TempInit](double &x) {
			x = TempInit;
		});
	}
	double operator() (double x) {
		if (Opt == TimeResolution::WITHIN_DAY) {
			MeanValue = (1 - T_Step)*MeanValue + T_Step*x;
			return MeanValue;
		}
		else if (Opt == TimeResolution::DAILY_MEAN) {
			return x;
		}
		else {
			_v.erase(_v.begin());
			_v.push_back(x);
			MeanValue = mean(_v);
			return MeanValue;
		}
	}
private:
	enum TimeResolution Opt;
	vector<double> _v;
	double MeanValue, T_Step;
};

class PrecCorrFactor {
public:
	PrecCorrFactor(const double rainlim, double snowlim, const double a0, double a1) : PRainLim(rainlim), PSnowLim(snowlim), PrecA0Corr(cref(a0)), PrecA1Corr(a1) {}
	double operator () (double Temp) {
		if (Temp <= PSnowLim)
			ThQualP = 1;
		else if (Temp >= PRainLim)
			ThQualP = 0;
		else if (PSnowLim < PRainLim)
			ThQualP = min(1., max(0., (Temp - PRainLim) / (PSnowLim - PRainLim)));
		return PrecA0Corr + ThQualP*PrecA1Corr;//
	}
	void SetNewValue(double rainlim) { PRainLim = rainlim; };
private:
	double PrecA0Corr, ThQualP, PrecA1Corr, PSnowLim, PRainLim;

};
