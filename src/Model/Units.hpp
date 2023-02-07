#pragma once

enum class UNIT_TYPES {
	MEGA_HEATFLOW_UNIT,
	HEATFLOW_UNIT, HEATSOURCE_UNIT, HEATSTORE_UNIT, MEGA_HEATSTORE_VOL_UNIT,
	TEMP_UNIT, TEMPSUM_UNIT, HEATCOND_UNIT, MASS_UNIT, CARBON_MASS_UNIT, CARBON_MASSFLOW_UNIT, NITROGEN_MASS_UNIT, NITROGEN_MASSFLOW_UNIT,UNIT_TYPES::MASSFLOW_UNIT, WIND_SPEED_UNIT,
	WATERFLOW_UNIT, WATERCONTENT_UNIT, PRESSUREHEAD_UNIT, AREA_UNIT, MASSDENSITY_UNIT, KILO_MASSDENSITY_UNIT, INVERSE_MASS_UNIT,
	mMASS_UNIT, mMASSFLOW_UNIT, WATERMASS_UNIT, PRESSURE_UNIT, LENGTH_UNIT, LENGTH_PER_TIME_UNIT, AERODYNAMIC_RESISTANCE_UNIT,
	MIKRO_MOL_SEC_RATE, AREA_PER_TIME_UNIT, CM_AREA_PER_SEC_TIME_UNIT, GAS_MASS_UNIT, GAS_CONC_UNIT, YEAR_AGE_UNIT, PERCENTAGE, TIME_UNIT, ENERGY_MOLE,
	mMASS_LITER_UNIT, mikro_MASS_LITER_UNIT, NO_UNIT, RATECOEF_UNIT, INVERSE_WATERFLOW_UNIT, INVERSE_SQUARED_FLUX_PER_VOLUME, MASS_RATIO_UNIT, VOLUME_RATIO_UNIT,
	MIKRO_MASS_RATIO, MIKRO_MASS_FLOW_UNIT, MIKRO_MASS_RATE, TIME_MINUTE_UNIT, ANGLE_UNIT, THERMAL_CONDUCTIVY_UNIT

};

struct MODELUNITS {
	enum M_UNITS {};
	string value;
};
static map<UNIT_TYPES, string> m_TypeBasedUnits;
static map<string, UNIT_TYPES> m_StringBasedUnits;

namespace Units { 
	string unit, m2;


	
	static size_t GetNumUnits() { return m_TypeBasedUnits.size(); };
	
	static string GetUnitString(UNIT_TYPES key, bool withparanthesis = false) {
		auto it = m_TypeBasedUnits.find(key);
		if (it != m_TypeBasedUnits.end()) {
			if (withparanthesis) {
				return "(" + (*it).second + ")";
			}
			else
				return (*it).second;
		}
		else return "";
	};
	static UNIT_TYPES GetUnitType(string key) {
		auto it = m_StringBasedUnits.find(key);
		if (it != m_StringBasedUnits.end())
			return (*it).second;
		else
			return UNIT_TYPES::NO_UNIT;
	};
	static vector<string> GetAllStringBasedUnits() {
		vector<string> out;
		for (auto it = m_TypeBasedUnits.begin(); it != m_TypeBasedUnits.end(); ++it) {
			out.push_back((*it).second);
		};
		return out;
	};
	static vector<string> GetSelectedStringBasedUnits(vector<UNIT_TYPES> key) {
		vector<string> out;
		for_each(key.begin(), key.end(), [&](UNIT_TYPES& key) {
			auto it = m_TypeBasedUnits.find(key);
			out.push_back((*it).second);
			});
		return out;
	};

 }


	
