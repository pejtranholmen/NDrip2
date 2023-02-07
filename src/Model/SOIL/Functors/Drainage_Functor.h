#pragma once
#include "../../../Simulator/ModelInfo.h"
#include "../Soil_HeatPump.h"
#include "../Soil_Drainage.h"
#include "../../Structure/Structure.h"
#include "../../Structure/Functors/Structure_Functors.h"
enum class DriveDrainLevel_Sw { Parameter,Driving_File,Function_of_Surface_Runoff };
enum class LBoundSaturated_Sw { No_Flow,Unit_Grad_Flow,Seepage_Flow,Static_Grad_Flow,Dynamic_Grad_Flow };
enum class LBoundUnSaturated_Sw { Constant_Psi,Maximum_constant_psi,Dynamic_Psi,No_Flow,Unit_Grad_Flow };
enum class ReturnFlow_Sw { OFF, ON};
enum class EmpiricalDrainEq_Sw { OFF, ON};
enum class PhysicalDrainEq_Sw { off,Linear_Model,Ernst_Model,Hooghoudt_Model };
enum class Pump_Sw { OFF, ON };
enum class LateralGWSource_Sw { off, Later_Input_File };
enum class GWIndexFileName_Sw { OFF, ON };

class EmpDrainage {
public:
	EmpDrainage(double GFLevBase, double GFLevPeak, double GFlowBase, double GFlowPeak):
		GFLevBase(GFLevBase), GFLevPeak(GFLevPeak), GFlowBase(GFlowBase),GFlowPeak(GFlowPeak) {}
	double operator() (double Satlevel) {
		return GFlowPeak*max(0., (GFLevPeak - Satlevel) / GFLevPeak) +
		GFlowBase*max(0., (GFLevBase - Satlevel) / GFLevBase);
	}
private:
	double GFLevBase, GFLevPeak,  GFlowBase, GFlowPeak;

};
class DrainFlowRate {
public:
	DrainFlowRate(Soil_Drainage_Functions *pHyd) : pHyd(pHyd) {
		EstimateReistances();
		DLevel = pHyd->DLevel; DDist = pHyd->DDist; Dlayer = pHyd->Dlayer;
	}
	double operator() (size_t i, double SatLev) {return FlowRate(i, SatLev);};
	double operator() (double SatLev) {
		double out=0.;
		for (size_t i = 1; i < pHyd->DepthLayers.size(); i++) {
			out+= FlowRate(i, SatLev);
		}
		return out;
	};

private:
	double FlowRate(size_t i, double SatLev) {
		double DepMu, Gradient, DepML, DepMLL, DepMuu, WCond, Area, Head, HU, HL, ThickUpper, ThickLower;//		! Local variables
		DepMu = max(pHyd->DepthMidpLayer[i - 1], -SatLev);
		Gradient = (SatLev - DLevel) / DDist;
		if (Gradient<0 && ReturnFlow_Sw(pHyd->ReturnFlow) == ReturnFlow_Sw::ON)
			DepML = pHyd->DepthMidpLayer[i];
		else if (Gradient>0.&&-SatLev<pHyd->DepthMidpLayer[i])
			DepML = min(-1.*DLevel, pHyd->DepthMidpLayer[i]);
		else
			DepML = DepMu;

		WCond = pHyd->f_CondSaturated(i - 1);
		if (PhysicalDrainEq_Sw(pHyd->PhysicalDrainEq) == PhysicalDrainEq_Sw::Linear_Model) {
			Area = max(0., DepML - DepMu);
			if (ReturnFlow_Sw(pHyd->ReturnFlow) == ReturnFlow_Sw::OFF) Gradient = max(0., Gradient);
			return Gradient*WCond*Area;
		}
		else if (PhysicalDrainEq_Sw(pHyd->PhysicalDrainEq) > PhysicalDrainEq_Sw::Linear_Model) {
			Head = SatLev - DLevel;
			if (ReturnFlow_Sw(pHyd->ReturnFlow) == ReturnFlow_Sw::OFF)
				Head = max(0., Head);

			DepMLL = min(pHyd->DepthMidpLayer[i], Dlayer - DLevel);
			if (i == 2)
				DepMuu = max(-1.*DLevel, pHyd->DepthMidpLayer[i - 1]);
			else
				DepMuu = max(-1.*DLevel, pHyd->DepthMidpLayer[i - 1]);


			if (pHyd->DepthMidpLayer[i] < -1.*DLevel) {
				//!c         Layer totally above drain depth,  HU and HL are heights above drain level
				HU = -DLevel - DepMu;
				HL = -DLevel - DepML;
				if (Head > 0.0) {
					ThickUpper = max(0., DepML - DepMu - (pow(HU, 2) - pow(HL, 2)) / (2 * Head));
					return 8 * WCond*ThickUpper*Head / pow(DDist, 2);
				}
				else
					return 0.;

			}
			else if (pHyd->DepthMidpLayer[i - 1] < -1.*DLevel) {
				//!c         Layer partially above drain depth
				ThickLower = max(0., DepMLL - DepMuu);
				if (Head > 0.) {
					HU = -DLevel - DepMu;
					HL = max(0., -DLevel - DepML);
					ThickUpper = max(0., DepML - DepMu - (pow(HU, 2) - pow(HL, 2)) / (2 * Head));
					return 8 * WCond*ThickUpper*Head / pow(DDist, 2) +
						8 * WCond*ThickLower*Head*CorrHH[i - 1] / pow(DDist, 2);
				}
				else
					return WCond*ThickLower*Head / DDist;
			}
			else if (pHyd->DepthMidpLayer[i - 1] < -1.*DLevel + Dlayer) {
				//!c         Layer totally below drain depth            
				ThickLower = max(0., DepMLL - DepMuu);
				if (Head > 0)
					return 8 * WCond*ThickLower*Head*CorrHH[i - 1] / pow(DDist, 2);
				else
					return WCond*ThickLower*Head / DDist;
			}
			else
				//!c         Layer totally below drain depth + equivalent layer
				return 0.;
		}
		else return 0.;



	};
	Soil_Drainage_Functions *pHyd;
	double DLevel, DDist, Dlayer;
	vector<double> CorrHH;
	double DEQLAY;
	void EstimateReistances() {

		double FH, Dist, DD;
		int IR;
		vector<double> SUMV, SUMR, SUMH;
		SUMV.resize(pHyd->NL);
		SUMR.resize(pHyd->NL);
		SUMH.resize(pHyd->NL);
		CorrHH.resize(pHyd->NL);

		pHyd->LayerOfDrain = 1;
		//DO  i=1,NL
		for (size_t i = 0; i<pHyd->DepthMidpLayer.size(); i++) if (pHyd->DepthMidpLayer[i]<-1 * pHyd->DLevel) pHyd->LayerOfDrain = i;
		//ENDDO

		if (size_t(pHyd->GWSoLayer)>pHyd->LayerOfDrain) pHyd->GWSoLayer = pHyd->LayerOfDrain;

// Adjustment of XNLEV removed 2008-01-08
		//if (size_t(pHyd->m_pModelInfo->XNLev)<pHyd->LayerOfDrain) m_pModelInfo->XNLev = pHyd->LayerOfDrain;

		//!C********** Calculate dlay for the Hooghoudt formula

		if (PhysicalDrainEq_Sw(pHyd->PhysicalDrainEq) == PhysicalDrainEq_Sw::Hooghoudt_Model) {
			FH = pow(pHyd->DDist - pHyd->Dlayer*sqrt(2.), 2.) / (8 * pHyd->Dlayer*pHyd->DDist) + 1 / 3.14*log(pHyd->Dlayer / (pHyd->RPipe*sqrt(2.)));
			if (FH>0.)
				DEQLAY = pHyd->DDist / (8 * FH);
			else
				DEQLAY = pHyd->Dlayer;


			if (pHyd->DepthLayers.back() + pHyd->DLevel<DEQLAY)
				CorrHH.assign(pHyd->NL, 1.);
			else {
				for (size_t i = 0; i<CorrHH.size(); i++) {
					if (pHyd->DepthMidpLayer[i]<-1 * pHyd->DLevel + pHyd->Dlayer)
						CorrHH[i] = DEQLAY / pHyd->Dlayer;
					else
						CorrHH[i] = 0.;

				}
			}
			pHyd->LayerOfDrain = pHyd->NL - 1;
		}
		else if (PhysicalDrainEq_Sw(pHyd->PhysicalDrainEq) == PhysicalDrainEq_Sw::Ernst_Model) {
			for (size_t i = pHyd->NL - 1; i != string::npos; i--) {
				CorrHH[i] = 0.;
				SUMR[i] = 0.;
				SUMV[i] = 0.;
				IR = 0;

				for (size_t ii = i; ii >= pHyd->LayerOfDrain; ii--) {
					if (pHyd->DepthLayers[ii] <= pHyd->Dlayer - pHyd->DLevel) {
						Dist = max(pHyd->RPipe, pHyd->DepthLayers[ii] + pHyd->DLevel - pHyd->RPipe);// ! to drain level
						SUMV[i] += pHyd->ThicknessLayers[ii] / (0.001*pHyd->M_SatTotCond[ii]);
						SUMR[i] += pHyd->DDist*log(pHyd->Dlayer / pHyd->RPipe) / (3.1415*0.001*pHyd->M_SatTotCond[ii]);
						IR++;
					}
				}

				if (pHyd->DepthMidpLayer[i] <= pHyd->Dlayer - pHyd->DLevel&& IR>0) {
					SUMR[i] /= IR;
					DD = min(1., max(0., (pHyd->DepthLayers[i] + pHyd->DLevel - pHyd->RPipe) / pHyd->Dlayer))*0.5*3.14159;
					SUMH[i] = pow(pHyd->DDist - cos(DD)*pHyd->Dlayer, 2.) / (8 * pHyd->M_SatTotCond[i] * 0.001*Dlayer);
					double HREF = pow(pHyd->DDist, 2) / (8 * pHyd->M_SatTotCond[i] * 0.001*pHyd->Dlayer);
					CorrHH[i] = HREF / (SUMH[i] + SUMR[i] + SUMV[i]);
				}
				else
					CorrHH[i] = 0.;
			}
			pHyd->LayerOfDrain = pHyd->NL - 1;
		}
	
	};
};
class SeepageFlow {
public:
	SeepageFlow(Soil_Drainage_Functions *pHyd) {
		DLevelLB = pHyd->DLevelLB; DDistLB = pHyd->DDistLB;
		WcondM = pHyd->f_CondSaturated(pHyd->NL - 1);
	}
	double operator() (double SatLevel) {
		double Head = max(SatLevel - DLevelLB, 0.);
		return 8 * WcondM*pow(Head, 2) / pow(DDistLB, 2);
	}

private:
	double DLevelLB, DDistLB, WcondM;
};
class DitchLevel {
public:
	DitchLevel(double DrainLevelMin) : DrainLevelMin(DrainLevelMin) {}
	double operator()	(double Runoff) {
		if (Runoff > 0.01)
			return	DrainLevelMin + Runoff*0.1;
		else
			return DrainLevelMin;
	}
private:
	double DrainLevelMin;
};

class UnitGradFlow {
public: 
	UnitGradFlow(Soil_Drainage_Functions* pDrain, Soil_HeatF *pHeat, ModelInfo* pModelInfo) : pDrain(pDrain), pHeat(pHeat), pModelInfo(pModelInfo) {}
	double operator() () {
		double WCondMean = pDrain->f_CondTheta(pDrain->NL - 1, pDrain->Theta.back(), pHeat->Temp.back());
		return min(WCondMean, pModelInfo->XFMAX);
}
private:
	Soil_Drainage_Functions* pDrain;
	Soil_HeatF* pHeat;
	ModelInfo *pModelInfo;
};
class NoDeepFlow {
public: NoDeepFlow() {}
double operator() () { return 0.; };
};
class FixedUnsaturated_WithReturn {
public:
	FixedUnsaturated_WithReturn(Soil_Drainage_Functions* pDrain): pDrain(pDrain) {
	ThetaFixed = pDrain->f_ThetaPsi_M(pDrain->NL - 1, pDrain->PsiBottom); 
	Thickness = pDrain->ThicknessLayers.back();
	}
double operator() () {
	return ((pDrain->Theta.back() - ThetaFixed)* Thickness * 10) / pDrain->T_Step;
}
private:
	Soil_Drainage_Functions* pDrain;
	double ThetaFixed;
	double Thickness;


};
class FixedUnsaturated_NoReturn {
public:
	FixedUnsaturated_NoReturn(Soil_Drainage_Functions* pDrain) : pDrain(pDrain) {
		ThetaFixed = pDrain->f_ThetaPsi_M(pDrain->NL - 1, pDrain->PsiBottom);
		Thickness = pDrain->ThicknessLayers.back();
	}
	double operator() () {
		return max(0.,((pDrain->Theta.back() - ThetaFixed)* Thickness * 10) / pDrain->T_Step);
	}
private:
	Soil_Drainage_Functions* pDrain;
	double ThetaFixed;
	double Thickness;


};
class CallingSeepageFlow {
public:
	CallingSeepageFlow(Soil_Drainage_Functions* pDrain): pDrain(pDrain) {}
	double operator() () { return pDrain->f_SeepageFlow(pDrain->SatLev); };

private:
	Soil_Drainage_Functions* pDrain;
};
class FractionOfUnitGradFlow {
public:
	FractionOfUnitGradFlow(Soil_Drainage_Functions* pDrain, Soil_HeatF *pHeat, ModelInfo* pModelInfo)  {
		f_UnitGradFlow = UnitGradFlow(pDrain, pHeat, pModelInfo); LowerB_GwGrad = pDrain->LowerB_GwGrad;
	}
	double operator() () {
		return LowerB_GwGrad*f_UnitGradFlow();
	}
private:
	double LowerB_GwGrad;
	function<double()> f_UnitGradFlow;
};
class DynamicGradFlow {
public:
	DynamicGradFlow(Soil_Drainage_Functions* pDrain, Soil_HeatF *pHeat):  pDrain(pDrain), pHeat(pHeat) {}
	double operator() () {
		//!C*************** Parameter gorverned dynamic Pressure level from DriveDrain file and saturation level  lower boundary        
		double Resistance = 0;
		//DO  i=NL,LayerOfSatLevel,-1
		size_t maxlayer = max(size_t(2), pDrain->LayerOfSatLevel);
		for (size_t i = pDrain->NL - 1; i >= maxlayer - 1; i--)
			Resistance += pDrain->ThicknessLayers[i] / pDrain->f_CondTheta(i, pDrain->Theta[i], pHeat->Temp[i]) + Resistance;

		if (Resistance > 1.E-5)
			return  max(0., (pDrain->SatLev - pDrain->vDriveDrainLevel) / Resistance);
		else
			return 0.;
}

private:
	Soil_Drainage_Functions* pDrain;
	Soil_HeatF* pHeat;
};

class DeepPercolation {
public:
	DeepPercolation(Structure *pModelStructure, Soil_Drainage_Functions* pDrain, Soil_HeatF *pHeat, ModelInfo* pModelInfo) : pDrain(pDrain) {
		if (GroundWaterFlow_Sw(pModelStructure->GroundWaterFlow) == GroundWaterFlow_Sw::off) {
			if (LBoundUnSaturated_Sw(pDrain->LBoundUnSaturated) == LBoundUnSaturated_Sw::Unit_Grad_Flow) 
				f_DeepDrain = UnitGradFlow(pDrain, pHeat, pModelInfo);
			else if (LBoundUnSaturated_Sw(pDrain->LBoundUnSaturated) == LBoundUnSaturated_Sw::No_Flow)
				f_DeepDrain = NoDeepFlow();
			else if (LBoundUnSaturated_Sw(pDrain->LBoundUnSaturated) == LBoundUnSaturated_Sw::Maximum_constant_psi)
				f_DeepDrain = FixedUnsaturated_NoReturn(pDrain);
			else 
				f_DeepDrain = FixedUnsaturated_WithReturn(pDrain);
		}
		else {
			if (LBoundSaturated_Sw(pDrain->LBoundSaturated) == LBoundSaturated_Sw::Unit_Grad_Flow) 
				f_DeepDrain = UnitGradFlow(pDrain, pHeat, pModelInfo);
			else if (LBoundSaturated_Sw(pDrain->LBoundSaturated) == LBoundSaturated_Sw::Seepage_Flow)
				f_DeepDrain = CallingSeepageFlow(pDrain);
			else if (LBoundSaturated_Sw(pDrain->LBoundSaturated) == LBoundSaturated_Sw::No_Flow)
				f_DeepDrain = NoDeepFlow();
			else if (LBoundSaturated_Sw(pDrain->LBoundSaturated) == LBoundSaturated_Sw::Static_Grad_Flow) 
				f_DeepDrain = FractionOfUnitGradFlow(pDrain, pHeat, pModelInfo);		
			else if (LBoundSaturated_Sw(pDrain->LBoundSaturated) == LBoundSaturated_Sw::Dynamic_Grad_Flow) 
				f_DeepDrain = DynamicGradFlow(pDrain, pHeat);
		}
	}
	void operator() () { pDrain->DeepPerc= f_DeepDrain(); };

private:
	Soil_Drainage_Functions* pDrain;
	function<double(void)> f_DeepDrain;


};


class DrainageFlux {

	void operator() () { pDrain->DeepPerc = f_DrainageFlux(); };

private:
	Soil_Drainage_Functions* pDrain;
	function<double(void)> f_DrainageFlux;

};



