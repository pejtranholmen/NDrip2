#pragma once
#include "../SimModule.h"
#include "../SOIL/Soil_Frost.h"
#include "../SOIL/Soil_HeatPump.h"
#include "../Atmosphere/Irrigation.h"
#include "../PLANT_NC/NC_Plant.h"
#include "../SOIL_NC/NC_SoilManagement.h"
#include "../../SoilData/GetSoilStorage.h"
//#include "../Plant\WaterUptake.h"
class WaterUptake;

class Technical_Functions : public SimModule
{
	public:
		Technical_Functions(void);
		~Technical_Functions(void);
		bool Def();



		double SumLikelihood(size_t i, double sim, double obs);
	    double SumLikelihood(size_t i, vector<double> *sim, vector<double> *obs);
		double boxcox(double y, double lam1, double lam2) {if(lam1<=1.E-6) return log(y+lam2);else return (pow(y+lam2,lam1)-1)/lam1;} ;
		double boxcox_derive(double y, double lam1, double lam2) {return pow(y+lam2,lam1-1);}; 
		double MC_ScalingStepFunction(size_t IndexRun);

Soil_Frost *p_Soil_WaterF; // Soil Water Flow module
Soil_HeatPump *p_Soil_HeatF; // Soil Water Flow module
Irrigation *p_Irrigation; // Surface and atmosphere module
NC_Plant *p_NC_Plant;
NC_SoilManagement *p_NC_Soil;
WaterUptake *p_Plant;



size_t	Develop_Option;//		! Switch to hidden pure technical switches
size_t StartWithPrevious;//	! Use initial values from previous run

size_t IndexForValid;//! To define index from a number serie 2003-08-10
size_t DynamicValidFileIndex;
size_t LateralInputFile;	//!  To make Quasi 2-D simulations
size_t NoSingleRunSimFile;
size_t CreateFileResiduals;
size_t SimulationPeriodFromMetFile;
size_t Nelder_Mead_Objective;

// Below from Technical functions


size_t	AVERAGEX;	//! Mean value of X-variable
size_t	AVERAGET;	//! Mean value of T-variable
size_t	AVERAGEG;	//! Mean value of G-variable
size_t	AVERAGED;	//! Mean value of D-variable

size_t	OUTFORN, INSTATE;		//! Output of Fortran names
								//int  		//! Initial values for state 
								//! variable from file
size_t	OUTSTATE, DRIVPG, DRIVNPG, LISALLV, CHAPAR, ADDSIM, TimeResolution;	//! Final values for state
																			//! variables to file
																			//int			//! Driving variable file
																			//int	DRIVNPG		//! Driving variable file
																			//int	LISALLV		//! Output all variable statistics
																			//! to summary file
																			//int			//! Redefine parameter values at
																			//! dates given by MINNEW
size_t	DrainFileUse, ValidationMode;		//! Run series of simulations
											//Integer,pointer ::   //! DrainFile
											//Integer, pointer ::  //! To make Validation

size_t	EnableSaltSource, EnableInputPGSalt, EnableInputN;//		! Run series of simulations
														  //int			! Run series of simulations
														  //int			! Run series of simulations

size_t	PressureHeadSign, HeatFluxUnit;//	! Previous UNITPOT
									   //int		! Previous UNITPOT


double  Val_Index, Val_IndexMax, Val_IndexRepeat, ValFile_Index;
double FRandomSeed, BayesianStepChangeSens, BayesianStepMinimumScaling, MCStepChangePeriod;
double AggregateCalibNumber;

vector<double> ObservedScaling, ObservedScalingIndex, BoxCox_p1, BoxCox_p2, Abs_Error, Rel_Error;
size_t NumScaling;


vector<double> IndexParGroups;

size_t TimeSerieOutputLevels, ValidationOutputLevels, BayesianStepFunction, BayesianLikelihoodFunction;
size_t BoxCoxTransformed;
CPG FileValidation[16], OutputFile;
ifstream m_DataBaseCSV;
ifstream m_MultiParStream[5];
size_t DataBaseCSV;


vector<P*> ErrorTab, ScalingObsTab;
vector<P*> IndexMultiParGroups;
size_t NumberParGroups, UserDefinedCalibrationMode;
size_t Mascur_Soil_Data;

protected:






	vector<double> CandidateValues;
	bool First, AcceptedRun, CalibrationMode;






};

