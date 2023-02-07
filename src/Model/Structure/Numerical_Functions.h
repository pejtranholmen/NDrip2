#pragma once
#include "./Technical.h"

class Numerical_Functions : public Technical
{
	public:
		Numerical_Functions(void);
		~Numerical_Functions(void);
		bool Def();
//Use CommonStruc, Only	:	GroundWaterFlow, ThroughFall,  Frost, PrecCorr

//Use FileInfo, Only		:	TimeResolution
//Use Profile
//Use General

size_t  TimeStepOption, FindTimeStep,NitrogenCarbonStep;
double	Xloop,  XADiv, Precision, SaftyPerCent;
double	Timestep;
double	NC_Iterations;

		//! Local Indicator for max-infiltration
//Real*4 			//! Local Indicator for upflow

bool Next;//			! Local Variable

vector<double> ThickMinimum, OldTemperature;
vector <size_t> Diff_At;

//! Adaptative solution
bool	HIGHPREC, RESTART, APEAK;
double NEWDELTAT;
size_t ID_Loop_Count, DiffPos;


//Real*8, Dimension(:), allocatable :: WaterFlowOutputs(:,:),WaterFlowOutputs_Start(:,:),WaterFlowOutputs_Next(:,:)
vector<double> TimeStepOutputs,TimeStepOutputs_Start,TimeStepOutputs_Next;
vector<double> CMAX1,CMAX2,ETA1,ETA2,VAL2_NORM, ERR_NorM;

//Local variables
size_t ITT;

double GetNewStep();

};

