#include "P_ExternalInputs.h"



P_ExternalInputs::P_ExternalInputs(void)
{


}
P_ExternalInputs::~P_ExternalInputs(void)
{
}

bool P_ExternalInputs::Ini()
{
	NC_ExternalInputs::Ini();
	return true;
}
bool P_ExternalInputs::IniFunctors()
{
	NC_ExternalInputs::IniFunctors();
	return true;
}
bool P_ExternalInputs::Dyn()
{
	NC_ExternalInputs::Dyn();
	Flux();
	Integ();
	return true;
}
void P_ExternalInputs::Flux()
{
	if (N_Fertilization > 0) {

		P_Fert_App = 0.;
		if (N_Fertilization == 1) {
			for (size_t i = 0; i < N_Fert_DayNo.size(); i++) {
				if (m_pModelInfo->JDayNum == int(N_Fert_DayNo[i] + 0.1)) {
					P_Fert_App = P_Fert_Rate[i];
					break;
				}
			}
		}
		P_Fert_Sol = P_Fert * P_FertDiss_k;
	}
}
void P_ExternalInputs::Integ()
{
	if (N_Fertilization > 0) {
		InFlow(P_Fert, P_Fert_App);
		OutFlow(P_Fert, P_Fert_Sol);

		InFlow(Soil_PO4.front(), P_Fert_Sol);

	}
	if (N_Deposition > 0) {
		InFlow(Soil_PO4.front(), Dep_P_DryRate);
	}

}

bool P_ExternalInputs::End()
{
	NC_ExternalInputs::End();
	return true;
}