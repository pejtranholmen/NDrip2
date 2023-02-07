#pragma once
#include "./Interception.h"

enum  class RoadSaltApplication_Sw{ Parameter_Driven,On_Road_PG_File };
enum class SaltRoadStorage_Sw{off,on};
enum class SaltSnowStorage_Sw {off, on};
enum class SaltDeposition_Sw { Parameter_Driven, From_Road_PG_File, From_PG_File_par };
enum class BlomquistEquation_Sw { off, on };
enum class TraceElementUptake_Sw { off, on };
enum class PassiveUptake_Sw	{off,on};
enum class ActiveUptake_Sw {off, on};
enum class InitialSaltConc_Sw { Uniform_conc, conc_z };
enum class Adsorption_Sw {off, on};
enum class IrrigConcInput_Sw { Parameter, PG_file };
enum class SaltGWSource_Sw { off, Lateral_Input_File };
enum class ResistivityOutput_Sw	{off, on};
class Salt : public Interception
{
	public:
		Salt(void);
		~Salt(void);
		bool Ini();
		bool IniFunctors();
		bool Def();
		bool Dyn();
		void Flux();
		void Deposition();
		void Integ();
		bool End();


		size_t  RoadSaltApplication, ActiveUptake, PassiveUptake, InitialSaltConc;
		size_t  Adsorption, IrrigConcInput, SaltRoadStorage, SaltDeposition, BlomquistEquation;
		size_t ResistivityOutput, SaltSnowStorage, SaltGWSource;
		size_t TraceElementUptake;

		size_t NResProbe;
		vector<double> SaltStorage, SaltFlow, SaltDrainFlow, SaltSourceFlow;
		vector<double> SaltPlantUptake;
		//! Flux variable Trace element
		vector<double> TE_PlantLeafUptake, TE_PlantStemUptake, TE_PlantRootUptake;
		vector<double> SingleUptake, TE_SingleUptake;
		vector<double> SaltHalfReduction, SaltPowerCoef, OsmoticPressure;

		double AccSaltInput, AccSaltOutput, SaltToRoad;
		double SaltSurfaceOutFlow;//	! Added Nov 16 2004 PEJ
		double TotSaltSourceFlow;//
		double  SaltOnRoad, SaltInSnow, SaltToSnow, SaltFromSnow;

		double SaltInfiltration, SaltDeepPercolation;

		double SaltInitConc, SaltInfilConc, SaltInputConcPar, ConcV_Index, SaltGWSoFlowConc;

		double SaltIrrigationConcPar, SaltIrrigationConc;
		double SaltAppRate, TempSaltLowLimit, TempSaltHighLimit, TotalSaltDrainFlow;
		vector<double> WaterResistivity, SoilResistivity, SaltConc, SaltInitConcDist;

		double Archie_A, Archie_M, Archie_N, Res_Temp_N, Res_Temp_Sens, Res_IonCoef;


		//! New for Riitta
		double DepthofFront;//	! Variables
		double ConcForFront;//	! Parameter


							//Real, Dimension(:), Allocatable :: VUTPG_File	! Variables in PG-file


							//! To be handled in NC_Plant

							//! State variables Trace element
		vector <double> TE_Grain, TE_Stem, TE_Roots, TE_Leaf, TE_OldStem, TE_OldRoots, TE_OldLeaf;
		vector <double> TE_RootsLitter, TE_OldRootsLitter, TE_RootsLitter1;
		vector <double> TE_Litter1, TE_Humus;
		double	TE_Balance, TE_SurfaceLitter;


		//! Flux variable Trace element

		vector<double> TE_StemGrain, TE_LeafGrain, TE_RootsGrain;
		double   SaltEmissions;
		//!Real*8, pointer		:: TE_StemHarvest(:), TE_LeafHarvest(:), TE_GrainHarvest(:), TE_RootsHarvest(:) Harvest of trace element
		//!Real*8, pointer		:: TE_OldStemHarvest(:), TE_OldLeafHarvest(:), TE_OldRootsHarvest(:)
		vector<double> TE_LeafSurfaceLitter, TE_StemSurfaceLitter, TE_GrainSurfaceLitter;
		vector<double> TE_OldLeafSurfaceLitter, TE_OldStemSurfaceLitter, TE_Litter1HumusRate;
		double TE_SurfaceLitter_L1, TE_SurfaceLitter_Hum;

		//!, TE_OldGrainSurfaceLitter(:)
		vector<double> TE_StemOldStem, TE_LeafOldLeaf, TE_RootsOldRoots;

		//! TE Mineralization
		vector<double> TE_Litter1MinRate, TE_HumusMinRate;

		//! Trace Elements
		vector<double> TEC_RatioLeaf, TEC_RatioGrain, TEC_RatioStem, TEC_RatioRoots;
		vector<double> TEC_RatioOldLeaf, TEC_RatioOldStem, TEC_RatioOldRoots;

		double TE_TotPlant, TE_TotHumus, TE_TotLitter, TE_TotMineral, TE_TotLitterFall;
		double TE_TotMineralisation, TE_TotPlantUptake, TE_TotStorage;


		double PassiveUptScaling, PassiveUptAlloFLeaf, PassiveUptAlloFStem;
		double ActiveUptEffLeaf, ActiveUptEffStem, ActiveUptEffRoots, ActiveUptMaxEffConc;
		//! Road Salt module

		double FractionOfRoad, EmissionRateCoef, CaptureFracEmission;
		//! New August 2005

		double SaltAirEmission, SaltPloughEmission, SaltRunOffEmission, SaltCloseToRoad;
		double SaltRoadCaptured, SaltRoadLossed, SaltRunOffEmission2;
		double SaltAirDeposition, SaltPloughDeposition, SaltRunOffDeposition;
		double SaltRunOffEffCoef, SaltPloughEffCoef;
		double FractAirEmission, FractPloughEmission, FractRunOffEmission;
		vector<double> Ad_c;
		double Dep_IndexStart, Dep_IndexInc;
		double DistKSplash, DistKAir1, DistKAir2, DistFromRoad, SnowPloughFrac, FracAir1, RoadWidth;

		//Tables
		vector<P*> mTab_Salt, mTab_Ad;

		// Driving input files

		CPG m_PG_RoadSurface, m_PG_Salt;
		PGPOS m_PGPos_RoadSurface, m_PGPos_Salt;
		ININFO AirDepPos, PloughDepPos, RunOffDepPos;

		//! Local logical Control Variable

		bool TE_UptPassiveReset, TE_UptActiveReset, TE_UptReset;
		double TE_InitStorage;

		vector<double> ActiveUptakeLayers;	//! Active Uptake from different Layes

		void   TE_ActiveUptake(vector<double> Dist, size_t IndexP);
		double ActiveUptakeLeafFunc(double Conc);
		double ActiveUptakeStemFunc(double Conc);
		double ActiveUptakeRootsFunc(double Conc);
		double ActiveMaxEffFunc(double conc);
		void   TE_PassiveUptake(vector<double> Wflow, size_t IndexP);
		double PassiveUptakeRootsFunc(size_t Index, double Wflow);
		double PassiveUptakeStemFunc(size_t Index, double Wflow);
		double SaltSolutionFunctionL(size_t index);
		double SaltSolutionFunction(double SaltAmount, double WaterAmount, double Ad_c);
		double FrontLowerBoundary();
		bool   CheckFromRoadFile();
		double DistanceDepFunction(double length);
		double DistanceDepAirFunction(double length);
		double SaltRunOffDepositionFunc(double EmissionV);
		double Temp_Sensitivity(double Temperature);
		double Cond_Estimate(double Conc);
		double WaterResistivityL(size_t Layer);
		double Archie(double Porosity, double  WaterContent, double  WaterResistivity);
		double ArchieL(size_t Layer);
		double OsmoticPressureFuncL(size_t index);
		double OsmoticPressureFunc(double Conc, double Temp);
		double SaltRunOffCloseToRoadFunc();
		double SaltRunOffEmissionFunc();
		double SaltPloughEmissionFunc();
		double SaltAirEmissionFunc();
		double SaltPloughDepositionFunc(double EmissionV);
		double SaltAirDepositionFunc(double EmissionV);
};
