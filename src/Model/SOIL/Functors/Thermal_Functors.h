#pragma once

#include "../../SimModule.h"
#include "./Profile_Functors.h"
//#include "../Soil_HeatF.h"
//#include "../ThermalCoef.h"
enum class ThCondFunction_Sw{ Kersten,Balland_Arp };
struct KERSTEN_COEF {
	double UC1, UC2, UC3;
	double FC1, FC2, FC3, FC4;
};
class DryBulkDensity {
public: DryBulkDensity(vector<double> Meas_Saturation,
	vector<double> UpperDepth, vector<double> LowerDepth, vector<double> DepthLayers, vector<double> ThicknessLayers, vector<double> DepthMidpLayer) {
	function<void(vector<double>&, vector<double>&, vector<double>& , bool)> f_ModelLayers = FromMeasuredLayersToModelLayers(UpperDepth, LowerDepth, DepthLayers, ThicknessLayers, DepthMidpLayer);
	vector<double> M_Saturation, B_Saturation;
	f_ModelLayers(Meas_Saturation, M_Saturation, B_Saturation, true);
	for (size_t i = 0; i < B_Saturation.size(); i++) {
		DryBulkDens.push_back((100 - B_Saturation[i]) / 100.*Fix::DensityofSolidSoil);
		M_DryBulkDens.push_back((100 - M_Saturation[i]) / 100.*Fix::DensityofSolidSoil);
	}
}
	vector<double> operator() (bool Boundary) {
		if (Boundary)
			return DryBulkDens;
		else
			return M_DryBulkDens;
}
private:
	vector<double> DryBulkDens, M_DryBulkDens;
};
class HeatCapacitySolid {  // J/m2 - scaled to model layers
public: HeatCapacitySolid(vector<double> M_Saturation, vector<double> ThicknessLayers) {
	for (size_t i = 0; i < M_Saturation.size(); i++)  HeatCapSolid.push_back(Fix::HeatCapSolidSoil*(100. - M_Saturation[i]) / 100 * ThicknessLayers[i]);
	}
	vector<double> operator() () { return HeatCapSolid; }
	double operator() (size_t index, double Water) { return HeatCapSolid[index]+Water*Fix::SPECW; }

private:
	vector<double> HeatCapSolid;
};

class SoilHeatCapacity {
public: SoilHeatCapacity(vector<double> HeatCapacitySolid_Layer, vector<double> ThicknessLayers) {

	for (size_t i = 0; i < HeatCapacitySolid_Layer.size(); i++)
		HeatCapacity_Vol.push_back(HeatCapacitySolid_Layer[i] / ThicknessLayers[i]);
}
		double operator() (size_t index, double Theta, double ThQual) {
			return HeatCapacity_Vol[index] + ThQual*Theta*Fix::SPECI+Theta*(1-ThQual)*Fix::SPECW;
}
private:
	vector<double> HeatCapacity_Vol;
};

class NonFrozenWater {
public: NonFrozenWater(double FpointFWi, vector<double> M_Wilting, vector<double> ThicknessLayer )  {
	for (size_t i = 0; i < M_Wilting.size(); i++) ScaledValue.push_back(M_Wilting[i] * ThicknessLayer[i] * 10.*FpointFWi);
}
double operator() (size_t index, double MassWater) {return min(ScaledValue[index], MassWater);}
private:
	vector<double> ScaledValue;
};


enum class FreezingStartOption{Zero, FixedParam, Dynamic };
class FreezingStartTemperature {
public:FreezingStartTemperature(size_t option, double FreezingStartTemp, double OsmoticScaleSens) :FreezingStartTemp(FreezingStartTemp) {
	Choice = FreezingStartOption(option); OsmoticScalingFactor = pow(10., OsmoticScaleSens)*Fix::TempToOsmoticP;
}
	double operator() () {
	if (Choice == FreezingStartOption::Zero)
		return 0.;
	else 
		return FreezingStartTemp;
}
	double operator() (double OsmoticPressure) { return OsmoticScalingFactor*OsmoticPressure; }

private:

	FreezingStartOption Choice;
	double FreezingStartTemp;
	double OsmoticScalingFactor;
};



class SoilThermalQuality {
public:SoilThermalQuality(size_t option, double FreezingStartTemp, double OsmoticScaleSens, double FpointFWi, double FpointF0, double FpointF1,
	vector<double> Lambda,
	vector<double> M_Saturation, vector<double> M_Wilting, vector<double> ThicknessLayers) {
	Choice = FreezingStartOption(option);
	for (size_t i = 0; i < Lambda.size(); i++) PowerCoef.push_back(FpointF0 + FpointF1*Lambda[i]);
	f_StartTempStatic = FreezingStartTemperature(option, FreezingStartTemp, OsmoticScaleSens);
	f_StartTempDynamic = FreezingStartTemperature(option, FreezingStartTemp, OsmoticScaleSens);
	f_HeatCapSoilLiquidWater = HeatCapacitySolid(M_Saturation, ThicknessLayers);
	f_NonFrozenWater = NonFrozenWater(FpointFWi, M_Wilting, ThicknessLayers);
	thicknessupperlayer = ThicknessLayers[0];
}
// pair return two double - First Temperature , Second ThQual
	pair<double, double> operator() (double SoilHeat, double Theta) {
		  pair<double, double> out;
		  size_t index = 0;
		  double WatV;
		  SoilHeat *= thicknessupperlayer;
		  WatV = thicknessupperlayer * Theta*10.;
		  double HeatCapUnfrozen = f_HeatCapSoilLiquidWater(index, WatV);
		  double HeatUnFrozen = f_StartTempStatic() * HeatCapUnfrozen;
		  if (SoilHeat < HeatUnFrozen) {
			  double NonFrozenWat = f_NonFrozenWater(index, WatV);
			  double MaxIceAmount = max(0., WatV - NonFrozenWat);
			  double HeatCapSoilWaterIceMixture = f_HeatCapSoilLiquidWater(index, NonFrozenWat) + MaxIceAmount * Fix::SPECI;
			  double HeatCompleteFrozen = -Fix::LATMELT * MaxIceAmount + (Fix::TempCompFrozenSoil - f_StartTempStatic()) * HeatCapSoilWaterIceMixture + f_StartTempStatic() * HeatCapUnfrozen;
			  if (SoilHeat > HeatCompleteFrozen) {
				  double SensibleHeat = SensibleHeatFunc(index, SoilHeat - HeatUnFrozen, HeatCompleteFrozen, -MaxIceAmount * Fix::LATMELT, -WatV * Fix::LATMELT);
				  out.first = SensibleHeat / HeatCapSoilWaterIceMixture + f_StartTempStatic();
				  out.second = min(1., (SoilHeat - HeatUnFrozen - SensibleHeat) / -WatV * Fix::LATMELT);
			  }
			  else { // Completely Frozen
				  out.first = (SoilHeat + Fix::LATMELT * MaxIceAmount) / HeatCapSoilWaterIceMixture;
				  out.second = min(1., MaxIceAmount / WatV);
			  }
		  }
		  else {// Unfrozen soil
			  out.first = SoilHeat / HeatCapUnfrozen;
			  out.second = 0.;
		  }
		  return out;
	  }
	pair<double, double> operator() (double SoilHeat, double Theta, double OsmoticPressure) {
		pair<double, double> out;
		size_t index = 0;
		double WatV;
		SoilHeat *= thicknessupperlayer;
		WatV = thicknessupperlayer * Theta*10.;
		double HeatCapUnfrozen = f_HeatCapSoilLiquidWater(index, WatV);
		double HeatUnFrozen = f_StartTempDynamic(OsmoticPressure) * HeatCapUnfrozen;
		if (SoilHeat < HeatUnFrozen) {
			double NonFrozenWat = f_NonFrozenWater(index, WatV);
			double MaxIceAmount = max(0., WatV - NonFrozenWat);
			double HeatCapSoilWaterIceMixture = f_HeatCapSoilLiquidWater(index, NonFrozenWat) + MaxIceAmount * Fix::SPECI;
			double HeatCompleteFrozen = -Fix::LATMELT * MaxIceAmount + (Fix::TempCompFrozenSoil - f_StartTempDynamic(OsmoticPressure)) * HeatCapSoilWaterIceMixture + f_StartTempDynamic(OsmoticPressure) * HeatCapUnfrozen;
			if (SoilHeat > HeatCompleteFrozen) {
				double SensibleHeat = SensibleHeatFunc(index, SoilHeat - HeatUnFrozen, HeatCompleteFrozen, -MaxIceAmount * Fix::LATMELT, -WatV * Fix::LATMELT);
				out.first = SensibleHeat / HeatCapSoilWaterIceMixture + f_StartTempDynamic(OsmoticPressure);
				out.second = min(1., (SoilHeat - HeatUnFrozen - SensibleHeat) / -WatV * Fix::LATMELT);
			}
			else { // Completely Frozen
				out.first = (SoilHeat + Fix::LATMELT * MaxIceAmount) / HeatCapSoilWaterIceMixture;
				out.second = min(1., MaxIceAmount / WatV);
			}
		}
		else {// Unfrozen soil
			out.first = SoilHeat / HeatCapUnfrozen;
			out.second = 0.;
		}
		return out;
	}
	pair<double,double> operator() (size_t index, double SoilHeat, double WatV) {
		   pair<double,double> out;
		   double HeatCapUnfrozen = f_HeatCapSoilLiquidWater(index, WatV);
		   double HeatUnFrozen = f_StartTempStatic()*HeatCapUnfrozen;
		   if (SoilHeat < HeatUnFrozen) {
			   double NonFrozenWat= f_NonFrozenWater(index, WatV);
			   double MaxIceAmount = max(0., WatV - NonFrozenWat);
			   double HeatCapSoilWaterIceMixture = f_HeatCapSoilLiquidWater(index, NonFrozenWat) + MaxIceAmount*Fix::SPECI;
			   double HeatCompleteFrozen = -Fix::LATMELT*MaxIceAmount + (Fix::TempCompFrozenSoil - f_StartTempStatic())*HeatCapSoilWaterIceMixture + f_StartTempStatic()*HeatCapUnfrozen;
			   if (SoilHeat > HeatCompleteFrozen) {
				   double SensibleHeat = SensibleHeatFunc(index, SoilHeat-HeatUnFrozen, HeatCompleteFrozen, -MaxIceAmount*Fix::LATMELT, -WatV*Fix::LATMELT);
				   out.first=SensibleHeat / HeatCapSoilWaterIceMixture + f_StartTempStatic();
				   out.second=min(1., (SoilHeat - HeatUnFrozen - SensibleHeat) / -WatV*Fix::LATMELT);
			   }
			   else { // Completely Frozen
				   out.first=(SoilHeat + Fix::LATMELT*MaxIceAmount) / HeatCapSoilWaterIceMixture;
				   out.second=min(1., MaxIceAmount / WatV);
			   }
		   }
		   else {// Unfrozen soil
			   out.first=SoilHeat / HeatCapUnfrozen;
			   out.second=0.;
		   }
		   return out;
	}
	pair<double,double> operator() (size_t index, double SoilHeat, double WatV, double OsmoticPressure) {
		   pair<double,double> out;
		   double HeatCapUnfrozen = f_HeatCapSoilLiquidWater(index, WatV);
		   double HeatUnFrozen = f_StartTempDynamic( OsmoticPressure)*HeatCapUnfrozen;
		   if (SoilHeat < HeatUnFrozen) {
			   double NonFrozenWat = f_NonFrozenWater(index, WatV);
			   double MaxIceAmount = max(0., WatV - NonFrozenWat);
			   double HeatCapSoilWaterIceMixture = f_HeatCapSoilLiquidWater(index, NonFrozenWat) + MaxIceAmount*Fix::SPECI;
			   double HeatCompleteFrozen = -Fix::LATMELT*MaxIceAmount + (Fix::TempCompFrozenSoil - f_StartTempDynamic(OsmoticPressure))*HeatCapSoilWaterIceMixture + f_StartTempDynamic(OsmoticPressure)*HeatCapUnfrozen;
			   if (SoilHeat > HeatCompleteFrozen) {
				   double SensibleHeat = SensibleHeatFunc(index, SoilHeat - HeatUnFrozen, HeatCompleteFrozen, -MaxIceAmount*Fix::LATMELT, -WatV*Fix::LATMELT);
				   out.first=SensibleHeat / HeatCapSoilWaterIceMixture + f_StartTempDynamic(OsmoticPressure);
				   out.second=min(1., (SoilHeat - HeatUnFrozen - SensibleHeat) / -WatV*Fix::LATMELT);
			   }
			   else { // Completely Frozen
				   out.first=(SoilHeat + Fix::LATMELT*MaxIceAmount) / HeatCapSoilWaterIceMixture;
				   out.second=min(1., MaxIceAmount / WatV);
			   }
		   }
		   else {// Unfrozen soil
			   out.first=SoilHeat / HeatCapUnfrozen;
			   out.second=0.;
		   }
		   return out;
	   }

	double operator() (bool inverse, size_t index, double Temp, double WatV) {
		double   NonFrozenWat, MFWat, HeatCap, EFrost, Q, ELOWER, EUPPER, RatioLatent, SensibleHeat, TempLocal, EMiddle;
		double TempFrozen;
		if (Temp <= 0.) {
			//!  NON-FREEZ WAT
			NonFrozenWat = f_NonFrozenWater(index, WatV);
			//!C ******   FROZEN WATER <MM>
			MFWat = WatV - NonFrozenWat;
			HeatCap = f_HeatCapSoilLiquidWater(index, NonFrozenWat) + MFWat*Fix::SPECI ;
			if (Temp <= Fix::TempCompFrozenSoil)
				TempFrozen = HeatCap*Temp - Fix::LATMELT*MFWat;
			else {
				//!			david=0
				//!			if(david.gt.0)then
				//!			TempFrozen=-1.*(Tempv/TempCompFrozenSoil)**(1/(M_Lambda[index]*FpointF1)+FpointF0) &
				//!	                     *LATMELT*MFWat+HeatCap*Tempv
				//!			else
				//			! iterative solution (simple but stable bisection method)
				//			! 50 iterations takes approximately 0.1 second computation time
				EFrost = HeatCap*Fix::TempCompFrozenSoil - Fix::LATMELT*MFWat;
				ELOWER = EFrost;
				EUPPER = 0.0;
				EMiddle = (EUPPER + ELOWER)*0.5;
				for (int j = 0; j<50; j++) {
					//	! freezing point depression

					Q = (1 - pow(EMiddle / EFrost, PowerCoef[index])) *
						min(1., (EFrost - EMiddle) / (EFrost + Fix::LATMELT*MFWat));
					//! ratio of latent heat to total heat at Tf
					RatioLatent = -Fix::LATMELT*MFWat / EFrost;
					//! sensible heat at any heat storage
					SensibleHeat = EMiddle - RatioLatent*(1 - Q)*EMiddle - Q*EMiddle;
					TempLocal = SensibleHeat / HeatCap;
					//! change estimate of heat storage if TempLocal too high or low
					if ((TempLocal - Temp) >= 0.0) {
						EUPPER = EMiddle;
						EMiddle = (EUPPER + ELOWER)*0.5;
					}
					else {
						ELOWER = EMiddle;
						EMiddle = (EUPPER + ELOWER)*0.5;
					}
				}
				TempFrozen = EMiddle;
			}
		}
		else {
			HeatCap = f_HeatCapSoilLiquidWater(index, WatV);
			TempFrozen = Temp*HeatCap;
		}
		return TempFrozen;
}

private:
	double SensibleHeatFunc(size_t index, double HeatDiff, double HeatCompleteFrozen, double HeatMaxIce, double HeatNoUnfrozenWater) {
		double Q = (1 - pow(HeatDiff / HeatCompleteFrozen, PowerCoef[index]))
			*min(1., (HeatCompleteFrozen - HeatDiff) / (HeatCompleteFrozen - HeatMaxIce));
		double RatioLatent = HeatMaxIce / HeatCompleteFrozen;
		return HeatDiff - RatioLatent*HeatDiff - (1 - RatioLatent)*Q*HeatDiff;
	}
	double TempFunc(double SensibleHeat, double HeatCapSoilIceMixture, double FreezingStartTemperature) {
		return SensibleHeat / HeatCapSoilIceMixture + FreezingStartTemperature;
	}
	double ThermalQuality(double SensibleHeat, double HeatDiff, double HeatNoUnfrozenWater) {
		return min(1., (HeatDiff - SensibleHeat) / HeatNoUnfrozenWater);
	}
	FreezingStartOption Choice;
	vector<double> PowerCoef;
	double thicknessupperlayer;

	function<double(void)> f_StartTempStatic;
	function<double(double)> f_StartTempDynamic;
	function<double(size_t, double)> f_NonFrozenWater;
	function<double(size_t, double)> f_HeatCapSoilLiquidWater;
};


class SoilSurfaceHeatFlowDamping {
public:
	SoilSurfaceHeatFlowDamping(double sens_coef, double max_relConduction) : coef(sens_coef), max_relCond(max_relConduction) {
}
	double operator() (double SurfaceTemp) {
		if (SurfaceTemp >= 0)
			return 1.;
		else
			return exp(coef*SurfaceTemp)*max_relCond + (1 - max_relCond);
}
private:
	double coef, max_relCond;
};


class ThCond_KerstenEq {
public:
	ThCond_KerstenEq(double uc1, double uc2, double uc3,double fc1, double fc2, double fc3, double fc4, vector<double> bd) :
	c1(uc1), c2(uc2), c3(uc3), fc1(fc1), fc2(fc2), fc3(fc3), fc4(fc4), bd(bd) {}
	ThCond_KerstenEq(double uc1, double uc2, double uc3, double fc1, double fc2, double fc3, double fc4) :
		c1(uc1), c2(uc2), c3(uc3), fc1(fc1), fc2(fc2), fc3(fc3), fc4(fc4) {}
	double operator() (size_t index, double Theta, double ThQual) {
		double ThetaW;
		ThetaW = max(.5, Theta / bd[index]);

		double THCOND = (c1*log10(ThetaW) + c2)*pow(10, c3*bd[index]);
		if (ThQual>0.0001) {
			double ThCondF = (fc1*pow(10., fc2*bd[index])+ fc3*pow(10., fc4*bd[index])*ThetaW);
			THCOND = ThQual*ThCondF + (1 - ThQual)*THCOND;
		}
		return max(min(10.,THCOND), 0.02);
	}
	double operator() (double Theta, double BulkDensity, double ThQual) {
		double ThetaW;
		ThetaW = max(.5, Theta / BulkDensity);

		double THCOND = (c1*log10(ThetaW) + c2)*pow(10, c3*BulkDensity);
		if (ThQual>0.0001) {
			double ThCondF = (fc1*pow(10., fc2*BulkDensity) + fc3*pow(10., fc4*BulkDensity)*ThetaW);
			THCOND = ThQual*ThCondF + (1 - ThQual)*THCOND;
		}
		return max(min(10.,THCOND), 0.02);
	}
private:
	double c1, c2, c3, fc1, fc2, fc3, fc4;
	vector<double> bd;
};
class ThCond_Org {
public:
	ThCond_Org(double c1, double c2, double cfrozen) : c1(c1), c2(c2), cfrozen(cfrozen) {}
	double operator() (double Theta, double ThQual) {
		double THCOND = c1 + c2*Theta;
		THCOND = cfrozen*ThQual*THCOND*pow(Theta / 100, 2) + THCOND;
		return THCOND;
	}
private:
	double c1, c2, cfrozen;
};

enum class SoilType {Org, Sand, Clay};
class ThCond_Kersten{
public: ThCond_Kersten(double OrganicLayerThickness,  double c1, double c2, double cfrozen,KERSTEN_COEF Clay, KERSTEN_COEF Sand, vector<double> BulkDensity,
	vector<double> ThScaleLog ,vector<double> DepthMidpLayer, vector<double> DistanceMidpLayers, vector<double> B_Lambda, vector<double> M_Wilting) {


	f_Org = ThCond_Org(c1, c2, cfrozen);
	f_Sand = ThCond_KerstenEq(Sand.UC1, Sand.UC2, Sand.UC3, Sand.FC1, Sand.FC2, Sand.FC3, Sand.FC4, BulkDensity);
	f_Clay = ThCond_KerstenEq(Clay.UC1, Clay.UC2, Clay.UC3, Clay.FC1, Clay.FC2, Clay.FC3, Clay.FC4, BulkDensity);
	HumusMixedBoundaryId = 0;
	FractionOfOrgMixedBoundary = 0.;
	if (OrganicLayerThickness>0.&&OrganicLayerThickness<DepthMidpLayer.front()) {
		HumusMixedBoundaryId = 0;
		FractionOfOrgMixedBoundary = OrganicLayerThickness /DepthMidpLayer.front();
	}
	m_SoilTypes.assign(DepthMidpLayer.size(), SoilType::Sand);
	for (size_t i = 0; i < m_SoilTypes.size(); i++) {
		//!		Check for Soil type
		if (OrganicLayerThickness > DepthMidpLayer[i])
			m_SoilTypes[i] = SoilType::Org;		
		else if (B_Lambda[i]<.1 || M_Wilting[i]>7.)
			m_SoilTypes[i] = SoilType::Clay;
		else
			m_SoilTypes[i] = SoilType::Sand;//

		if (i < m_SoilTypes.size() - 1) {
			if (OrganicLayerThickness >=DepthMidpLayer[i] &&
				OrganicLayerThickness < DepthMidpLayer[i + 1]) {
				HumusMixedBoundaryId = i + 1;
				FractionOfOrgMixedBoundary = (OrganicLayerThickness - DepthMidpLayer[i]) /DistanceMidpLayers[i + 1];
			}
		}
	}
	ThScale.resize(ThScaleLog.size());
	for (size_t i = 0; i<ThScaleLog.size(); i++) {
		ThScale[i] = pow(10., ThScaleLog[i]);
	}
}
	double operator() (size_t index, double Theta, double ThQual) {	return ThCond(index, Theta, ThQual);}
	void operator() (vector<double> *ThCondV, vector<double> *Theta, vector<double> *ThQual) { for (size_t i = 0; i < ThCondV->size(); i++) { ThCondV->at(i)=ThCond(i, Theta->at(i), ThQual->at(i)); }	}

private:
	double ThCond(size_t index, double Theta, double ThQual) {
		SoilType IChoice;
		double ThMineral, THCOND;
		bool Mixing = true;
		ThMineral = THCOND = 0.1;
		IChoice = m_SoilTypes[index];
		while (Mixing) {
			switch (IChoice) {
			case SoilType::Org:
				THCOND = f_Org(Theta, ThQual);
				break;
			case SoilType::Sand:
				THCOND = f_Sand(index, Theta, ThQual);
				break;
			case SoilType::Clay:
				THCOND = f_Clay(index, Theta, ThQual);
			}
			if (index != HumusMixedBoundaryId || (IChoice == SoilType::Org && HumusMixedBoundaryId == 0) || FractionOfOrgMixedBoundary <= 0.001)
				Mixing = false;
			else if (Mixing&&IChoice != SoilType::Org) {
				ThMineral = THCOND;
				IChoice = SoilType::Org;
			}
			else {
				THCOND = 1. / (FractionOfOrgMixedBoundary / THCOND + (1 - FractionOfOrgMixedBoundary) / ThMineral);
				Mixing = false;
			}
		}
		return max(0.01, min(THCOND*ThScale[index], 10.));
	}
	function<double(double, double)> f_Org;
	function<double(size_t, double, double)> f_Sand, f_Clay;
	vector<double> ThScale;
	size_t HumusMixedBoundaryId;
	double FractionOfOrgMixedBoundary;
	vector<SoilType> m_SoilTypes;

};

class ThSolidFrac {
public: ThSolidFrac(double ThermalCond, double Frac) : Cond(ThermalCond), Frac(Frac) {
	out = pow(ThermalCond, Frac);
}
	double operator() () {return out;}
private:
	double Cond,Frac, out;
};
class KerstenFactor {
public:
	KerstenFactor(double Th_Beta, vector<double> SolidVol_FracOrg, vector<double> BallardArpCoef, vector<double> Saturation) : Th_Beta(Th_Beta), m_SolidVol_FracOrg(SolidVol_FracOrg),
		m_BallardArpCoef(BallardArpCoef), m_Saturation(Saturation) { }
	double operator() (size_t index, double Theta, double ThQual) {
		double RelSat;
		if (m_SolidVol_FracOrg.size() <= index) return 0.;
		RelSat = min(1., Theta / m_Saturation[index]);
		if (ThQual <= 0) {
			if (RelSat >= 1) return 1.;
			return pow(RelSat, m_BallardArpCoef[index])*pow(1. / (1 + exp(-Th_Beta*RelSat)), 3.)
				- pow(pow((1 - RelSat) / 2, 3), 1 - m_SolidVol_FracOrg[index]);
		}
		else
			return pow(RelSat*(1 - ThQual) + RelSat*ThQual*1.1, 1 + m_SolidVol_FracOrg[index]);
	}

private:
	vector<double> m_SolidVol_FracOrg, m_BallardArpCoef, m_Saturation;
	double Th_Beta;

};

class ThCond_BallarpArp {
public: ThCond_BallarpArp(double Th_a, double Th_Alfa, double Th_Beta, vector<double> Meas_Sand, vector<double> Meas_Gravel, vector<double> Meas_Organic, vector<double> Meas_QuartsFraction,
	vector<double> Meas_Saturation,
	vector<double> UpperDepth, vector<double> LowerDepth, vector<double> DepthLayers, vector<double> ThicknessLayers, vector<double> DepthMidpLayer) {

	vector<double> FracSolid, Sim_Saturation, FracGravel,  FracSand, FracQuarts, FracOrg,  BulkDens;
	vector<double> DummyFracMidPoint;

	function<void(vector<double>&, vector<double>&, vector<double>& , bool)> f_ModelLayers= FromMeasuredLayersToModelLayers(UpperDepth, LowerDepth, DepthLayers, ThicknessLayers, DepthMidpLayer);

	f_ModelLayers(Meas_Sand, DummyFracMidPoint, FracSand, true);
	f_ModelLayers(Meas_QuartsFraction, DummyFracMidPoint, FracQuarts, true);
	f_ModelLayers(Meas_Organic, DummyFracMidPoint, FracOrg, true);
	f_ModelLayers(Meas_Gravel, DummyFracMidPoint, FracGravel, true);
	f_ModelLayers(Meas_Saturation, DummyFracMidPoint, Saturation, true);
	BulkDens.resize(Saturation.size());
	FracSolid.resize(Saturation.size());

	for (size_t i = 0; i < FracSand.size(); i++) {
		BulkDens[i]=(100 - Saturation[i]) / 100.*Fix::DensityofSolidSoil;
		FracSand[i] = FracSand[i] / 100.;
		FracQuarts[i] = FracQuarts[i] / 100.;
		FracOrg[i] = FracOrg[i] / 100.;
		FracGravel[i] = FracGravel[i] / 100.;
		FracSolid[i] = 1. - Saturation[i] / 100.; // Convert from Saturation to Volume of Solid...

	}

	vector<double> VolFracOrg;
	for (size_t i = 0; i < FracSolid.size(); i++) {
		f_Mineral = ThSolidFrac(Fix::ThermalCond_Mineral, 1 - FracSolid[i]*FracQuarts[i] - FracSolid[i]*FracOrg[i]);
		f_Org = ThSolidFrac(Fix::ThermalCond_Organic, FracSolid[i]*FracOrg[i]);
		f_Quarts = ThSolidFrac(Fix::ThermalCond_Quarts, FracSolid[i]*FracQuarts[i]);
		VolFracOrg.push_back(FracSolid[i] * FracOrg[i]);
		Saturation_Fraction.push_back(1. -FracSolid[i]); // volumetric %
		double ThSolid = f_Org()*f_Quarts()*f_Mineral()	;
		f_Solid = ThSolidFrac(ThSolid, FracSolid[i]);
		f_Sat = ThSolidFrac(Fix::ThermalCond_Water, Saturation_Fraction[i]);
		ThSolid_Out.push_back(ThSolid);
		ThDry_Out.push_back(((Th_a*ThSolid - Fix::ThermalCond_Air)*BulkDens[i] + Fix::ThermalCond_Air*Fix::DensityofSolidSoil) /
			(Fix::DensityofSolidSoil - (1 - Th_a)*BulkDens[i]));
		ThFixedSaturated_Out.push_back(f_Solid()*f_Sat());
		Balland_Arp_PowerCoef.push_back(0.5*(1 + FracSolid[i] * FracOrg[i] - Th_Alfa* FracSolid[i] * FracSand[i] - FracSolid[i] * FracGravel[i]));

	}
	f_KerstenFactor = KerstenFactor(Th_Beta, VolFracOrg, Balland_Arp_PowerCoef, Saturation);
}
	  ThCond_BallarpArp(double Th_a, double Th_Alfa, double Th_Beta, vector<double> Meas_Sand, vector<double> Meas_Gravel, vector<double> Meas_Organic, vector<double> Meas_QuartsFraction,
		  vector<double> Meas_Saturation): Saturation(Meas_Saturation) {

		  vector<double> BulkDens, FracSand, FracQuarts, FracOrg, FracGravel, FracSolid;
		  

		  BulkDens.resize(Meas_Saturation.size()); FracSand.resize(Meas_Saturation.size());
		  FracQuarts.resize(Meas_Saturation.size()); FracOrg.resize(Meas_Saturation.size());
		  FracGravel.resize(Meas_Saturation.size()); FracSolid.resize(Meas_Saturation.size());

		  for (size_t i = 0; i < Meas_Saturation.size(); i++) {
			  BulkDens[i] = (100 - Meas_Saturation[i]) / 100. * Fix::DensityofSolidSoil;
			  FracSand[i] = Meas_Sand[i] / 100.;
			  FracQuarts[i] = Meas_QuartsFraction[i] / 100.;
			  FracOrg[i] = Meas_Organic[i] / 100.;
			  FracGravel[i] = Meas_Gravel[i] / 100.;
			  FracSolid[i] = 1. - Meas_Saturation[i] / 100.; // Convert from Saturation to Volume of Solid...

		  }

		  vector<double> VolFracOrg;
		  for (size_t i = 0; i < FracSolid.size(); i++) {
			  f_Mineral = ThSolidFrac(Fix::ThermalCond_Mineral, 1 - FracSolid[i] * FracQuarts[i] - FracSolid[i] * FracOrg[i]);
			  f_Org = ThSolidFrac(Fix::ThermalCond_Organic, FracSolid[i] * FracOrg[i]);
			  f_Quarts = ThSolidFrac(Fix::ThermalCond_Quarts, FracSolid[i] * FracQuarts[i]);
			  VolFracOrg.push_back(FracSolid[i] * FracOrg[i]);
			  Saturation_Fraction.push_back(1. - FracSolid[i]); // volumetric %
			  double ThSolid = f_Org() * f_Quarts() * f_Mineral();
			  f_Solid = ThSolidFrac(ThSolid, FracSolid[i]);
			  f_Sat = ThSolidFrac(Fix::ThermalCond_Water, Saturation_Fraction[i]);
			  ThSolid_Out.push_back(ThSolid);
			  ThDry_Out.push_back(((Th_a * ThSolid - Fix::ThermalCond_Air) * BulkDens[i] + Fix::ThermalCond_Air * Fix::DensityofSolidSoil) /
				  (Fix::DensityofSolidSoil - (1 - Th_a) * BulkDens[i]));
			  ThFixedSaturated_Out.push_back(f_Solid() * f_Sat());
			  Balland_Arp_PowerCoef.push_back(0.5 * (1 + FracSolid[i] * FracOrg[i] - Th_Alfa * FracSolid[i] * FracSand[i] - FracSolid[i] * FracGravel[i]));

		  }
		  f_KerstenFactor = KerstenFactor(Th_Beta, VolFracOrg, Balland_Arp_PowerCoef, Saturation);
	  }

	double operator() (size_t index, double Theta, double ThQual) {

		double ThSat;
		if (ThQual < 0.001) ThSat = ThSolid_Out[index] * f_Sat();
		else {
			function<double()> f_Water, f_Ice;
			f_Water = ThSolidFrac(Fix::ThermalCond_Water, Saturation_Fraction[index]*(1-ThQual));
			f_Ice = ThSolidFrac(Fix::ThermalCond_Ice, Saturation_Fraction[index] * ThQual);
			ThSat = ThSolid_Out[index] * f_Water()*f_Ice();
		}
		return ThSat*f_KerstenFactor(index, Theta, ThQual) + (1 - f_KerstenFactor(index, Theta, ThQual)*ThDry_Out[index]);

	}
private:
	function<double()> f_Mineral, f_Quarts, f_Org, f_Solid, f_Sat;
	function<double(size_t, double, double)> f_KerstenFactor;
	vector<double> ThSolid_Out, ThDry_Out, ThFixedSaturated_Out, Balland_Arp_PowerCoef;
	vector<double> Saturation, Saturation_Fraction;
};

class SoilTempDampingDepth {




};








