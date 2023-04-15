
//#include "../stdafx.h"
#include "../ModelTypes/SimB.h"
#include "../Model/SOIL/Functors/Profile_Functors.h"
#include "../Model/SOIL/Functors/HydFunctors.h"
#include "../Model/SOIL/Functors/Thermal_Functors.h"
#include "../Model/SOIL/Functors/Drainage_Functor.h"
#include "../Model/Atmosphere/Functors/Met_Functors.h"
#include "../Model/Atmosphere/Functors/Radiation_Functors.h"
#include "../Model/Atmosphere/Functors/SoilEvaporation_Functors.h"
#include "../Model/Atmosphere/Functors/SnowPack_Functors.h"
#include "../Model/Atmosphere/Functors/SurfaceWater_Functors.h"
#include "../Model/Atmosphere/Functors/IrrigationWater_Functors.h"
#include "../Model/PLANT/Functors/Plant_Functors.h"
#include "../Model/PLANT/Functors/Salt_Functors.h"
#include "../Model/PLANT/Functors/WaterUptake_Functors.h"
#include "../Model/PLANT_NC/Functors/NC_Plant_Functors.h"
#include "../Model/SOIL_NC/Functors/NC_Soil_Functors.h"


#include "../Model/HBV/Functor/HBV_Functor.h"
#include "./FunctFactory.h"
#include "../Model/ModelFunctions.h"
 


FunctFactory::FunctFactory (const FunctorProp func, vector<double> Parv) : m_Prop(func), m_Ps_Par(Parv) {
	if (m_Prop.FuncName != MyFunc::NO_PARFUNC) {
		switch (m_Prop.Arg) {
		case MyArgType::D1: m_functor1 = Set_D1_Functor(); break;
		case MyArgType::D2: m_functor2 = Set_D2_Functor(); break;
		case MyArgType::D3: m_functor3 = Set_D3_Functor(); break;
		case MyArgType::D4: m_functor4 = Set_D4_Functor(); break;
		case MyArgType::D5: m_functor5 = Set_D5_Functor(); break;
		case MyArgType::T_D1: m_Tablefunctor1 = Set_D1_TableFunctor(); break;
		default:;
		}
	}
	else {
		switch (m_Prop.Arg) {
		case MyArgType::D1: m_functor1 = Set_D1_PhysFunctor(); break;
		case MyArgType::D2: m_functor2 = Set_D2_PhysFunctor(); break;
		case MyArgType::D3: m_functor3 = Set_D3_PhysFunctor(); break;
		case MyArgType::D4: m_functor4 = Set_D4_PhysFunctor(); break;
		default:;
		}
	}
	m_pHydCoef = nullptr;
}; 
FunctFactory::FunctFactory(const FunctorProp func, vector<double> Parv, vector<vector<double>> Parva) : m_Prop(func), m_Ps_Par(Parv), m_P_Par(Parva) {
	switch (m_Prop.Arg) {
	case MyArgType::T_D1: m_Tablefunctor1 = Set_D1_TableFunctor(); break;
	case MyArgType::T_D2: m_Tablefunctor2 = Set_D2_TableFunctor(); break;
	case MyArgType::T_D3: m_Tablefunctor3 = Set_D3_TableFunctor(); break;
	case MyArgType::T_D4: m_Tablefunctor4 = Set_D4_TableFunctor(); break;
	case MyArgType::D1: m_functor1 = Set_D1_Functor(); break;
	default:;
	}
	m_pHydCoef = nullptr;
};
FunctFactory::FunctFactory(const FunctorProp func, vector<vector<double>> Parv) : m_Prop(func), m_P_Par(Parv) {
	switch (m_Prop.Arg) {
		case MyArgType::D1: m_Tablefunctor1 = Set_D1_TableFunctor(); break;
		case MyArgType::D2: m_Tablefunctor2 = Set_D2_TableFunctor(); break;
		case MyArgType::D3: m_Tablefunctor3 = Set_D3_TableFunctor(); break;
		case MyArgType::D4: m_Tablefunctor4 = Set_D4_TableFunctor(); break;
		default:;
	}
	m_pHydCoef = nullptr;
};
FunctFactory::FunctFactory(const FunctorProp func, HYDRAULIC_COEF *m_pHydCoef) : m_Prop(func), m_pHydCoef(m_pHydCoef) {
	switch (m_Prop.Arg) {
	case MyArgType::D1: m_Tablefunctor1 = Set_D1_TableFunctor(); break;
	case MyArgType::D2: m_Tablefunctor2 = Set_D2_TableFunctor(); break;
	case MyArgType::D3: m_Tablefunctor3 = Set_D3_TableFunctor(); break;
	case MyArgType::D4: m_Tablefunctor4 = Set_D4_TableFunctor(); break;
	default:;
	}
};
function<double(double)> FunctFactory::Set_D1_Functor() {

	if (m_Prop.FunctorInitOption != string::npos) {
		switch (m_Prop.FuncName) {
		case MyFunc::AIRTEMP_DAILYMEAN:
			if (m_Ps_Par.size() != 2) { m_functor1.~function(); return m_functor1; }
			return DailyAirMeanTemp(m_Prop.FunctorInitOption, m_Ps_Par[0], m_Ps_Par[1]);
		default:;
		}



	}
	else {
		switch (m_Prop.FuncName) {
		case MyFunc::NO_PARFUNC:
			return Set_D1_PhysFunctor();
		case MyFunc::DRAINAGE_EMPIRICAL:
			if (m_Ps_Par.size() != 4) { m_functor1.~function(); return m_functor1; }
			return EmpDrainage(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_Ps_Par[3]);
		case MyFunc::DRAINAGE_PHYSICAL:
			return 	DrainFlowRate(((Soil_Drainage_Functions*)m_Prop.pHyd));

		case MyFunc::PRECCORR:
			if (m_Ps_Par.size() != 4) { m_functor1.~function(); return m_functor1; }
			return PrecCorrFactor(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_Ps_Par[3]);
		case MyFunc::PREC_GENERATOR:
			if (m_Ps_Par.size() != 3) { m_functor1.~function(); return m_functor1; }
			return PrecGenerator(m_Ps_Par[0], int(m_Ps_Par[1]), int(m_Ps_Par[2]));
		case MyFunc::AIRTEMP_ALT_CORR:
			if (m_Ps_Par.size() != 3) { m_functor1.~function(); return m_functor1; }
			return AirTempAltCorrection(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2]);
		case MyFunc::ATM_TRANSPARENCY:
			if (m_Ps_Par.size() != 2) { m_functor1.~function(); return m_functor1; }
			return AtmTransparency_FromRelSun(m_Ps_Par[0], m_Ps_Par[1]);
		case MyFunc::ALBEDO_SOIL:
			if (m_Ps_Par.size() != 3) { m_functor1.~function(); return m_functor1; }
			return AlbedoSoil(m_Ps_Par[0], m_Ps_Par[1],  m_Ps_Par[2]);
		case MyFunc::RADLONG_OUT_EM:
			if (m_Ps_Par.size() != 1) { m_functor1.~function(); return m_functor1; }
			return RadLongOut_Em(m_Ps_Par[0]);
		case MyFunc::BEER_LAW_SINGLE_LEAF:
			if (m_Ps_Par.size() != 1) { m_functor1.~function(); return m_functor1; }
			return BeerLaw(m_Ps_Par[0]);
		case MyFunc::RA_BELOWCANOPYLAI:
			if (m_Ps_Par.size() != 1) { m_functor1.~function(); return m_functor1; }
			return RaBoundaryLAIFunction(m_Ps_Par[0]);
		case MyFunc::SNOW_THERMALCOND:
			if (m_Ps_Par.size() != 1) { m_functor1.~function(); return m_functor1; }
			return ThermalConductivitySnow(m_Ps_Par[0]);
		case MyFunc::BARESOIL_SNOWHEIGHT:
			if (m_Ps_Par.size() == 1) return BareSoilCoverFromSnowHeight(m_Ps_Par[0]);
			break;
		case MyFunc::SURFACE_RUNOFF:
			if (m_Ps_Par.size() != 2) { m_functor1.~function(); return m_functor1; }
			return SurfaceRunOff(m_Ps_Par[0], m_Ps_Par[1]);
		case MyFunc::SURFACE_WATER_COVER:
			if (m_Ps_Par.size() != 3) { m_functor1.~function(); return m_functor1; }
			return SurfacePoolCover(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2]);
		case MyFunc::LAI_DAYOFYEAR:
		case MyFunc::ALBEDO_DAYOFYEAR:
		case MyFunc::CANOPYHEIGHT_DAYOFYEAR:
		case MyFunc::CANOPYRESISTANCE_DAYOFYEAR:
		case MyFunc::ROUGHNESSLENGTH_DAYOFYEAR:
		case MyFunc::DISPLACEMENT_DAYOFYEAR:
			if (m_P_Par.size() == 3) {
				return PlantTableSeasonTimeFunction(m_P_Par[0], m_P_Par[1], m_P_Par[2]);
			}
			break;
		case MyFunc::ROOTDEPTH_DAYOFYEAR:
		case MyFunc::ROOTLENGTH_DAYOFYEAR:
			if (m_P_Par.size() == 2) {
				return RootTimeVarFunction(m_P_Par[0], m_P_Par[1]);
			}
			break;
		case MyFunc::POTTRANSPIRATION_DAYOFYEAR:
			if (m_Ps_Par.size() == 3) {
				return SimplePotTranspiration(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2]);
			}
			break;
		case MyFunc::CONDUCTANCE_CO2:
			if (m_Ps_Par.size() == 2) {
				return Conductance_CO2(m_Ps_Par[0], m_Ps_Par[1]);
			}
			break;
		case MyFunc::INTERCEPTION_RAIN_CAPACITY:
			if (m_Ps_Par.size() == 2) {
				return InterceptionCapacity(m_Ps_Par[0], m_Ps_Par[1]);
			}
			break;
		case MyFunc::EMPIRICALSOILSURFACEDAMPING:
			if (m_Ps_Par.size() == 2) return SoilSurfaceHeatFlowDamping(m_Ps_Par[0], m_Ps_Par[1]);
			break;
		case MyFunc::HBV_PEAK_RUNOFF:
			if (m_Ps_Par.size() ==3) {
				return PeakRunOff(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2]);
			}
			break;
		case MyFunc::HBV_BASE_RUNOFF:
			if (m_Ps_Par.size() == 1) {
				return BaseRunOff(m_Ps_Par[0]);
			}
			break;
		case MyFunc::HBV_GROUNDWATERLEVEL:
			if (m_Ps_Par.size() == 4) {
				return GroundWaterLevel(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_Ps_Par[3]);
			}
			break;
		case MyFunc::HBV_GROUNDWATER_REDIST:
			if (m_Ps_Par.size() == 1) {
				return GroundWaterReDistribution(m_Ps_Par[0]);
			}
			break;
		case MyFunc::MAXPLANTWATER_FROM_LAI:
			if (m_Ps_Par.size() == 1) {
				return MaxWaterStorage(m_Ps_Par[0]);
			}
			break;
		case MyFunc::ROOT_DISTANCE_FROM_DENSITY:
			if (m_Ps_Par.size() == 3) {
				return WaterUptakeDistance_FromDensity(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2]);
			}
			break;
		case MyFunc::STOMATA_RELATIVEOPENING_FROM_PLANTWATERPOTENTIAL:
			if (m_Ps_Par.size() == 2) {
				return RelativeStomataClosure(m_Ps_Par[0], m_Ps_Par[1]);
			}
			break;
		case MyFunc::ABIOTIC_TEMP_Q10_RESPONSE:
			if(m_Ps_Par.size()==2) 
				return Q10(m_Ps_Par[0], m_Ps_Par[1]);
			breal:
		case MyFunc::ABIOTIC_TEMP_Q10THRESHOLD_RESPONSE:
			if (m_Ps_Par.size() == 3)
				return Q10(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2]);
			break;
		case MyFunc::ABIOTIC_TEMP_RATKOWSKY_RESPONSE:
			if (m_Ps_Par.size() == 2)
				return Ratkowsky(m_Ps_Par[0], m_Ps_Par[1]);
			break;
		case MyFunc::ABIOTIC_TEMP_NEILL_RESPONSE:
			if (m_Ps_Par.size() == 3)
				return Neill(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2]);
			break;
		case MyFunc::DENIT_TEMP_Q10_RESPONSE:
			if (m_Ps_Par.size() == 2)
				return Q10(m_Ps_Par[0], m_Ps_Par[1]);
			break;
		case MyFunc::DENIT_TEMP_Q10THRESHOLD_RESPONSE:
			if (m_Ps_Par.size() == 3)
				return Q10(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2]);
			break;
		case MyFunc::DENIT_TEMP_RATKOWSKY_RESPONSE:
			if (m_Ps_Par.size() == 2)
				return Ratkowsky(m_Ps_Par[0], m_Ps_Par[1]);
			break;
		case MyFunc::NITRI_TEMP_Q10_RESPONSE:
			if (m_Ps_Par.size() == 2)
				return Q10(m_Ps_Par[0], m_Ps_Par[1]);
			break;
		case MyFunc::NITRI_TEMP_Q10THRESHOLD_RESPONSE:
			if (m_Ps_Par.size() == 3)
				return Q10(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2]);
			break;
		case MyFunc::NITRI_TEMP_RATKOWSKY_RESPONSE:
			if (m_Ps_Par.size() == 2)
				return Ratkowsky(m_Ps_Par[0], m_Ps_Par[1]);
			break;
		case MyFunc::PLANTRESP_Q10_RESPONSE:
			if (m_Ps_Par.size() == 2)
				return Q10(m_Ps_Par[0], m_Ps_Par[1]);
			break;
		case MyFunc::PLANTRESP_Q10THRESHOLD_RESPONSE:
			if (m_Ps_Par.size() == 3)
				return Q10(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2]);
			break;
		case MyFunc::MOISTURE_RESPONSE_EMERGENCE:
			if (m_Ps_Par.size() == 1)
				return EmergenceMoistureControl(5., m_Ps_Par[0]);
			break;
		case MyFunc::ZADOK_GSI:
			if (m_Ps_Par.size() == 3)
				return ZadokFunctionFromGSI(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2]);
			break;
		case MyFunc::ABIOTIC_TEMP_ARRHENIUS_RESPONSE:
			if (m_Ps_Par.size() == 2)
				return Arrhenius(m_Ps_Par[0], m_Ps_Par[1]);
			break;
		case MyFunc::ABIOTIC_CO2_RESPONSE:
			if (m_Ps_Par.size() == 2)
				return CommonCO2Response(m_Ps_Par[0], m_Ps_Par[1]);
			break;
		case MyFunc::PH_WEATHERING_RESPONSE:
			if (m_Ps_Par.size() == 2)
				return PH_Response_Weathering(m_Ps_Par[0], m_Ps_Par[1]);
			break;
		case MyFunc::WEATHERINGRATECOEF:
			if (m_Ps_Par.size() == 1)
				return FirstOrderResponse(m_Ps_Par[0]);
			break;
		case MyFunc::PO4_ADSORPTION:
			if (m_Ps_Par.size() == 2)
				return LangmuirAdsorption(m_Ps_Par[0], m_Ps_Par[1]);
			break;
		case MyFunc::P_SURFACERUNOFF_CONC:
			if (m_Ps_Par.size() == 3)
				return P_Surfacerunoff_Conc(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2]);
			break;
		case MyFunc::MICROBE_CN_RATIO_RESPONSE:
			if (m_Ps_Par.size() == 2)
				return CN_RatioLinearResponse(m_Ps_Par[0], m_Ps_Par[1]);
			break;
		case MyFunc::MICROBE_CONC_RESPONSE:
			if (m_Ps_Par.size() == 1)
				return SubstrateConcentrationResponse(m_Ps_Par[0]);
			break;
		case MyFunc::POT_GROWTHCONSUMPTION_MICROBE :
			if (m_Ps_Par.size() == 1)
				return FirstOrderRateResponse(m_Ps_Par[0]);
			break;
		case MyFunc::RESPIRATION_MICROBE:
			if (m_Ps_Par.size() == 1) {

				return FirstOrderRateResponse(m_Ps_Par[0]);
			}
			break;
		case MyFunc::LITTER_TRANS_SURFACE_L1:
		case MyFunc::LITTER_TRANS_SURFACE_L2:
		case MyFunc::LITTER_TRANS_SURFACE_HUM:
		case MyFunc::METHANE_OXIDATION:
			if (m_Ps_Par.size() == 1)
				return FirstOrderRateResponse(m_Ps_Par[0]);
			break;
		case MyFunc::N_FERTILIZATION_DAY:
			if (m_P_Par.size() == 2)
				return Day_FertRates(m_P_Par[0], m_P_Par[1]);
			break;
		case MyFunc::DENIT_NITRATE_RESPONSE:
		case MyFunc::METHANE_OXIDATION_REGULATION_BY_CH4:
			if (m_Ps_Par.size() == 1)
				return Michaelis_Menten_Response(m_Ps_Par[0]);
			break;
		case MyFunc::METHANE_PLANTOXIDATION_REGULATION_BY_ROOTRESPIRATION:
			if (m_Ps_Par.size() == 1)
				return ExpSquaredDecline_Response(m_Ps_Par[0]);
			break;
		case MyFunc::METHANE_PLANTOXIDATION_BY_ROOTRESPIRATION:
			if (m_Ps_Par.size() == 2)
				return MethanePlantOxidation(m_Ps_Par[0], m_Ps_Par[1]);
			break;
		case MyFunc::METHANE_EBULLITION:
			if (m_Ps_Par.size() == 2)
				return Ebullition(m_Ps_Par[0], m_Ps_Par[1]);
			break;
		case MyFunc::METHANE_BOUNDARYLAYER_FLUX	:
			if (m_Ps_Par.size() == 2)
				return MethaneBoundaryLayerFlux(m_Ps_Par[0], m_Ps_Par[1]);
			break;
		case MyFunc::METHANE_TRANSPIRATION_EFFICIENCY:
			if (m_Ps_Par.size() == 3)
				return MethaneEmissionByTranspiration(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2]);
			break;
		case MyFunc::NITRI_N2O_PROD_RESPONSE:
		case MyFunc::NITRI_NO_PROD_RESPONSE:
		case MyFunc::METHANE_PLANTOXIDATION_OF_WATERUPTAKE:
			if (m_Ps_Par.size() == 1)
				return FirstOrderResponse(m_Ps_Par[0]);
			break;
		case MyFunc::ANAEROBIC_BALLON:
			if (m_Ps_Par.size() == 1)
				return ExpSquared_Response(m_Ps_Par[0]);
			break;
		case MyFunc::DiFFUSION_SOIL_OXYGEN:
			if (m_Ps_Par.size() == 1)
				return SoilAirDiffusion(GasElement::OXYGEN,m_Ps_Par[0]);
			break;
		case MyFunc::DiFFUSION_SOIL_CO2:
			if (m_Ps_Par.size() == 4)
				return SoilAirDiffusion(GasElement::CO2, m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_Ps_Par[3]);
			break;
		case MyFunc::NITRI_NO_PROD_TEMP_RESPONSE:
			if (m_Ps_Par.size() == 3)
				return N2O_TempResponse(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2]);
			break;
		case MyFunc::NITRI_N2O_PROD_MOIST_RESPONSE:
		case MyFunc::NITRI_NO_PROD_MOIST_RESPONSE:
			if (m_Ps_Par.size() == 2)
				return NOxide_MoistureResponse(m_Ps_Par[0], m_Ps_Par[1]);
			break;
		case MyFunc::NITRI_NO_PROD_PH_RESPONSE:
			if (m_Ps_Par.size() == 1)
				return NOxide_PH_Response(m_Ps_Par[0]);
			break;
		case MyFunc::DENIT_ANAEROBIC_FRAC_RESPONSE:
			if (m_Ps_Par.size() == 1)
				return DenitrifiersAnaerobicMoistureResponse(m_Ps_Par[0]);
			break;
		case MyFunc::EXCHANGE_EFFICIENY_ANAEROBIC:
			if (m_Ps_Par.size() == 2)
				return FromAnaerobicToAerobicDiffusion(m_Ps_Par[0], m_Ps_Par[1]);
			break;

		default:;
		}
	}
	return nullptr;
};
function<double(double, double)> FunctFactory::Set_D2_Functor() {
	switch (m_Prop.FuncName) {
		case MyFunc::NO_PARFUNC:
			return Set_D2_PhysFunctor();
		case MyFunc::RADNETLONG_SATURATED_AIR:
			if (m_Ps_Par.size() != 4) { m_functor3.~function(); return m_functor2; }
			return RadNetLong(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_Ps_Par[3]);
		case MyFunc::RADLONGIN_KONZELMANN_EMISSIVITY:
			if (m_Ps_Par.size() != 3) { m_functor2.~function(); return m_functor2; }
			return RadLongIn(0, true, m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2]);
		case MyFunc::RADLONGIN_SATTERLUND_EMISSIVITY:
			if (m_Ps_Par.size() != 2) { m_functor2.~function(); return m_functor2; }
			return RadLongIn(1, true, m_Ps_Par[0], m_Ps_Par[1]);
		case MyFunc::RADLONGIN_BRUNT_EMISSIVITY:
			if (m_Ps_Par.size() != 3) { m_functor2.~function(); return m_functor2; }
			return RadLongIn(2, true, m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2]);
		case MyFunc::RADLONGIN_KONZELMANN:
			if (m_Ps_Par.size() != 3) { m_functor2.~function(); return m_functor2; }
			return RadLongIn(0, false, m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2]);
		case MyFunc::RADLONGIN_SATTERLUND:
			if (m_Ps_Par.size() != 2) { m_functor2.~function(); return m_functor2; }
			return RadLongIn(1, false, m_Ps_Par[0], m_Ps_Par[1]);
		case MyFunc::RADLONGIN_BRUNT:
			if (m_Ps_Par.size() != 3) { m_functor2.~function(); return m_functor2; }
			return RadLongIn(2, false, m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2]);
		case MyFunc::ALBEDO_SNOW_TAIR:
			if (m_Ps_Par.size() != 5) { m_functor2.~function(); return m_functor2; }
			return AlbedoSnow_Tair(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_Ps_Par[3], m_Ps_Par[4]);
		case MyFunc::ALBEDO_SNOW_TSUMAIR:
			if (m_Ps_Par.size() != 4) { m_functor2.~function(); return m_functor2; }
			return AlbedoSnowAgeTempSum(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_Ps_Par[3]);
		case MyFunc::BRISTOW_FIXEDB: {
			RadTempAmpPar p;
			p.Bristow_B = m_Ps_Par[0]; p.Bristow_C = m_Ps_Par[1]; p.Bristow_Turb = m_Ps_Par[2];
			return RadGlobalTempAmplitude_FixedB(p);
		}
		case MyFunc::BRISTOW_SEASONALB: {
			RadTempAmpPar p;
			p.Bristow_B = m_Ps_Par[0]; p.Bristow_C = m_Ps_Par[1]; p.Bristow_Turb = m_Ps_Par[2];
			p.Thornton_B0 = m_Ps_Par[3]; p.Thornton_B1 = m_Ps_Par[4]; p.Thornton_B2 = m_Ps_Par[5];
			p.DTR_YEAR_MEAN = m_Ps_Par[6]; p.DTR_YEAR_AMP = m_Ps_Par[7]; p.DTR_YEAR_SHIFT = m_Ps_Par[8];
			return RadGlobalTempAmplitude_SeasonalB(p);
		}
		case MyFunc::RAD_RATIO_DIFFUSE:
			if (m_Ps_Par.size() != 1) { m_functor2.~function(); return m_functor2; }
			return DiffuseRadRatio(m_Ps_Par[0]);
		case MyFunc::CANOPY_SHADE_COVER:
			if (m_Ps_Par.size() != 2) { m_functor2.~function(); return m_functor2; }
			return CanopyShadeCover(m_Ps_Par[0], m_Ps_Par[1]);
		case MyFunc::SOIL_SURFACE_RES_1P:
			if (m_Ps_Par.size() != 1) { m_functor2.~function(); return m_functor2; }
			return SurfaceResistanceSoilSurface_1Par(m_Ps_Par[0]);
		case MyFunc::SOIL_SURFACE_RES_3P:
			if (m_Ps_Par.size() != 3) { m_functor2.~function(); return m_functor2; }
			return SurfaceResistanceSoilSurface_3Par(m_Ps_Par[0],m_Ps_Par[1], m_Ps_Par[2]);
		case MyFunc::RA_SNOW_NEUTRAL:
		case MyFunc::RA_HEATSOIL_NEUTRAL:
			if (m_Ps_Par.size() != 3) { m_functor2.~function(); return m_functor2; }
			return ResAirSoilNeutral(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2]);
		case MyFunc::ROADICE_SALTMELT:
			if (m_Ps_Par.size() != 1) { m_functor2.~function(); return m_functor2; }
			return SaltMeltOnRoad(m_Ps_Par[0]);
		case MyFunc::TEMPDIFF_FROMSENSIBLEFLOW:
			return TempDifferenceFromSensibleFlow();
		case MyFunc::NEWSNOW_DENSITY_LINEAR:
			return NewSnowDensity_Linear(m_Ps_Par[0], m_Ps_Par[1]);
		case MyFunc::NEWSNOW_DENSITY_EXP:
			return NewSnowDensity_Exponential(m_Ps_Par[0], m_Ps_Par[1]);
		case MyFunc::ROOTDEPTH_EXP_DISTRIBUTION:
			return RootDepthExpDistribution(m_Ps_Par[0]);
		case MyFunc::CANOPYRESISTANCE:
			return CanopyResistance();
		case MyFunc::SHAWPERRERIA_ROUGHL:
			if (m_Ps_Par.size() == 3)
				return RoughnessLengthCanopy(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2]);
		case MyFunc::INTERCEPTION_SNOW_TA:
			if (m_Ps_Par.size() == 3) {
				return InterceptionCapacity_TA(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2]);
			}
			break;
		case MyFunc::INTERCEPTION_SNOW_THQUAL:
			if (m_Ps_Par.size() == 3) {
				return InterceptionCapacity_THQUAL(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2]);
			}
			break;
		case MyFunc::POTENTIAL_WATERUPTAKE_INTERCEPTION_IMPLICIT:
			if (m_Ps_Par.size() == 1) {
				return PotentialWaterUptake(m_Ps_Par[0]);
			}
			break;
		case MyFunc::HBV_ACTUALEVAPORATION:
			if (m_Ps_Par.size() == 2) {
				return ActualEvaporation(m_Ps_Par[0], m_Ps_Par[1]);
			}
			break;
		case MyFunc::WATERUPTAKETEMP_ORIG:
			if (m_Ps_Par.size() == 3) {
				return WaterUptakeTempFunction(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2]);
			}
			break;
		case MyFunc::WATERUPTAKETEMP_TRIGTEMP:
			if (m_Ps_Par.size() == 3) {
				return WaterUptakeTempFunction(true, m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2]);
			}
			break;
		case MyFunc::WATERUPTAKETEMP_TRIGTEMP_POLY:
			if (m_Ps_Par.size() == 4) {
				return WaterUptakeTempFunction(true, m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_Ps_Par[3]);
			}
			break;

		case MyFunc::INTERNAL_PLANTRESISTANCE:
			if (m_Ps_Par.size() == 2) {
				return PlantResistance(m_Ps_Par[0], m_Ps_Par[1]);
			}
			break;
		case MyFunc::MAXPLANTWATER_FROM_LAIHEIGHT:
			if (m_Ps_Par.size() == 2) {
				return MaxWaterStorage(m_Ps_Par[0], m_Ps_Par[1]);
			}
			break;
		case MyFunc::THERMALCOND_ORGANIC:
			if (m_Ps_Par.size() == 3) {
				return ThCond_Org(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2]);
			}
			break;
		case MyFunc::N_FERTILIZATION_STAGE_YEAR:
			if (m_P_Par.size() == 4) {
				return YearStage_FertRates(m_P_Par[0], m_P_Par[1], m_P_Par[2], m_P_Par[3]);
			}
			break;
		case MyFunc::N_FERTILIZATION_TABLE_YEAR_1APP:
			if (m_P_Par.size() == 3) {
				return YearTable_FertRates(m_P_Par[0], m_P_Par[1], m_P_Par[2]);
			}
			break;
		case MyFunc::N_FERTILIZATION_TABLE_YEAR_3APP:
			if (m_P_Par.size() == 7) {
				return YearTable_FertRates(m_P_Par[0], m_P_Par[1], m_P_Par[2],
					m_P_Par[3], m_P_Par[4], m_P_Par[5], m_P_Par[6]);
			}
			break;
		case MyFunc::METHANE_VOLATIVITY:
			if (m_Ps_Par.size() == 3) {
				return HenryLawMethaneVolatility(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2]);
			}
			break;
		case MyFunc::METHANE_PRODUCTION_FROM_ONE_LITTER:
			if (m_Ps_Par.size() == 4) {
				return MethaneProduction(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_Ps_Par[3]);
			}
			break;
		case MyFunc::METHANE_EBULLITION:
			if (m_Ps_Par.size() == 2)
				return Ebullition(m_Ps_Par[0], m_Ps_Par[1]);
			break;
		case MyFunc::DiFFUSION_SOIL_OXYGEN:
			if (m_Ps_Par.size() == 1)
				return SoilAirDiffusion(GasElement::OXYGEN, m_Ps_Par[0]);
			break;
		case MyFunc::DiFFUSION_SOIL_CO2:
			if (m_Ps_Par.size() == 4)
				return SoilAirDiffusion(GasElement::CO2, m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2],  m_Ps_Par[3]);
			break;
		case MyFunc::NITRIFICATION_SIMPLE:
			if (m_Ps_Par.size() == 2)
				return NitrificationRate( m_Ps_Par[0], m_Ps_Par[1]);
			break;
		default:;
	}
	return nullptr;
}
function<double(double, double, double)> FunctFactory::Set_D3_Functor() {
	switch (m_Prop.FuncName) {
	case MyFunc::SIMPLE_SNOWDENSITY:
		if (m_Ps_Par.size() != 3) { m_functor3.~function(); return m_functor3; }
		return SimpleSnowDensity(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2]);
	case MyFunc::RADNETLONG_AIR:
		if (m_Ps_Par.size() != 4) { m_functor3.~function(); return m_functor3; }
		return RadNetLong(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_Ps_Par[3]);
	case MyFunc::RADLONGIN_KONZELMANN_EMISSIVITY:
		if (m_Ps_Par.size() != 3) { m_functor3.~function(); return m_functor3; }
		return RadLongIn(0,true, m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2]);
	case MyFunc::RADLONGIN_SATTERLUND_EMISSIVITY:
		if (m_Ps_Par.size() != 2) { m_functor3.~function(); return m_functor3; }
		return RadLongIn(1, true, m_Ps_Par[0], m_Ps_Par[1]);
	case MyFunc::RADLONGIN_BRUNT_EMISSIVITY:
		if (m_Ps_Par.size() != 3) { m_functor3.~function(); return m_functor3; }
		return RadLongIn(2,true, m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2]);
	case MyFunc::RADLONGIN_KONZELMANN:
		if (m_Ps_Par.size() != 3) { m_functor3.~function(); return m_functor3; }
		return RadLongIn(0, false, m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2]);
	case MyFunc::RADLONGIN_SATTERLUND:
		if (m_Ps_Par.size() != 2) { m_functor3.~function(); return m_functor3; }
		return RadLongIn(1, false, m_Ps_Par[0], m_Ps_Par[1]);
	case MyFunc::RADLONGIN_BRUNT:
		if (m_Ps_Par.size() != 3) { m_functor3.~function(); return m_functor3; }
		return RadLongIn(2, false, m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2]);
	case MyFunc::REL_HUMIDITY_GAS_EQUILIBRIUM:
		if (m_Ps_Par.size() != 1) { m_functor3.~function(); return m_functor3; }
		return RelHumiditySoilSurface(m_Ps_Par[0]);
	case MyFunc::SHAWPERRERIA_DISPLACEMENT:
		if(m_Ps_Par.size() == 2)
			return DisplacementLength(m_Ps_Par[0], m_Ps_Par[1]);
	case MyFunc::RA_CANOPY_NEUTRAL:
		if (m_Ps_Par.size() == 2)
			return ResAirCanopyNeutral(m_Ps_Par[0], m_Ps_Par[1]);
		else return nullptr;
	case MyFunc::INTERCEPTION_EVAPORATION_IMPLICIT_PLANT:
		if (m_Ps_Par.size() == 3)
			return InterceptionEvaporationSinglePlant(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2]);
		else
			return nullptr;
	case MyFunc::INTERCEPTION_EVAPORATION_EXPLICIT_PLANT:
		if (m_Ps_Par.size() == 2)
			return InterceptionEvaporationSinglePlant(m_Ps_Par[0], m_Ps_Par[1]);
		else
			return nullptr;
	case MyFunc::POTENTIAL_WATERUPTAKE_INTERCEPTION_EXPLICIT:
		if (m_Ps_Par.size() == 0) {
			return PotentialWaterUptake();
		}
		break;
	case MyFunc::HBV_INFILTRATION:
		if (m_Ps_Par.size() == 2) {
			return Infiltration(m_Ps_Par[0], m_Ps_Par[1]);
		}
		break;
	case MyFunc::THERMALCOND_KERSTEN_CLAY:
	case MyFunc::THERMALCOND_KERSTEN_SAND:

		if (m_Ps_Par.size() == 7) {
			return ThCond_KerstenEq(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_Ps_Par[3], m_Ps_Par[4], m_Ps_Par[5], m_Ps_Par[6]);
		}
		break;
	case MyFunc::METHANE_PRODUCTION_FROM_TWO_LITTER:
		if (m_Ps_Par.size() == 4) {
			return MethaneProduction(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_Ps_Par[3]);
		}
		break;

	default:;
	}
	return nullptr;
}
function<double(double, double, double, double)> FunctFactory::Set_D4_Functor() {
	/*switch (m_Prop.FuncName) {
	default:;
	}*/
	if (m_Prop.FunctorInitOption != string::npos) {
		switch (m_Prop.FuncName) {
			case MyFunc::NO_PARFUNC:
				return Set_D4_PhysFunctor();
			default:;
		}



	}
	else {
		switch (m_Prop.FuncName) {
		case MyFunc::NO_PARFUNC:
			return Set_D4_PhysFunctor();
		case MyFunc::STABILITY_ANALYTICAL:
			return StabilityCorrAnalytical(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_Ps_Par[3]);
		case MyFunc::RA_HEATSOIL_ANALYTICAL:
			return RaHeatFunctionSoil_Analytical(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_Ps_Par[3], m_Ps_Par[4], m_Ps_Par[5],
				m_Ps_Par[6], m_Ps_Par[7]);
		case MyFunc::RA_HEATSOIL_NUMERICAL:
			return RaHeatFunctionSoil_Numerical(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_Ps_Par[3], m_Ps_Par[4], m_Ps_Par[5],
				m_Ps_Par[6], m_Ps_Par[7]);
		case MyFunc::RA_SNOW_ANALYTICAL:
			return RaHeatFunctionSnow_Analytical(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_Ps_Par[3], m_Ps_Par[4], m_Ps_Par[5],
				m_Ps_Par[6]);
		case MyFunc::RA_SNOW_NUMERICAL:
			return RaHeatFunctionSnow_Numerical(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_Ps_Par[3], m_Ps_Par[4], m_Ps_Par[5],
				m_Ps_Par[6]);
		case MyFunc::INTERCEPTION_EVAPORATION_EXPLICIT_PLANT:
			if (m_Ps_Par.size() == 2)
				return InterceptionEvaporationSinglePlant(m_Ps_Par[0], m_Ps_Par[1]);
			else
				return nullptr;
		default:;
		}
	}
	return m_functor4;
}
function<double(double, double, double, double, double)> FunctFactory::Set_D5_Functor() {

	switch (m_Prop.FuncName) {
		case MyFunc::NO_PARFUNC:
			return Set_D5_PhysFunctor();


		case MyFunc::RA_HEATSOIL_ANALYTICAL:
			return RaHeatFunctionSoil_Analytical(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_Ps_Par[3], m_Ps_Par[4], m_Ps_Par[5],
				m_Ps_Par[6], m_Ps_Par[7]);
		case MyFunc::RA_HEATSOIL_NUMERICAL:
			return RaHeatFunctionSoil_Numerical(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_Ps_Par[3], m_Ps_Par[4], m_Ps_Par[5],
				m_Ps_Par[6], m_Ps_Par[7]);
		case MyFunc::SNOW_MELTING_EMPIRICAL:
			return SnowMeltFunctor(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_Ps_Par[3], m_Ps_Par[4], m_Ps_Par[5]);
		case MyFunc::RA_CANOPY_ANALYTICAL:
			if (m_Ps_Par.size() == 6)
				return ResAirCanopy_Analytical(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_Ps_Par[3], m_Ps_Par[4], m_Ps_Par[5]);
		case MyFunc::RA_CANOPY_NUMERICAL:
			if (m_Ps_Par.size() == 6)
				return ResAirCanopy_Numerical(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_Ps_Par[3], m_Ps_Par[4], m_Ps_Par[5]);
	default:;
	}

	return m_functor5;
}
function<double(double, double, double, double, double,double)> FunctFactory::Set_D6_Functor() {

	switch (m_Prop.FuncName) {
	case MyFunc::NO_PARFUNC:
		return Set_D6_PhysFunctor();

	default:;
	}

	return nullptr;
}

function<double(bool)> FunctFactory::Set_boolBlank_Functor() {

	if (m_Prop.FunctorInitOption != string::npos) {
		switch (m_Prop.FuncName) {

		default:;
	}



	}
	else {
		switch (m_Prop.FuncName) {

			case::MyFunc::MAXCONDUCTANCE:
				return Conductance(m_Ps_Par[0], m_Ps_Par[1]);

		default:;
		}
	}
	return nullptr;
};
function<double(bool, double)> FunctFactory::Set_boolD1_Functor() {

	if (m_Prop.FunctorInitOption != string::npos) {
		switch (m_Prop.FuncName) {
			default:;
		}



	}
	else {
		switch (m_Prop.FuncName) {

		default:;
		}
	}
	return nullptr;
};
function<double(bool, double, double)> FunctFactory::Set_boolD2_Functor() {
	switch (m_Prop.FuncName) {
	default:;
	}
	return nullptr;
}
function<double(bool, double, double, double)> FunctFactory::Set_boolD3_Functor() {
	switch (m_Prop.FuncName) {


	default:;
	}
	return nullptr;
}
function<double(bool, double, double, double, double)> FunctFactory::Set_boolD4_Functor() {
	/*switch (m_Prop.FuncName) {
	default:;
	}*/
	if (m_Prop.FunctorInitOption != string::npos) {
		switch (m_Prop.FuncName) {
			default:;
		}



	}
	else {
		switch (m_Prop.FuncName) {

		default:;
		}
	}
	return nullptr;
}
function<double(bool, double, double, double, double, double)> FunctFactory::Set_boolD5_Functor() {

	switch (m_Prop.FuncName) {

	default:;
	}

	return nullptr;
}



function<vector<double>()> FunctFactory::SetVector_Blank_Functor() {

	if (m_Prop.FunctorInitOption != string::npos) {
		switch (m_Prop.FuncName) {

		default:;
		}



	}
	else {
		switch (m_Prop.FuncName) {

		case::MyFunc::HEATCAPACITY_SOLID:
			return HeatCapacitySolid(m_P_Par[0], m_P_Par[1]);

		default:;
		}
	}
	return nullptr;
};
function<vector<double>(bool)> FunctFactory::SetVector_boolBlank_Functor() {

	if (m_Prop.FunctorInitOption != string::npos) {
		switch (m_Prop.FuncName) {

		default:;
		}



	}
	else {
		switch (m_Prop.FuncName) {

		case::MyFunc::DRYBULKDENSITY:
			return DryBulkDensity(m_P_Par[0], m_P_Par[1], m_P_Par[2], m_P_Par[3], m_P_Par[4], m_P_Par[5]);

		default:;
		}
	}
	return nullptr;
};
function<vector<double>(bool, double)> FunctFactory::SetVector_boolD1_Functor() {

	if (m_Prop.FunctorInitOption != string::npos) {
		switch (m_Prop.FuncName) {
		default:;
		}



	}
	else {
		switch (m_Prop.FuncName) {

		default:;
		}
	}
	return nullptr;
};
function<vector<double>(bool, double, double)> FunctFactory::SetVector_boolD2_Functor() {
	switch (m_Prop.FuncName) {
	default:;
	}
	return nullptr;
}
function<vector<double>(bool, double, double, double)> FunctFactory::SetVector_boolD3_Functor() {
	switch (m_Prop.FuncName) {


	default:;
	}
	return nullptr;
}
function<vector<double>(bool, double, double, double, double)> FunctFactory::SetVector_boolD4_Functor() {
	/*switch (m_Prop.FuncName) {
	default:;
	}*/
	if (m_Prop.FunctorInitOption != string::npos) {
		switch (m_Prop.FuncName) {
		default:;
		}



	}
	else {
		switch (m_Prop.FuncName) {

		default:;
		}
	}
	return nullptr;
}
function<vector<double>(bool, double, double, double, double, double)> FunctFactory::SetVector_boolD5_Functor() {
	switch (m_Prop.FuncName) {

	default:;
	}
	return nullptr;
}

function<void(vector<double>*)>  FunctFactory::SetVectorPointer_D1_Functor() {
	switch (m_Prop.FuncName) {


	default:;
	}
	return nullptr;
}
function<void(vector<double>*, vector<double>*)>  FunctFactory::SetVectorPointer_D2_Functor() {
	switch (m_Prop.FuncName) {

	default:;
	}
	return nullptr;
}
function<void(vector<double>*, vector<double>*, vector<double>*)>  FunctFactory::SetVectorPointer_D3_Functor() {
	switch (m_Prop.FuncName) {
	case::MyFunc::THERMALCOND_KERSTEN:
		if (m_P_Par.size() == 6) {
			KERSTEN_COEF Clay, Sand;

			Clay.UC1 = m_Ps_Par[4];
			Clay.UC2 = m_Ps_Par[5];
			Clay.UC3 = m_Ps_Par[6];
			Clay.FC1 = m_Ps_Par[7];
			Clay.FC2 = m_Ps_Par[8];
			Clay.FC3 = m_Ps_Par[9];
			Clay.FC4 = m_Ps_Par[10];
			Sand.UC1 = m_Ps_Par[11];
			Sand.UC2 = m_Ps_Par[12];
			Sand.UC3 = m_Ps_Par[13];
			Sand.FC1 = m_Ps_Par[14];
			Sand.FC2 = m_Ps_Par[15];
			Sand.FC3 = m_Ps_Par[16];
			Sand.FC4 = m_Ps_Par[17];
			return ThCond_Kersten(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_Ps_Par[3],
				Clay, Sand, m_P_Par[0], m_P_Par[1], m_P_Par[2], m_P_Par[3], m_P_Par[4], m_P_Par[5]);
		}
	default:;
	}
	return nullptr;
};
function<void(vector<double>*, vector<double>*, vector<double>*, vector<double>*)> FunctFactory::SetVectorPointer_D4_Functor() {
	switch (m_Prop.FuncName) {



	default:;
	}
	return nullptr;
};
function<void(vector<double>*, vector<double>*, vector<double>*, vector<double>*, vector<double>*)> FunctFactory::SetVectorPointer_D5_Functor() {
	switch (m_Prop.FuncName) {

	default:;
	}
	return nullptr;
};



function<double(double)> FunctFactory::Set_D1_PhysFunctor() {
	{
		switch (m_Prop.PhysFuncName) {
		case FUNC_CHOICE::SVP:
			return SVP; // Physf(FUNC_CHOICE::SVP);
			case FUNC_CHOICE::AIRTEMP_YEAR:
				return Physf(m_Prop.PhysFuncName, m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2]);
			case FUNC_CHOICE::AIRTEMP_DAY:
				return Physf(m_Prop.PhysFuncName, m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2]);
			case FUNC_CHOICE::DAYLENGTH:
				return Physf(m_Prop.PhysFuncName, m_Ps_Par[0]);
			case FUNC_CHOICE::POTRISHOUR:
				return Physf(m_Prop.PhysFuncName, m_Ps_Par[0],  m_Ps_Par[1]);
			default: return nullptr;
		}
	}


};
function<double(double, double)> FunctFactory::Set_D2_PhysFunctor() {
	switch (m_Prop.PhysFuncName) {
		case FUNC_CHOICE::AVP_TEMP_RH:
			return Physf(m_Prop.PhysFuncName);
		case FUNC_CHOICE::SENSHEAT_AIRFLUX:
			return Sens_AirF;
		case FUNC_CHOICE::LATENTHEAT_AIRFLUX:
			return Latent_AirF;
		case FUNC_CHOICE::POTRISHOUR:
			return Physf(m_Prop.PhysFuncName, m_Ps_Par[0], m_Ps_Par[1]);
		default:;
	}
	return Physf(m_Prop.PhysFuncName);
}
function<double(double, double, double)> FunctFactory::Set_D3_PhysFunctor() {
	switch (m_Prop.FuncName) {



	}
	return Physf(m_Prop.PhysFuncName);

}
function<double(double, double, double, double)> FunctFactory::Set_D4_PhysFunctor() {
	switch (m_Prop.PhysFuncName) {
	case FUNC_CHOICE::RES_AIR_MOM_HEAT:
		return Physf(m_Prop.PhysFuncName);
		default:;
	}
	return Physf(m_Prop.PhysFuncName);

}
function<double(double, double, double, double, double)> FunctFactory::Set_D5_PhysFunctor() {
	switch (m_Prop.PhysFuncName) {
	case FUNC_CHOICE::PM:
		return nullptr;
	default:;
	}
	return Physf(m_Prop.PhysFuncName);

}
function<double(double, double, double, double, double,double)> FunctFactory::Set_D6_PhysFunctor() {
	switch (m_Prop.PhysFuncName) {
	case FUNC_CHOICE::PM:
		return PMEquation;
	default:;
	}
	return Physf(m_Prop.PhysFuncName);

}
function<OrgTurnOver(double, double, double)> FunctFactory::SetOrgTurnOverFunctor() {
	switch (m_Prop.FuncName) {
	case MyFunc::FAECES_TURNOVER:
	case MyFunc::FAECES_TURNOVER_CARBON:
	case MyFunc::LITTER2_TURNOVER_CARBON:
	case MyFunc::LITTER2_TURNOVER:
	case MyFunc::LITTER_TURNOVER_CARBON:
	case MyFunc::LITTER_TURNOVER:
		if (m_Ps_Par.size() == 5) {
			return turnover(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_Ps_Par[3], m_Ps_Par[4]);
		}
		break;
	case MyFunc::HUMUS_TURNOVER_CARBON:
	case MyFunc::HUMUS_TURNOVER:
		if (m_Ps_Par.size() == 4) {
			return turnover(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_Ps_Par[3]);
		}
		break;
	default:;
	}

	return nullptr;


}
function<OrgTurnOver(double, double, double, double, double)> FunctFactory::SetOrg_D5_TurnOverFunctor() {
	switch (m_Prop.FuncName) {

	case MyFunc::LITTER_CNP_TURNOVER:
		if (m_Ps_Par.size() == 7) {
			return turnover(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_Ps_Par[3], m_Ps_Par[4],  m_Ps_Par[5], m_Ps_Par[6]);
		}
		break;
	case MyFunc::LITTER2_CNP_TURNOVER:
		if (m_Ps_Par.size() == 7) {
			return turnover(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_Ps_Par[3], m_Ps_Par[4],  m_Ps_Par[5], m_Ps_Par[6]);
		}
		break;
	case MyFunc::FAECES_CNP_TURNOVER:
		if (m_Ps_Par.size() == 7) {
			return turnover(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_Ps_Par[3], m_Ps_Par[4], m_Ps_Par[5], m_Ps_Par[6]);
		}
		break;
	case MyFunc::HUMUS_CNP_TURNOVER:
		if (m_Ps_Par.size() == 6) {
			return turnover(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_Ps_Par[3], m_Ps_Par[4], m_Ps_Par[5]);
		}
		break;
	default:;
	}

	return nullptr;


}
function<pair<double, double>(double, double)> FunctFactory::Set_Pair_D2_Functor() {
	switch (m_Prop.FuncName) {
	case MyFunc::HEAT_TEMP_THQUAL:
		m_Prop.FunctorInitOption = static_cast<Soil_Frost_Functions*>(m_Prop.pHyd)->FreezingPointOptions;
		if (m_Ps_Par.size() == 5) {
			return SoilThermalQuality( m_Prop.FunctorInitOption, m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_Ps_Par[3], m_Ps_Par[4],
				m_Prop.pHyd->M_Lambda, m_Prop.pHyd->M_Saturation, m_Prop.pHyd->M_Wilting, m_Prop.pHyd->ThicknessLayers);
		}
		break;

	default:;
	}

	return nullptr;


}
function<pair<double, double>(double, double, double)> FunctFactory::Set_Pair_D3_Functor() {
	switch (m_Prop.FuncName) {
	case MyFunc::HEAT_TEMP_THQUAL_OSMOTIC:
		if (m_Ps_Par.size() == 5) {
			return SoilThermalQuality(m_Prop.FunctorInitOption, m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_Ps_Par[3], m_Ps_Par[4],
				m_Prop.pHyd->M_Lambda, m_Prop.pHyd->M_Saturation, m_Prop.pHyd->M_Wilting, m_Prop.pHyd->ThicknessLayers);
		}
		break;

	default:;
	}

	return nullptr;


}
function<pair<double, double>(size_t, double, double)> FunctFactory::Set_Pair_D2_TableFunctor() {
	switch (m_Prop.FuncName) {
	case MyFunc::HEAT_TEMP_THQUAL:
		if (m_Ps_Par.size() == 5) {
			return SoilThermalQuality(m_Prop.FunctorInitOption, m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_Ps_Par[3], m_Ps_Par[4],
				m_Prop.pHyd->M_Lambda, m_Prop.pHyd->M_Saturation, m_Prop.pHyd->M_Wilting, m_Prop.pHyd->ThicknessLayers);
		}
		break;

	default:;
	}

	return nullptr;


}
function<pair<double, double>(size_t, double, double, double)> FunctFactory::Set_Pair_D3_TableFunctor() {
	switch (m_Prop.FuncName) {
	case MyFunc::HEAT_TEMP_THQUAL_OSMOTIC:
		if (m_Ps_Par.size() == 5) {
			return SoilThermalQuality(m_Prop.FunctorInitOption, m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_Ps_Par[3], m_Ps_Par[4],
				m_Prop.pHyd->M_Lambda, m_Prop.pHyd->M_Saturation, m_Prop.pHyd->M_Wilting, m_Prop.pHyd->ThicknessLayers);
		}
		break;

	default:;
	}

	return nullptr;


}
function< double(size_t, size_t, double, double)> FunctFactory::Set_T_D2_TableFunctor() {
	switch (m_Prop.FuncName) {
	case MyFunc::HEAT_TEMP_THQUAL:
		if (m_P_Par.size() == 5) {
		
		}
		break;

	default:;
	}

	return nullptr;


}
function< double(size_t, size_t, double,double, double)> FunctFactory::Set_T_D3_TableFunctor() {
	switch (m_Prop.FuncName) {
	case MyFunc::WATERUPTAKEMOISTURE:
		if (m_P_Par.size() == 5) {
			return WaterUptakeMoistureFunction(m_P_Par[0], m_P_Par[1], m_P_Par[2], m_P_Par[3], m_P_Par[4], m_Prop.pHyd);
		}
		break;

	default:;
	}

	return nullptr;


}

function<double(size_t)> FunctFactory::Set_BLANK_TableFunctor() {
	switch (m_Prop.FuncName) {

	case MyFunc::PHOTO_EFF_LIGHT:
		if (m_P_Par.size() == 1) {
			return PhoRadEff(m_P_Par[0]);
		}
	default:;
	}

	return nullptr;
};

function<double(size_t, double)> FunctFactory::Set_D1_TableFunctor() {
		switch (m_Prop.FuncName) {
			case MyFunc::BEER_LAW_PERLEAF:
				if (m_P_Par.size() == 1) {
					vector<double> beerpar;
					for (size_t i = 0; i < m_P_Par[0].size(); i++) {
						beerpar.push_back(m_P_Par[0][i]);
					}
					return BeerLaw(beerpar);
				}
			case MyFunc::LEAFAREAINDEX_PER_MASS:
				if (m_P_Par.size() == 2) {
					vector<double> par; vector<vector<double>> parvv;
					for (size_t j = 0; j < m_P_Par.size(); j++) {
						parvv.push_back(par);
						for (size_t i = 0; i < m_P_Par[j].size(); i++) {
							parvv[j].push_back(m_P_Par[j][i]);
						}
					}
					return LeafAreaIndex(parvv[0], parvv[1]);
				}
			case MyFunc::ROOTLENGTH_PER_MASS:
				if (m_P_Par.size() == 1) {
					vector<double> par; vector<vector<double>> parvv;
					for (size_t j = 0; j < m_P_Par.size(); j++) {
						parvv.push_back(par);
						for (size_t i = 0; i < m_P_Par[j].size(); i++) {
							parvv[j].push_back(m_P_Par[j][i]);
						}
					}
					return RootLength(parvv[0]);
				}
			case MyFunc::LEAFAREAINDEX_SEASON:
			case MyFunc::CANOPYHEIGHT_SEASON:
			case MyFunc::ALBEDOVEG_SEASON:
			case MyFunc::ROOTDEPTH_SEASON:
			case MyFunc::ROOTLENGTH_SEASON:

				if (m_P_Par.size() == 8) {
					PlantInterp pl;
					vector<double> par; vector<vector<double>> parvv;
					for (size_t j = 0; j < m_P_Par.size(); j++) {
						parvv.push_back(par);
						for (size_t i = 0; i < m_P_Par[j].size(); i++) {
							switch (j) {
							case 0: pl.Start.push_back(m_P_Par[j][i]); break;
							case 1: pl.Opt.push_back(m_P_Par[j][i]); break;
							case 2: pl.End.push_back(m_P_Par[j][i]); break;
							case 3: pl.ShapeStart.push_back(m_P_Par[j][i]); break;
							case 4: pl.ShapeEnd.push_back(m_P_Par[j][i]); break;
							case 5: pl.StartValue.push_back(m_P_Par[j][i]); break;
							case 6: pl.OptValue.push_back(m_P_Par[j][i]); break;
							case 7: pl.EndValue.push_back(m_P_Par[j][i]); break;

							}
						}
					}
					return PlantSeasonDev(pl);
				}

			case MyFunc::PSI_THETA: {
				RETENTION_COEF r;
				vector<RETENTION_COEF> coef;
				if (m_pHydCoef == nullptr) return nullptr;
				for (size_t i = 0; i < m_pHydCoef->ret.size(); i++) {
					r.Galpha = m_pHydCoef->ret[i].Galpha;
					r.GM = m_pHydCoef->ret[i].GM;
					r.GN = m_pHydCoef->ret[i].GN;
					r.Saturation = m_pHydCoef->ret[i].Saturation;
					r.Wilting = m_pHydCoef->ret[i].Wilting;
					r.UpperBTheta = m_pHydCoef->ret[i].UpperBTheta;
					r.UpperBPsi = m_pHydCoef->ret[i].UpperBPsi;
					r.ResWater = m_pHydCoef->ret[i].ResWater;
					r.
						MacroPore = m_pHydCoef->ret[i].MacroPore;
					r.Lambda = m_pHydCoef->ret[i].Lambda;
					r.PsiEntry = m_pHydCoef->ret[i].PsiEntry;
					r.ScalePsi = m_pHydCoef->ret[i].ScalePsi;
					coef.push_back(r);
				}
				return Psi_fromTheta(coef, 0);
			}
			case MyFunc::CANOPYCOVER:
				if (m_P_Par.size() == 2) {
					return CanopyCover(m_P_Par[0], m_P_Par[1]);
				}
				break;
			case MyFunc::ROOTDEPTH_PER_MASS:
				if (m_P_Par.size() == 2) {
					return RootDepth(m_P_Par[0], m_P_Par[1]);
				}
				break;
			case MyFunc::ALBEDOVEG_PER_GROWTHSTAGE:
				if (m_P_Par.size() == 2) {
					return PlantAlbedo(m_P_Par[0], m_P_Par[1]);
				}
				break;
			case MyFunc::WATERUPTAKE_SALT:
				if (m_P_Par.size() == 2) {
					return WaterUptakeSaltFunction(m_P_Par[0], m_P_Par[1]);
				}
				break;
			case::MyFunc::PHOTO_DIRECT_TEMP:
				if (m_P_Par.size() == 4) {
					return PhoTempResponseDirect(m_P_Par[0], m_P_Par[1], m_P_Par[2], m_P_Par[3]);
				}
			case::MyFunc::PHOTO_TEMPSUM:
				if (m_P_Par.size() == 2) {
					return PhoTempSumResponse(m_P_Par[0], m_P_Par[1]);
				}
			case::MyFunc::PHOTO_PLEAF:
			case::MyFunc::PHOTO_NLEAF:
				if (m_P_Par.size() == 2) {
					return PhoLeafResponse(m_P_Par[0], m_P_Par[1]);
				}
			case::MyFunc::PHOTO_GREENINDEX:
				if (m_P_Par.size() == 2) {
					return PhotoGreenIndexResponse(m_P_Par[0], m_P_Par[1]);
				}
				break;
			case::MyFunc::LOGISTIC_GROWTH:
				if (m_P_Par.size() == 6) {
					return LogisticGrowth(m_P_Par[0], m_P_Par[1], m_P_Par[2], m_P_Par[3], m_P_Par[4], m_P_Par[5], m_Prop.pModelInfo);
				}
			case::MyFunc::ALLO_ROOT_MASS_FIXED:
				if (m_P_Par.size() == 1) {
					return CAlloRootMass_Fixed(m_P_Par[0]);
				}
			case::MyFunc::ALLO_ROOT_MASS_LINEAR:
				if (m_P_Par.size() == 2) {
					return CAlloRootMass_Linear(m_P_Par[0], m_P_Par[1], m_Ps_Par[0]);
				}
			case::MyFunc::ALLO_ROOT_MASS_EXP:
				if (m_P_Par.size() == 3) {
					return CAlloRootMass_Exp(m_P_Par[0], m_P_Par[1], m_P_Par[2]);
			}
			case::MyFunc::ALLO_ROOT_PHOSPHORUS_FIXED:
			case::MyFunc::ALLO_ROOT_NITROGEN_FIXED:
				if (m_P_Par.size() == 1) {
					return CAlloRootNitrogen_Fixed(m_P_Par[0]);
				}
			case::MyFunc::ALLO_ROOT_PHOSPHORUS_LINEAR:
			case::MyFunc::ALLO_ROOT_NITROGEN_LINEAR:
				if (m_P_Par.size() == 2) {
					return CAlloRootNitrogen_Linear(m_P_Par[0], m_P_Par[1]);
				}
			case::MyFunc::ALLO_ROOT_PHOSPHORUS_EXP:
			case::MyFunc::ALLO_ROOT_NITROGEN_EXP:
				if (m_P_Par.size() == 3) {
					return CAlloRootNitrogen_Exp(m_P_Par[0], m_P_Par[1], m_P_Par[2]);
			}
			case::MyFunc::ALLO_ROOT_WATER_FIXED:
				if (m_P_Par.size() == 1) {
					return CAlloRootWater_Fixed(m_P_Par[0]);
				}
			case::MyFunc::ALLO_ROOT_WATER_LINEAR:
				if (m_P_Par.size() == 2) {
					return CAlloRootWater_Linear(m_P_Par[0], m_P_Par[1]);
				}
			case::MyFunc::ALLO_ROOT_WATER_EXP:
				if (m_P_Par.size() == 3) {
					return CAlloRootWater_Exp(m_P_Par[0], m_P_Par[1], m_P_Par[2]);
				}
			case::MyFunc::ALLO_LEAF_MASS_FIXED:
				if (m_P_Par.size() == 1) {
					return CAlloLeaf_Fixed(m_P_Par[0]);
				}
			case::MyFunc::ALLO_LEAF_MASS_LINEAR:
				if (m_P_Par.size() == 2) {
					return CAlloLeaf_Linear(m_P_Par[0], m_P_Par[1]);
				}
			case::MyFunc::ALLO_LEAF_MASS_EXP:
				if (m_P_Par.size() == 3) {
					return CAlloLeaf_Exp(m_P_Par[0], m_P_Par[1], m_P_Par[2]);
				}
			case MyFunc::PHOTO_EFF_FARQUAR:
				if (m_P_Par.size() == 1) {
					return PhoRadEff(m_P_Par[0]);
				}
			case MyFunc::PHOTO_EFF_LIGHT:
				if (m_P_Par.size() == 1) {
					return PhoRadEff(m_P_Par[0]);
				}
			case MyFunc::PHOTO_GRAINFILL:
				if (m_P_Par.size() == 1) {
					return PhoGrainFillResponse(m_P_Par[0]);
				}
			case MyFunc::PHOTO_SATURATION:
				if (m_P_Par.size() == 1) {
					return PhoSaturationResponse(m_P_Par[0]);
				}

			case MyFunc::LEAF_LITTERFALL_LAI:
				if (m_P_Par.size() == 1) {
					return LeafLitterFall_LAI(m_P_Par[0]);
				}
			case MyFunc::ABIOTIC_THETA_RESPONSE:
				if (m_Ps_Par.size() == 4&&m_P_Par.size() == 2) {
					return CommonThetaResponse(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_Ps_Par[3], m_P_Par[0], m_P_Par[1]);
				}
			case MyFunc::DENIT_THETA_RESPONSE:
				if (m_Ps_Par.size() == 2 && m_P_Par.size() == 1) {
					return DenitTheta(m_Ps_Par[0], m_Ps_Par[1],m_P_Par[0]);
				}
			case MyFunc::GROWTH_RESPIRATION:
				if (m_P_Par.size() == 1)
					return FirstOrderFunc(m_P_Par[0]);
				break;
			case MyFunc::TEMP_SUM_EMERGENCE:
			case MyFunc::TEMP_SUM_SOWING:
			case MyFunc::TEMP_SUM_GRAINFILLING:
			case MyFunc::TEMP_SUM_RIPENING:
				if (m_P_Par.size() == 2)
					return PlantTempSum(m_P_Par[0], m_P_Par[1]);
				break;
			case MyFunc::PRIOR_GRAIN_DAYLENGTH:
				if (m_P_Par.size() == 5)
					return GrainStart(m_P_Par[0], m_P_Par[1], m_P_Par[2], m_P_Par[3], m_P_Par[4]);
				break;
			case MyFunc::GROWTH_CONSUMPTION_MICROBE:
				if (m_Ps_Par.size() == 4) {
					vector<double> v;
					for (size_t i = 0; i < 4; i++)
						v.push_back(m_Ps_Par[i]);
					double humfrac = max(0., min(1., 1 - m_Ps_Par[1] - m_Ps_Par[2] - m_Ps_Par[3]));
					v.push_back(humfrac);
					return FirstOrderRateResponse(v);
				}
				break;
			case MyFunc::RESP_GROWTH_CONSUMPTION_MICROBE:
				if (m_Ps_Par.size() == 4) {
					vector<double> v;
					v.push_back(1. - m_Ps_Par[0]);	// This is the complement to the efficiency
					for (size_t i = 1; i < 4; i++)
						v.push_back(m_Ps_Par[i]);
					double humfrac = max(0., min(1., 1 - m_Ps_Par[1] - m_Ps_Par[2] - m_Ps_Par[3]));
					v.push_back(humfrac);
					return FirstOrderRateResponse(v);
				}
				break;
			case MyFunc::MORTALITY_MICROBE:
				if (m_Ps_Par.size() == 4) {
					vector<double> v;
					for (size_t i = 0; i < 4; i++)
						v.push_back(m_Ps_Par[i]);
					double humfrac = max(0., min(1., 1 - m_Ps_Par[1] - m_Ps_Par[2] - m_Ps_Par[3]));
					v.push_back(humfrac);
					return FirstOrderRateResponse(v);
				}
				break;
			case MyFunc::HUMIFICATION_MICROBE:
				if (m_Ps_Par.size() == 6) {
					vector<double> v;
					for (size_t i = 0; i < 3; i++)
						v.push_back(m_Ps_Par[i]);
					double humfrac = max(0., min(1., 1 - m_Ps_Par[0] - m_Ps_Par[1] - m_Ps_Par[2]));
					v.push_back(humfrac);
					for (size_t i = 0; i < 3; i++)
						v.push_back(m_Ps_Par[3+i]);
					humfrac = max(0., min(1., 1 - m_Ps_Par[3] - m_Ps_Par[4] - m_Ps_Par[5]));
					v.push_back(humfrac);
					return FirstOrderRateResponse(v);
				}
				break;

			case MyFunc::NFIXATION_RESPONSE_MIN_SOIL:
				if (m_P_Par.size() == 3) {
					return NitrogenFixationResponseToMinNSoil(m_P_Par[0], m_P_Par[1], m_P_Par[2]);
				}
				break;
			case MyFunc::ALLO_RATECOEF_NEWMOBILE:
				if (m_P_Par.size() == 1) {
					return FirstOrderResponse(m_P_Par[0]);
				}
			default:;
		}

	return nullptr;
};
function<double(size_t, double, double)> FunctFactory::Set_D2_TableFunctor() {
	switch (m_Prop.FuncName) {
	case MyFunc::LEAFAREAINDEX_PER_MASS_STAGE:
		if (m_P_Par.size() == 2) {
			vector<double> par; vector<vector<double>> parvv;
			for (size_t j = 0; j < m_P_Par.size(); j++) {
				parvv.push_back(par);
				for (size_t i = 0; i < m_P_Par[j].size(); i++) {
					parvv[j].push_back(m_P_Par[j][i]);
				}
			}
			return LeafAreaIndex(parvv[0], parvv[1]);
		}
	case MyFunc::THERMALCOND_MODEL_BALLAND_ARP:
		if (m_P_Par.size() == 10) // Corresponds to model layers
			return ThCond_BallarpArp(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_P_Par[0],
				m_P_Par[1], m_P_Par[2], m_P_Par[3], m_P_Par[4], m_P_Par[5], m_P_Par[6],
				m_P_Par[7], m_P_Par[8], m_P_Par[9]);
		break;
	case MyFunc::THERMALCOND_MEAS_BALLAND_ARP:
	if (m_P_Par.size() == 5) // Corresponds to measured layers
			return ThCond_BallarpArp(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_P_Par[0],
				m_P_Par[1], m_P_Par[2], m_P_Par[3], m_P_Par[4]);
		break;
	case MyFunc::THERMALCOND_KERSTEN:
		if (m_P_Par.size() == 6) {
			KERSTEN_COEF Clay, Sand;

			Clay.UC1 = m_Ps_Par[4];
			Clay.UC2 = m_Ps_Par[5];
			Clay.UC3 = m_Ps_Par[6];
			Clay.FC1 = m_Ps_Par[7];
			Clay.FC2 = m_Ps_Par[8];
			Clay.FC3 = m_Ps_Par[9];
			Clay.FC4 = m_Ps_Par[10];
			Sand.UC1 = m_Ps_Par[11];
			Sand.UC2 = m_Ps_Par[12];
			Sand.UC3 = m_Ps_Par[13];
			Sand.FC1 = m_Ps_Par[14];
			Sand.FC2 = m_Ps_Par[15];
			Sand.FC3 = m_Ps_Par[16];
			Sand.FC4 = m_Ps_Par[17];
			return ThCond_Kersten(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_Ps_Par[3],
				Clay, Sand, m_P_Par[0], m_P_Par[1], m_P_Par[2], m_P_Par[3], m_P_Par[4], m_P_Par[5]);
		}
		break;
	case MyFunc::DISPLACEMENTFRACTION_SEASON:
		if (m_P_Par.size() == 8) {

			PlantInterp pl;
			for (size_t j = 0; j < 8; j++) {
				for (size_t i = 0; i < m_P_Par[j].size(); i++) {
					switch (j) {
					case 0: pl.Start.push_back(m_P_Par[j][i]); break;
					case 1: pl.Opt.push_back(m_P_Par[j][i]); break;
					case 2: pl.End.push_back(m_P_Par[j][i]); break;
					case 3: pl.ShapeStart.push_back(m_P_Par[j][i]); break;
					case 4: pl.ShapeEnd.push_back(m_P_Par[j][i]); break;
					case 5: pl.StartValue.push_back(m_P_Par[j][i]); break;
					case 6: pl.OptValue.push_back(m_P_Par[j][i]); break;
					case 7: pl.EndValue.push_back(m_P_Par[j][i]); break;

					}
				}
			}
			return DisplacementLength(pl);
		}
	case MyFunc::LEAF_LITTERFALL_TSUM:
		if (m_P_Par.size() == 5) return LitterFall_Tsum(m_P_Par[0], m_P_Par[1], m_P_Par[2], m_P_Par[3], m_P_Par[4]);
		break;
	case MyFunc::PRIOR_GRAIN_TEMP:
		if (m_P_Par.size() == 5) return GrainStart(m_P_Par[0], m_P_Par[1], m_P_Par[2], m_P_Par[3], m_P_Par[4]);
		break;
	case MyFunc::PHOTO_COMBINED_N_WATER_RESPONSE:
		if (m_P_Par.size() == 2) return PhotoResponsetoWaterAndNitrogen(m_P_Par[0], m_P_Par[1]);
		break;
	case MyFunc::WATERUPTAKETEMP_ORIG:
		if (m_P_Par.size() == 3) return WaterUptakeTempFunction(m_P_Par[0], m_P_Par[1], m_P_Par[2]);
		break;
	case MyFunc::WATERUPTAKETEMP_TRIGTEMP:
		if (m_P_Par.size() == 3) return WaterUptakeTempFunction(true, m_P_Par[0], m_P_Par[1], m_P_Par[2]);
		break;
	case MyFunc::WATERUPTAKETEMP_TRIGTEMP_POLY:
		if (m_P_Par.size() == 4) return WaterUptakeTempFunction(true, m_P_Par[0], m_P_Par[1], m_P_Par[2], m_P_Par[3]);
		break;
	case MyFunc::ABIOTIC_COMBINED_THETA_CO2_RESPONSE:
		if (m_Ps_Par.size() == 5 && m_P_Par.size() == 2) {
			return CombinedCO2ThetaResponse(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_Ps_Par[3], m_Ps_Par[4], m_P_Par[0], m_P_Par[1]);
		}
		break;

	}
	return nullptr;
}
function<double(size_t, double, double, double)> FunctFactory::Set_D3_TableFunctor() {
	switch (m_Prop.FuncName) {
	case MyFunc::CANOPYHEIGHT_PER_MASS_AGE:
		if (m_P_Par.size() == 5) {
			return PlantHeight(m_P_Par[0], m_P_Par[1], m_P_Par[2], m_P_Par[3], m_P_Par[4]);
		}
		case MyFunc::ROUGHNESSFRACTION_SEASON:
			if (m_P_Par.size() == 10) {

				PlantInterp pl;
					for (size_t i = 0; i < m_P_Par[0].size(); i++) {
						  pl.Start.push_back(m_P_Par[0][i]);
						  pl.Opt.push_back(m_P_Par[1][i]); 
						  pl.End.push_back(m_P_Par[2][i]); 
						  pl.ShapeStart.push_back(m_P_Par[3][i]);
						  pl.ShapeEnd.push_back(m_P_Par[4][i]);
						  pl.StartValue.push_back(m_P_Par[5][i]);
						  pl.OptValue.push_back(m_P_Par[6][i]);
						  pl.EndValue.push_back(m_P_Par[7][i]);
				}
				return RoughnessLengthCanopy(pl, m_P_Par[8], m_P_Par[9]);
			}
			break;
		case MyFunc::WATERUPTAKEMOISTURE:
			return WaterUptakeMoistureFunction(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_Ps_Par[3], m_Ps_Par[4], m_Prop.pHyd);
		case MyFunc::WATERUPTAKEMOISTURE_DRIP:
			return WaterUptakeMoistureFunction(m_Ps_Par[0], m_Ps_Par[1], m_Ps_Par[2], m_Ps_Par[3], m_Ps_Par[4], m_Ps_Par[5], m_Prop.pHyd);
		case MyFunc::GRAIN_LITTERFALL_DYNAMIC:
			return LitterFallYear(m_P_Par[0], m_P_Par[1], m_P_Par[2], m_P_Par[3], m_P_Par[4]);
		case MyFunc::GRAIN_LITTERFALL_STATIC:
			return LitterFallYear(m_P_Par[0]);
		case MyFunc::FUNGI_LITTERFALL_DYNAMIC:
			return LitterFallYear(m_P_Par[0], m_P_Par[1], m_P_Par[2], m_P_Par[3], m_P_Par[4]);
		case MyFunc::FUNGI_LITTERFALL_STATIC:
			return LitterFallYear(m_P_Par[0]);
	}
	return nullptr;
}
function<double(size_t, double, double, double, double)> FunctFactory::Set_D4_TableFunctor() {







	return nullptr;
}

function<double(bool, size_t)> FunctFactory::Set_boolBlank_TableFunctor() {
	switch (m_Prop.FuncName) {
	case::MyFunc::MAXCONDUCTANCE_MC:
		return Conductance(m_Ps_Par[0], m_P_Par[0]);



	default:;
	}

	return nullptr;
};
function<double(bool, size_t, double)> FunctFactory::Set_boolD1_TableFunctor() {
	switch (m_Prop.FuncName) {
	
	
	default:;
	}

	return nullptr;
};
function<double(bool, size_t, double, double)> FunctFactory::Set_boolD2_TableFunctor() {
	switch (m_Prop.FuncName) {
	

	}
	return nullptr;
}
function<double(bool, size_t, double, double, double)> FunctFactory::Set_boolD3_TableFunctor() {
	switch (m_Prop.FuncName) {
	case::MyFunc::STEM_LITTERFALL_DYNAMIC:
		return LitterFallYear(m_P_Par[0], m_P_Par[1], m_P_Par[2], m_P_Par[3], m_P_Par[4], m_P_Par[5], m_P_Par[6]);
	case::MyFunc::STEM_LITTERFALL_STATIC:
		return LitterFallYear(m_P_Par[0], m_P_Par[1], m_P_Par[2]);
	case::MyFunc::ROOT_LITTERFALL_DYNAMIC:
		return LitterFallYear(m_P_Par[0], m_P_Par[1], m_P_Par[2], m_P_Par[3], m_P_Par[4], m_P_Par[5], m_P_Par[6]);
	case::MyFunc::ROOT_LITTERFALL_STATIC:
		return LitterFallYear(m_P_Par[0], m_P_Par[1], m_P_Par[2]);
	case::MyFunc::COARSEROOT_LITTERFALL_DYNAMIC:
		return LitterFallYear(m_P_Par[0], m_P_Par[1], m_P_Par[2], m_P_Par[3], m_P_Par[4], m_P_Par[5], m_P_Par[6]);
	case::MyFunc::COARSEROOT_LITTERFALL_STATIC:
		return LitterFallYear(m_P_Par[0], m_P_Par[1], m_P_Par[2]);

	default:;





	}



	return nullptr;
}
function<double(bool, size_t, double, double, double, double)> FunctFactory::Set_boolD4_TableFunctor() {

	switch (m_Prop.FuncName) {
		case::MyFunc::LEAF_LITTERFALL_DYNAMIC:
			return LitterFallYear(m_P_Par[0], m_P_Par[1], m_P_Par[2], m_P_Par[3], m_P_Par[4], m_P_Par[5], m_P_Par[6], m_P_Par[7]);
			break;
		case::MyFunc::LEAF_LITTERFALL_STATIC:
			return LitterFallYear(m_P_Par[0], m_P_Par[1], m_P_Par[2], m_P_Par[3]);
			break;

	default:;
	}

	return nullptr;



}

