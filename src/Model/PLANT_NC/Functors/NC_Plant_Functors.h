#pragma once

enum class Growth_Sw{ off,Logistic_function,Water_use_efficiency,Radiation_use_efficiency, Farquhar};
enum class PhotoInput_Sw{Simulated,PG_file};
enum class Winter_Regulation_Sw{off,on};
enum class N_Fixation_Sw{ off,C_demand_driven,Root_biomass_driven,Stem_and_Leaf_biomass_driven};
enum class N_Fixed_Supply_Sw{off, on};
enum class Root_Density_Influence_Sw{no, on_nitrogen_uptake};
enum class Root_Allocation_Combination_Sw{ Maximum_value,Multiplicative_response,Average_response};
enum class Root_Allocation_TotMass_Sw { Independent,Linear,Exp};
enum class Root_Alloc_N_Leaf_Sw{ Independent, Linear, Exp };
enum class Root_Alloc_P_Leaf_Sw { Independent, Linear, Exp };
enum class Root_Alloc_WaterStress_Sw{ Independent, Linear, Exp };
enum class Leaf_Alloc_TotMass_Sw{ Independent, Linear, Exp};
enum class N_ReAllocation_Sw{off, on};
enum class Phos_ReAllocation_Sw { As_Carbon, As_Nitrogen,Indepedent };
enum class Resp_Temp_Response_Sw{ Common,Q10_whole_range,Q10_above_threshold};

enum class LitterFall_Dynamics_Sw{ Static ,GrowthTempSum,DormingTempSum};
enum class Salinity_Stress_Sw { off, on_photosynthesis, on_respiration };
enum class Plant_Stress_Sw{ All_multiplicative,Combined_Water_and_Nitrogen_function };
enum class PhoSaturation_Sw{off, on};
enum class PlantRespiration_Sw{ Maintenance_Only,Growth_and_Maintenance,Growth_and_Maintainance_and_NitrogenUptake };
enum class GrowthResponseVar_Sw{ Actual,Mean_and_Delayed };
enum class Rad_Interception_Sw{ Simulated_Input,Assumed_Input};
enum class InitialPlantConditions_Sw{As_Nitrogen,As_Nitogen_and_Carbon};
enum class InitialPlantPConditions_Sw { As_Phosphorus, As_CP_ratio_and_Carbon };
enum class TerminationHarvest_Sw{off, on};
enum class FungalGrowth_Sw{off, simple, detailed};
enum class ReAllocationToLeaf_Sw{off, on};
enum class ResetGSI_AtHarvest_Sw{off, on, on_at_completegrainharvest};
enum class Harvest_Day_Sw{ Fixed_Single,Fixed_Multiple_Days,Estimated_GSI,Estimated_LAI,PG_File_specified ,In_Year_Table };
enum class PlantsToHavest_Sw{ All_Plants,Only_One_Plant };
enum class Cleaning_Sw{off, All_Years, Single_Years};
enum class Sowing_Switch_Sw{ off,Fixed_dayNo,Temperature_Sum,Multiple_DayNo};
enum class Emergence_Switch_Sw{ off,Fixed_dayNo,Temperature_Sum,Multiple_DayNo};
enum class StartSeasonTableExist_Sw{off,on};
enum class GreenIndexForcing_Sw{off, on};
enum class Grazing_Sw{off, Continous,At_PG_File_Specified_Days_Static_Grazing};
enum class GrazingClosure_Sw { Growth_Estimate_from_Manure,Manure_Estimate_from_Growth };
enum class ZadokScale_Sw { 	off, on, withOutPut2File, withOutPut3File};
enum class Seedling_Management_Sw {	off, After_Final_Harvest, Specific_Years};
enum class CanopyCO2_Storage_Sw {Static, Dynamic};
enum class ZadokForcingFromFileInput_Sw{off, on};


#include "./BiomassDescription/BioMass.h"
#include "./GrowthStages/GrowthStages.h"
#include "./PhotoSynthesis/PhotoTemp.h"
#include "./PhotoSynthesis/PhotoNLeafResponse.h"
#include "./PhotoSynthesis/LogisticGrowth.h"
#include "./PhotoSynthesis/PhotoRadEff.h"
#include "./PhotoSynthesis/GPP_Drivers.h"
#include "./Allocation/AlloRootMass.h"
#include "./Allocation/AlloLeafMass.h"
#include "./LitterFall/LitterTemplate.h"
#include "./Respiration/RespirationTemplate.h"
#include "./UptakeFromSoil/PlantUptake.h"



