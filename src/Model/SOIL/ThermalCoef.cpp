#include "ThermalCoef.h"

Thermal_Coef::Thermal_Coef() {
	m_ThermalCond.Air=0.025;
	m_ThermalCond.Organic=0.25;// Organic Matter
	m_ThermalCond.Mineral=2.5;
	m_ThermalCond.Quarts=8;
	m_ThermalCond.Water=0.57;
	m_ThermalCond.Ice=2.21;
};