#pragma once

#include "../../../std.h"


enum class InitialMineral_N_Sw {
    Uniform_concentration, Depth_Dependence
};
enum class Denitrification_Sw {
    off, Old_SOILN, Microbial_based
};
enum class DenitrifierGrowth_Sw {
    Mineral_N, Organic_N, Both_Sources
};
enum class NitrifierGrowth_Sw {
    Mineral_N, Organic_N
};
enum class Denit_Temp_Response_Sw {
    Common, Q10_whole_range, Q10_above_threshold, Ratkowsky
};
enum class Nitrific_Temp_Response_Sw {
    Common, Q10_whole_range, Q10_above_threshold, Ratkowsky
};
enum class Denit_Depth_Distribution_Sw {
    Table, Linear_decrease, Constant, Exponential
};
enum class Ammonium_Mobility_Sw {
    No, Linear_isotherm
};
enum class Nitrification_Sw {
    Old_SOILN, Microbial_based
};
enum class Initial_Nitrifier_Sw {
    Linear_decrease, Constant, Exponential
};
enum class Flexible_Uptake_Sw {
    Common_for_all_plants, Specific_for_each_plant
};


class NitrificationRate {
public:
    NitrificationRate(double Nitr_Amm_Ratio, double Nitr_Specific_Rate) noexcept : ratio(Nitr_Amm_Ratio), ratecoef(Nitr_Specific_Rate) {}

    double operator() (double NH4, double NO3) {
        return  ratecoef * max(0., (NH4 - NO3 / ratio));
    }


private: 
    double ratio, ratecoef;

};