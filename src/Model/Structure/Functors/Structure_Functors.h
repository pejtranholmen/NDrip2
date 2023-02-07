#pragma once
#include "../../../std.h"
#include "./Technical_Functors.h"

enum class PlantType_Sw {NoVegetation, Implicit_Big_Leaf, Explicit_Single_Leaf, Explicit_Multi_Leaves};
enum class SaltTracer_Sw {off, on};
enum class GroundWaterFlow_Sw{off, on};
enum class WaterEq_Sw{ off,On_Richard_Equation,On_but_without_Richard_Equation};
enum class HeatEq_Sw{off, on};
enum class Irrigation_Sw{off, on};
enum class SoilVapour_Sw{ off,Only_SoilVapourflow,Soil_and_SnowVapourflow,Only_SnowVapourflow };
enum class SnowPack_Option_Sw {off, on};
enum class Evaporation_Sw { off,Simple_input_style,Radiation_input_style};
enum class Nitrogen_Carbon_Sw { off,Dynamic_interaction_with_abiotics,Using_abiotic_driving_variables };
enum class LateralInput_Sw { No,In_driving_file,WaterShed_approach };
enum class HBV_Soil_Sw {Off, As_ComplementaryModel, ExcludingRichardEquation};
enum class OnlyNC_Sw {No, Yes};
enum class Phosphorus_Model_Sw { off, Simple, Detailed };
