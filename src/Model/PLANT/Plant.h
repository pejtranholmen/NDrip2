#pragma once
#include "../SOIL/Soil_Frost.h"
#include "./Plant_Functions.h"

#include "../SimModule.h"

class Irrigation;
class NC_Plant;

struct TIME_INTP
{
	size_t index; double weight;
};


class Plant : public SimModule
{
	public:
		Plant(void);
		~Plant(void);
		bool Ini();
		bool IniFunctors();
		bool Def();
		bool Dyn();
		void Flux();
		void Integ();
		bool End();
		void DefineMultiplePlantShapes();
		bool CheckEcoDriv2();


		double CanopyHeightVar;

		size_t PlantDev, RootDistribution, RootInput, LAIInput, SpatialPlantDist;
		size_t AlbedoVeg, AboveTable, CanopyHeightInput;
		size_t WuptBasic_equation;  // From WaterUptake

		size_t NumPlants;
		size_t NoDayNum;
		size_t RootDistribution_Outputs;


		vector<double> DayNum, LaiV, CForm, AlbedoV, CanopyHeight;

		double RFracLow;
		double TempCanopy;

		size_t	NRootDayN, SLA_Dynamic;
		vector<double>	RootF, PRootDepth, PRootDayNumber, PRootLength;

		vector <double> RRoot;//	! Actual root uptake fraction
		size_t NNUP, NNUP_MAX;

		double TSumStart, TSumCrit, AlbedoLeaf, TsumPlant;
		double LAI_Total;
		vector <double> LAI, RootDepth, PlantHeight, PlantAlbedo, DiameterHeightRatio;
		vector <double> RootLength;
		double RootLength_Total;
		vector <double> MaxPlantCover, KExpPlantCover, Xcenter;
		vector <double> SoilCoverFraction;
		double TotSoilCovFrac;

		// Variables related to Plant Growth submodel
		vector <double> SimLeafAreaIndex, SimRootDepth, SimPlantHeight, SimPlantAlbedo, SimRootLength;
		//! Parameters
		vector <double> PlantMaxHeight, PlantHeightC1, PlantHeightC2, PlantHeightC3, PlantHeightC4;

		vector <double> RootLowestDepth, RootIncDepth, SpecificLeafArea, SpecificRootLength;
		vector <double> AlbedoVegStage, AlbedoGrainStage, SpecificLeafAreaBegin;


		//! Constucted LAI
		PlantInterp pl;
		vector <double> StartDayNo, OptDayNo, EndDayNo, ShapeStart, ShapeEnd;
		vector <double> L_StartValue, L_OptValue, L_EndValue;
		//! Constucted Height
		vector <double> H_StartValue, H_OptValue, H_EndValue;
		//! Constucted RootDepth
		vector <double> R_StartValue, R_OptValue, R_EndValue;
		//! Constucted RootLength
		vector <double> RL_StartValue, RL_OptValue, RL_EndValue;

		//! Constucted Albedo
		vector <double> A_StartValue, A_OptValue, A_EndValue;

		//Tables
		vector<P*> mTab_RootDist;
		vector<P*> mTab_RootDepth_t, mTab_Above_t;  //Time
		vector<P*> mTab_Size, mTab_LAI, mTab_Height, mTab_Albedo, mTab_RootDepth, mTab_RootLength, mTab_CanCover, mTab_Diam;
		vector<P*> mTab_Pos;
		//From PotTranspiration
		vector<P*> mTab_EvapSingle, mTab_EvapMulti, mTab_RoughMulti, mTab_DisplMulti;


		// Index in driving variable file

		double PlantV_Index;

		double PlantDynamic_IndexFile;
		double RandomNumberSeed;


		//! Local Plant Variables
		double T5{0.}, TimeZ, PotRis;
		bool Winter;

		double Islut{ 0. };

		double TTinterp;
		size_t IvalInterp;

		vector<double> RelRootDist;
		size_t Num_NUpt, Num_NUptMax;

		//!	Driving variables

		CPG m_PG_Crop;
		vector<float>  VrootUt;
		// vector<int>  VUTPG_File;//	! Variables in PG-file
		PGPOS m_PGPos_Crop;
		ININFO LAI_Ipos, RoughIpos, DisplIpos, HeightIpos, RootIpos, RootLengthIpos, AlbedoVegIpos, ResSurfIpos;
		vector<size_t> PlantExtraPosIvar;
		size_t NumValidInput2;



		Soil_Frost *p_Soil_WaterF;
		SimModule *p_Soil_HeatF;
		Irrigation *p_Atm;
		NC_Plant *pNC_Plant;
		SimModule *p_Structure;


		bool DrivPG2;

		function<double(double)>  f_LAI_Time, f_Albedo_Time, f_CanopyHeight_Time, f_RootDepth_Time, f_RootLength_Time;
		function<double(double, double)>  f_RootDepthExpDistribution;
		function<double(size_t, double)> f_LAI_Mass, f_LAI_Season, f_CanopyCover, f_RootDepth_Mass, f_RootLength_Mass, f_Albedo_GrowthStage;
		function<double(size_t, double, double)> f_LAI_Mass_Stage;
		function<double(size_t, double, double, double)> f_CanopyHeight_Mass_Age;
		function<double(size_t, double)> f_CanopyHeight_Season, f_AlbedoVeg_Season, f_RootDepth_Season, f_RootLength_Season;

		// Soil Water Flow module

private:
	
	// Functions
	bool Root_Check();










};
