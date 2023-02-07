#pragma once
#include "../std.h"
#include "PG.H"
struct NOASTAT
{	int ID;
	string Name;
	double Long;
	double Lat;
	double Alt;
};
struct SUBSTAT
{	int VarSub;
	int VarSubRep;
	int nSub;
	int VarUsed;
	int VarUsedRep;
	float R2;
	float A0;
	float A1;
};

class NoaaCoord
{
public:
	NoaaCoord(void);
	~NoaaCoord(void);
	bool ReadStation(string filename, CPG *pPG=nullptr);
	bool MergeSimilarStation(CPG *pPG);

	bool TemporalCorrelation(CPG *pPG);
	void SetSourceFile(CPG *pPG);
	bool SplitPGFile();
	CPG* CreateNewSpatialCorrelationFile(CPG *pPG);
	CPG* CreateNewSinglePosFile(double Lat1=58.6666, double Long1=15.667, CPG *pPG=nullptr); 
	CPG* CreateNewGridFile(double LatNorth=59.6666, double LatSouth=58.6666, double LongEast=16.667, double LongWest=15.667, int n_Lat=2, int n_Long=2, CPG *pPG=nullptr);
	CPG* CreateNewInterPolatedFile(float MaxDaysThreshold=3.,CPG *pPG=nullptr);
	CPG* CreateNewRandomFillingFile(CPG *pPG=nullptr);	
	
	
	float NewInterpolatedValue(int varno, long rec_no);
	bool EstimateWeightforInterpolation(double Lat1, double Long1);
	string m_DestFileName;
	vector<SUBSTAT> m_statsub;
	vector<string> m_ResRandomSub;
	CPG *m_pPGSourceReg;

protected:

	CPG *m_pPGSource;
	vector<double> m_distance;
	vector<double> m_weight;
	vector<int> m_disindex;

	double m_sumweight;
	int m_Min_Involvedstations;
	int m_num_used_stations;
	int m_dis_threshold;
};

