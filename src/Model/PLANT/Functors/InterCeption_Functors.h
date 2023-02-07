#pragma once
#include "../../../std.h"
#include "../../../Util/StatUtil.h"
#include "../../../NewBase/PhysFunc.h"
#include "./PotTranspiration_Functors.h"
#include "../../Atmosphere/Irrigation.h"
#include "../../Structure/Functors/Structure_Functors.h"

enum class PrecInterception_Sw {OFF, ON};
enum class SnowInterception_Sw {OFF, ON};
enum class SnowIntUnload_Sw { THERMAL_QUALITY, AIR_TEMPERATURE};
enum class InterceptionModel_Sw {THRESHOLD, EXPONENTIAL};

class InterceptionCapacity {
public: InterceptionCapacity(double IntLAI, double IntBase): sense(IntLAI), base(IntBase) {}
		double operator() (double LAI) {return sense*LAI + base;}
		vector<double> operator() (vector<double> LAI) {
			vector<double> out;
			for_each(LAI.begin(), LAI.end(), [&](double &lai) {
				out.push_back(lai*sense + base);
			});
			return out;
		}
private:
	double sense, base;
};
class InterceptionCapacity_TA {
public: InterceptionCapacity_TA(double IntLAI, double IntBase, double IntLAIFrozen) : sense_ice(IntLAIFrozen) { f_IntBase = InterceptionCapacity(IntLAI, IntBase); }
		InterceptionCapacity_TA(Irrigation *pMet, double IntLAI, double IntBase, double IntLAIFrozen) : pMet(pMet), sense_ice(IntLAIFrozen) { f_IntBase = InterceptionCapacity(IntLAI, IntBase); }
		double operator() (double LAI, double TA) {
			return IntCap(LAI, TA);
		}
		vector<double> operator() (vector<double> LAI, double TA) {
			vector<double> out; size_t index=0;
			for_each(LAI.begin(), LAI.end(), [&](double &lai) {
				if (pMet->SnowReduceLAIFactor[index] >= pMet->SnowReduceLAIThreshold)
					out.push_back(IntCap(lai, TA));
				else
					out.push_back(0.);
				index++;
			});
			return out;
		}
private:
	double IntCap(double LAI, double TA) {
		if (TA < 0)
			return f_IntBase(LAI) + sense_ice*LAI;
		else
			return f_IntBase(LAI);
	};
	double sense_ice;
	function<double(double)> f_IntBase;
	Irrigation *pMet;
};
class InterceptionCapacity_THQUAL {
public: InterceptionCapacity_THQUAL(double IntLAI, double IntBase, double IntLAIFrozen) : sense_ice(IntLAIFrozen) { f_IntBase = InterceptionCapacity(IntLAI, IntBase); }
		InterceptionCapacity_THQUAL(Irrigation *pMet, double IntLAI, double IntBase, double IntLAIFrozen) : pMet(pMet), sense_ice(IntLAIFrozen) { f_IntBase = InterceptionCapacity(IntLAI, IntBase); }
		double operator() (double LAI, double ThQual) {
			return f_IntBase(LAI) + max(0., sense_ice*LAI*ThQual);
		}
		vector<double> operator() (vector<double> LAI, double ThQual) {
			vector<double> out; size_t index = 0;
			for_each(LAI.begin(), LAI.end(), [&](double &lai) {
				if (pMet->SnowReduceLAIFactor[index] >= pMet->SnowReduceLAIThreshold)
					out.push_back(f_IntBase(lai) + max(0., sense_ice*lai*ThQual));
				else
					out.push_back(0.);
				index++;

			});
			return out;
		}
private:
	double sense_ice;
	function<double(double)> f_IntBase;
	Irrigation *pMet;
};

class InterceptionCover {
public:
	InterceptionCover(vector<double> MaxCover, vector<double> LAICoverSens):MCover(MaxCover), CoverSens(LAICoverSens) {}
	double operator() (size_t index, double LAI) {
		if (LAI >= 0.&&LAI<100.)
			return MCover[index] * (1. - exp(-CoverSens[index] * LAI));
		else
			return 0.;
	}
private:
	vector<double> MCover, CoverSens;
};
class UnloadCoefSnow {
public: UnloadCoefSnow(double RainLim, double TiStep): RainLim(RainLim), TiStep(TiStep) {}
		double operator() (double PrecRate, double TAir) {
		if (TAir >= RainLim)
			return 1.0;
		else
			return 0.7*(1.0 + PrecRate / 140.0*max(0., min(1., (TiStep*24. - 1.) / 23.)));
}
private: 
	double RainLim, TiStep;
};
class UnloadInputRate {
public:
	UnloadInputRate(double RainLim, double TiStep, vector<double> MaxCover, vector<double> LAICoverSens) {
		f_UnloadCoef = UnloadCoefSnow(RainLim, TiStep); f_Cover= InterceptionCover(MaxCover, LAICoverSens);
	}
	double operator() (size_t index, double Precip, double LAI, double TAir, double StorageCapacity) {
		if (Precip > 0)
			return f_UnloadCoef(Precip, TAir)*(1 - exp(-Precip*f_Cover(index, LAI / 2) / StorageCapacity));
		else
			return 1.;
	}
private:
	function<double(double, double)> f_UnloadCoef;
	function<double(size_t, double)> f_Cover;
};
class MultiFillByPass {
public: MultiFillByPass(bool ModelOption, double RainLim, double TiStep, vector<double> MaxCover, vector<double> LAICoverSens): ModelOption(ModelOption), TiStep(TiStep) {
	f_Cover = InterceptionCover(MaxCover, LAICoverSens); f_UnloadFraction = UnloadInputRate(RainLim, TiStep, MaxCover, LAICoverSens);
}
	double operator() (vector<double> *pMC_IntStorage,  vector<size_t> *pIndexList, vector<double> *pMC_IntCapacity, vector<double> LAI, double TAir, double ThroughFall) {
		double In, ByPass, Deficit, ActualCapacity, IntStartStorage;
		size_t IndexP;
		bool Below;
		double FractionOfDirectThroughfall, DirectThroughfall_Flux;
		FractionOfDirectThroughfall = 1.0;
		In = ThroughFall;
		if (In > 0.) {
			int koll = 0;


		}
		IntStartStorage = sum(pMC_IntStorage);
		vector<bool> hit; hit.assign(pMC_IntStorage->size(), false);


		for (size_t i = 0; i < pIndexList->size(); i++) {
			IndexP = pIndexList->at(i);

			if (!hit[IndexP]) {
				hit[IndexP] = true;
				Below = false;
			}
			else
				Below = true;
	
			if (Below) {
				Deficit = (pMC_IntCapacity->at(IndexP) - pMC_IntStorage->at(IndexP));
				ActualCapacity = pMC_IntCapacity->at(IndexP);
			}
			else {
				Deficit = (pMC_IntCapacity->at(IndexP) - pMC_IntStorage->at(IndexP))*0.5;
				ActualCapacity = pMC_IntCapacity->at(IndexP) * 0.5;
			}
			//! Interception unloading i.e. the Capacity of storing water will decrease
			if (Deficit>0.) {
				if (ModelOption && ActualCapacity>0.)  // ! PEJ Added ActualCapacity condition
					Deficit = f_UnloadFraction(IndexP, In, LAI[IndexP], TAir, ActualCapacity)*Deficit;
			}
			//     ! Bypass removal
			if (In>0.) {
				ByPass = In*(1. - f_Cover(IndexP, LAI[IndexP]));
				In = In - ByPass;
			}
			else
				ByPass = 0.;

			DirectThroughfall_Flux = FractionOfDirectThroughfall * ByPass;
			if (In >= Deficit) {
				if (Deficit>0.0)
					DirectThroughfall_Flux += FractionOfDirectThroughfall * (In - Deficit);
				else
					DirectThroughfall_Flux += FractionOfDirectThroughfall * In;
			}
			//! Interception
			if (Deficit<0.) { //! an excess of water unloading
				pMC_IntStorage->at(IndexP) += Deficit;
				In -= Deficit;
			}
			else if (In*TiStep >= Deficit) {
				pMC_IntStorage->at(IndexP) +=  Deficit;
				In -= Deficit/TiStep;
			}
			else if (In>0.) {
				pMC_IntStorage->at(IndexP) += In*TiStep;
				In = 0;
			}
			In = In + ByPass;//   ! Adding back bypass       
							 //! Update FractionOFDirectTHroughfall
			if (In>0.0)
				FractionOfDirectThroughfall = DirectThroughfall_Flux / In;
			else
				FractionOfDirectThroughfall = 0.0;
		}
		ThroughFall += (IntStartStorage - sum(pMC_IntStorage)) / TiStep;
		return ThroughFall;
	}
private:
	function<double(size_t, double) > f_Cover;
	function<double(size_t, double, double, double, double)> f_UnloadFraction;
	bool ModelOption;
	double TiStep;
};
class ThroughFallRateWithoutPlants {
public: ThroughFallRateWithoutPlants(Irrigation *pMet) : pMet(pMet) {}
	double operator() () {
		return pMet->PrecCorr + pMet->IrrigationRate*(1 - pMet->SoilIrrigF);
	}
private:
	Irrigation *pMet;
};
class ThroughFallRateSinglePlant {
public: ThroughFallRateSinglePlant(double DirectThroughfall, double TiStep, Irrigation *pMet):DirectFraction(DirectThroughfall), tstep(TiStep) {
	f_PrecInput = ThroughFallRateWithoutPlants(pMet);

}
double operator() (double IntCapacity, double IntStorage) {
	return max((f_PrecInput()*tstep)*(1. - DirectFraction) - IntCapacity + IntStorage, 0.) / tstep +
		DirectFraction*(f_PrecInput());
}
private:
	double DirectFraction;
	double tstep;
	function<double()> f_PrecInput;
};
class InterceptionStorageSinglePlant {
public: InterceptionStorageSinglePlant(double DirectThroughfall, double TiStep, Irrigation *pMet) :DirectFraction(DirectThroughfall), tstep(TiStep) {
	f_PrecInput = ThroughFallRateWithoutPlants(pMet);
}
	double operator() (double IntCapacity, double IntStorage) {
		return min(IntCapacity, f_PrecInput()*tstep*(1.-DirectFraction) + IntStorage);
	}
private:
	double DirectFraction;
	double tstep;
	function<double()> f_PrecInput;
};
class InterceptionEvaporationSinglePlant {
public:
	InterceptionEvaporationSinglePlant(double IntEvapFracMin, double EpRatio, double TiStep) : FracMin(IntEvapFracMin), tstep(TiStep), EpRatio(EpRatio) {}
	InterceptionEvaporationSinglePlant(double IntEvapFracMin, double TiStep) : FracMin(IntEvapFracMin), tstep(TiStep) {}
	double operator() (double PotTransp,double IntStorage, double IntCapacity) {
		return EvaporationRate(IntStorage, IntCapacity, PotTransp, EpRatio);
	}
	double operator() (double PotTransp, double IntStorage, double IntCapacity,  double EpRatio) {
		return EvaporationRate(IntStorage, IntCapacity, PotTransp, EpRatio);
	}
private:
	double EvaporationRate(double IntStorage, double IntCapacity, double PotTransp, double EpRatio) {
		double EvapFrac = 1.;
		if (PotTransp>0.&&IntCapacity>0.)	EvapFrac = min(1.0, max(FracMin, IntStorage / IntCapacity));
		return min(IntStorage / tstep, PotTransp*EpRatio*EvapFrac);
	}
	double tstep;
	double FracMin;
	double EpRatio;
};
class InterceptionEvaporationMultiPlant {
public: InterceptionEvaporationMultiPlant(double IntEvapFracMin, double MaxCondensation, double TiStep) : FracMin(IntEvapFracMin), MaxCondensation(MaxCondensation), tstep(TiStep) {}

		vector<double> operator() (vector<double> MC_IntStorage, vector<double> MC_IntCapacity, vector<double> MC_EvaPot) {
			vector<double> Evap;
			size_t index = 0;
			double IntFrac;
			for_each(MC_EvaPot.begin(), MC_EvaPot.end(), [&](double &Evapot) {
				if (Evapot > 0) {
					if (MC_IntCapacity[index] > 0)
						IntFrac = min(1., max(FracMin, MC_IntStorage[index] / MC_IntCapacity[index]));
					else
						IntFrac = FracMin;									
				}
				else {
					Evapot = max(-MaxCondensation, Evapot);
					IntFrac = 1.;
				}
				Evap.push_back(min(max(0., MC_IntStorage[index] / tstep), Evapot*IntFrac));
				index++;
			});
			return Evap;
		}
private:
	double tstep;
	double FracMin;
	double MaxCondensation;

};
class PotentialWaterUptake {
public:
	PotentialWaterUptake(double EpRatio): EpRatio(EpRatio) {}
	PotentialWaterUptake() {}
	double operator() (double PotTransp, double EvaActInt) {
		return _PotentialWaterUptakeRate(PotTransp, EvaActInt, EpRatio);
	}
	double operator() (double PotTransp, double EvaActInt, double EpRatio) {
		return _PotentialWaterUptakeRate(PotTransp, EvaActInt, EpRatio);
	}
	vector<double> operator() (vector<double> PotTransp, vector<double> EvaActInt, vector<double> PotIntercept) {
		vector<double> PotUptake;
		for (size_t i = 0; i < PotTransp.size(); i++) {
			if (PotTransp[i] > 0 && EvaActInt[i] > 0) 
				PotUptake.push_back(PotTransp[i] - EvaActInt[i] / (PotIntercept[i] / PotTransp[i]));	
			else
				PotUptake.push_back(max(0., PotTransp[i]));
		}
		return PotUptake;
	}

private:
	double _PotentialWaterUptakeRate(double PotTransp, double EvaActInt, double EpRatio) {
		PotTransp = max(0.001, PotTransp);
		return max(0.001, min(PotTransp, PotTransp - abs(EvaActInt) / EpRatio));
	}
	double EpRatio;
};

class PotentialEvaporation_Interception {
public: PotentialEvaporation_Interception(Irrigation *pMet, double IntRS) : pMet(pMet), IntRS(IntRS) {
	f_PM_Equation = Physf(FUNC_CHOICE::PM);
}
	vector<double> operator() (vector<double> ResAir, vector<double> LAI) {
		vector<double> PotEvap;
		for (size_t i = 0; i < pMet->MC_RntDist.size(); i++) {
			if (pMet->SnowReduceLAIFactor[i] >= pMet->SnowReduceLAIThreshold)
				PotEvap.push_back(f_PM_Equation(pMet->TAir, pMet->MC_RntDist[i], pMet->VapourPDef, ResAir[i], IntRS, LAI[i]));
			else
				PotEvap.push_back(0.);
		}
		return PotEvap;
}
private:
	Irrigation *pMet;
	function<double(double, double, double, double, double, double)> f_PM_Equation;
	double IntRS;
};

class ThermalQualityIntceptedWater {
public:
	ThermalQualityIntceptedWater(Irrigation* pMet, double IntSnowMeltScale) : pMet(pMet), IntSnowMeltScale(IntSnowMeltScale) {}

	double operator() (double IntThQual, double IntStorage, double LAI_Total) {
		return ThermalQualityInterceptedWater(pMet->TAir, pMet->RadInShort, IntThQual, IntStorage, LAI_Total);
	}
private:
	double ThermalQualityInterceptedWater(double TAir, double RIS, double IntThQual, double IntStorage, double LAI_Total) {
		if (IntStorage > 0) {
			double Thick = IntStorage*Fix::DENSW / (Fix::DENSW*(1. - IntThQual) + 100 * IntThQual) / LAI_Total;
			double IntMelt = IntSnowMeltScale*pMet->f_SnowMelt(TAir, RIS,0., Thick, 0.);
			return max(0., min(1., (IntThQual*IntStorage - IntMelt) / IntStorage));
	}
		else
			return 0.;
	}
	Irrigation *pMet;
	double IntSnowMeltScale;
};
class ThermalQualityThroughFall {
public: ThermalQualityThroughFall(Irrigation *pMet){}
	void operator() (double IntThQual, double ThroughFall, double DirectThroughFall) {
		pMet->ThQualP = IntThQual*(ThroughFall - DirectThroughFall) / ThroughFall + pMet->ThQualP*DirectThroughFall / ThroughFall;
	}
private:
	Irrigation *pMet;
};

