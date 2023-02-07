#pragma once
#include "../NewBase/PhysFunc.h"
class ModelInfo;
class Hydraulic;
class SimModule;
//#include "../Model/SimModule.cpp"

struct OrgTurnOver { double Respiration; double N_Trans_Conv; double P_Trans_Conv; double Trans_Org; double N_Mineralisation; double P_Mineralisation; };


enum class MyArgType { BLANK, D1, D2, D3, D4, D5, T_D1,T_D2, T_D3, T_D4,TT_D3, TURNOVER_D3, TURNOVER_D5, Pair_T_D2, Pair_T_D3, };
enum class  MyFunc {
	PREC_GENERATOR, PRECCORR, AIRTEMP_ALT_CORR, AIRTEMP_DAILYMEAN, ATM_TRANSPARENCY,
	BEER_LAW_SINGLE_LEAF, BEER_LAW_PERLEAF, ALBEDO_SOIL, ALBEDO_SNOW_TAIR, ALBEDO_SNOW_TSUMAIR, RADLONG_OUT_EM,
	BRISTOW_FIXEDB, BRISTOW_SEASONALB, RADNETLONG_AIR, RADNETLONG_SATURATED_AIR, RADNETLONG_AIR_SURF, RADLONGIN_KONZELMANN_EMISSIVITY, RADLONGIN_SATTERLUND_EMISSIVITY, RADLONGIN_BRUNT_EMISSIVITY,
	RADLONGIN_KONZELMANN, RADLONGIN_SATTERLUND, RADLONGIN_BRUNT,
	RAD_RATIO_DIFFUSE, CANOPY_SHADE_COVER,
	PSI_THETA,
	STABILITY_ANALYTICAL, RA_BELOWCANOPYLAI, REL_HUMIDITY_GAS_EQUILIBRIUM,
	SOIL_SURFACE_RES_1P, SOIL_SURFACE_RES_3P, RA_HEATSOIL_NEUTRAL,
	RA_HEATSOIL_ANALYTICAL, RA_HEATSOIL_NUMERICAL,
	TEMPDIFF_FROMSENSIBLEFLOW, VAPOURDIFF_FROMLATENTFLOW,
	SNOW_THERMALCOND, ROADICE_SALTMELT, SNOW_MELTING_EMPIRICAL, NEWSNOW_DENSITY_LINEAR,
	NEWSNOW_DENSITY_EXP,
	SURFACE_RUNOFF, SURFACE_WATER_COVER,
	LAI_DAYOFYEAR, ALBEDO_DAYOFYEAR, CANOPYHEIGHT_DAYOFYEAR, ROOTDEPTH_DAYOFYEAR, ROOTLENGTH_DAYOFYEAR, LEAFAREAINDEX_PER_MASS, LEAFAREAINDEX_PER_MASS_STAGE,
	LEAFAREAINDEX_SEASON, CANOPYHEIGHT_SEASON, CANOPYHEIGHT_PER_MASS_AGE, ALBEDOVEG_SEASON, ALBEDOVEG_PER_GROWTHSTAGE, ROOTDEPTH_SEASON, ROOTLENGTH_SEASON, ROOTLENGTH_PER_MASS,
	CANOPYCOVER, ROOTDEPTH_EXP_DISTRIBUTION, ROOTDEPTH_PER_MASS,
	CANOPYRESISTANCE_DAYOFYEAR, ROUGHNESSLENGTH_DAYOFYEAR, DISPLACEMENT_DAYOFYEAR, ROUGHNESSFRACTION_SEASON, DISPLACEMENTFRACTION_SEASON, POTTRANSPIRATION_DAYOFYEAR,
	CANOPYRESISTANCE, MAXCONDUCTANCE, MAXCONDUCTANCE_MC, CONDUCTANCE, CONDUCTANCE_MC, CONDUCTANCE_CO2,
	SHAWPERRERIA_ROUGHL, SHAWPERRERIA_DISPLACEMENT,
	RA_CANOPY_NEUTRAL, RA_CANOPY_ANALYTICAL, RA_CANOPY_NUMERICAL, RA_SNOW_NEUTRAL, RA_SNOW_ANALYTICAL, RA_SNOW_NUMERICAL, BARESOIL_SNOWHEIGHT,
	SIMPLE_SNOWDENSITY,
	INTERCEPTION_RAIN_CAPACITY, INTERCEPTION_SNOW_TA, INTERCEPTION_SNOW_THQUAL,
	INTERCEPTION_EVAPORATION_IMPLICIT_PLANT, INTERCEPTION_EVAPORATION_EXPLICIT_PLANT, POTENTIAL_WATERUPTAKE_INTERCEPTION_IMPLICIT, POTENTIAL_WATERUPTAKE_INTERCEPTION_EXPLICIT,
	BALLAND_ARP_RECALC, THERMALCOND_MODEL_BALLAND_ARP, THERMALCOND_MEAS_BALLAND_ARP ,THERMALCOND_KERSTEN, THERMALCOND_ORGANIC, THERMALCOND_KERSTEN_CLAY, THERMALCOND_KERSTEN_SAND, DRYBULKDENSITY, HEATCAPACITY_SOLID, EMPIRICALSOILSURFACEDAMPING,
	HBV_ACTUALEVAPORATION, HBV_INFILTRATION, HBV_PEAK_RUNOFF, HBV_BASE_RUNOFF, HBV_GROUNDWATERLEVEL, HBV_GROUNDWATER_REDIST,
	ARCHIE_RESISTIVITY,
	WATERUPTAKETEMP_NO, WATERUPTAKETEMP_ORIG, WATERUPTAKETEMP_TRIGTEMP, WATERUPTAKETEMP_TRIGTEMP_POLY, 
	WATERUPTAKEMOISTURE, WATERUPTAKEMOISTURE_DRIP, WATERUPTAKE_SALT, ROOT_DISTANCE_FROM_DENSITY, INTERNAL_PLANTRESISTANCE, MAXPLANTWATER_FROM_LAI, MAXPLANTWATER_FROM_LAIHEIGHT,
	STOMATA_RELATIVEOPENING_FROM_PLANTWATERPOTENTIAL,
	HYDRAULIC_RECALC, HYDRAULIC_CONDTEMPFACTOR, PSI_THETA_MEASURED, HYDRAULIC_COND_THETA, HYDRAULIC_COND_PSI, HYDRAULIC_COND_THETA_MEASURED, HYDRAULIC_COND_PSI_MEASURED,
	PROFILE_RECALC, DRAINAGE_EMPIRICAL, DRAINAGE_PHYSICAL, DRAINAGE_SEEPAGE, DRAINAGE_DITCHLEVEL, DRAINAGE_RECALC,
	PHOTO_DIRECT_TEMP, PHOTO_TEMPSUM, PHOTO_NLEAF, PHOTO_GREENINDEX, LOGISTIC_GROWTH, PHOTO_PLEAF,
	PHOTO_EFF_LIGHT, PHOTO_EFF_FARQUAR, PHOTO_GRAINFILL, PHOTO_SATURATION, PHOTO_COMBINED_N_WATER_RESPONSE,
	ALLO_ROOT_MASS_FIXED, ALLO_ROOT_MASS_LINEAR, ALLO_ROOT_MASS_EXP,
	ALLO_ROOT_NITROGEN_FIXED, ALLO_ROOT_NITROGEN_LINEAR, ALLO_ROOT_NITROGEN_EXP,
	ALLO_ROOT_PHOSPHORUS_FIXED, ALLO_ROOT_PHOSPHORUS_LINEAR, ALLO_ROOT_PHOSPHORUS_EXP,
	ALLO_ROOT_WATER_FIXED, ALLO_ROOT_WATER_LINEAR, ALLO_ROOT_WATER_EXP,
	ALLO_LEAF_MASS_FIXED, ALLO_LEAF_MASS_LINEAR, ALLO_LEAF_MASS_EXP, ALLO_RATECOEF_NEWMOBILE,
	LEAF_LITTERFALL_STATIC, LEAF_LITTERFALL_DYNAMIC, LEAF_LITTERFALL_TSUM, LEAF_LITTERFALL_LAI,
	STEM_LITTERFALL_STATIC, STEM_LITTERFALL_DYNAMIC, STEM_LITTERFALL_TSUM,
	GRAIN_LITTERFALL_STATIC, GRAIN_LITTERFALL_DYNAMIC, GRAIN_LITTERFALL_TSUM,
	ROOT_LITTERFALL_STATIC, ROOT_LITTERFALL_DYNAMIC, ROOT_LITTERFALL_TSUM,
	COARSEROOT_LITTERFALL_STATIC, COARSEROOT_LITTERFALL_DYNAMIC, COARSEROOT_LITTERFALL_TSUM,
	FUNGI_LITTERFALL_STATIC, FUNGI_LITTERFALL_DYNAMIC, FUNGI_LITTERFALL_TSUM,
	GROWTH_RESPIRATION, MAINTENANCE_RESPIRATION, LEAF_M_RESPIRATION, STEM_M_RESPIRATION, LEAF_RESPIRATION,
	PLANTRESP_Q10_RESPONSE, PLANTRESP_Q10THRESHOLD_RESPONSE,
	ABIOTIC_TEMP_Q10_RESPONSE, ABIOTIC_TEMP_Q10THRESHOLD_RESPONSE, ABIOTIC_TEMP_RATKOWSKY_RESPONSE, ABIOTIC_TEMP_NEILL_RESPONSE,
	ABIOTIC_THETA_RESPONSE, ABIOTIC_CO2_RESPONSE,  ABIOTIC_COMBINED_THETA_CO2_RESPONSE,
	DENIT_TEMP_Q10_RESPONSE, DENIT_TEMP_Q10THRESHOLD_RESPONSE, DENIT_TEMP_RATKOWSKY_RESPONSE, DENIT_NITRATE_RESPONSE,
	NITRI_TEMP_Q10_RESPONSE, NITRI_TEMP_Q10THRESHOLD_RESPONSE, NITRI_TEMP_RATKOWSKY_RESPONSE,
	DENIT_THETA_RESPONSE, NITRI_NO_PROD_TEMP_RESPONSE, NITRI_N2O_PROD_MOIST_RESPONSE, NITRI_NO_PROD_MOIST_RESPONSE,
	NITRI_N2O_PROD_RESPONSE, NITRI_NO_PROD_RESPONSE, NITRIFICATION_SIMPLE,
	NITRI_NO_PROD_PH_RESPONSE, EXCHANGE_EFFICIENY_ANAEROBIC,  DENIT_ANAEROBIC_FRAC_RESPONSE,
	TEMP_SUM_SOWING, TEMP_SUM_EMERGENCE, MOISTURE_RESPONSE_EMERGENCE, ZADOK_GSI, PRIOR_GRAIN_DAYLENGTH, PRIOR_GRAIN_TEMP,
	TEMP_SUM_GRAINFILLING, TEMP_SUM_RIPENING, ABIOTIC_TEMP_ARRHENIUS_RESPONSE, PO4_ADSORPTION, PH_WEATHERING_RESPONSE, WEATHERINGRATECOEF, P_SURFACERUNOFF_CONC,
	LITTER_TURNOVER, LITTER_TURNOVER_CARBON, LITTER2_TURNOVER, LITTER2_TURNOVER_CARBON, FAECES_TURNOVER, FAECES_TURNOVER_CARBON,  HUMUS_TURNOVER, HUMUS_TURNOVER_CARBON, LITTER_CNP_TURNOVER, LITTER2_CNP_TURNOVER, FAECES_CNP_TURNOVER,HUMUS_CNP_TURNOVER,
	MICROBE_CN_RATIO_RESPONSE, MICROBE_CONC_RESPONSE, POT_GROWTHCONSUMPTION_MICROBE,
	GROWTH_CONSUMPTION_MICROBE, RESP_GROWTH_CONSUMPTION_MICROBE, MORTALITY_MICROBE, HUMIFICATION_MICROBE, RESPIRATION_MICROBE, MINERALISATION_MICROBE,
	LITTER_TRANS_SURFACE_L1, LITTER_TRANS_SURFACE_L2, LITTER_TRANS_SURFACE_HUM, NFIXATION_RESPONSE_MIN_SOIL,
	N_FERTILIZATION_DAY,N_FERTILIZATION_STAGE_YEAR, N_FERTILIZATION_TABLE_YEAR_1APP, N_FERTILIZATION_TABLE_YEAR_3APP,  METHANE_OXIDATION , 
	METHANE_OXIDATION_REGULATION_BY_CH4, METHANE_PLANTOXIDATION_REGULATION_BY_ROOTRESPIRATION, METHANE_PLANTOXIDATION_BY_ROOTRESPIRATION,
	METHANE_EBULLITION, METHANE_VOLATIVITY,	 METHANE_BOUNDARYLAYER_FLUX, METHANE_PRODUCTION_FROM_ONE_LITTER, METHANE_PRODUCTION_FROM_TWO_LITTER,
	METHANE_TRANSPIRATION_EFFICIENCY,  METHANE_PLANTOXIDATION_OF_WATERUPTAKE, ANAEROBIC_BALLON,
	DiFFUSION_SOIL_OXYGEN, DiFFUSION_SOIL_CO2,	HEAT_TEMP_THQUAL, HEAT_TEMP_THQUAL_OSMOTIC,
	NO_PARFUNC};

enum class FUNC_CHOICE : int;

struct FunctorProp {
    enum  MyFunc FuncName;
    enum  FUNC_CHOICE PhysFuncName;
    enum  MyArgType Arg;
	size_t FunctorInitOption;
	Hydraulic* pHyd{nullptr};
	ModelInfo* pModelInfo{nullptr};
	SimModule* pSimModule{nullptr};
	bool Set_PlotPar;
};

struct HYDRAULIC_COEF;
class FunctFactory {
public:
    FunctFactory(const FunctorProp func, vector<double> Parva);
    FunctFactory(const FunctorProp func, vector<double> Parv, vector<vector<double>> Parva);
    FunctFactory(const FunctorProp func, vector<vector<double>> Parva);
    FunctFactory(const FunctorProp func, HYDRAULIC_COEF *pHydCoef);

	function<double(double)> Get_D1_Functor() {	return Set_D1_Functor();}
	function<double(double, double)> Get_D2_Functor() {	return Set_D2_Functor();}
	function<double(double, double, double)> Get_D3_Functor() {return Set_D3_Functor();	}
	function<double(double, double, double, double)> Get_D4_Functor() {	return Set_D4_Functor();}
	function<double(double, double, double, double, double)> Get_D5_Functor() { return Set_D5_Functor(); }
	function<double(double, double, double, double, double, double)> Get_D6_Functor() { return Set_D6_Functor(); }

	function<double(bool)> Get_boolBlank_Functor() { return Set_boolBlank_Functor(); }
	function<double(bool, double)> Get_boolD1_Functor() { return Set_boolD1_Functor(); }
	function<double(bool, double, double)> Get_boolD2_Functor() { return Set_boolD2_Functor(); }
	function<double(bool, double, double, double)> Get_boolD3_Functor() { return Set_boolD3_Functor(); }
	function<double(bool, double, double, double, double)> Get_boolD4_Functor() { return Set_boolD4_Functor(); }
	function<double(bool, double, double, double, double, double)> Get_boolD5_Functor() { return Set_boolD5_Functor(); }

	function<vector<double>()> GetVector_Blank_Functor() { return SetVector_Blank_Functor(); }
	function<vector<double>(bool)> GetVector_boolBlank_Functor() { return SetVector_boolBlank_Functor(); }
	function<vector<double>(bool, double)> GetVector_boolD1_Functor() { return SetVector_boolD1_Functor(); }
	function<vector<double>(bool, double, double)> GetVector_boolD2_Functor() { return SetVector_boolD2_Functor(); }
	function<vector<double>(bool, double, double, double)> GetVector_boolD3_Functor() { return SetVector_boolD3_Functor(); }
	function<vector<double>(bool, double, double, double, double)> GetVector_boolD4_Functor() { return SetVector_boolD4_Functor(); }
	function<vector<double>(bool, double, double, double, double, double)> GetVector_boolD5_Functor() { return SetVector_boolD5_Functor(); }


	function<void(vector<double>*)> GetVectorPointer_D1_Functor() { return SetVectorPointer_D1_Functor(); }
	function<void(vector<double>*, vector<double>*)> GetVectorPointer_D2_Functor() { return SetVectorPointer_D2_Functor(); }
	function<void(vector<double>*, vector<double>*, vector<double>*)> GetVectorPointer_D3_Functor() { return SetVectorPointer_D3_Functor(); }
	function<void(vector<double>*, vector<double>*, vector<double>*, vector<double>*)> GetVectorPointer_D4_Functor() { return SetVectorPointer_D4_Functor(); }
	function<void(vector<double>*, vector<double>*, vector<double>*, vector<double>*, vector<double>*)> GetVectorPointer_D5_Functor() { return SetVectorPointer_D5_Functor(); }

	function<double(size_t)> Get_BLANK_TableFunctor() { return Set_BLANK_TableFunctor(); }
	function<double(size_t, double)> Get_D1_TableFunctor() { return Set_D1_TableFunctor(); }
	function<double(size_t, double, double)> Get_D2_TableFunctor() { return Set_D2_TableFunctor(); }
	function<double(size_t, double, double, double)> Get_D3_TableFunctor() { return Set_D3_TableFunctor(); }
	function<double(size_t, double, double, double, double)> Get_D4_TableFunctor() { return Set_D4_TableFunctor(); }
	

	function<double(size_t, size_t, double, double)> Get_T_D2_TableFunctor() { return Set_T_D2_TableFunctor(); }
	function<double(size_t, size_t, double, double, double)> Get_T_D3_TableFunctor() { return Set_T_D3_TableFunctor(); }
	function<double(size_t, size_t, double, double, double, double)> Get_T_D4_TableFunctor() { return Set_T_D4_TableFunctor(); }

	function<double(bool, size_t)> Get_boolBlank_TableFunctor() { return Set_boolBlank_TableFunctor(); }
	function<double(bool, size_t, double)> Get_D1_boolTableFunctor() { return Set_boolD1_TableFunctor(); }
	function<double(bool, size_t, double, double)> Get_D2_boolTableFunctor() { return Set_boolD2_TableFunctor(); }
	function<double(bool, size_t, double, double, double)> Get_D3_boolTableFunctor() { return Set_boolD3_TableFunctor(); }
	function<double(bool, size_t, double, double, double, double)> Get_D4_boolTableFunctor() { return Set_boolD4_TableFunctor(); }

	function<OrgTurnOver(double, double, double)> Get_OrgTurnOverFunctor() { return SetOrgTurnOverFunctor(); }
	function<OrgTurnOver(double, double, double, double, double)> Get_D5_OrgTurnOverFunctor() { return SetOrg_D5_TurnOverFunctor(); }
	
	function<pair<double, double>(double, double)> Get_Pair_D2Functor() { return Set_Pair_D2_Functor(); }
	function<pair<double, double>(double, double, double)> Get_Pair_D3Functor() { return Set_Pair_D3_Functor(); }

	function<pair<double, double>(size_t, double, double)> Get_Pair_D2_TableFunctor() { return Set_Pair_D2_TableFunctor(); }
	function<pair<double, double>(size_t, double, double, double)> Get_Pair_D3_TableFunctor() { return Set_Pair_D3_TableFunctor(); }
private:
	

	function<double(double)> Set_D1_Functor();
	function<double(double, double)> Set_D2_Functor();
	function<double(double, double, double)> Set_D3_Functor(); 
	function<double(double, double, double, double)> Set_D4_Functor(); 
	function<double(double, double, double, double, double)> Set_D5_Functor();
	function<double(double, double, double, double, double,double )> Set_D6_Functor();
	
	function<double(bool)> Set_boolBlank_Functor();
	function<double(bool, double)> Set_boolD1_Functor();
	function<double(bool, double, double)> Set_boolD2_Functor();
	function<double(bool, double, double, double)> Set_boolD3_Functor();
	function<double(bool, double, double, double, double)> Set_boolD4_Functor();
	function<double(bool, double, double, double, double, double)> Set_boolD5_Functor();

	function<vector<double>()> SetVector_Blank_Functor();
	function<vector<double>(bool)> SetVector_boolBlank_Functor();
	function<vector<double>(bool, double)> SetVector_boolD1_Functor();
	function<vector<double>(bool, double, double)> SetVector_boolD2_Functor();
	function<vector<double>(bool, double, double, double)> SetVector_boolD3_Functor();
	function<vector<double>(bool, double, double, double, double)> SetVector_boolD4_Functor();
	function<vector<double>(bool, double, double, double, double, double)> SetVector_boolD5_Functor();

	function<void(vector<double>*)>  SetVectorPointer_D1_Functor(); 
	function<void(vector<double>*, vector<double>*)>  SetVectorPointer_D2_Functor(); 
	function<void(vector<double>*, vector<double>*, vector<double>*)>  SetVectorPointer_D3_Functor(); 
	function<void(vector<double>*, vector<double>*, vector<double>*, vector<double>*)> SetVectorPointer_D4_Functor(); 
	function<void(vector<double>*, vector<double>*, vector<double>*, vector<double>*, vector<double>*)> SetVectorPointer_D5_Functor(); 



	function<double(size_t)> Set_BLANK_TableFunctor();
	function<double(size_t, double)> Set_D1_TableFunctor();
	function<double(size_t, double, double)> Set_D2_TableFunctor();
	function<double(size_t, double, double, double)> Set_D3_TableFunctor();
	function<double(size_t, double, double, double, double)> Set_D4_TableFunctor();
	
	function<double(bool, size_t)> Set_boolBlank_TableFunctor();
	function<double(bool, size_t, double)> Set_boolD1_TableFunctor();
	function<double(bool, size_t, double, double)> Set_boolD2_TableFunctor();
	function<double(bool, size_t, double, double, double)> Set_boolD3_TableFunctor();
	function<double(bool, size_t, double, double, double, double)> Set_boolD4_TableFunctor();


	function<double(double)> Set_D1_PhysFunctor();
	function<double(double, double)> Set_D2_PhysFunctor();
	function<double(double, double, double)> Set_D3_PhysFunctor();
	function<double(double, double, double, double)> Set_D4_PhysFunctor();
	function<double(double, double, double, double, double)> Set_D5_PhysFunctor();
	function<double(double, double, double, double, double, double)> Set_D6_PhysFunctor();

	function<OrgTurnOver(double, double, double)> SetOrgTurnOverFunctor();
	function<OrgTurnOver(double, double, double, double, double)> SetOrg_D5_TurnOverFunctor();

	function<pair<double, double>(size_t, double, double)> Set_Pair_D2_TableFunctor();
	function<pair<double, double>(size_t, double, double, double)> Set_Pair_D3_TableFunctor();
	function<pair<double, double>(double, double)> Set_Pair_D2_Functor();
	function<pair<double, double>(double, double, double)> Set_Pair_D3_Functor();
	
	
	function<double(size_t, size_t, double, double)> Set_T_D2_TableFunctor();
	function<double(size_t, size_t, double, double, double)> Set_T_D3_TableFunctor();
	function<double(size_t, size_t, double, double, double, double)> Set_T_D4_TableFunctor();
	
	vector<double> m_Ps_Par;
	vector<vector<double>> m_P_Par;

	function<double(double)> m_functor1;
	function<double(double, double)> m_functor2;
	function<double(double, double, double)> m_functor3;
	function<double(double, double, double, double)> m_functor4;
	function<double(double, double, double, double, double)> m_functor5;
	function<double(size_t, double)> m_Tablefunctor1;
	function<double(size_t, double, double)> m_Tablefunctor2;
	function<double(size_t, double, double, double)> m_Tablefunctor3;
	function<double(size_t, double, double, double, double)> m_Tablefunctor4;
	FunctorProp m_Prop;

	HYDRAULIC_COEF *m_pHydCoef;
};
