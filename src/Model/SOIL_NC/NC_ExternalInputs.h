#pragma once

#include "./NC_ExternalInputs_Functions.h"
class NC_ExternalInputs : public NC_ExternalInputs_Functions
{
	public:
		NC_ExternalInputs(void);
		~NC_ExternalInputs(void);
		bool Ini();
		bool IniFunctors();
		bool Dyn();
		void Flux();
		void Integ();
		bool End();
		bool CheckEcoDriv4();
		bool CheckEcoDeposition();
		bool ReadAndSetParameterValues(string newname);
		function<double(double)> f_NFert_WithinYear;



};
