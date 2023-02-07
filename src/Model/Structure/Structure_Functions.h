#pragma once
#include "./Numerical.h"

class Structure_Functions : public Numerical
{
	public:
		Structure_Functions(void);
		~Structure_Functions(void);
		bool Def();



		size_t	RadiationInput, OnlyNC;

		size_t	SaltTracer;	//! Salt switch
		size_t	GroundWaterFlow;///		! GWFLOW switch
		size_t	WaterEq;//		! Water
		size_t SoilVapour;//	! Soil vapour flow
		size_t Irrigation;//	! Irrigation
		size_t IrrigationInput;//	! Irrigation control
		size_t SnowPack_Option;//	! Snow
		size_t HeatEq;//		! Heat
		size_t HeatPump;// 	! Soil Heat Pump
		size_t PlantType;//	! Previous EVAPOTR <= 2
		size_t LateralInput;//	! Previous NSOURCE
		size_t LateralGWSource;//	! New Ground Water Source August 05
		size_t SaltGWSource;//	! New Salt Ground Water Source August 05
		//size_t RunOnInput;//	! New Surface runon Source August 05
		size_t Nitrogen_Carbon;//! New coupling to SOILN July 99
							   //Integer, pointer :: Sw_Minteq			! New independent link to Minteq May 02
							   //Integer, pointer :: Dripper		! New Switch from Irrigation
		size_t	Evaporation, WuptBasic_equation;
		size_t HBV_Soil; //! New HBV soil module
		size_t Phosphorus_Model; //! New HBV soil module

		size_t m_NumActPlantElements;




};

