#pragma once
#include "./Hydraulic.h"
class Soil_Drainage_Functions : public Hydraulic
{
	public:
		Soil_Drainage_Functions(void);
		~Soil_Drainage_Functions(void);
		bool Def();

		bool FunctionDef();

		double GetFunctionValue(size_t option, double x1, bool check=false);
		double GetFunctionValue(size_t option, size_t index, double x1);


//  Use CommonStruc, Only	: GroundWaterFlow, LateralInput, WaterEq,  &
//						  Theta, Psi, Temp, SatLev, WaterDrainFlow, LateralGWSource, &
//                            Hflow_Input_Index, HFlow_Output_Index

// Use FileInfo, Only		:	TimeResolution

	double 	GWSoLayer, GWSoFlow;
	double 	GFLevBase,GFLevPeak, GFlowBase, GFlowPeak;
	double  DLevel, DDist, DLevelLB, DDistLB, DrainLevelMin;
	double  PsiBottom, RPipe, Dlayer, GWFileName_Index;
	double  PumpHighLevel, PumpLowLevel, PumpFlowRate,vDriveDrainLevel;
	double LowerB_GwGrad;
	double DeepPerc;
	double  HFlow_Input_Index, HFlow_Output_Index;


	size_t LBoundSaturated, LBoundUnSaturated, ReturnFlow;
	size_t	DriveDrainLevel, PhysicalDrainEq, EmpiricalDrainEq;
	size_t Pump, GWIndexFileName;
	
	CPG  m_PG_Gw, m_PG_PressureHead;
	PGPOS m_PGPOS_Gw, m_PGPOS_PressureHead;
	ININFO IGW_Pos, IPressureHead_Pos;

	size_t LayerOfDrain,  LayerOfSatLevelSaved, LayerOfSource, LayerOfSatLevel;

	double	DEQLAY;	//	! Equivalent layer Thickness
	double	CORRH;
	vector<double>  CorrHH, WaterDrainFlow; 
	vector<double> Theta, Psi,  WaterStorage; // from Soil_WaterF

	double	NetEmpDrainage ,NetPhysDrainage, TotalDrainage, TotalRunoff;
	double SatLev;
	bool Zero, Below;
	function<double(double)> f_EmpiricalDrainage,f_PhysicalDrainage, f_SeepageFlow, f_DitchLevel;
	function<double(size_t, double)> f_PhysDrainageRate;


	function<void(void)> f_DeepPerc;

	bool SetDrainageFunctors();


	void Drain_Drive();

	void PsiLower_Drive();
	void SetDrainLevel();
	double SumDrainage();
	double GW_LowestLayer();
	double DrainFlow(size_t i);


	bool GWposition(size_t i, double PsiUpper, double PsiLower);
	double SatLev_F(size_t i, double PsiUpper);
	double GWpressure(size_t i, double SatLev);


};

