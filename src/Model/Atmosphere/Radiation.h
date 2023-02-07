#pragma once
#include "./Radiation_Functions.h"


enum radtypes {DIRECT_SHORT, ALL, LONGVAWE, SIMLAI}; 
struct RADDIST
{
	vector<double> DistFrac;
	vector<double> LAI_Above;
	double RadSoil=0., RadSoil1=0., RadSoil2=0.;
	double TotalSoilCover=0.;
	double RadLongSoil=0.;
};
struct SOILSURFACEZONE
{
	double RadSoil1, RadSoil2;
	double TotalSoilCover;
};

struct BORDERS
{
	double Left;
	double Right;
};
struct ZONES
{
	double above;
	vector<double> canopy;
	double below;
};
struct OUTDIST
{
	vector<double> can_distribution;
	double	longInGround;
};

class Radiation : public Radiation_Functions
{
	public:
		Radiation(void);
		~Radiation(void);
		bool Ini();
		bool IniFunctors();
		bool Dyn();
		void Flux();
		void Integ();
		bool End();

		RADDIST RadInterceptFunc(double Total, enum radtypes,bool SoilZone=false);
		vector<BORDERS> DefineBorders();
		vector<double> DefineEdges(vector<BORDERS> *pBorder);
		vector<double> DefineRadAbove(vector<double> *pEdges, double Rad);
		vector<ZONES>  DefineRadLongZones(vector<double> *pEdges, double RadLongIn, double RadLongOut);
		vector<double> DefineSortedHeight();
		OUTDIST DistributeRad(vector<double> *pRadAboveZone, vector<double> *pHSort, vector<BORDERS> *pBorder, vector<double> *pEdges, vector <ZONES> *pRadLongZones=nullptr);
		void Snow_On_RadImpact(double SnowHeight=0.);
		SOILSURFACEZONE DistributeRad_At_Surface(vector<double> *pRadAboveZone, vector<BORDERS> *pBorder, vector<double> *pEdges, vector<double> *pFracCoverSoilZone, vector <ZONES> *pRadLongZones=nullptr);

};
