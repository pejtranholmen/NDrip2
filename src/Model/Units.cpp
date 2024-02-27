
#include "Units.h"




 Units::Units() {  
	 string unit, m2;
	SetUnitMap(UNIT_TYPES::WATERMASS_UNIT, "mm");
	SetUnitMap(UNIT_TYPES::WATERFLOW_UNIT,"mm/day");
	SetUnitMap(UNIT_TYPES::INVERSE_WATERFLOW_UNIT, "day/mm");
	
	SetUnitMap(UNIT_TYPES::VOLPERCENTAGE_UNIT, "Vol %");
	SetUnitMap(UNIT_TYPES::WATERCONTENT_UNIT, "Vol %");
	SetUnitMap(UNIT_TYPES::PRESSUREHEAD_UNIT,"cm water");
	unit = "J/m"; unit += char(178); unit += "/day";
	SetUnitMap(UNIT_TYPES::HEATFLOW_UNIT,unit);
	unit = "J/m"; unit += char(178); unit += "/day";
	SetUnitMap(UNIT_TYPES::HEATFLOW_UNIT, unit);
	SetUnitMap(UNIT_TYPES::HEATSOURCE_UNIT, unit);
	unit = "MJ/m"; unit += char(178); unit += "/day";
	SetUnitMap(UNIT_TYPES::MEGA_HEATFLOW_UNIT, unit);

	unit = "1/";
	unit += char(176); unit += 'C';
	SetUnitMap(UNIT_TYPES::INVERSE_TEMP_UNIT, unit);

	unit=char(176);unit+='C';
	SetUnitMap(UNIT_TYPES::TEMP_UNIT, unit);


	unit += "day";
	SetUnitMap(UNIT_TYPES::TEMPSUM_UNIT, unit);

	unit = "day";
	SetUnitMap(UNIT_TYPES::DAYOFYEAR_UNIT, unit);

	unit = "W/m/";	unit += char(176); unit += 'C';
	SetUnitMap(UNIT_TYPES::THERMAL_CONDUCTIVY_UNIT, unit);


	m2 = "m"; m2 += char(178);
	string str;
	str = "day";
	str+=char(178);
	str += "m6/g";
	str+=char(178);
	SetUnitMap(UNIT_TYPES::INVERSE_SQUARED_FLUX_PER_VOLUME, str);
	SetUnitMap(UNIT_TYPES::MASS_RATIO_UNIT, "g/g");
	SetUnitMap(UNIT_TYPES::VOLUME_RATIO_UNIT, "m3/m3");

	SetUnitMap(UNIT_TYPES::HEATSTORE_UNIT, "J/" + m2);
	SetUnitMap(UNIT_TYPES::MEGA_HEATSTORE_VOL_UNIT, "MJ/m3");
	unit = "W/m/"; unit += char(176); unit += "C";
	SetUnitMap(UNIT_TYPES::HEATCOND_UNIT, unit);
	unit = "Kg/m", unit += char(179);
	SetUnitMap(UNIT_TYPES::KILO_MASSDENSITY_UNIT, unit);
	
	unit = "g/m", unit += char(179);
	SetUnitMap(UNIT_TYPES::MASSDENSITY_UNIT,unit);
	SetUnitMap(UNIT_TYPES::AREA_UNIT,m2);
	
	SetUnitMap(UNIT_TYPES::MASS_UNIT,"g/"+m2);
	SetUnitMap(UNIT_TYPES::INVERSE_MASS_UNIT, m2+"/g");
	SetUnitMap(UNIT_TYPES::CARBON_MASS_UNIT, "gC/" + m2);
	SetUnitMap(UNIT_TYPES::NITROGEN_MASS_UNIT, "gN/" + m2);


	unit = "g/" + m2 + "/day";
	SetUnitMap(UNIT_TYPES::MASSFLOW_UNIT, unit);
	unit = "gC/" + m2 + "/day";
	SetUnitMap(UNIT_TYPES::CARBON_MASSFLOW_UNIT, unit);
	unit = "gN/" + m2 + "/day";
	SetUnitMap(UNIT_TYPES::NITROGEN_MASSFLOW_UNIT, unit);

	
	unit = "g/m", unit += char(178);
	SetUnitMap(UNIT_TYPES::GAS_MASS_UNIT, unit);
	SetUnitMap(UNIT_TYPES::PPM_CONC_UNIT,"PPM");
	
	SetUnitMap(UNIT_TYPES::YEAR_AGE_UNIT ,"yr");
	
	unit = "m", unit += char(178); unit += "/day";
	SetUnitMap(UNIT_TYPES::AREA_PER_TIME_UNIT, unit);
	unit = "m", unit += char(178); unit += "/s";
	SetUnitMap(UNIT_TYPES::AREA_PER_SEC_TIME_UNIT, unit);

	unit = "s/m";
	SetUnitMap(UNIT_TYPES::AERODYNAMIC_RESISTANCE_UNIT, unit);
	unit = "m/s";
	SetUnitMap(UNIT_TYPES::WIND_SPEED_UNIT, unit);

	unit = "cm", unit += char(178); unit += "/s";
	SetUnitMap(UNIT_TYPES::CM_AREA_PER_SEC_TIME_UNIT, unit);

	
	unit = "mg/m"; unit += char(178);
	SetUnitMap(UNIT_TYPES::mMASS_UNIT, unit);
	unit+= "/day";
	SetUnitMap(UNIT_TYPES::mMASSFLOW_UNIT, unit);
	unit = char(181);
	unit += "g/m"; unit += char(178); unit += "/day"; 
	SetUnitMap(UNIT_TYPES::MIKRO_MASS_FLOW_UNIT, unit);



	unit = char(181); unit += "mol/m"; unit += char(178); unit += "/s";
	SetUnitMap(UNIT_TYPES::MIKRO_MOL_SEC_RATE, unit);
	SetUnitMap(UNIT_TYPES::PRESSURE_UNIT, "Pa");
	SetUnitMap(UNIT_TYPES::LENGTH_UNIT, "m");
	SetUnitMap(UNIT_TYPES::LENGTH_PER_TIME_UNIT, "m/s");
	SetUnitMap(UNIT_TYPES::RATECOEF_UNIT, "1/day");

	SetUnitMap(UNIT_TYPES::NO_UNIT, "-");
	SetUnitMap(UNIT_TYPES::PERCENTAGE, "%");
	SetUnitMap(UNIT_TYPES::ENERGY_MOLE, "J/mole");
	SetUnitMap(UNIT_TYPES::mMASS_LITER_UNIT, "mg/l");
	unit = char(181); unit += "g/l";
	SetUnitMap(UNIT_TYPES::mikro_MASS_LITER_UNIT, unit);
	unit = char(181); unit += "g/g";
	SetUnitMap(UNIT_TYPES::MIKRO_MASS_RATIO, unit);
	unit = char(181); unit += "g/day";
	SetUnitMap(UNIT_TYPES::MIKRO_MASS_RATE, unit);
	

	unit = "day";
	SetUnitMap(UNIT_TYPES::TIME_UNIT, unit);
	unit = "minutes";
	SetUnitMap(UNIT_TYPES::TIME_MINUTE_UNIT, unit);
	unit = "rad";
	SetUnitMap(UNIT_TYPES::ANGLE_UNIT, unit);

	auto koll = GetAllStringBasedUnits();
 }

 Units::~Units()
 {
 }
	
