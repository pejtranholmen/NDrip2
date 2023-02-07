#pragma once
#include "../SimModule.h"
#include "../SOIL/Soil_Frost.h"
#include "../PLANT_NC/NC_Plant.h"


class NC_ExternalInputs_Functions : public SimModule
{
	public:
		NC_ExternalInputs_Functions(void);
		~NC_ExternalInputs_Functions(void);
		bool Def();
		bool FunctionDef();
	
 Soil_Frost *p_Soil_WaterF; // Soil Water Flow module
 NC_Plant *p_NC_Plant;



//Integer IUnit
size_t FaecesPool;   // From NC_Organics

vector<double>  Dep_N_toLeaf, N_Deposition_Leaf; 
double	N_Fert, N_Fert_App, N_Fert_NO3, N_Fert_NH4, N_Deposition_NO3, N_Deposition_NH4 ;
double  Total_N_Deposition_Leaf;
double	N_Manure_NH4, N_Manure_Litter, N_Manure_Faeces, C_Manure_Litter, C_Manure_Faeces;
double  N_Irrig_NO3, N_Irrig_NH4;

size_t N_Fertilization, N_Deposition, N_Irrigation, FertilizationIndexFileName , DepositionIndexFileName;
size_t NFertDayNo, NManureDayNo, ManureInput, NFertStageNo, NFertYearTableNo, ReadYearBasedManagementTable;
double N_Irrig_App;

vector<double>  SoilNitrateInflow,SoilAmmoniumInflow;
vector<double> SoilNitrate, SoilAmmonium;

vector<double> O_State[NUM_ORG_FORMS][NUM_BIOELEM];
double O_State_Profile[NUM_ORG_FORMS][NUM_BIOELEM]; 
double O_State_TotOrg[NUM_BIOELEM];
double O_State_TotLitter[NUM_BIOELEM];


vector<double>   N_Fert_DayNo, N_Fert_Rate, N_Fert_YearStage, N_Fert_YearRate, N_Fert_YearFreq;
vector<double>   N_Fert_Year, N_Fert_DayNo1, N_Fert_Rate1, N_Fert_DayNo2, N_Fert_Rate2;
vector<double>   N_Fert_DayNo3, N_Fert_Rate3;
vector<double>  Manure_DayNo, Manure_Faeces, Manure_Litter,Manure_NH4;
vector<double> Manure_CN_Litter, Manure_CN_Faeces, Manure_Depth, Manure_YearFreq;


vector<P*> mTab_Fert, mTab_FertStage, mTab_NDep, mTab_Manure, mTab_Fert_YearTable;

double N_FertDiss_k, N_Fert_NH4_Frac;
double N_Irrig_NH4_Conc, N_Irrig_NO3_Conc;
double  Dep_N_DryRate, Dep_N_WetConc, Dep_NH4_FracW, Dep_NH4_FracD, Total_N_DepositionLeaf, ManureYearStart;


// New special Index parameter for reading Management Fertilizer File

double CsvFileManagementIndex;
bool m_NoUnderScore;
//! Local Variable
double ManDepth;

bool m_YearFertTest;
string m_FileNameParTable;
size_t m_CountStartFertYear;
size_t NumMan;//			! Number of Layers with manure application
bool	 ManureDay;
vector<double>  ManDepthDist;

CPG m_PG_Manure, m_PG_Deposition;
PGPOS m_PGPos_Manure, m_PGPos_Deposition;
ININFO FertNIpos, ManNHIpos, ManNLNIpos, CNBEDIpos, ManFNIpos, CNFECIpos, ManDepthIpos, NH4NIpos,NO3NIpos;

ifstream m_FertTableStream;
FCSV *m_pFertTableStream;

size_t NumValidInput4, NumValidDeposition;
//Real, Dimension, Allocatable :: NDrivEcoDat	! Driving variables


function<double(double)> f_N_DayFertization;
function<double(double, double)> f_N_AgeStageFertilization;
function<double(double, double)> f_N_YearStageFertilization;




};

